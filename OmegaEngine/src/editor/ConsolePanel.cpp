#include "editor/ConsolePanel.h"
#include <FL/fl_draw.H>
#include <cstdio>

namespace Omega { namespace Editor {

ConsolePanel::ConsolePanel(int x, int y, int w, int h) : Fl_Group(x, y, w, h) {
    this->begin();
    
    m_buffer = new Fl_Text_Buffer();
    m_display = new Fl_Text_Display(x, y + 25, w, h - 25);
    m_display->buffer(m_buffer);
    m_display->textsize(12);
    m_display->textfont(FL_COURIER);
    
    this->end();
}

ConsolePanel::~ConsolePanel() {
    delete m_buffer;
}

void ConsolePanel::logInfo(const std::string& msg) {
    appendLog("[INFO] ", msg, "black");
}

void ConsolePanel::logWarning(const std::string& msg) {
    appendLog("[WARN] ", msg, "orange");
}

void ConsolePanel::logError(const std::string& msg) {
    appendLog("[ERROR] ", msg, "red");
}

void ConsolePanel::clear() {
    m_buffer->text("");
}

void ConsolePanel::appendLog(const std::string& prefix, const std::string& msg, const char* color) {
    std::string line = prefix + msg + "\n";
    m_buffer->append(line.c_str());
    
    // التمرير للأسفل تلقائياً
    int lines = m_display->count_lines(0, m_buffer->length());
    m_display->scroll(lines, 0);
}

}} // namespace Omega::Editor
