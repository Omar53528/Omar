# Hybrid 3D Game Engine

مشروع مختبر يجمع بين Python وC# لاستخدامهما معًا في محرك ألعاب 3D خفيف الوزن.

## الميزات الجديدة
- بنية محرك تعتمد على عقد المشهد Scene Node
- دعم كائنات أساسية مثل Player وEnemy وCube
- حفظ وتحميل المشاهد بصيغة JSON
- واجهة محرر مبسطة مع Tkinter
- تصميم خفيف مناسب للأجهزة المحدودة الذاكرة

## المتطلبات
- Python 3.8+
- .NET 8 SDK

## التشغيل
```bash
dotnet run --project csharp/Hybrid3DGame -- 2
```

للمحرر الجديد:
```bash
python python/engine_ui.py
```

أو على Windows:
- run_engine_editor.bat
- run_hybrid_engine.bat
- run_gui_game.bat
- run_raylib_game.bat
