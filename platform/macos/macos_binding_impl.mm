#include "macos_binding_impl.h"

#include "core/logger.h"

#include "app/application.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_metal.h"

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>


static void glfw_error_callback(int error, const char* description) {
    Logger::log_err("[GLFW] Error %d: %s\n", error, description);
}


@implementation _RCALC_MacOS_BindingImpl

- (instancetype) init {
    self = [super init];
    return self;
}

- (Result<>) start {
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        Logger::log_err("Unable to initialize GLFW!");
        return Err(ERR_INIT_FAILURE);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);

    p_window = glfwCreateWindow(640, 480, "RCalc", nullptr, nullptr);
    if (p_window == nullptr) {
        Logger::log_err("Unable to create window!");
        return Err(ERR_INIT_FAILURE);
    }

    device = MTLCreateSystemDefaultDevice();
    commandQueue = [device newCommandQueue];

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(p_window, true);
    ImGui_ImplMetal_Init(device);

    p_native_window = glfwGetCocoaWindow(p_window);
    p_layer = [CAMetalLayer layer];
    p_layer.device = device;
    p_layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    p_native_window.contentView.layer = p_layer;
    p_native_window.contentView.wantsLayer = YES;

    p_render_pass_descriptor = [MTLRenderPassDescriptor new];

    return Ok();
}

- (void) stop {
    glfwDestroyWindow(p_window);
    glfwTerminate();
}

- (void) beginFrame {
    glfwPollEvents();

    int width, height;
    glfwGetFramebufferSize(p_window, &width, &height);
    p_layer.drawableSize = CGSizeMake(width, height);
    drawable = [p_layer nextDrawable];

    commandBuffer = [commandQueue commandBuffer];
    p_render_pass_descriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);
    p_render_pass_descriptor.colorAttachments[0].texture = drawable.texture;
    p_render_pass_descriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    p_render_pass_descriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:p_render_pass_descriptor];
    [renderEncoder pushDebugGroup:@"RCalc"];

    ImGui_ImplMetal_NewFrame(p_render_pass_descriptor);
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

- (void) renderFrame {
    ImGui::Render();
    ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);

    [renderEncoder popDebugGroup];
    [renderEncoder endEncoding];

    [commandBuffer presentDrawable:drawable];
    [commandBuffer commit];

    [drawable release];
    [commandBuffer release];
    [renderEncoder release];
}

- (bool) windowShouldClose {
    return glfwWindowShouldClose(p_window);
}

- (void) copyToClipboard:(const char*)string {
    glfwSetClipboardString(p_window, string);
}

- (CGFloat) getScreenDPI {
    return p_native_window.screen.backingScaleFactor;
}

@end