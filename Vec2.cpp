#include "Vec2.h"
#include "math.h"

Vec2::Vec2()
{
}

Vec2::Vec2(float xin, float yin)
	: x(xin), y(yin)
{
}

bool Vec2::operator==(const Vec2& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator!=(const Vec2& rhs) const
{
	return !(*this == rhs);
}

Vec2 Vec2::operator+(const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator/(const float val) const
{
	return Vec2(x/val, y/val);
}

Vec2 Vec2::operator*(const float val) const
{
	return Vec2(x*val, y*val);
}

void Vec2::operator+=(const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

void Vec2::operator-=(const Vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

void Vec2::operator/=(const float val)
{
	x /= val;
	y /= val;
}

void Vec2::operator*=(const float val)
{
	x *= val;
	y += val;
}

float Vec2::dist(const Vec2& rhs) const
{
	return sqrtf(powf((x - rhs.x),2) + powf((y - rhs.y), 2));
}


float Vec2::distSqrd(const Vec2& rhs) const
{
	return ((x - rhs.x) * (x - rhs.x)) + ((y - rhs.y) * (y - rhs.y));
}

Vec2 Vec2::lerp(Vec2& rhs, float percent)
{
	return *this + (rhs - *this) * percent;
}
