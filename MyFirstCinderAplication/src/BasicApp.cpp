#include <time.h>
#include <string>
#include <iostream>
#include <fstream>
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"
#include "Circle.h"
#include "cinder/Rand.h"
#include "cinder/Log.h"
#include "cinder/app/Window.h"
#include "cinder/app/FileDropEvent.h"
#include "Square.h"
#include "Rectangle.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

using namespace ci;
using namespace ci::app;

// We'll create a new Cinder Application by deriving from the App class.
class BasicApp : public App {
  public:
	// Cinder will call 'keyDown' when the user presses a key on the keyboard.
	// See also: keyUp.
	void keyDown( KeyEvent event ) override;
	void keyUp(KeyEvent event) override;
	void mouseDown(MouseEvent event) override;

	// Cinder will call 'draw' each time the contents of the window need to be redrawn.
	void draw() override;
	void setup() override;
	void update() override;
	void fileDrop(FileDropEvent event);
	std::string makeJson();
	void showListWindow();
	void showPropertiesWindow();

	std::vector<Shape*> shapeCollection;
	int selectedIndex = -1;
	int selectedShape= 0;
	std::vector<gl::Texture2dRef> textures;

  private:
	// This will maintain a list of points which we will draw line segments between
	
	bool heldA = false, heldD = false, heldW = false, heldS = false;
	Rand generator;
};


void prepareSettings( BasicApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void BasicApp::setup()
{
	ImGui::Initialize();
	generator = Rand();
	generator.randomize();
	selectedIndex = -1;
}


void BasicApp::mouseDown(MouseEvent event)
{
	if (event.isLeft()) {
		switch (selectedShape)
		{
		case 0:
			shapeCollection.push_back(new Circle(generator.randFloat(10, 100), event.getPos(), Color(generator.randFloat(0, 1), generator.randFloat(0, 1), generator.randFloat(0, 1)), normalize(vec2(generator.randFloat(-1, 1), generator.randFloat(-1, 1))) * generator.randFloat(0.2f, 3.f)));
			break;
		case 1:
			shapeCollection.push_back(new Square(generator.randFloat(10, 100), event.getPos(), Color(generator.randFloat(0, 1), generator.randFloat(0, 1), generator.randFloat(0, 1)), normalize(vec2(generator.randFloat(-1, 1), generator.randFloat(-1, 1))) * generator.randFloat(0.2f, 3.f)));
			break;
		case 2:
			shapeCollection.push_back(new Rectangle(generator.randFloat(10, 100), generator.randFloat(10, 100), event.getPos(), Color(generator.randFloat(0, 1), generator.randFloat(0, 1), generator.randFloat(0, 1)), normalize(vec2(generator.randFloat(-1, 1), generator.randFloat(-1, 1))) * generator.randFloat(0.2f, 3.f)));
			break;
		}
		
		
		
	}
	else if (event.isRight()) {
		for (int i = (int) shapeCollection.size() - 1; i >= 0; i--) {
			
			if (shapeCollection[i]->collision(event.getPos())) {
				Shape* tempShape = shapeCollection.at(i);
				shapeCollection.erase(shapeCollection.begin()+i);
				delete tempShape;
				if (i == selectedIndex) selectedIndex = -1;
				else if (selectedIndex>shapeCollection.size()-1) selectedIndex -=1;
				break;
			}
		}
	}
	
}

void BasicApp::update() {
	vec2 keyDirection = vec2(0, 0);
	bool moving = false;
	if (heldA) {
		keyDirection.x -= 1;
		moving = true;
	}
	if (heldD) {
		keyDirection.x += 1;
		moving = true;
	}
	if (heldW) {
		keyDirection.y -= 1;
		moving = true;
	}
	if (heldS) {
		keyDirection.y += 1;
		moving = true;
	}
	if (moving) {
		normalize(keyDirection);
		keyDirection *= 5;
		if (!shapeCollection.empty()) {
			shapeCollection.back()->updateLocation(keyDirection);
		}
	}
	for (auto* shapeTemp : shapeCollection) {
		shapeTemp->applyDirection();
		shapeTemp->bounceBounds(getWindowBounds());
	}
}

void BasicApp::keyUp(KeyEvent event) {
	if (event.getChar() == 'a') {
		heldA = false;
	}
	if (event.getChar() == 'd') {
		heldD = false;
	}
	if (event.getChar() == 'w') {
		heldW = false;
	}
	if (event.getChar() == 's') {
		heldS = false;
	}
}

void BasicApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		// Toggle full screen when the user presses the 'f' key.
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getCode() == KeyEvent::KEY_ESCAPE ) {
		// Exit full screen, or quit the application, when the user presses the ESC key.
		selectedIndex = -1;
	}
	if (event.getChar() == 'a') {
		heldA = true;
	}else if (event.getChar() == 'd') {
		heldD = true;
	}else if (event.getChar() == 'w') {
		heldW = true;
	}else if (event.getChar() == 's') {
		heldS=true;
	}	


	
}

void BasicApp::fileDrop(FileDropEvent event) {
	std::vector<fs::path > files = event.getFiles();
	textures.clear();
	for (auto file : files) {
		auto texture = loadImage(file);
		textures.push_back(gl::Texture2d::create(texture));
	}
}

