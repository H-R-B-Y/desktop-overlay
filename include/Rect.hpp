#pragma once

struct Rect {
	double x;
	double y;
	double w;
	double h;

	bool contains(double px, double py) const
	{
		return px >= x && py >= y && px <= x + w && py <= y + h;
	}
};
