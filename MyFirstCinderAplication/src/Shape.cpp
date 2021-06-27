#include "Shape.h"

Shape::Shape(PossibleShapes objectShape, ci::vec2 location, ci::Color color): objectShape(objectShape), location(location), color(color), direction(ci::vec2(0,0)), rogue(true) {}
Shape::Shape(PossibleShapes objectShape, ci::vec2 location, ci::Color color, ci::vec2 direction): objectShape(objectShape), location(location), color(color), direction(direction), rogue(true) {}
Shape::Shape(PossibleShapes objectShape, ci::vec2 location, ci::Color color, ci::vec2 direction, bool rogue) : objectShape(objectShape), location(location), color(color), direction(direction), rogue(rogue) {}

Shape::Shape()
{
	objectShape = PossibleShapes::none;
}

Shape::~Shape()
{
}

void Shape::drawShape(){}

void Shape::updateLocation(ci::vec2 velocity) {
	location += velocity;
}

bool Shape::collision(ci::vec2 point) { return false; }

void Shape::applyDirection() {
	updateLocation(direction);
}

void Shape::bounceBounds(ci::Area bounds) {};

bool Shape::listCharGetter(void* data, int n, const char** out_text)
{
	const std::vector<Shape*>* v = (std::vector<Shape*>*)data;
	switch (v->at(n)->objectShape) {
	case PossibleShapes::circle:
		*out_text = "circle";
		break;
	case PossibleShapes::square:
		*out_text = "square";
		break;
	case PossibleShapes::rectangle:
		*out_text = "rectangle";
		break;
	default:
		*out_text = "shape";
		break;
	}
	return true;
}

void Shape::serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) {
	
	writer.Key("location");
	writer.StartArray();
		writer.Double(location.x);
		writer.Double(location.y);
	writer.EndArray();
	writer.Key("color");
	writer.StartArray();
		writer.Double(color.r);
		writer.Double(color.g);
		writer.Double(color.b);
	writer.EndArray();
	writer.Key("direction");
	writer.StartArray();
		writer.Double(direction.x);
		writer.Double(direction.y);
	writer.EndArray();
	writer.Key("rogue");
	writer.Bool(rogue);
}

