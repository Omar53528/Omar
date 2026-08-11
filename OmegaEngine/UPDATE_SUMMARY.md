# Omega Engine - تحديثات متقدمة تم تنفيذها

## ✅ الملفات الجديدة المُضافة

### نظام المواد المتقدم (PBR Materials)
- `include/graphics/Material.h` - تعريف مواد PBR، القوام الإجرائية، والعقد البصرية
- `src/graphics/Material.cpp` - تطبيق توليد القوام ومدير المواد

### نظام الطبيعة والبيئة
- `include/graphics/Nature.h` - التضاريس، الأشجار، العشب، السماء، والضباب

### نظام التحريك السينمائي
- `include/graphics/Animation.h` - منحنيات التحريك، الهيكل العظمي، المزج، والكاميرا السينمائية
- `src/graphics/Animation.cpp` - تطبيق كامل لأنظمة التحريك

### الذكاء الاصطناعي المتقدم
- `include/ai/AI.h` - أشجار السلوك، الملاحة، محاكاة الحشود
- `src/ai/AI.cpp` - تطبيق كامل للذكاء الاصطناعي

## 🎯 الميزات الجديدة

### 1. نظام المواد المتقدم (PBR)
- مواد فيزيائية واقعية (Metallic, Roughness, AO, Emissive)
- توليد قوام إجرائية:
  - Checkerboard
  - Noise
  - Gradient
  - Normal Maps
- محرر عقد بصري للـ Shaders
- مدير مواد مع مواد جاهزة (Default, Metal, Glass)

### 2. نظام الطبيعة والبيئة
- **التضاريس**:_chunks_ ديناميكية مع خرائط الارتفاع
- **الأشجار**: زراعة فردية وجماعية مع LOD
- **العشب**: GPU Instancing لأداء عالي
- **السماء**: 
  - لون ثابت
  - تدرج لوني
  - سماء إجرائية
  - HDRI Skybox
- **الضباب**: Linear, Exponential, ExponentialSquared
- أدوات نحت التضاريس:
  - رفع/خفض
  - تسوية
  - تنعيم

### 3. نظام التحريك السينمائي
- **منحنيات التحريك**: Keyframes مع Interpolation (Linear, Constant, Bezier)
- **التحريك الهيكلي**: Skeleton, Skinned Mesh, Bone Transforms
- **Animator Controller**:
  - Blend Trees (1D, 2D, Additive)
  - Parameters (Float, Int, Bool, Trigger)
  - Fade between clips
- **الكاميرا السينمائية**:
  - Camera Keyframes
  - Interpolation (Linear, Smooth, EaseIn, EaseOut, EaseInOut)
  - FOV Animation

### 4. الذكاء الاصطناعي المتقدم
- **أشجار السلوك (Behavior Trees)**:
  - Composite Nodes (Sequence, Selector, Parallel)
  - Decorator Nodes (Inverter, Succeeder, Failer, Repeat)
  - Condition & Action Nodes
  - Blackboard لمشاركة البيانات
- **الملاحة (NavMesh)**:
  - بناء NavMesh من الهندسة
  - A* Pathfinding
  - NavAgents مع تحديد الأهداف
- **محاكاة الحشود (Crowd Simulation)**:
  - Steering Behaviors (Seek, Avoid, Separate, Align, Cohere)
  - تجنب العقبات
  - حركة جماعية واقعية
- **الإدراك الحسي (AI Perception)**:
  - Vision, Hearing, Touch, Smell
  - كشف الكيانات
  - تتبع آخر موقع معروف

## 📊 إحصائيات المشروع

| الفئة | العدد |
|-------|------|
| ملفات Header (.h) | 28+ |
| ملفات Source (.cpp) | 25+ |
| أنظمة كاملة | 10+ |
| أسطر الكود | 5000+ |

## 🔧 كيفية الاستخدام

### مثال: إنشاء مادة PBR
```cpp
PBRMaterial metalMat;
metalMat.albedo = Color(0.8f, 0.8f, 0.85f, 1.0f);
metalMat.metallic = 0.9f;
metalMat.roughness = 0.2f;
uint32_t matID = MaterialManager::Instance().CreateMaterial(metalMat);
```

### مثال: زراعة أشجار تلقائية
```cpp
NatureSystem::Instance().SetTerrainSize(1000.0f, 1000.0f);
NatureSystem::Instance().AutoPlantTrees(0.5f, 12345); // density, seed
NatureSystem::Instance().AutoPlantGrass(2.0f, 12345);
```

### مثال: شجرة سلوك للعدو
```cpp
auto tree = std::make_unique<BTSelector>();

// Chase or Attack
auto chaseSeq = std::make_unique<BTSequence>();
chaseSeq->children.push_back(std::make_unique<BTCondition>([](){ 
    return playerInRange(); 
}));
chaseSeq->children.push_back(std::make_unique<BTAction>([](float dt){
    moveToPlayer(dt);
    return BTNodeStatus::Running;
}));

tree->children.push_back(std::move(chaseSeq));

// Patrol
tree->children.push_back(std::make_unique<BTAction>([](float dt){
    patrol(dt);
    return BTNodeStatus::Running;
}));

enemyBehaviorTree.SetRoot(std::move(tree));
```

### مثال: تحريك سينمائي
```cpp
CinematicCamera cam;

CameraKeyframe kf1;
kf1.time = 0.0f;
kf1.position = Vector3(0, 10, -20);
kf1.lookAt = Vector3(0, 0, 0);
kf1.fov = 60.0f;

CameraKeyframe kf2;
kf2.time = 5.0f;
kf2.position = Vector3(20, 5, 0);
kf2.lookAt = Vector3(0, 0, 0);
kf2.fov = 45.0f;

cam.AddKeyframe(kf1);
cam.AddKeyframe(kf2);
cam.Play(5.0f, true); // loop
```

## 🚀 البناء على Windows MSYS2 UCRT64

```bash
cd OmegaEngine
mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release -j4
```

## 📦 المكتبات المطلوبة

```bash
pacman -S mingw-w64-ucrt-x86_64-toolchain
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-SDL2
pacman -S mingw-w64-ucrt-x86_64-SDL2_image
pacman -S mingw-w64-ucrt-x86_64-SDL2_mixer
pacman -S mingw-w64-ucrt-x86_64-sqlite3
pacman -S mingw-w64-ucrt-x86_64-box2d
pacman -S mingw-w64-ucrt-x86_64-qt5  # للمحرر
```

---

**Omega Engine** - محرك ألعاب متكامل بخفة وزن ومناسب للأجهزة المحدودة!
