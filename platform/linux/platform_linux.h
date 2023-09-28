#include "platform/platform.h"

#include <GLFW/glfw3.h>
#include "imgui.h"

class PlatformLinux : public Platform {
public:
    virtual Result<> init() override;
    virtual void runloop() override;
    virtual void cleanup() override;

    virtual void start_frame() override;
    virtual void render_frame() override;

    virtual bool app_menu_bar() override { return true; }
    virtual void copy_to_clipboard(const std::string_view& string) override;

private:
    GLFWwindow* p_window;
    ImVector<ImWchar> glyph_ranges;
    ImFontGlyphRangesBuilder glyphs;
    ImFont* p_font;
};