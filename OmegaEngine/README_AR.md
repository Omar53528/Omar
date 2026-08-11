# Omega Engine 🎮

محرك ألعاب متقدم ثنائي وثلاثي الأبعاد مبني بلغة C++ مع واجهة محرر شبيهة بـ Godot.

## ✨ الميزات

### الأساسية
- دعم 2D و 3D كامل
- لغة برمجة نصية مخصصة (.om)
- نظام كيانات ومكونات (ECS)
- ملفات مشاهد بامتداد .oms

### الرسوميات
- Renderer2D للرسومات ثنائية الأبعاد
- Renderer3D للرسومات ثلاثية الأبعاد
- دعم تحميل النماذج (glTF, FBX, OBJ)
- إدارة القوام والمواد
- إضاءة وظلال

### المحرر
- واجهة مستخدم كاملة بـ Qt
- Viewport تفاعلي
- Scene Hierarchy
- Properties Panel
- Project Browser
- Console Panel

### أنظمة إضافية
- فيزياء 2D (Box2D)
- صوت (SDL2_mixer)
- شبكات
- قاعدة بيانات SQLite3

## 🛠️ البناء على Windows MSYS2 UCRT64

### المتطلبات
```bash
pacman -S mingw-w64-ucrt-x86_64-toolchain
pacman -S mingw-w64-ucrt-x86_64-cmake
pacman -S mingw-w64-ucrt-x86_64-qt5
pacman -S mingw-w64-ucrt-x86_64-SDL2
pacman -S mingw-w64-ucrt-x86_64-SDL2_image
pacman -S mingw-w64-ucrt-x86_64-SDL2_mixer
pacman -S mingw-w64-ucrt-x86_64-sqlite3
pacman -S mingw-w64-ucrt-x86_64-box2d
```

### البناء
```bash
cd OmegaEngine
mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release -j4
```

## 📁 هيكل المشروع

```
OmegaEngine/
├── CMakeLists.txt
├── README_AR.md
├── include/
│   ├── core/          # الأنظمة الأساسية
│   ├── graphics/      # الرسوميات
│   ├── scripting/     # لغة OmegaScript
│   ├── scene/         # نظام المشاهد
│   ├── editor/        # واجهة المحرر
│   └── assets/        # إدارة الأصول
├── src/
│   ├── core/
│   ├── graphics/
│   ├── scripting/
│   ├── scene/
│   ├── editor/
│   └── tools/
├── examples/
└── assets/
```

## 📝 مثال بلغة OmegaScript (.om)

```om
// example.om
func start() {
    var score = 0
    print("Game Started!")
}

func update(dt) {
    if Input.isKeyPressed(Keys.Space) {
        player.jump()
    }
}
```

## 🔗 المكتبات المستخدمة

| المكتبة | الاستخدام |
|----------|-----------|
| SDL2 | النوافذ والإدخال |
| OpenGL | التصيير |
| Qt5/Qt6 | واجهة المحرر |
| SQLite3 | قاعدة البيانات |
| Box2D | الفيزياء 2D |

## 📄 الترخيص

MIT License
