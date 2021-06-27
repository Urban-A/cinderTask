#ifndef SQUARE_H
#define SQUARE_H
#pragma once

#include "Shape.h"

class Square : public Shape
{
public:
	Square();
	Square(float size, ci::vec2 location, ci::Color color);
	Square(float size, ci::vec2 location, ci::Color color, ci::vec2 direction);
	Square(float size, ci::vec2 location, ci::Color color, ci::vec2 direction, float rotation, bool rogue);
	~Square();
	void drawShape() override;
	bool collision(ci::vec2 point) override;
	void bounceBounds(ci::Area bounds) override;
	float size, rotation;
	void serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) override;
};

#endif
