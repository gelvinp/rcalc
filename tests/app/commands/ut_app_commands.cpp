#include "snitch/snitch.hpp"
#include "app/application.h"
#include "app/renderers/dummy/dummy_renderer.h"

using namespace RCalc;


TEST_CASE("Clear command", "[app][commands]") {
    Application app;
    app.early_init({ .renderer_name = "dummy" });
    app.init();

    app.on_renderer_submit_text("123");
    REQUIRE(app.get_stack().get_items().size() == 1);

    app.on_renderer_submit_text("\\clear");
    REQUIRE(app.get_stack().get_items().size() == 0);
}


TEST_CASE("Pop command", "[app][commands]") {
    Application app;
    app.early_init({ .renderer_name = "dummy" });
    app.init();

    app.on_renderer_submit_text("123");
    app.on_renderer_submit_text("456");
    REQUIRE(app.get_stack().get_items().size() == 2);
    REQUIRE(app.get_stack().get_items()[0].result == Value { (Int)123 });
    REQUIRE(app.get_stack().get_items()[1].result == Value { (Int)456 });

    app.on_renderer_submit_text("\\pop");
    REQUIRE(app.get_stack().get_items().size() == 1);
    REQUIRE(app.get_stack().get_items()[0].result == Value { (Int)123 });
}


TEST_CASE("Swap command", "[app][commands]") {
    Application app;
    app.early_init({ .renderer_name = "dummy" });
    app.init();

    app.on_renderer_submit_text("123");
    app.on_renderer_submit_text("456");
    app.on_renderer_submit_text("789");
    REQUIRE(app.get_stack().get_items().size() == 3);
    REQUIRE(app.get_stack().get_items()[0].result == Value { (Int)123 });
    REQUIRE(app.get_stack().get_items()[1].result == Value { (Int)456 });
    REQUIRE(app.get_stack().get_items()[2].result == Value { (Int)789 });

    app.on_renderer_submit_text("\\swap");
    REQUIRE(app.get_stack().get_items().size() == 3);
    REQUIRE(app.get_stack().get_items()[0].result == Value { (Int)123 });
    REQUIRE(app.get_stack().get_items()[1].result == Value { (Int)789 });
    REQUIRE(app.get_stack().get_items()[2].result == Value { (Int)456 });
}


TEST_CASE("Duplicate command", "[app][commands]") {
    Application app;
    app.early_init({ .renderer_name = "dummy" });
    app.init();
    const DummyRenderer& renderer = *((const DummyRenderer*)(app.get_renderer()));

    app.on_renderer_submit_text("123");
    REQUIRE(app.get_stack().get_items().size() == 1);

    app.on_renderer_submit_text("\\dup");
    REQUIRE(app.get_stack().get_items().size() == 2);
    REQUIRE(app.get_stack().get_items()[0].result == Value { (Int)123 });
    REQUIRE(app.get_stack().get_items()[1].result == Value { (Int)123 });

    app.set_max_stack_size(2);
    app.on_renderer_submit_text("\\dup");
    REQUIRE(app.get_stack().get_items().size() == 2);
    REQUIRE(renderer.last_message_was_error);
}


TEST_CASE("Count command", "[app][commands]") {
    Application app;
    app.early_init({ .renderer_name = "dummy" });
    app.init();
    const DummyRenderer& renderer = *((const DummyRenderer*)(app.get_renderer()));

    app.on_renderer_submit_text("123");
    app.on_renderer_submit_text("456");
    REQUIRE(app.get_stack().get_items().size() == 2);
    REQUIRE(app.get_stack().get_items()[0].result == Value { (Int)123 });
    REQUIRE(app.get_stack().get_items()[1].result == Value { (Int)456 });

    app.on_renderer_submit_text("\\count");
    REQUIRE(app.get_stack().get_items().size() == 3);
    REQUIRE(app.get_stack().get_items()[0].result == Value { (Int)123 });
    REQUIRE(app.get_stack().get_items()[1].result == Value { (Int)456 });
    REQUIRE(app.get_stack().get_items()[2].result == Value { (Int)2 });

    app.set_max_stack_size(3);
    app.on_renderer_submit_text("\\count");
    REQUIRE(app.get_stack().get_items().size() == 3);
    REQUIRE(renderer.last_message_was_error);
}


TEST_CASE("Undo command", "[app][commands]") {
    Application app;
    app.early_init({ .renderer_name = "dummy" });
    app.init();

    app.on_renderer_submit_text("123");
    app.on_renderer_submit_text("456");
    app.on_renderer_submit_text("789");
    REQUIRE(app.get_stack().get_items().size() == 3);
    REQUIRE(app.get_stack().get_items()[0].result == Value { (Int)123 });
    REQUIRE(app.get_stack().get_items()[1].result == Value { (Int)456 });
    REQUIRE(app.get_stack().get_items()[2].result == Value { (Int)789 });

    app.on_renderer_submit_text("\\clear");
    REQUIRE(app.get_stack().get_items().size() == 0);

    app.on_renderer_submit_text("\\undo");
    REQUIRE(app.get_stack().get_items().size() == 3);
    REQUIRE(app.get_stack().get_items()[0].result == Value { (Int)123 });
    REQUIRE(app.get_stack().get_items()[1].result == Value { (Int)456 });
    REQUIRE(app.get_stack().get_items()[2].result == Value { (Int)789 });
}


