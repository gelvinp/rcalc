#include "snitch/snitch.hpp"
#include "core/units/units.h"
#include "core/comparison.h"
#include "core/logger.h"
#include "core/value.h"
#include "tests/test_serializers.h"

#include <numbers>

using namespace RCalc;
using namespace RCalc::TypeComparison;

namespace {

template<typename T>
void roundtrip(Unit* p_unit);

template<>
void roundtrip<Real>(Unit* p_unit) {
    const UnitImpl<Real>& impl = p_unit->get_impl<Real>();

    constexpr std::array<Real, 20> values = {
        1.0,
        0.0,
        -1.0,
        100.0,
        1000.0,
        1000000.0,
        1.0000001234,
        10000000.1234,
        10000000.0000001234,
        -100.0,
        -1000.0,
        -1000000.0,
        -1.0000001234,
        -10000000.1234,
        -10000000.0000001234,
        13245678987654321.0,
        0.12345678987654321,
        847240.8019643391,
        -380903.084273644,
        349488.8151081577
    };

    for (Real value : values) {
        CAPTURE(value);
        Result<Real> roundtripped = impl.to_base(value).and_then<Real>(impl.from_base);
        if (roundtripped) {
            // Not all of those values might be valid for every unit
            CAPTURE(roundtripped.unwrap());
            REQUIRE(compare(value, roundtripped.unwrap()));
        }
    }
}

template<>
void roundtrip<Vec2>(Unit* p_unit) {
    const UnitImpl<Vec2>& impl = p_unit->get_impl<Vec2>();

    constexpr std::array<Vec2, 20> values = {
        Vec2 { 0.0 },
        Vec2 { 1.0 },
        Vec2 { -1.0 },
        Vec2 { 0.7764245681637996, 0.6599854050860513 },
        Vec2 { 0.009476016649513586, 0.6726574243996938 },
        Vec2 { 0.7634725547288033, 0.6535456358302066 },
        Vec2 { 0.47008809034754717, 0.1119476214038655 },
        Vec2 { 0.964174422699477, 0.002655336790813645 },
        Vec2 { 216.68093936526327, 129.57314260286816 },
        Vec2 { 71.0497277939522, 127.87953203217684 },
        Vec2 { 66.14822469113886, 229.97364859356165 },
        Vec2 { 132.26619575359646, 233.82289902177357 },
        Vec2 { 149.3905425838206, 204.05720973539778 },
        Vec2 { -470228.1263467991, -950093.3752326814 },
        Vec2 { 938319.7166170846, -217152.83355726837 },
        Vec2 { 355910.8627065772, 865372.024343994 },
        Vec2 { -377412.17451474676, 56547.36211794312 },
        Vec2 { -403684.3389505369, -196345.34213501203 },
        Vec2 { -734888.7743354993, -108191.15061534208 },
        Vec2 { -188619.65711516887, -22840.750472560525 }
    };

    for (Vec2 value : values) {
        CAPTURE(value);
        Result<Vec2> roundtripped = impl.to_base(value).and_then<Vec2>(impl.from_base);
        if (roundtripped) {
            // Not all of those values might be valid for every unit
            CAPTURE(roundtripped.unwrap()); 
            REQUIRE(compare(value, roundtripped.unwrap()));
        }
    }
}

template<>
void roundtrip<Vec3>(Unit* p_unit) {
    const UnitImpl<Vec3>& impl = p_unit->get_impl<Vec3>();

    constexpr std::array<Vec3, 20> values = {
        Vec3 { 0.0 },
        Vec3 { 1.0 },
        Vec3 { -1.0 },
        Vec3 { 0.7764245681637996, 0.6599854050860513, 0.5908836121278814 },
        Vec3 { 0.009476016649513586, 0.6726574243996938, 0.16465326362018062 },
        Vec3 { 0.7634725547288033, 0.6535456358302066, 0.09991122147208875 },
        Vec3 { 0.47008809034754717, 0.1119476214038655, 0.8758533655989874 },
        Vec3 { 0.964174422699477, 0.002655336790813645, 0.3317434507124971 },
        Vec3 { 216.68093936526327, 129.57314260286816, 198.60220399680492 },
        Vec3 { 71.0497277939522, 127.87953203217684, 138.11249608284146 },
        Vec3 { 66.14822469113886, 229.97364859356165, 44.801735951414116 },
        Vec3 { 132.26619575359646, 233.82289902177357, 22.86677928460439 },
        Vec3 { 149.3905425838206, 204.05720973539778, 81.13925751437797 },
        Vec3 { -470228.1263467991, -950093.3752326814, 569422.3605430408 },
        Vec3 { 938319.7166170846, -217152.83355726837, 177274.84254226298 },
        Vec3 { 355910.8627065772, 865372.024343994, -148121.5139950054 },
        Vec3 { -377412.17451474676, 56547.36211794312, -622914.190788927 },
        Vec3 { -403684.3389505369, -196345.34213501203, -948019.2471256928 },
        Vec3 { -734888.7743354993, -108191.15061534208, -580974.9800735129 },
        Vec3 { -188619.65711516887, -22840.750472560525, 281832.85914544994 }
    };

    for (Vec3 value : values) {
        CAPTURE(value);
        Result<Vec3> roundtripped = impl.to_base(value).and_then<Vec3>(impl.from_base);
        if (roundtripped) {
            // Not all of those values might be valid for every unit
            CAPTURE(roundtripped.unwrap());
            REQUIRE(compare(value, roundtripped.unwrap()));
        }
    }
}

template<>
void roundtrip<Vec4>(Unit* p_unit) {
    const UnitImpl<Vec4>& impl = p_unit->get_impl<Vec4>();

    constexpr std::array<Vec4, 20> values = {
        Vec4 { 0.0 },
        Vec4 { 1.0 },
        Vec4 { -1.0 },
        Vec4 { 0.27261383249253834, 0.9477482228880327, 0.9649779375339164, 0.38159450895636116 },
        Vec4 { 0.8609254204399263, 0.27425310900233113, 0.21097799191655253, 0.8022562371413853 },
        Vec4 { 0.25431891876834645, 0.31109254469012027, 0.5974752191631924, 0.140767684518609 },
        Vec4 { 0.6825483187838564, 0.2228557460721935, 0.5194071157433777, 0.6565736286154654 },
        Vec4 { 0.4121756400273874, 0.47561485645662405, 0.8707542052773354, 0.13074869474753692 },
        Vec4 { 73.00160337590567, 115.69683307771096, 60.08796414034978, 101.37282546502774 },
        Vec4 { 81.84482286840948, 19.53495149750452, 158.5013225618764, 207.187297604656 },
        Vec4 { 202.58980233772724, 61.2655296883374, 195.620240269528, 221.861175275404 },
        Vec4 { 112.68504213039627, 52.582615234856384, 181.7971184109144, 43.691057396767896 },
        Vec4 { 100.64218482127639, 242.6508361062558, 38.4426287192963, 64.25689481161446 },
        Vec4 { -930877.9671258889, 312971.6698453757, 510982.48448803974, 438632.88467574026 },
        Vec4 { -610869.9534002184, -346367.0068618567, -533132.7508273817, 933134.4533928647 },
        Vec4 { -556744.3185540766, 589810.4716542559, -798696.4001934036, 618353.184513418 },
        Vec4 { -304773.21537508466, -984156.4189792653, -939704.1664438372, -189187.42374936026 },
        Vec4 { -88135.3021235012, 978871.9675037658, -16938.019525642507, -563731.517204857 },
        Vec4 { -668409.7700096179, 311303.2004102967, 422721.8696767646, 847247.2335387378 },
        Vec4 { -634684.6462112241, -381855.2072627046, -25397.052800783888, 36721.788480425836 }
    };

    for (Vec4 value : values) {
        CAPTURE(value);
        Result<Vec4> roundtripped = impl.to_base(value).and_then<Vec4>(impl.from_base);
        if (roundtripped) {
            // Not all of those values might be valid for every unit
            CAPTURE(roundtripped.unwrap());
            REQUIRE(compare(value, roundtripped.unwrap()));
        }
    }
}

#ifdef GPERF_ENABLED // Non-gperf builds use std-map
TEST_CASE("Unit Roundtrip", "[core][units]") {
#else
TEST_CASE("Unit Roundtrip", "[core][units][allocates]") {
#endif
    // Make sure every unit can convert a value from itself to the base unit and back while maintaining the same value

    UnitsMap& map = UnitsMap::get_units_map();
    map.build();

    for (UnitFamily const * const p_family : map.get_alphabetical()) {
        CAPTURE(p_family->p_name);
        for (Unit* p_unit : p_family->units) {
            CAPTURE(p_unit->p_name);

            REQUIRE(map.find_unit(p_unit->p_usage) == p_unit);
            switch (p_family->base_type) {
                case TYPE_REAL:
                    roundtrip<Real>(p_unit);
                    break;

                case TYPE_VEC2:
                    roundtrip<Vec2>(p_unit);
                    break;

                case TYPE_VEC3:
                    roundtrip<Vec3>(p_unit);
                    break;

                case TYPE_VEC4:
                    roundtrip<Vec4>(p_unit);
                    break;
                
                default:
                    Logger::log_err("[Unit Roundtrip] No test case for type %s", Value::get_type_name(p_family->base_type));
                    throw std::logic_error("No test case for this type!");
            }
        }
    }
}

}

TEST_CASE("find_unit returns nullopt", "[core][units]") {
    REQUIRE(UnitsMap::get_units_map().find_unit("__THIS_UNIT_DOES_NOT_EXIST__") == std::nullopt);
}