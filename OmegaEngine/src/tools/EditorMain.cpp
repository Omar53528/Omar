#include "editor/EditorWindow.h"
#include <QApplication>
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
    // Initialize SDL for subsystems
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        return -1;
    }
    
    QApplication app(argc, argv);
    QApplication::setStyle("Fusion");
    
    Omega::EditorWindow editor;
    editor.initialize();
    editor.show();
    
    int result = app.exec();
    
    editor.shutdown();
    SDL_Quit();
    
    return result;
}
