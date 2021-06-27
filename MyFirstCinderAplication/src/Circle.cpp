#include "Circle.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"


using namespace ci;
using namespace ci::app;

Circle::Circle(): radius(0){}

Circle::~Circle(){}

Circle::Circle(float radius, ci::vec2 location, ci::Color color) : Shape(Shape::PossibleShapes::circle, location, color), radius(radius){}
Circle::Circle(float radius, ci::vec2 location, ci::Color color, ci::vec2 direction) : Shape(Shape::PossibleShapes::circle, location, color, direction), radius(radius) {}
Circle::Circle(float radius, ci::vec2 location, ci::Color color, ci::vec2 direction, bool rogue) : Shape(Shape::PossibleShapes::circle, location, color, direction, rogue), radius(radius) {}

void Circle::drawShape() {
	gl::color(color);
	gl::drawSolidCircle(location, radius);
}

bool Circle::collision(ci::vec2 point) {
	float diffX, diffY, distance;
	diffX = location.x - point.x;
	diffY = location.y - point.y;
	distance = sqrt(pow(diffX, 2) + pow(diffY, 2));
	//std::string tempString = "x: " + std::to_string(location.x) + " y: " + std::to_string(location.y) + "dx: " + std::to_string(diffX) + " dy: " + std::to_string(diffY) + " dist: " + std::to_string(distance);
	//CI_LOG_D(tempString);
	return(distance<=radius);
}

void Circle::bounceBounds(ci::Area bounds) {
	if (location.y - radius < 0) {
		direction.y = abs(direction.y);
	}else if (location.y + radius > bounds.getHeight()) {
		direction.y = abs(direction.y)*-1;
	}

	if (location.x - radius < 0) {
		direction.x = abs(direction.x);
	}
	else if (location.x + radius > bounds.getWidth()) {
		direction.x = abs(direction.x) * -1;
	}
}

void Circle::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) {
	writer.StartObject();
		writer.Key("shape");
		writer.String("circle");
		writer.Key("radius");
		writer.Double(radius);
		Shape::serialize(writer);
	writer.EndObject();
}