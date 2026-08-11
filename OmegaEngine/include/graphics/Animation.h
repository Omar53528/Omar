#pragma once

#include "core/Types.h"
#include <vector>
#include <string>
#include <functional>

namespace Omega {

// Animation Curve Keyframe
struct Keyframe {
    float time;
    float value;
    enum InterpolationMode {
        Linear,
        Constant,
        Bezier
    } mode = Linear;
    
    // Bezier control points
    Vector2 inTangent;
    Vector2 outTangent;
};

// Animation Curve (Float)
class AnimationCurve {
public:
    std::vector<Keyframe> keyframes;
    
    void AddKeyframe(float time, float value, Keyframe::InterpolationMode mode = Keyframe::Linear);
    void RemoveKeyframe(int index);
    float Evaluate(float time) const;
    void Clear();
    
private:
    float Lerp(float a, float b, float t) const;
    float BezierInterpolate(float t0, float v0, float t1, float v1, float t) const;
};

// Transform Animation Track
struct TransformTrack {
    AnimationCurve positionX, positionY, positionZ;
    AnimationCurve rotationX, rotationY, rotationZ, rotationW;
    AnimationCurve scaleX, scaleY, scaleZ;
    
    Transform Evaluate(float time) const;
};

// Skeleton Bone
struct Bone {
    std::string name;
    int parentIndex = -1;
    Matrix4x4 inverseBindMatrix;
    Matrix4x4 localTransform;
    Matrix4x4 globalTransform;
};

// Skinned Mesh
struct SkinnedMesh {
    std::vector<Bone> bones;
    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<Vector2> uvs;
    std::vector<uint32_t> indices;
    
    // Skin weights (4 bones per vertex)
    struct VertexBoneData {
        int boneIDs[4] = {0, 0, 0, 0};
        float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    };
    std::vector<VertexBoneData> boneData;
};

// Animation Clip
struct AnimationClip {
    std::string name;
    float duration = 0.0f;
    float fps = 30.0f;
    bool loop = true;
    
    std::vector<TransformTrack> tracks; // One per bone
    
    Transform EvaluateBoneTransform(int boneIndex, float time) const;
};

// Animator State
enum class AnimatorState {
    Idle,
    Playing,
    Paused,
    Stopped
};

// Animation Blend Tree Node
enum class BlendTreeNodeType {
    SingleClip,
    Blend1D,
    Blend2D,
    Additive,
    Layer
};

struct BlendTreeNode {
    BlendTreeNodeType type;
    std::string clipName;
    float weight = 1.0f;
    
    // For 1D blending
    std::string parameter1D;
    std::vector<std::pair<float, std::string>> blendPoints1D;
    
    // For 2D blending
    std::string parameterX, parameterY;
    struct BlendPoint2D {
        float x, y;
        std::string clipName;
    };
    std::vector<BlendPoint2D> blendPoints2D;
    
    std::vector<int> childIndices;
};

// Animator Controller
class Animator {
public:
    Animator();
    
    // Setup
    void SetSkeleton(const std::vector<Bone>& bones);
    void AddClip(const AnimationClip& clip);
    void SetBlendTree(const std::vector<BlendTreeNode>& nodes);
    
    // Playback
    void Play(const std::string& clipName, float fadeDuration = 0.0f);
    void Stop();
    void Pause();
    void Resume();
    
    // Parameters for blending
    void SetFloat(const std::string& name, float value);
    void SetInt(const std::string& name, int value);
    void SetBool(const std::string& name, bool value);
    void SetTrigger(const std::string& name);
    void ResetTrigger(const std::string& name);
    
    // Update
    void Update(float deltaTime);
    
    // Get current transforms
    const std::vector<Matrix4x4>& GetFinalBoneMatrices() const { return finalBoneMatrices; }
    
    // State
    AnimatorState GetState() const { return state; }
    float GetNormalizedTime() const { return normalizedTime; }
    std::string GetCurrentClip() const { return currentClip; }
    
private:
    std::vector<Bone> skeleton;
    std::vector<AnimationClip> clips;
    std::vector<BlendTreeNode> blendTree;
    
    std::unordered_map<std::string, float> floatParams;
    std::unordered_map<std::string, int> intParams;
    std::unordered_map<std::string, bool> boolParams;
    std::unordered_map<std::string, bool> triggers;
    
    std::string currentClip;
    float currentTime = 0.0f;
    float normalizedTime = 0.0f;
    AnimatorState state = AnimatorState::Stopped;
    
    float targetWeight = 1.0f;
    float currentWeight = 0.0f;
    float fadeTimer = 0.0f;
    
    std::vector<Matrix4x4> finalBoneMatrices;
    
    Matrix4x4 CalculateBlendTransform(float deltaTime);
    void UpdateBoneMatrices();
};

// Cinematic Camera Sequence
struct CameraKeyframe {
    float time;
    Vector3 position;
    Vector3 lookAt;
    float fov = 60.0f;
    float roll = 0.0f;
    enum InterpolationMode {
        Linear,
        Smooth,
        EaseIn,
        EaseOut,
        EaseInOut
    } interpolation = Smooth;
};

class CinematicCamera {
public:
    void AddKeyframe(const CameraKeyframe& kf);
    void RemoveKeyframe(int index);
    void Clear();
    
    void Play(float duration, bool loop = false);
    void Stop();
    void SetTime(float time);
    
    // Get interpolated camera state
    Vector3 GetPosition() const { return currentPosition; }
    Vector3 GetLookAt() const { return currentLookAt; }
    float GetFOV() const { return currentFOV; }
    
    bool IsPlaying() const { return isPlaying; }
    float GetTime() const { return currentTime; }
    float GetDuration() const { return duration; }
    
private:
    std::vector<CameraKeyframe> keyframes;
    float currentTime = 0.0f;
    float duration = 0.0f;
    bool isPlaying = false;
    bool isLooping = false;
    
    Vector3 currentPosition;
    Vector3 currentLookAt;
    float currentFOV = 60.0f;
    
    Vector3 InterpolatePosition(float time) const;
    Vector3 InterpolateLookAt(float time) const;
    float InterpolateFOV(float time) const;
};

} // namespace Omega
