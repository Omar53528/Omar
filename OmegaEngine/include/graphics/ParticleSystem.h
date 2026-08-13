#pragma once

#include "core/Types.h"
#include <vector>
#include <string>
#include <functional>
#include <memory>

namespace Omega {

    /**
     * @brief جسيم فردي في النظام
     */
    struct Particle {
        Vector2 position;
        Vector2 velocity;
        Color color;
        float life;          // العمر الحالي
        float maxLife;       // العمر الأقصى
        float size;
        float rotation;
        float rotationSpeed;
        
        bool active;

        Particle() : active(false) {}
    };

    /**
     * @brief إعدادات نظام الجسيمات
     */
    struct ParticleSystemConfig {
        std::string texturePath; // مسار القوام (اختياري)
        int maxParticles = 1000;
        Vector2 emissionArea = {0, 0}; // مساحة الانبعاث (0,0 لنقطة)
        Vector2 direction = {0, -1};   // اتجاه الانبعاث الافتراضي
        float spread = 0.5f;           // انتشار الاتجاه
        float speedMin = 10.0f;
        float speedMax = 50.0f;
        float lifeMin = 1.0f;
        float lifeMax = 3.0f;
        float sizeMin = 5.0f;
        float sizeMax = 15.0f;
        Color startColor = {255, 255, 255, 255};
        Color endColor = {255, 255, 255, 0};
        float gravity = 0.0f;
        bool loop = true;
        float emissionRate = 10.0f; // جسيمات في الثانية
    };

    /**
     * @brief نظام جسيمات متقدم للرسوميات 2D و 3D
     * يدعم المحاكاة على CPU مع إمكانية التطوير لـ GPU
     */
    class ParticleSystem {
    public:
        ParticleSystem();
        ~ParticleSystem();

        /**
         * @brief تهيئة النظام بالإعدادات المحددة
         */
        void init(const ParticleSystemConfig& config);

        /**
         * @brief بدء الانبعاث
         */
        void emit();

        /**
         * @brief إيقاف الانبعاث
         */
        void stop();

        /**
         * @brief تحديث حالة الجسيمات (يستدعى كل إطار)
         * @param deltaTime الوقت منذ الإطار السابق
         */
        void update(float deltaTime);

        /**
         * @brief رسم الجسيمات النشطة
         * يفترض أن يكون OpenGL Context نشطاً
         */
        void render();

        /**
         * @brief إضافة دالة مخصصة لتعديل سلوك الجسيمات
         */
        using ParticleUpdateCallback = std::function<void(Particle&, float)>;
        void setUpdateCallback(ParticleUpdateCallback callback);

        bool isEmitting() const { return m_emitting; }
        int getActiveCount() const;

    private:
        void spawnParticle();
        void resetParticle(Particle& p);

        std::vector<Particle> m_particles;
        ParticleSystemConfig m_config;
        
        bool m_emitting;
        float m_emissionTimer;
        int m_spawnCount;

        ParticleUpdateCallback m_updateCallback;
        
        // مخزن مؤقت للرسم (يمكن تحسينه بـ VBO)
        std::vector<float> m_vertexBuffer;
    };

} // namespace Omega
