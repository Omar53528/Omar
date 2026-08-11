#include "graphics/Animation.h"
#include <algorithm>
#include <cmath>

namespace Omega {

// AnimationCurve Implementation
void AnimationCurve::AddKeyframe(float time, float value, Keyframe::InterpolationMode mode) {
    Keyframe kf;
    kf.time = time;
    kf.value = value;
    kf.mode = mode;
    keyframes.push_back(kf);
    
    // Sort by time
    std::sort(keyframes.begin(), keyframes.end(), 
        [](const Keyframe& a, const Keyframe& b) { return a.time < b.time; });
}

void AnimationCurve::RemoveKeyframe(int index) {
    if (index >= 0 && index < (int)keyframes.size()) {
        keyframes.erase(keyframes.begin() + index);
    }
}

float AnimationCurve::Lerp(float a, float b, float t) const {
    return a + (b - a) * t;
}

float AnimationCurve::BezierInterpolate(float t0, float v0, float t1, float v1, float t) const {
    float u = 1.0f - t;
    return u * u * v0 + 2 * u * t * ((v0 + v1) / 2.0f) + t * t * v1;
}

float AnimationCurve::Evaluate(float time) const {
    if (keyframes.empty()) return 0.0f;
    if (keyframes.size() == 1) return keyframes[0].value;
    
    // Clamp to range
    if (time <= keyframes.front().time) return keyframes.front().value;
    if (time >= keyframes.back().time) return keyframes.back().value;
    
    // Find surrounding keyframes
    for (size_t i = 0; i < keyframes.size() - 1; ++i) {
        const Keyframe& k1 = keyframes[i];
        const Keyframe& k2 = keyframes[i + 1];
        
        if (time >= k1.time && time <= k2.time) {
            float t = (time - k1.time) / (k2.time - k1.time);
            
            switch (k1.mode) {
                case Keyframe::Constant:
                    return k1.value;
                case Keyframe::Linear:
                    return Lerp(k1.value, k2.value, t);
                case Keyframe::Bezier:
                    return BezierInterpolate(k1.time, k1.value, k2.time, k2.value, t);
            }
        }
    }
    
    return keyframes.back().value;
}

void AnimationCurve::Clear() {
    keyframes.clear();
}

// TransformTrack Evaluation
Transform TransformTrack::Evaluate(float time) const {
    Transform result;
    result.position.x = positionX.Evaluate(time);
    result.position.y = positionY.Evaluate(time);
    result.position.z = positionZ.Evaluate(time);
    
    // Simplified rotation evaluation
    float rx = rotationX.Evaluate(time);
    float ry = rotationY.Evaluate(time);
    float rz = rotationZ.Evaluate(time);
    result.rotation = Vector3(rx, ry, rz);
    
    result.scale.x = scaleX.Evaluate(time);
    result.scale.y = scaleY.Evaluate(time);
    result.scale.z = scaleZ.Evaluate(time);
    
    return result;
}

// AnimationClip Evaluation
Transform AnimationClip::EvaluateBoneTransform(int boneIndex, float time) const {
    if (boneIndex < 0 || boneIndex >= (int)tracks.size()) {
        return Transform();
    }
    return tracks[boneIndex].Evaluate(time);
}

// Animator Implementation
Animator::Animator() {
    finalBoneMatrices.resize(1);
}

void Animator::SetSkeleton(const std::vector<Bone>& bones) {
    skeleton = bones;
    finalBoneMatrices.resize(bones.size());
}

void Animator::AddClip(const AnimationClip& clip) {
    clips.push_back(clip);
}

void Animator::Play(const std::string& clipName, float fadeDuration) {
    currentClip = clipName;
    currentTime = 0.0f;
    state = AnimatorState::Playing;
    fadeTimer = fadeDuration;
    targetWeight = 1.0f;
    currentWeight = fadeDuration > 0 ? 0.0f : 1.0f;
}

void Animator::Stop() {
    state = AnimatorState::Stopped;
    currentTime = 0.0f;
    normalizedTime = 0.0f;
}

void Animator::Pause() {
    if (state == AnimatorState::Playing) {
        state = AnimatorState::Paused;
    }
}

void Animator::Resume() {
    if (state == AnimatorState::Paused) {
        state = AnimatorState::Playing;
    }
}

void Animator::SetFloat(const std::string& name, float value) {
    floatParams[name] = value;
}

void Animator::SetInt(const std::string& name, int value) {
    intParams[name] = value;
}

void Animator::SetBool(const std::string& name, bool value) {
    boolParams[name] = value;
}

void Animator::SetTrigger(const std::string& name) {
    triggers[name] = true;
}

void Animator::ResetTrigger(const std::string& name) {
    triggers[name] = false;
}

void Animator::Update(float deltaTime) {
    if (state != AnimatorState::Playing) return;
    
    // Find clip
    auto it = std::find_if(clips.begin(), clips.end(),
        [this](const AnimationClip& c) { return c.name == currentClip; });
    
    if (it == clips.end()) return;
    
    AnimationClip& clip = *it;
    
    // Update fade
    if (fadeTimer > 0) {
        fadeTimer -= deltaTime;
        currentWeight = 1.0f - (fadeTimer / (fadeDuration > 0 ? fadeDuration : 1.0f));
        if (fadeTimer <= 0) {
            currentWeight = 1.0f;
        }
    }
    
    // Update time
    currentTime += deltaTime;
    if (currentTime >= clip.duration) {
        if (clip.loop) {
            currentTime = fmod(currentTime, clip.duration);
        } else {
            currentTime = clip.duration;
            state = AnimatorState::Stopped;
        }
    }
    
    normalizedTime = currentTime / clip.duration;
    
    UpdateBoneMatrices();
}

Matrix4x4 Animator::CalculateBlendTransform(float deltaTime) {
    // Placeholder for blend tree evaluation
    return Matrix4x4::Identity();
}

void Animator::UpdateBoneMatrices() {
    auto it = std::find_if(clips.begin(), clips.end(),
        [this](const AnimationClip& c) { return c.name == currentClip; });
    
    if (it == clips.end() || skeleton.empty()) return;
    
    for (size_t i = 0; i < skeleton.size(); ++i) {
        Transform t = it->EvaluateBoneTransform((int)i, currentTime);
        finalBoneMatrices[i] = Matrix4x4::FromTransform(t);
    }
}

// CinematicCamera Implementation
void CinematicCamera::AddKeyframe(const CameraKeyframe& kf) {
    keyframes.push_back(kf);
    std::sort(keyframes.begin(), keyframes.end(),
        [](const CameraKeyframe& a, const CameraKeyframe& b) { return a.time < b.time; });
    
    if (!keyframes.empty()) {
        duration = keyframes.back().time;
    }
}

void CinematicCamera::RemoveKeyframe(int index) {
    if (index >= 0 && index < (int)keyframes.size()) {
        keyframes.erase(keyframes.begin() + index);
        duration = keyframes.empty() ? 0.0f : keyframes.back().time;
    }
}

void CinematicCamera::Clear() {
    keyframes.clear();
    currentTime = 0.0f;
    duration = 0.0f;
    isPlaying = false;
}

void CinematicCamera::Play(float dur, bool loop) {
    duration = dur;
    isLooping = loop;
    isPlaying = true;
    currentTime = 0.0f;
}

void CinematicCamera::Stop() {
    isPlaying = false;
    currentTime = 0.0f;
}

void CinematicCamera::SetTime(float time) {
    currentTime = std::clamp(time, 0.0f, duration);
    currentPosition = InterpolatePosition(currentTime);
    currentLookAt = InterpolateLookAt(currentTime);
    currentFOV = InterpolateFOV(currentTime);
}

Vector3 CinematicCamera::InterpolatePosition(float time) const {
    if (keyframes.empty()) return Vector3();
    if (keyframes.size() == 1) return keyframes[0].position;
    
    for (size_t i = 0; i < keyframes.size() - 1; ++i) {
        if (time >= keyframes[i].time && time <= keyframes[i + 1].time) {
            float t = (time - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time);
            return Vector3::Lerp(keyframes[i].position, keyframes[i + 1].position, t);
        }
    }
    
    return keyframes.back().position;
}

Vector3 CinematicCamera::InterpolateLookAt(float time) const {
    if (keyframes.empty()) return Vector3(0, 0, -1);
    if (keyframes.size() == 1) return keyframes[0].lookAt;
    
    for (size_t i = 0; i < keyframes.size() - 1; ++i) {
        if (time >= keyframes[i].time && time <= keyframes[i + 1].time) {
            float t = (time - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time);
            return Vector3::Lerp(keyframes[i].lookAt, keyframes[i + 1].lookAt, t);
        }
    }
    
    return keyframes.back().lookAt;
}

float CinematicCamera::InterpolateFOV(float time) const {
    if (keyframes.empty()) return 60.0f;
    if (keyframes.size() == 1) return keyframes[0].fov;
    
    for (size_t i = 0; i < keyframes.size() - 1; ++i) {
        if (time >= keyframes[i].time && time <= keyframes[i + 1].time) {
            float t = (time - keyframes[i].time) / (keyframes[i + 1].time - keyframes[i].time);
            return keyframes[i].fov + (keyframes[i + 1].fov - keyframes[i].fov) * t;
        }
    }
    
    return keyframes.back().fov;
}

} // namespace Omega
