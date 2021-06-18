#ifndef RECTANGLE_H
#define RECTANGLE_H
#pragma once

#include "Shape.h"

class Rectangle : public Shape
{
public:
	Rectangle();
	Rectangle(float width, float height, ci::vec2 location, ci::Color color);
	Rectangle(float width, float height, ci::vec2 location, ci::Color color, ci::vec2 direction);
	~Rectangle();
	void drawShape() override;
	bool collision(ci::vec2 point) override;
	void bounceBounds(ci::Area bounds) override;
	float width, height;
};


#endif