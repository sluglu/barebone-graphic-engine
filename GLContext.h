#include <libs/imgui/imgui.h>
#include "libs/imgui/backends/imgui_impl_glfw.h"
#include "libs/imgui/backends/imgui_impl_opengl3.h"
#include <stdio.h>

#include <glad/glad.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "libs/lodepng/lodepng.h"
#include <glm/glm.hpp>

#include <Windows.h>
#include <commdlg.h>
#include <iostream>
#include <chrono>



using namespace std;
using namespace glm;

namespace GLContext {	
	extern const char* window_name;
	extern bool alpha;  
	extern bool fullscreen;
	extern bool fpsCounter;
	extern int SCR_WIDTH;
	extern int SCR_HEIGHT;

	extern int viewportResW;
	extern int viewportResH;

	extern vec4 background;
	extern void (*onInput)(int key);
	extern void (*onDraw)();
	extern void (*initialize)();
	extern void (*onDrawUI)();

	struct Time {
		int seconds;
		int minutes;
		int hours;
		int days;
		int months;
		int years;

		Time() {
			auto now = std::chrono::system_clock::now();
			std::time_t now_time = std::chrono::system_clock::to_time_t(now);
			struct tm time_info;
			localtime_s(&time_info, &now_time);
			seconds = localtime(&now_time)->tm_sec;
			minutes = localtime(&now_time)->tm_min;
			hours = localtime(&now_time)->tm_hour;
			days = localtime(&now_time)->tm_mday;
			months = localtime(&now_time)->tm_mon;
			years = localtime(&now_time)->tm_year;
		}
	};

	int init(int width = 500, int height = 500);
	
	string TakeScreenshot();

	void setWindowName(const char* name);

	double getElapsedTime();
	Time getAbsoluteTime();
	
	void drawPoint(vec2 position, float size = 5, vec4 color = vec4(1, 1, 1, 1));
	void drawLine(vec2 position1, vec2 position2, float width = 1, vec4 color = vec4(1, 1, 1, 1));
	void drawSquare(vec2 pos, float Size = 1.0f, float Thickness = 2.0f, vec4 Color = vec4(1, 1, 1, 1));
	void drawCircle(vec2 pos, float radius = 1.0f, int numSegments = 5, float Thickness = 2.0f, vec4 Color = vec4(1, 1, 1, 1));
	void alphaClear();


	
}

