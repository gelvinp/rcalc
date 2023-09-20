#include "platform/platform.h"

#include <GLFW/glfw3.h>

class PlatformLinux : public Platform {
public:
    virtual Result<> init() override;
    virtual void start_frame() override;
    virtual void render_frame() override;
    virtual void cleanup() override;

    virtual void copy_to_clipboard(const std::string& string) override;

private:
    GLFWwindow* p_window;
};