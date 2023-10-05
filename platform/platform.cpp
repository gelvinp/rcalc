#include "platform.h"

#include <stdexcept>


Result<> Platform::create_application(RCalc::AppConfig config) {
    if (p_application) {
        throw std::logic_error("Cannot create two applications!");
    }

    Result<RCalc::Application*> app_res = RCalc::Application::create(config);

    if (app_res) {
        p_application = app_res.unwrap();
        return Ok();
    }
    else {
        return app_res.unwrap_err();
    }
}


Platform::~Platform() {
    if (p_application) {
        delete p_application;
    }
}