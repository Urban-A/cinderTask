#ifndef CIRCLE_H
#define CIRCLE_H
#pragma once

#include "Shape.h"

class Circle: public Shape
{
public:
	Circle();
	Circle(float radius, ci::vec2 location, ci::Color color);
	Circle(float radius, ci::vec2 location, ci::Color color, ci::vec2 direction);
	~Circle();
	void drawShape() override;
	bool collision(ci::vec2 point) override;
	void bounceBounds(ci::Area bounds) override;
	float radius;
};

#endif
