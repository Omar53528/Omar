cmake_minimum_required(VERSION 3.16)
project(OmegaEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# تحسينات للأجهزة محدودة الموارد (4GB RAM) - استبدال Qt بـ FLTK
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -ffunction-sections -fdata-sections")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections -s")
endif()

# العثور على المكتبات المطلوبة (FLTK بدلاً من Qt)
find_package(FLTK REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SQLite3 REQUIRED)

# مكتبات اختيارية
find_package(Box2D QUIET)
find_package(Assimp QUIET)
find_package(SDL2_image QUIET)
find_package(SDL2_mixer QUIET)

# مكتبات Windows الإضافية
if(WIN32)
    set(EXTRA_LIBS ws2_32 winmm imm32 oleaut32 uuid comctl32)
else()
    set(EXTRA_LIBS "")
endif()

# تحديد الملفات المصدرية للمحرك الأساسي
set(ENGINE_SOURCES
    # Core
    src/core/Log.cpp
    src/core/Time.cpp
    src/core/Window.cpp
    src/core/Input.cpp
    src/core/FileSystem.cpp
    src/core/Database.cpp
    
    # Graphics
    src/graphics/Renderer2D.cpp
    src/graphics/Renderer3D.cpp
    src/graphics/Camera2D.cpp
    src/graphics/Camera3D.cpp
    src/graphics/Shader.cpp
    src/graphics/Texture.cpp
    src/graphics/Mesh.cpp
    src/graphics/Material.cpp
    src/graphics/ParticleSystem.cpp
    src/graphics/Animation.cpp
    
    # Scripting
    src/scripting/OMParser.cpp
    src/scripting/OMCompiler.cpp
    
    # Scene & ECS
    src/scene/Scene.cpp
    src/scene/Entity.cpp
    src/scene/Component.cpp
    src/scene/SceneManager.cpp
    
    # Assets
    src/assets/AssetManager.cpp
    
    # AI
    src/ai/AI.cpp
    
    # Editor (FLTK)
    src/editor/EditorApp.cpp
    src/editor/EditorWindow.cpp
    src/editor/MainMenuBar.cpp
    src/editor/ViewportGL.cpp
    src/editor/Viewport.cpp
    src/editor/SceneTreePanel.cpp
    src/editor/PropertiesPanel.cpp
    src/editor/ConsolePanel.cpp
    src/editor/Toolbar.cpp
    src/editor/SceneHierarchy.cpp
    src/editor/ProjectBrowser.cpp
)

# إنشاء مكتبة المحرك
add_library(OmegaEngine STATIC ${ENGINE_SOURCES})

target_include_directories(OmegaEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${FLTK_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${SQLite3_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
)

target_link_libraries(OmegaEngine PUBLIC
    ${FLTK_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${SQLite3_LIBRARIES}
    ${EXTRA_LIBS}
)

if(BOX2D_FOUND)
    target_link_libraries(OmegaEngine PUBLIC Box2D::Box2D)
    target_compile_definitions(OmegaEngine PUBLIC OMEGA_PHYSICS_2D)
endif()

if(ASSIMP_FOUND)
    target_link_libraries(OmegaEngine PUBLIC assimp)
    target_compile_definitions(OmegaEngine PUBLIC OMEGA_MODEL_SUPPORT)
endif()

if(SDL2_IMAGE_FOUND)
    target_link_libraries(OmegaEngine PUBLIC SDL2_image)
    target_compile_definitions(OmegaEngine PUBLIC OMEGA_IMAGE_SUPPORT)
endif()

if(SDL2_MIXER_FOUND)
    target_link_libraries(OmegaEngine PUBLIC SDL2_mixer)
    target_compile_definitions(OmegaEngine PUBLIC OMEGA_AUDIO_SUPPORT)
endif()

target_compile_definitions(OmegaEngine PUBLIC OMEGA_EDITOR_ENABLED)

# أداة ترجمة لغة OmegaScript (.om)
add_executable(OMCompiler src/tools/OMCompilerMain.cpp)
target_link_libraries(OMCompiler PRIVATE OmegaEngine)

# أداة حزم الألعاب (Omega Packager)
add_executable(OmegaPackager src/tools/PackagerTool.cpp)
target_link_libraries(OmegaPackager PRIVATE OmegaEngine)

# تطبيق المحرر بـ FLTK
add_executable(OmegaEditor WIN32 src/main_editor.cpp)
target_link_libraries(OmegaEditor PRIVATE OmegaEngine)

# مثال لعبة تجريبية
add_executable(DemoGame examples/demo_game.cpp)
target_link_libraries(DemoGame PRIVATE OmegaEngine)

# تثبيت الملفات
install(TARGETS OmegaEngine OMCompiler OmegaPackager OmegaEditor DemoGame
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY assets/ DESTINATION share/omegaengine/assets)

message(STATUS "Omega Engine Configuration:")
message(STATUS "  - Editor GUI: FLTK (Lightweight)")
message(STATUS "  - Physics 2D: ${BOX2D_FOUND}")
message(STATUS "  - Model Support: ${ASSIMP_FOUND}")
message(STATUS "  - Image Support: ${SDL2_IMAGE_FOUND}")
message(STATUS "  - Audio Support: ${SDL2_MIXER_FOUND}")
message(STATUS "  - Particle System: Enabled")
message(STATUS "  - Package System: Enabled (std::filesystem)")
message(STATUS "  - AI System: Enabled")
message(STATUS "  - Animation System: Enabled")
