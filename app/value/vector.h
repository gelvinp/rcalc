#pragma once


class Vec2 {
public:
    Vec2(double x, double y);
    Vec2(double v);

    double x() const;
    double y() const;

private:
    double _x, _y;
};