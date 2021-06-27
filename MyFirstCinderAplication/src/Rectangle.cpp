#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "Rectangle.h"


using namespace ci;
using namespace ci::app;

Rectangle::Rectangle() : width(0), height(0) {}

Rectangle::~Rectangle() {}

Rectangle::Rectangle(float width, float height, ci::vec2 location, ci::Color color) : Shape(Shape::PossibleShapes::circle, location, color), width(width), height(height), rotation(0) {}
Rectangle::Rectangle(float width, float height, ci::vec2 location, ci::Color color, ci::vec2 direction) : Shape(Shape::PossibleShapes::rectangle, location, color, direction), height(height), width(width), rotation(0) {}
Rectangle::Rectangle(float width, float height, ci::vec2 location, ci::Color color, ci::vec2 direction, float rotation, bool rogue) : Shape(Shape::PossibleShapes::rectangle, location, color, direction, rogue), height(height), width(width), rotation(rotation) {}

void Rectangle::drawShape() {

	gl::pushModelMatrix();
	gl::translate(location.x, location.y);

	gl::rotate(rotation);

	gl::color(color);
	gl::drawSolidRect(RectT(vec2(0 - width / 2, 0 - height / 2), vec2(width/2, height/2)));

	gl::popModelMatrix();

	
}

bool Rectangle::collision(ci::vec2 point) {

	return((point.x >= location.x - width / 2 && point.x <= location.x + width/2) && (point.y >= location.y - height / 2 && point.y <= location.y + height/2));
}

void Rectangle::bounceBounds(ci::Area bounds) {
	if (location.y - height / 2  < 0 ) {
		direction.y = abs(direction.y);
	}
	else if (location.y + height/2 > bounds.getHeight()) {
		direction.y = abs(direction.y) * -1;
	}

	if (location.x - width / 2 < 0 ) {
		direction.x = abs(direction.x);
	}
	else if (location.x + width/2 > bounds.getWidth()) {
		direction.x = abs(direction.x) * -1;
	}
}

void Rectangle::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) {
	writer.StartObject();
	writer.Key("shape");
	writer.String("rectangle");
	writer.Key("width");
	writer.Double(width);
	writer.Key("height");
	writer.Double(height);
	writer.Key("rotation");
	writer.Double(rotation);
	Shape::serialize(writer);
	writer.EndObject();
}