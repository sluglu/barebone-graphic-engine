#include <GLContext.h>
#include <QuadTree.h>

using namespace GLContext;

vec2 pos = vec2 (0,0);
float radius = 1.0f;
int numSegments = 5;
float Thickness = 5.0f;
vec4 Color = vec4(1, 1, 1, 1);
float i = 0.0f;
float n = 0.0f;


void frame() {
	//i = cos(getElapsedTime());
	//n = (sin(getElapsedTime()) + 1)*10;
	//Color = vec4(cos(getElapsedTime()), sin(getElapsedTime()), tan(getElapsedTime()), 1);
	//GLContext::drawCircle(pos,i,n,Thickness,Color);
	//cout << getAbsoluteTime().seconds << endl;

	//incrementSimTimer();
	//cout << getSimTime() << endl;

	//drawRect(vec2(-0.5, -0.5), vec2(0.5, 0.5), 5, vec4(1, 1, 1, 1));

	//AABB aabb1 = AABB(-0.5, 0.5, -0.5, 0.5);

	//for (int i = 0; i < 4; i++) {
	//	aabb1.getChild(i).draw(5, vec4(1, 1, 1, 1));
	//}

	Quadtree<int> quadtree = Quadtree<int>(AABB(-1, 1, -1, 1));
	for (int i = 0; i < 100; i++) {
		quadtree.insert(i, cos(), cos());
	}


	quadtree.draw(1, vec4(1, 1, 1, 1));
}

void ui() {
	ImGui::Begin("Parameters");
		ImGui::InputFloat2("Position", &pos[0]);
		ImGui::InputInt("numSegments", &numSegments);
		ImGui::InputFloat("radius", &radius);
		ImGui::InputFloat("Thickness", &Thickness);
		ImGui::ColorEdit4("Color", &Color[0]);
		double step = getSimTimeStep();
		if (ImGui::InputDouble("Time Step", &step)) {
			setSimTimeStep(step);
		}
		if(ImGui::Button("Restart Timer") ){
			restartSimTimer();
		};
	ImGui::End();
}

void main() {
	GLContext::onDrawUI = ui;
	GLContext::onDraw = frame;
	GLContext::fpsCounter = true;
	GLContext::setSimTimeStep(0.0001);
	GLContext::init(1000, 1000);
}