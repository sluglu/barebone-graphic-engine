#include <GLContext.h>

using namespace GLContext;

vec2 pos = vec2 (0,0);
float radius = 1.0f;
int numSegments = 5;
float Thickness = 5.0f;
vec4 Color = vec4(1, 1, 1, 1);
float i = 0.0f;
float n = 0.0f;


void frame() {
	i = cos(getElapsedTime());
	n = (sin(getElapsedTime()) + 1)*10;
	Color = vec4(cos(getElapsedTime()), sin(getElapsedTime()), tan(getElapsedTime()), 1);
	GLContext::drawCircle(pos,i,n,Thickness,Color);

}

void ui() {
	ImGui::Begin("Parameters");
		ImGui::InputFloat2("Position", &pos[0]);
		ImGui::InputInt("numSegments", &numSegments);
		ImGui::InputFloat("radius", &radius);
		ImGui::InputFloat("Thickness", &Thickness);
		ImGui::ColorEdit4("Color", &Color[0]);
	ImGui::End();
}

void main() {
	GLContext::onDrawUI = ui;
	GLContext::onDraw = frame;
	GLContext::fpsCounter = true;
	GLContext::init(1000, 1000);
}