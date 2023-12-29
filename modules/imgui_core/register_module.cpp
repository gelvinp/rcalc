#include "register_module.h"

#include "imgui.h"
#include "core/memory/allocator.h"

void initialize_imgui_core_module() {
    ImGuiMemAllocFunc imgui_alloc = [](size_t size_bytes, void* p_udata) {
        return RCalc::Allocator::alloc(size_bytes);
    };
    ImGuiMemFreeFunc imgui_free = [](void* ptr, void* p_udata) {
        RCalc::Allocator::free(ptr);
    };
    
    ImGui::SetAllocatorFunctions(imgui_alloc, imgui_free);
};

void cleanup_imgui_core_module() {}