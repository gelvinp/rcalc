#pragma once

#ifdef __OBJC__

#include "core/error.h"

#include <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>


@interface _RCALC_MacOS_BindingImpl : NSObject {
    GLFWwindow* p_window;

    id <MTLDevice> device;
    id <MTLCommandQueue> commandQueue;

    NSWindow* p_native_window;
    CAMetalLayer* p_layer;
    MTLRenderPassDescriptor* p_render_pass_descriptor;

    id<CAMetalDrawable> drawable;
    id<MTLCommandBuffer> commandBuffer;
    id <MTLRenderCommandEncoder> renderEncoder;
}

- (instancetype) init;

- (Result<>) start;
- (void) stop;

- (void) beginFrame;
- (void) renderFrame;

- (bool) windowShouldClose;
- (void) copyToClipboard:(const char*)string;

@end


#endif