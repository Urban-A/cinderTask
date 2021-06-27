#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "Square.h"


using namespace ci;
using namespace ci::app;

Square::Square() : size(0) {}

Square::~Square() {}

Square::Square(float size, ci::vec2 location, ci::Color color) : Shape(Shape::PossibleShapes::circle, location, color), size(size), rotation(0) {}
Square::Square(float size, ci::vec2 location, ci::Color color, ci::vec2 direction) : Shape(Shape::PossibleShapes::square, location, color, direction), size(size), rotation(0) {}
Square::Square(float size, ci::vec2 location, ci::Color color, ci::vec2 direction, float rotation, bool rogue) : Shape(Shape::PossibleShapes::square, location, color, direction, rogue), size(size), rotation(rotation) {}

void Square::drawShape() {

	gl::pushModelMatrix();
	gl::translate(location.x, location.y);

	gl::rotate(rotation);

	gl::color(color);
	gl::drawSolidRect(RectT(vec2(0 - size / 2, 0 - size / 2), vec2(size / 2, size / 2)));

	gl::popModelMatrix();
}

bool Square::collision(ci::vec2 point) {
	
	return((point.x >= location.x -size/2 && point.x <= location.x + size/2) && (point.y >= location.y -size/2 && point.y <= location.y + size/2));
}

void Square::bounceBounds(ci::Area bounds) {
	if (location.y -size /2 < 0) {
		direction.y = abs(direction.y);
	}
	else if (location.y + size/2 > bounds.getHeight()) {
		direction.y = abs(direction.y) * -1;
	}

	if (location.x - size / 2 < 0) {
		direction.x = abs(direction.x);
	}
	else if (location.x + size/2 > bounds.getWidth()) {
		direction.x = abs(direction.x) * -1;
	}
}

void Square::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) {
	writer.StartObject();
	writer.Key("shape");
	writer.String("square");
	writer.Key("size");
	writer.Double(size);
	writer.Key("rotation");
	writer.Double(rotation);
	Shape::serialize(writer);
	writer.EndObject();
}