#pragma once

class Vector2
{
public:

	int x = 0;
	int y = 0;

	constexpr Vector2(void) = default;
	constexpr Vector2(int vX, int vY) : x(vX), y(vY) {}
};