std::string BasicApp::makeJson(){
	rapidjson::StringBuffer s;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
		for (auto* shape : shapeCollection) {
			switch (shape->objectShape) {
				case Shape::PossibleShapes::circle:
					writer.Key("circle");
					break;
				case Shape::PossibleShapes::square:
					writer.Key("square");
					break;
				case Shape::PossibleShapes::rectangle:
					writer.Key("rectangle");
					break;
			}
				writer.StartObject();
				writer.Key("location");
				writer.StartArray();
					writer.Double(shape->location.x);
					writer.Double(shape->location.y);
				writer.EndArray();
				writer.Key("color");
				writer.StartArray();
					writer.Double(shape->color.r);
					writer.Double(shape->color.g);
					writer.Double(shape->color.b);
				writer.EndArray();
				writer.Key("direction");
				writer.StartArray();
					writer.Double(shape->direction.x);
					writer.Double(shape->direction.y);
				writer.EndArray();

				switch (shape->objectShape) {
					case Shape::PossibleShapes::circle:
						{
							Circle* temp = (Circle*)shape;
							writer.Key("radius");
							writer.Double(temp->radius);

						}
					break;
						case Shape::PossibleShapes::square:
						{
							Square* temp = (Square*)shape;
							writer.Key("size");
							writer.Double(temp->size);

						}
					break;
						case Shape::PossibleShapes::rectangle:
						{
							Rectangle* temp = (Rectangle*)shape;
							writer.Key("width");
							writer.Double(temp->width);
							writer.Key("height");
							writer.Double(temp->height);

						}
					break;
				}

				writer.Key("rogue");
				writer.Bool(shape->rogue);
			writer.EndObject();
		}
		
	writer.EndObject();

	return(s.GetString());
}

void BasicApp::draw()
{
	// Clear the contents of the window. This call will clear
	// both the color and depth buffers. 
	gl::clear( Color::gray( 0.1f ) );

	// Set the current draw color to orange by setting values for
	// red, green and blue directly. Values range from 0 to 1.
	// See also: gl::ScopedColor
	
	gl::color(1.0f, 1.f, 1.f);
	for (auto texture : textures) {
		gl::draw(texture, Rectf(vec2(0, 0), vec2(640, 480)));
	}

	for (auto *shape1 : shapeCollection) {
		shape1->drawShape();
	}

	//ImGui::ShowDemoWindow();

	showListWindow();
	showPropertiesWindow();
}

void BasicApp::showListWindow() {
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;
	if (!ImGui::Begin("List", NULL, window_flags))
	{
		ImGui::End();
		return;
	}

	ImGui::PushItemWidth(200);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open", "Ctrl+O", false, false)) {};
			if (ImGui::MenuItem("Save", "Ctrl+S")) {
				std::ofstream saveFile("project.json");
				saveFile << makeJson();
				saveFile.close();
			};
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Text("Draw shape");
	ImGui::RadioButton("circle", &selectedShape, 0); ImGui::SameLine();
	ImGui::RadioButton("square", &selectedShape, 1); ImGui::SameLine();
	ImGui::RadioButton("rectangle", &selectedShape, 2);

	ImGui::Spacing();
	
	ImGui::Text("Drawn list");
	ImGui::ListBox("", &selectedIndex, Shape::listCharGetter, (void*)&shapeCollection, (int)shapeCollection.size());
	ImGui::SameLine();

	ImGui::End();
}
void BasicApp::showPropertiesWindow() {
	if (!ImGui::Begin("Properties"))
	{
		ImGui::End();
		return;
	}

	//ImGui::InputInt("", &selectedIndex, 1, 0);
	if (selectedIndex >= 0) {
		//ImGui::Checkbox("rogue", &(shapeCollection.at(selectedIndex)->rogue));
		ImGui::InputFloat("x", &(shapeCollection.at(selectedIndex)->location.x), 20.f, 100.f);
		ImGui::InputFloat("y", &(shapeCollection.at(selectedIndex)->location.y), 20.f, 100.f);
		ImGui::InputFloat("x velocity", &(shapeCollection.at(selectedIndex)->direction.x), 0.1f, 1.f);
		ImGui::InputFloat("y velocity", &(shapeCollection.at(selectedIndex)->direction.y), 0.1f, 1.f);
		ImGui::ColorEdit3("color", &(shapeCollection.at(selectedIndex)->color));
		switch (shapeCollection.at(selectedIndex)->objectShape) {
			case Shape::PossibleShapes::circle:
			{
				Circle* temp = (Circle*)shapeCollection.at(selectedIndex);
				if (ImGui::InputFloat("radius", &(temp->radius), 5.f, 20.f)) {
					temp->radius = (temp->radius >= 5) ? temp->radius : 5;
				}
			
			}
		break;
		case Shape::PossibleShapes::square:
			{
				Square* temp = (Square*)shapeCollection.at(selectedIndex);
				if (ImGui::InputFloat("size", &(temp->size), 5.f, 20.f)) {
					temp->size = (temp->size >= 5) ? temp->size : 5;
				}
			
			}
			break;
		case Shape::PossibleShapes::rectangle:
			{
				Rectangle* temp = (Rectangle*)shapeCollection.at(selectedIndex);
				if (ImGui::InputFloat("width", &(temp->width), 5.f, 20.f)) {
					temp->width = (temp->width >= 5) ? temp->width : 5;
				}
				if (ImGui::InputFloat("height", &(temp->height), 5.f, 20.f)) {
					temp->height = (temp->height >= 5) ? temp->height : 5;
				}

			}
			break;
		}
	}

	ImGui::End();
	
}



// This line tells Cinder to actually create and run the application.
CINDER_APP( BasicApp, RendererGl, prepareSettings )