TEST_CASE("Type command", "[app][commands]") {
    Application app;
    app.early_init({ .renderer_name = "dummy" });
    app.init();
    const DummyRenderer& renderer = *((const DummyRenderer*)(app.get_renderer()));

    app.on_renderer_submit_text("123");
    app.on_renderer_submit_text("\\type");
    REQUIRE(renderer.last_message == "Type: Int");
    REQUIRE_FALSE(renderer.last_message_was_error);

    app.on_renderer_submit_text("123456789876543212345678987654321");
    app.on_renderer_submit_text("\\type");
    REQUIRE(renderer.last_message == "Type: BigInt");
    REQUIRE_FALSE(renderer.last_message_was_error);

    app.on_renderer_submit_text("[1, 2]");
    app.on_renderer_submit_text("\\type");
    REQUIRE(renderer.last_message == "Type: Vec2");
    REQUIRE_FALSE(renderer.last_message_was_error);

    app.on_renderer_submit_text("[1, 2, 3]");
    app.on_renderer_submit_text("\\type");
    REQUIRE(renderer.last_message == "Type: Vec3");
    REQUIRE_FALSE(renderer.last_message_was_error);

    app.on_renderer_submit_text("[1, 2, 3, 4]");
    app.on_renderer_submit_text("\\type");
    REQUIRE(renderer.last_message == "Type: Vec4");
    REQUIRE_FALSE(renderer.last_message_was_error);

    app.on_renderer_submit_text("{[1, 2], [3, 4]}");
    app.on_renderer_submit_text("\\type");
    REQUIRE(renderer.last_message == "Type: Mat2");
    REQUIRE_FALSE(renderer.last_message_was_error);

    app.on_renderer_submit_text("{[1, 2, 3], [4, 5, 6], [7, 8, 9]}");
    app.on_renderer_submit_text("\\type");
    REQUIRE(renderer.last_message == "Type: Mat3");
    REQUIRE_FALSE(renderer.last_message_was_error);

    app.on_renderer_submit_text("{[1, 2, 3, 4], [5, 6, 7, 8], [9, 10, 11, 12], [13, 14, 15, 16]}");
    app.on_renderer_submit_text("\\type");
    REQUIRE(renderer.last_message == "Type: Mat4");
    REQUIRE_FALSE(renderer.last_message_was_error);

    app.on_renderer_submit_text("_m");
    app.on_renderer_submit_text("\\type");
    REQUIRE(renderer.last_message == "Type: Unit");
    REQUIRE_FALSE(renderer.last_message_was_error);

    app.on_renderer_submit_text("\"abc\"");
    app.on_renderer_submit_text("\\type");
    REQUIRE(renderer.last_message == "Type: Identifier");
    REQUIRE_FALSE(renderer.last_message_was_error);
}


TEST_CASE("Variable commands", "[app][commands]") {
    Application app;
    app.early_init({ .renderer_name = "dummy" });
    app.init();
    const DummyRenderer& renderer = *((const DummyRenderer*)(app.get_renderer()));

    app.on_renderer_submit_text("123");
    app.on_renderer_submit_text("\"abc\"");
    app.on_renderer_submit_text("\\store");
    
    {
        const CowVec<std::pair<std::string, Value>>& values = app.get_variable_map().get_values();
        REQUIRE(values.size() == 1);
        REQUIRE(values[0].first == "abc");
        REQUIRE(values[0].second == Value { (Int)123 });
    }

    app.on_renderer_submit_text("\\clear");
    REQUIRE(app.get_stack().get_items().size() == 0);

    app.on_renderer_submit_text("\"abc\"");
    app.on_renderer_submit_text("\\load");
    REQUIRE(app.get_stack().get_items().size() == 1);
    REQUIRE(app.get_stack().get_items()[0].result == Value { (Int)123 });

    app.on_renderer_submit_text("\\clearvars");
    REQUIRE(app.get_variable_map().get_values().size() == 0);

    app.on_renderer_submit_text("\\clear");
    REQUIRE(app.get_stack().get_items().size() == 0);

    app.on_renderer_submit_text("\"abc\"");
    app.on_renderer_submit_text("\\load");
    REQUIRE(app.get_stack().get_items().size() == 1);
    REQUIRE(app.get_stack().get_items()[0].result == Value { "abc" });
    REQUIRE(renderer.last_message_was_error);
}