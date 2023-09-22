#include "vector.h"


Vec2::Vec2(double x, double y) : _x(x), _y(y) {}
Vec2::Vec2(double v) : _x(v), _y(v) {}

double Vec2::x() const { return _x; }
double Vec2::y() const { return _y; }
