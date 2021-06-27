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
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"
#include "ImageInfo.h"

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
	void showImageWindow();
	void showPropertiesWindow();
	void emptyCollection();

	std::vector<Shape*> shapeCollection;
	int selectedIndex = -1;
	int selectedShape= 0;
	std::vector<ImageInfo*> textures;
	gl::GlslProgRef	mGlsl;

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

	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameRounding = 6;
	style.FrameBorderSize = 0;
	style.Colors[ImGuiCol_WindowBg]= ImVec4(20/255.f,20/255.f,20/255.f,250/255.f);

	mGlsl = gl::GlslProg::create(gl::GlslProg::Format()
		.vertex(CI_GLSL(150,
			uniform mat4	ciModelViewProjection;
			in vec4 ciPosition;
			in vec2 ciTexCoord0;
			out vec2 vertTexCoord0;

	void main(void) {
		vertTexCoord0 = ciTexCoord0;
		gl_Position = ciModelViewProjection * ciPosition;
	}
	))
		.fragment(CI_GLSL(150,
	uniform sampler2D uTex0;
	uniform float min;
	uniform float max;
	uniform vec4 colorMulti;
	in vec2 vertTexCoord0;
	out vec4 outColor;

	void main(void) {
		vec3 fragColor = texture(uTex0, vertTexCoord0).rgb;
		bool greyscale = (colorMulti.a!=1.f);
		fragColor.b = fragColor.b * colorMulti.b * float(!greyscale) + fragColor.r * colorMulti.r * float(greyscale);
		fragColor.g = fragColor.g * colorMulti.g * float(!greyscale) + fragColor.r * colorMulti.r * float(greyscale);
		fragColor.r *= colorMulti.r;
		float alpha = float(vertTexCoord0.x >= min && vertTexCoord0.x < max);
		outColor = vec4(fragColor, alpha);
	}
	)));
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
	for (fs::path file : files) {
		//auto texture = loadImage(file);
		textures.push_back(new ImageInfo(file));
	}
}

std::string BasicApp::makeJson(){
	rapidjson::StringBuffer s;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
	writer.StartObject();
	writer.Key("shapes");
		writer.StartArray();
			for (auto* shape : shapeCollection) {
				shape->serialize(writer);
			}
		
		writer.EndArray();
	writer.EndObject();

	return(s.GetString());
}

void BasicApp::draw()
{
	// Clear the contents of the window. This call will clear
	// both the color and depth buffers. 
	gl::clear( Color::gray( 0.1f ) );
	gl::clearDepth(0.f);

	// Set the current draw color to orange by setting values for
	// red, green and blue directly. Values range from 0 to 1.
	// See also: gl::ScopedColor
	{
		gl::FboRef myFbo = gl::Fbo::create(640, 480);
		
		gl::color(1.0f, 1.f, 1.f);

		myFbo->bindFramebuffer();
		int counter = 0;
		for (ImageInfo* texture : textures) {
			//gl::draw(texture, Rectf(vec2(0, 0), vec2(640, 480)));
			gl::ScopedGlslProg scpGlsl(mGlsl);
			gl::ScopedTextureBind texScp(texture->texture);
			mGlsl->uniform("uTex0", 0);
			mGlsl->uniform("colorMulti", texture->colorInfo);
			mGlsl->uniform("min", counter * (1.f / textures.size()));
			mGlsl->uniform("max", (counter+1) * (1.f / textures.size()));
			gl::drawSolidRect(Rectf(0, getWindowHeight()- 480, 640, getWindowHeight()));
			//Rectf(0, 0, 640, 480)
			counter++;
		}
		//gl::drawSolidCircle(vec2(100, 100), 50);
		//gl::drawSolidRect(Rectf(200, 200, 320, 240));
		myFbo->unbindFramebuffer();
		//gl::ScopedGlslProg scpGlsl(mGlsl);
		if (getWindowHeight() < 480 || getWindowWidth() < 640) {
			gl::draw(myFbo->getColorTexture(), Rectf(0, getWindowHeight() - 480, 640, getWindowHeight()));
		}
		else {
			gl::draw(myFbo->getColorTexture(), getWindowBounds());
		}
		

	}

	for (auto* shape1 : shapeCollection) {
		shape1->drawShape();
	}

	//ImGui::ShowDemoWindow();

	showListWindow();
	showPropertiesWindow();
	showImageWindow();
}

