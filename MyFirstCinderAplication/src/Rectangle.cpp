#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "Rectangle.h"


using namespace ci;
using namespace ci::app;

Rectangle::Rectangle() : width(0), height(0) {}

Rectangle::~Rectangle() {}

Rectangle::Rectangle(float width, float height, ci::vec2 location, ci::Color color) : Shape(Shape::PossibleShapes::circle, location, color), width(width), height(height) {}
Rectangle::Rectangle(float width, float height, ci::vec2 location, ci::Color color, ci::vec2 direction) : Shape(Shape::PossibleShapes::rectangle, location, color, direction), height(height), width(width) {}

void Rectangle::drawShape() {

	//gl::pushModelMatrix();
	//gl::translate(location.x-width/2, location.y-height/2);
	//gl::rotate(45);

	gl::color(color);
	gl::drawSolidRect(RectT(vec2(location.x, location.y), vec2(location.x + width, location.y + height)));

	//gl::popModelMatrix();

	
}

bool Rectangle::collision(ci::vec2 point) {

	return((point.x >= location.x && point.x <= location.x + width) && (point.y >= location.y && point.y <= location.y + height));
}

void Rectangle::bounceBounds(ci::Area bounds) {
	if (location.y < 0) {
		direction.y = abs(direction.y);
	}
	else if (location.y + height > bounds.getHeight()) {
		direction.y = abs(direction.y) * -1;
	}

	if (location.x < 0) {
		direction.x = abs(direction.x);
	}
	else if (location.x + width > bounds.getWidth()) {
		direction.x = abs(direction.x) * -1;
	}
}