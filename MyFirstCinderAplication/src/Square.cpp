#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "Square.h"


using namespace ci;
using namespace ci::app;

Square::Square() : size(0) {}

Square::~Square() {}

Square::Square(float size, ci::vec2 location, ci::Color color) : Shape(Shape::PossibleShapes::circle, location, color), size(size) {}
Square::Square(float size, ci::vec2 location, ci::Color color, ci::vec2 direction) : Shape(Shape::PossibleShapes::square, location, color, direction), size(size) {}

void Square::drawShape() {
	gl::color(color);
	gl::drawSolidRect(RectT(vec2(location.x, location.y), vec2(location.x + size, location.y + size)));
}

bool Square::collision(ci::vec2 point) {
	
	return((point.x >= location.x && point.x <= location.x + size) && (point.y >= location.y && point.y <= location.y + size));
}

void Square::bounceBounds(ci::Area bounds) {
	if (location.y < 0) {
		direction.y = abs(direction.y);
	}
	else if (location.y + size > bounds.getHeight()) {
		direction.y = abs(direction.y) * -1;
	}

	if (location.x < 0) {
		direction.x = abs(direction.x);
	}
	else if (location.x + size > bounds.getWidth()) {
		direction.x = abs(direction.x) * -1;
	}
}