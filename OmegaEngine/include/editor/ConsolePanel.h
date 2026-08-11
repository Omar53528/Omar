#ifndef CONSOLE_PANEL_H
#define CONSOLE_PANEL_H

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <string>

namespace Omega { namespace Editor {

/**
 * @brief لوحة السجل والأخطاء باستخدام FLTK
 */
class ConsolePanel : public Fl_Group {
public:
    ConsolePanel(int x, int y, int w, int h);
    ~ConsolePanel();

    void logInfo(const std::string& msg);
    void logWarning(const std::string& msg);
    void logError(const std::string& msg);
    void clear();

private:
    void appendLog(const std::string& prefix, const std::string& msg, const char* color);

    Fl_Text_Display* m_display;
    Fl_Text_Buffer* m_buffer;
};

}} // namespace Omega::Editor

#endif // CONSOLE_PANEL_H
