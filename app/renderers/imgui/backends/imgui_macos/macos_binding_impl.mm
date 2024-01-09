#include "macos_binding_impl.h"

#include "core/logger.h"

#include "app/application.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_metal.h"

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>


static void glfw_error_callback(int error, const char* description) {
    RCalc::Logger::log_err("[GLFW] Error %d: %s\n", error, description);
}


@implementation _RCALC_MacOS_BindingImpl

- (instancetype) init {
    self = [super init];
    return self;
}

- (RCalc::Result<>) start {
    glfwSetErrorCallback(glfw_error_callback);

    glfwInitHint(GLFW_COCOA_MENUBAR, GLFW_FALSE); // We'll make our own

    if (!glfwInit()) {
        RCalc::Logger::log_err("Unable to initialize GLFW!");
        return RCalc::Err(RCalc::ERR_INIT_FAILURE);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);

    p_window = glfwCreateWindow(640, 480, "RCalc", nullptr, nullptr);
    if (p_window == nullptr) {
        RCalc::Logger::log_err("Unable to create window!");
        return RCalc::Err(RCalc::ERR_INIT_FAILURE);
    }

    device = MTLCreateSystemDefaultDevice();
    commandQueue = [device newCommandQueue];

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    ImGui_ImplGlfw_InitForOpenGL(p_window, true);
    ImGui_ImplMetal_Init(device);

    p_native_window = glfwGetCocoaWindow(p_window);
    p_layer = [CAMetalLayer layer];
    p_layer.device = device;
    p_layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    p_native_window.contentView.layer = p_layer;
    p_native_window.contentView.wantsLayer = YES;

    p_render_pass_descriptor = [MTLRenderPassDescriptor new];

    // Create menubar
    id menuBar = [NSMenu new];

    // App menu
    id appMenu = [NSMenu new];

    id settingsMenuItem = [[NSMenuItem alloc] initWithTitle: @"Settings" action:@selector(menuCallbackSettings) keyEquivalent:@","];
    id hideMenuItem = [[NSMenuItem alloc] initWithTitle: @"Hide RCalc" action:@selector(hide:) keyEquivalent:@"h"];
    id quitMenuItem = [[NSMenuItem alloc] initWithTitle: @"Quit RCalc" action:@selector(terminate:) keyEquivalent:@"q"];

    [settingsMenuItem setTarget:self];

    [appMenu addItem:settingsMenuItem];
    [appMenu addItem:hideMenuItem];
    [appMenu addItem:[NSMenuItem separatorItem]];
    [appMenu addItem:quitMenuItem];

    id appMenuItem = [NSMenuItem new];
    [appMenuItem setSubmenu:appMenu];

    // File menu
    id fileMenu = [[NSMenu alloc] initWithTitle: @"File"];

    id copyMenuItem = [[NSMenuItem alloc] initWithTitle: @"Copy Answer" action:@selector(menuCallbackCopy) keyEquivalent: @"c"];
    id duplicateMenuItem = [[NSMenuItem alloc] initWithTitle: @"Duplicate Item" action:@selector(menuCallbackDuplicate) keyEquivalent: @"d"];

    [copyMenuItem setTarget:self];
    [duplicateMenuItem setTarget:self];

    [fileMenu addItem:copyMenuItem];
    [fileMenu addItem:duplicateMenuItem];

    id fileMenuItem = [NSMenuItem new];
    [fileMenuItem setSubmenu:fileMenu];

    // Window menu
    id windowMenu = [[NSMenu alloc] initWithTitle: @"Window"];

    id minimizeMenuItem = [[NSMenuItem alloc] initWithTitle: @"Minimize RCalc" action:@selector(miniaturize:) keyEquivalent:@"m"];

    [windowMenu addItem:minimizeMenuItem];

    id windowMenuItem = [NSMenuItem new];
    [windowMenuItem setSubmenu:windowMenu];

    // Help menu
    id helpMenu = [[NSMenu alloc] initWithTitle: @"Help"];

    unichar f1 = NSF1FunctionKey;
    NSString* f1key = [NSString stringWithCharacters:&f1 length:1];
    id rcalcHelpMenuItem = [[NSMenuItem alloc] initWithTitle: @"RCalc Help" action:@selector(menuCallbackHelp) keyEquivalent:f1key];
    [rcalcHelpMenuItem setKeyEquivalentModifierMask:0];

    [rcalcHelpMenuItem setTarget:self];

    [helpMenu addItem:rcalcHelpMenuItem];

    id helpMenuItem = [NSMenuItem new];
    [helpMenuItem setSubmenu:helpMenu];

    [menuBar addItem:appMenuItem];
    [menuBar addItem:fileMenuItem];
    [menuBar addItem:windowMenuItem];
    [menuBar addItem:helpMenuItem];

    [NSApp setMainMenu:menuBar];

    NSDictionary* defaultPreferences = @{ @"Colors": @"System", @"UI_Scale": @1.0f, @"Precision": @8 };
    [[NSUserDefaults standardUserDefaults] registerDefaults:defaultPreferences];

    return RCalc::Ok();
}

- (void) stop {
    ImGui_ImplMetal_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
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

- (void) recreateFontAtlas {
    ImGui_ImplMetal_CreateFontsTexture(device);
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

- (bool) isDarkTheme {
    if (@available(macOS 10.14, *)) {
        if (![[NSUserDefaults standardUserDefaults] objectForKey:@"AppleInterfaceStyle"]) {
            return false;
        }
        else {
            return ([[[NSUserDefaults standardUserDefaults] stringForKey:@"AppleInterfaceStyle"] isEqual:@"Dark"]);
        }
    }
    else {
        return false;
    }
}

- (void) menuCallbackCopy {
    _submitTextCallback("\\copy");
}

- (void) menuCallbackDuplicate {
    _submitTextCallback("\\dup");
}

- (void) menuCallbackHelp {
    _submitTextCallback("\\help");
}

- (void) menuCallbackSettings {
    _submitTextCallback("\\settings");
}

@end