#pragma once

#include "core/error.h"

namespace RCalc {

class Application;

class RenderBackend {
public:
    template<typename Renderer>
    static RenderBackend* create();
    
    virtual void copy_to_clipboard(const std::string_view& string) = 0;

    bool close_requested = false;
};

}