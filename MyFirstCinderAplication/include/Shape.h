#ifndef SHAPE_H
#define SHAPE_H

#pragma once
#include "cinder/app/App.h"
#include "cinder/Area.h"

class Shape
{
public:
	enum class PossibleShapes { circle, square, rectangle, none };
	static bool Shape::listCharGetter(void* data, int n, const char** out_text);

	Shape();
	Shape(PossibleShapes objectShape,ci::vec2 location,ci::Color color);
	Shape(PossibleShapes objectShape, ci::vec2 location, ci::Color color, ci::vec2 direction);
	~Shape();
	void virtual drawShape();
	void updateLocation(ci::vec2 velocity);
	void applyDirection();
	void virtual bounceBounds(ci::Area bounds);
	bool virtual collision(ci::vec2 point);


	PossibleShapes objectShape;
	ci::vec2 location;
	ci::Color color;
	ci::vec2 direction;
	bool rogue;
};

#endif