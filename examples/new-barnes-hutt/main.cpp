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



struct particle {
	pair<double,double> pos;
	pair<double, double> vel;
	pair<double, double> acc;
	particle(pair<double, double> pos, pair<double, double> vel, pair<double, double> acc) : pos(pos), vel(vel), acc(acc) {}
};


Quadtree<particle> quadtree = Quadtree<particle>(AABB(vec2(-1, -1), vec2(1, 1)), 1, 10);

//vector<particle> particles;

void init() {
	//setViewportSize(2000, 2000);
	for (int i = 0; i < 1000; i++) {
		particle p = particle(pair<double, double>(randomDouble(-1, 1), randomDouble(-1, 1)), pair<double, double>(0,0), pair<double, double>(0,0));
		//particles.push_back(p);
		quadtree.insert(p, p.pos.first, p.pos.second);

	}
	//quadtree.insert(pair<double, double>(0.5, 0.5), 0.5, 0.5);
	//quadtree.insert(pair<double, double>(-0.35, -0.35), -0.35, -0.35);
	//quadtree.insert(pair<double, double>(-0.75, -0.75), -0.75, -0.75);
	//quadtree.insert(pair<double, double>(0.35, -0.35), 0.35, -0.35);
	//quadtree.insert(pair<double, double>(0.75, -0.75), 0.75, -0.75);
	//quadtree.insert(pair<double, double>(-0.35, 0.35), -0.35, 0.35);
	//quadtree.insert(pair<double, double>(-0.8, 0.8), -0.8, 0.8);
	//quadtree.insert(pair<double, double>(-0.7, 0.7), -0.7, 0.7);
}


void frame() {
	//i = cos(getElapsedTime());
	//n = (sin(getElapsedTime()) + 1)*10;
	//Color = vec4(cos(getElapsedTime()), sin(getElapsedTime()), tan(getElapsedTime()), 1);
	//GLContext::drawCircle(pos,i,n,Thickness,Color);
	//cout << getAbsoluteTime().seconds << endl;

	//incrementSimTimer();
	//cout << getSimTime() << endl;

	//drawRect(vec2(-0.5, -0.5), vec2(0.5, 0.5), 5, vec4(1, 1, 1, 1));

	//AABB aabb1 = AABB(vec2(-0.5, -0.5), vec2(0.9, 0.9));
	//aabb1.draw(5, vec4(1, 1, 1, 1));

	//for (int i = 0; i < 4; i++) {
	//	aabb1.getChild(i).draw(5, vec4(1, 1, 1, 1));
	//}

	


	quadtree.draw(1.5, vec4(1, 1, 1, 1));
	vector<particle> particles = quadtree.query(AABB(vec2(-1, -1), vec2(1, 1)));
	for (particle& p : particles) {
		drawPoint(vec2(p.pos.first, p.pos.second), 2, vec4(1, 0, 0, 1));
	}
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
	GLContext::onInit = init;
	GLContext::onDrawUI = ui;
	GLContext::onDraw = frame;
	GLContext::fpsCounter = true;
	GLContext::setSimTimeStep(0.0001);
	GLContext::init(1000, 1000);
}