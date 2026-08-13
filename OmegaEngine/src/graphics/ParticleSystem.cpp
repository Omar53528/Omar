#include "graphics/ParticleSystem.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <GL/gl.h>

namespace Omega {

    // متغيرات عامة للمساعدة في الأرقام العشوائية
    static std::random_device rd;
    static std::mt19937 gen(rd());

    ParticleSystem::ParticleSystem() 
        : m_emitting(false), m_emissionTimer(0), m_spawnCount(0) {}

    ParticleSystem::~ParticleSystem() = default;

    void ParticleSystem::init(const ParticleSystemConfig& config) {
        m_config = config;
        m_particles.resize(config.maxParticles);
        m_vertexBuffer.reserve(config.maxParticles * 4 * 2); // 4 vertices per particle, 2 coords
    }

    void ParticleSystem::emit() {
        m_emitting = true;
    }

    void ParticleSystem::stop() {
        m_emitting = false;
    }

    void ParticleSystem::update(float deltaTime) {
        if (m_emitting) {
            m_emissionTimer += deltaTime;
            float interval = 1.0f / m_config.emissionRate;
            
            while (m_emissionTimer >= interval) {
                spawnParticle();
                m_emissionTimer -= interval;
            }
        }

        // تحديث جميع الجسيمات النشطة
        for (auto& p : m_particles) {
            if (!p.active) continue;

            p.life -= deltaTime;
            if (p.life <= 0) {
                p.active = false;
                continue;
            }

            // تطبيق الفيزياء
            p.position.x += p.velocity.x * deltaTime;
            p.position.y += p.velocity.y * deltaTime;
            
            // الجاذبية
            p.velocity.y += m_config.gravity * deltaTime;

            // الدوران
            p.rotation += p.rotationSpeed * deltaTime;

            // تغيير الحجم واللون بناءً على العمر
            float lifeRatio = p.life / p.maxLife;
            
            // استيفاء اللون (Lerp)
            p.color.r = static_cast<uint8_t>(m_config.endColor.r + (m_config.startColor.r - m_config.endColor.r) * lifeRatio);
            p.color.g = static_cast<uint8_t>(m_config.endColor.g + (m_config.startColor.g - m_config.endColor.g) * lifeRatio);
            p.color.b = static_cast<uint8_t>(m_config.endColor.b + (m_config.startColor.b - m_config.endColor.b) * lifeRatio);
            p.color.a = static_cast<uint8_t>(m_config.endColor.a + (m_config.startColor.a - m_config.endColor.a) * lifeRatio);

            p.size = m_config.sizeMin + (m_config.sizeMax - m_config.sizeMin) * lifeRatio;

            // استدعاء الدالة المخصصة إذا وجدت
            if (m_updateCallback) {
                m_updateCallback(p, deltaTime);
            }
        }
    }

    void ParticleSystem::render() {
        glDisable(GL_TEXTURE_2D); // رسم مربعات ملونة بسيطة للتوضيح
        // يمكن تفعيل القوام هنا إذا توفرت
        
        glBegin(GL_QUADS);
        
        for (const auto& p : m_particles) {
            if (!p.active) continue;

            glColor4ub(p.color.r, p.color.g, p.color.b, p.color.a);

            float halfSize = p.size * 0.5f;
            
            // حساب الزوايا للدوران (تبسيط: رسم بدون دوران كامل للكود المختصر)
            // للتطبيق الكامل يجب استخدام مصفوفة دوران
            glVertex2f(p.position.x - halfSize, p.position.y - halfSize);
            glVertex2f(p.position.x + halfSize, p.position.y - halfSize);
            glVertex2f(p.position.x + halfSize, p.position.y + halfSize);
            glVertex2f(p.position.x - halfSize, p.position.y + halfSize);
        }
        
        glEnd();
        glEnable(GL_TEXTURE_2D);
    }

    void ParticleSystem::setUpdateCallback(ParticleUpdateCallback callback) {
        m_updateCallback = callback;
    }

    int ParticleSystem::getActiveCount() const {
        return std::count_if(m_particles.begin(), m_particles.end(), 
                             [](const Particle& p) { return p.active; });
    }

    void ParticleSystem::spawnParticle() {
        // إيجاد جسيم غير نشط لإعادة استخدامه
        for (auto& p : m_particles) {
            if (!p.active) {
                resetParticle(p);
                p.active = true;
                return;
            }
        }
        // إذا امتلأ النظام، لا نفعل شيئاً (أو يمكن توسيع المصفوفة ديناميكياً)
    }

    void ParticleSystem::resetParticle(Particle& p) {
        std::uniform_real_distribution<float> distSpread(-m_config.spread, m_config.spread);
        std::uniform_real_distribution<float> distSpeed(m_config.speedMin, m_config.speedMax);
        std::uniform_real_distribution<float> distLife(m_config.lifeMin, m_config.lifeMax);
        std::uniform_real_distribution<float> distSize(m_config.sizeMin, m_config.sizeMax);
        std::uniform_real_distribution<float> distRot(-1.0f, 1.0f);

        // موقع الانبعاث
        float offsetX = (m_config.emissionArea.x > 0) ? 
                        (std::uniform_real_distribution<float>(-m_config.emissionArea.x/2, m_config.emissionArea.x/2)(gen)) : 0;
        float offsetY = (m_config.emissionArea.y > 0) ? 
                        (std::uniform_real_distribution<float>(-m_config.emissionArea.y/2, m_config.emissionArea.y/2)(gen)) : 0;
        
        p.position = {offsetX, offsetY};

        // السرعة والاتجاه
        float angle = std::atan2(m_config.direction.y, m_config.direction.x) + distSpread(gen);
        float speed = distSpeed(gen);
        p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed};

        p.life = distLife(gen);
        p.maxLife = p.life;
        p.size = distSize(gen);
        p.rotation = 0;
        p.rotationSpeed = distRot(gen) * 2.0f; // سرعة دوران عشوائية
    }

} // namespace Omega
