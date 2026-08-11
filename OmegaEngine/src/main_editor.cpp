#include "editor/EditorApp.h"
#include <FL/Fl.H>
#include "../core/Log.h"

int main(int argc, char* argv[]) {
    OMEGA_LOG_INFO("Starting Omega Engine Editor...");
    
    // تهيئة FLTK
    Fl::scheme("gtk+");
    
    // إنشاء نافذة المحرر
    Omega::Editor::EditorApp editor(1280, 720, "Omega Engine Editor - v1.0");
    
    // تشغيل المحرر
    editor.run();
    
    OMEGA_LOG_INFO("Omega Engine Editor closed.");
    
    return 0;
}