void BasicApp::emptyCollection() {
	for (int i = (int)shapeCollection.size() - 1; i >= 0; i--) {
		delete shapeCollection[i];
	}
	shapeCollection.clear();
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
			if (ImGui::MenuItem("Open", "Ctrl+O")) {
				std::ifstream savedFile("project.json");
				if (savedFile.good()) {
					rapidjson::IStreamWrapper isw(savedFile);

					rapidjson::Document doc;
					doc.ParseStream(isw);

					assert(doc.IsObject());
					assert(doc.HasMember("shapes"));

					const rapidjson::Value& savedShapes = doc["shapes"];
					assert(savedShapes.IsArray());
					emptyCollection();
					for (int i = 0; i < savedShapes.Size(); i++) {
						assert(savedShapes[i].HasMember("shape"));
						assert(savedShapes[i].HasMember("location"));
						assert(savedShapes[i].HasMember("color"));
						assert(savedShapes[i].HasMember("direction"));
						assert(savedShapes[i].HasMember("rogue"));
						std::string shape = savedShapes[i]["shape"].GetString();
						if (shape == "circle") {
							assert(savedShapes[i].HasMember("radius"));
							shapeCollection.push_back(new Circle(savedShapes[i]["radius"].GetDouble(), vec2(savedShapes[i]["location"][0].GetDouble(), savedShapes[i]["location"][1].GetDouble()),
								Color(savedShapes[i]["color"][0].GetDouble(), savedShapes[i]["color"][1].GetDouble(), savedShapes[i]["color"][2].GetDouble()),
								vec2(savedShapes[i]["direction"][0].GetDouble(), savedShapes[i]["direction"][1].GetDouble()), savedShapes[i]["rogue"].GetBool()));
						}else if (shape == "square") {
							assert(savedShapes[i].HasMember("size"));
							assert(savedShapes[i].HasMember("rotation"));
							shapeCollection.push_back(new Square(savedShapes[i]["size"].GetDouble(), vec2(savedShapes[i]["location"][0].GetDouble(), savedShapes[i]["location"][1].GetDouble()),
								Color(savedShapes[i]["color"][0].GetDouble(), savedShapes[i]["color"][1].GetDouble(), savedShapes[i]["color"][2].GetDouble()),
								vec2(savedShapes[i]["direction"][0].GetDouble(), savedShapes[i]["direction"][1].GetDouble()), savedShapes[i]["rotation"].GetDouble(), savedShapes[i]["rogue"].GetBool()));
						}else if (shape == "rectangle") {
							assert(savedShapes[i].HasMember("width"));
							assert(savedShapes[i].HasMember("height"));
							assert(savedShapes[i].HasMember("rotation"));
							shapeCollection.push_back(new Rectangle(savedShapes[i]["width"].GetDouble(), savedShapes[i]["height"].GetDouble(), vec2(savedShapes[i]["location"][0].GetDouble(), savedShapes[i]["location"][1].GetDouble()),
								Color(savedShapes[i]["color"][0].GetDouble(), savedShapes[i]["color"][1].GetDouble(), savedShapes[i]["color"][2].GetDouble()),
								vec2(savedShapes[i]["direction"][0].GetDouble(), savedShapes[i]["direction"][1].GetDouble()), savedShapes[i]["rotation"].GetDouble(), savedShapes[i]["rogue"].GetBool()));
						}
					}
						

				}
			};
			if (ImGui::MenuItem("Save", "Ctrl+S")) {
				if (std::ifstream("project.json").good()) {
					std::remove("old_project.json");
					if (std::rename("project.json", "old_project.json")) {}
				}
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

void BasicApp::showImageWindow() {
	if (!ImGui::Begin("Harmonica"))
	{
		ImGui::End();
		return;
	}

	//ImGui::BeginTable();
	int id = 0;
	for (ImageInfo* texture : textures) {
		std::string label = "##" + std::to_string(id);
		std::string labelA = "##" + std::to_string(id)+"a";
		std::string labelG = "use color ##" + std::to_string(id) + "g";
		bool greyScale = texture->colorInfo.a;
		ImGui::Text(texture->name.c_str());
		ImGui::Columns(2, NULL, false);
		ImGui::SetColumnWidth(0,55);
		ImGui::Image(texture->texture, vec2(50, 50), vec2(0, 1), vec2(1, 0));	
		ImGui::NextColumn();
		ImGui::Text("rgb multiplier values");
		ImGui::SliderFloat3(label.c_str(), (vec3*)&texture->colorInfo, 0.f, 10.f);
		ImGui::Checkbox(labelG.c_str(), &greyScale);
		texture->colorInfo.a = float(greyScale);
		ImGui::Columns(1);
		ImGui::Separator();
		id++;
	}

	ImGui::End();
}


void BasicApp::showPropertiesWindow() {

	if (!ImGui::Begin("Properties"))
	{
		ImGui::End();
		return;
	}

	if (selectedIndex >= 0) {

		ImGuiColorEditFlags flags_color = 0;
		ImGuiInputTextFlags flags_float = 0;

		ImGui::Checkbox("rogue", &(shapeCollection.at(selectedIndex)->rogue));
		if ((shapeCollection.at(selectedIndex)->rogue)) {
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
			flags_color |= ImGuiColorEditFlags_NoPicker;
			flags_color |= ImGuiColorEditFlags_NoInputs;
			flags_float |= ImGuiInputTextFlags_ReadOnly;

			//std::string(std::to_string(shapeCollection.at(selectedIndex)->location.x));

			ImGui::InputText("x", &std::string(std::to_string(shapeCollection.at(selectedIndex)->location.x)), flags_float);
			ImGui::InputText("y", &std::string(std::to_string(shapeCollection.at(selectedIndex)->location.y)), flags_float);
			ImGui::InputText("x velocity", &std::string(std::to_string(shapeCollection.at(selectedIndex)->direction.x)), flags_float);
			ImGui::InputText("y velocity", &std::string(std::to_string(shapeCollection.at(selectedIndex)->direction.y)), flags_float);

			

		}
		else {
			ImGui::InputFloat("x", &(shapeCollection.at(selectedIndex)->location.x), 20.f, 100.f);
			ImGui::InputFloat("y", &(shapeCollection.at(selectedIndex)->location.y), 20.f, 100.f);
			ImGui::InputFloat("x velocity", &(shapeCollection.at(selectedIndex)->direction.x), 0.1f, 1.f);
			ImGui::InputFloat("y velocity", &(shapeCollection.at(selectedIndex)->direction.y), 0.1f, 1.f);
		}
		
		ImGui::ColorEdit3("color", &(shapeCollection.at(selectedIndex)->color), flags_color);
		switch (shapeCollection.at(selectedIndex)->objectShape) {
			case Shape::PossibleShapes::circle:
			{
				Circle* temp = (Circle*)shapeCollection.at(selectedIndex);
				if (temp->rogue) {
					ImGui::InputText("radius", &std::string(std::to_string(temp->radius)), flags_float);
				}
				else {
					if (ImGui::InputFloat("radius", &(temp->radius), 5.f, 20.f)) {
						temp->radius = (temp->radius >= 5) ? temp->radius : 5;
					}
				}
				
			
			}
		break;
		case Shape::PossibleShapes::square:
			{
				Square* temp = (Square*)shapeCollection.at(selectedIndex);
				if (temp->rogue) {
					ImGui::InputText("size", &std::string(std::to_string(temp->size)), flags_float);
					ImGui::InputText("rotation", &std::string(std::to_string((int)toDegrees(temp->rotation))+" deg"), flags_float);
				}
				else {
					if (ImGui::InputFloat("size", &(temp->size), 5.f, 20.f)) {
						temp->size = (temp->size >= 5) ? temp->size : 5;
					}
					if (ImGui::SliderAngle("rotation", &(temp->rotation))) {
					}
				}
			
			}
			break;
		case Shape::PossibleShapes::rectangle:
			{
				Rectangle* temp = (Rectangle*)shapeCollection.at(selectedIndex);
				if (temp->rogue) {
					ImGui::InputText("width", &std::string(std::to_string(temp->width)), flags_float);
					ImGui::InputText("height", &std::string(std::to_string(temp->height)), flags_float);
					ImGui::InputText("rotation", &std::string(std::to_string((int)toDegrees(temp->rotation)) + " deg"), flags_float);
				}
				else {
					if (ImGui::InputFloat("width", &(temp->width), 5.f, 20.f)) {
						temp->width = (temp->width >= 5) ? temp->width : 5;
					}
					if (ImGui::InputFloat("height", &(temp->height), 5.f, 20.f)) {
						temp->height = (temp->height >= 5) ? temp->height : 5;
					}
					if (ImGui::SliderAngle("rotation", &(temp->rotation))) {
					}
				}
			}
			break;
		}
		if ((shapeCollection.at(selectedIndex)->rogue)) {
			ImGui::PopStyleVar();
		}
	}

	ImGui::End();
	
}



// This line tells Cinder to actually create and run the application.
CINDER_APP( BasicApp, RendererGl, prepareSettings )
