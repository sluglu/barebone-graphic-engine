#include "GLContext.h"


namespace GLContext {

    int oldWidth = 1000;
    int oldHeight = 1000;
    int oldX = 500;
    int oldY = 500;
    bool fullScreenUpdate = false;
    GLFWwindow* window;
    GLuint fbo = 0;
    GLuint texture = 0;

    double simTime = 0.0;
	double simTimeStep = 0.01;

    const char* window_name = "  ";
    bool alpha = false;
    bool GUI = true;
    bool fullscreen = false;
	bool fpsCounter = false;
    int SCR_WIDTH = 1920;
    int SCR_HEIGHT = 1080;
    int viewportResW = 1000;
    int viewportResH = 1000;
    vec4 background = vec4(0.1f, 0.1f, 0.1f, 1.0f);
    void (*onInput)(int key) {};
    void (*onDraw)() {};
    void (*onInit)() {};
    void (*onDrawUI)() {};

    std::random_device rd;
    std::mt19937 gen(rd()); 


    int randomInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }


    double randomDouble(double min, double max) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(gen);
    }

	void setWindowName(const char* name) {
		window_name = name;
        glfwSetWindowTitle(window, name);
	}


     Time getAbsoluteTime() {
		 return Time();
     }

    
    void updateFPS() {
        if (GLContext::fpsCounter) {
            string fpsLabel = "fps : " + to_string(ImGui::GetIO().Framerate);
            setWindowName(fpsLabel.c_str());
        }
    }

    void restartSimTimer(){
		simTime = 0.0;
    }

    double getSimTime()
    {
        return simTime;
    }

    void incrementSimTimer()
    {
		simTime += simTimeStep;
    }

    double getSimTimeStep()
    {
        return simTimeStep;
    }

    void setSimTimeStep(double step)
    {
		simTimeStep = step;
    }

    void setWindowSize(int width, int height)
    {
		SCR_WIDTH = width;
		SCR_HEIGHT = height;
		glfwSetWindowSize(window, width, height);
    }

    void setViewportSize(int width, int height)
    {
		viewportResW = width;
		viewportResH = height;
    }

    pair<int, int> getWindowSize()
    {
        return pair<int, int>(SCR_WIDTH,SCR_HEIGHT);
    }

    pair<int, int> getViewportSize()
    {
        return pair<int, int>(viewportResW,viewportResH);
    }

    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glfwGetWindowSize(window, &SCR_WIDTH, &SCR_HEIGHT);
        glViewport(0, 0, width, height);        
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (action == GLFW_PRESS) {
            if (onInput) { onInput(key); }
        }
    }

    static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }

    void updateFullScreen() {
        if (GLContext::fullScreenUpdate == fullscreen) {
            return;
        }

        if (fullscreen)
        {
            glfwGetWindowPos(GLContext::window, &GLContext::oldX, &GLContext::oldY);
            glfwGetWindowSize(GLContext::window, &GLContext::oldWidth, &GLContext::oldHeight);

            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);

            glfwSetWindowMonitor(GLContext::window, monitor, 0, 0, mode->width, mode->height, 0);
        }
        else
        {
            glfwSetWindowMonitor(GLContext::window, nullptr, GLContext::oldX, GLContext::oldY, GLContext::oldWidth, GLContext::oldHeight, 0);
        }
        GLContext::fullScreenUpdate = !GLContext::fullScreenUpdate;
    }

    void renderLoop() {
        while (!glfwWindowShouldClose(GLContext::window)) {
			updateFPS();
            updateFullScreen();
            glFlush();
            glfwPollEvents();
            
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

            


            ImGui::Begin("main window", nullptr, window_flags);
            ImGui::PopStyleVar(2);
            ImGuiID dockspace_id = ImGui::GetID("main window");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Options"))
                {
                    ImGui::MenuItem("Fullscreen", NULL, &fullscreen);
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            ImGui::End();

            if (onDraw) { 
                glBindFramebuffer(GL_FRAMEBUFFER, GLContext::fbo);

                if (!alpha) { glClear(GL_COLOR_BUFFER_BIT); glClearColor(background.x, background.y, background.z, background.w); }
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GLContext::texture, 0);

                GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
                if (status != GL_FRAMEBUFFER_COMPLETE) {
                    std::cout << "framebuffer creation failure" << endl;
                }

                glViewport(0, 0, viewportResW, viewportResH);

                onDraw(); 

                ImGui::Begin("Viewport");
                {
                    ImGui::BeginChild("Render");
                    auto wsize = ImGui::GetWindowSize();
                    ImGui::Image((void*)(intptr_t)GLContext::texture, wsize);
                    ImGui::EndChild();
                }
                ImGui::End();
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            if (onDrawUI) { onDrawUI(); }

            ImGui::Render();
            glClear(GL_COLOR_BUFFER_BIT); 
            glClearColor(background.x, background.y, background.z, background.w);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);

        }
    }

    int init(int width, int height) {
        SCR_WIDTH = width;
        SCR_HEIGHT = height;
        const char* glsl_version = "#version 130";
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            return 1;
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
        GLContext::window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, window_name, NULL, NULL);
        if (GLContext::window == NULL)
        {
            glfwTerminate();
            return -1;
        }
        glfwSetFramebufferSizeCallback(GLContext::window, framebuffer_size_callback);
        glfwSetKeyCallback(GLContext::window, key_callback);
        glfwMakeContextCurrent(GLContext::window);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(GLContext::window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to onInit GLAD" << endl;
            return -1;
        }
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        //cout << "OpenGL version : " << glGetString(GL_VERSION) << endl;
        if (onInit) { onInit(); }
        if (onDraw) {
            glGenFramebuffers(1, &GLContext::fbo);
            glGenTextures(1, &GLContext::texture);
            glBindTexture(GL_TEXTURE_2D, GLContext::texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewportResW, viewportResH, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
        renderLoop();
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(GLContext::window);
        glfwTerminate();
        return 0;
    }

    bool ShowSaveFileDialog(char* filePath, const char* filter) {
        HWND hwnd = glfwGetWin32Window(window);
        OPENFILENAME ofn;
        ZeroMemory(&ofn, sizeof(OPENFILENAME));

        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = hwnd;
        ofn.lpstrFilter = filter;
        ofn.lpstrFile = filePath;
        ofn.nMaxFile = MAX_PATH;
        ofn.Flags = OFN_OVERWRITEPROMPT;

        if (GetSaveFileNameA(&ofn))
        {
            return true;
        }

        return false;
    }

    string TakeScreenshot() {
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.IniFilename = NULL;
        char filePath[MAX_PATH] = " ";
        if (ShowSaveFileDialog(filePath, ".png")) {
            std::vector<unsigned char> image(viewportResW * viewportResH * 4);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glReadPixels(0, 0, viewportResW, viewportResH, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            std::vector<unsigned char> PngBuffer;
            unsigned error = lodepng::encode(PngBuffer, image, viewportResW, viewportResH);
            if (!error) { lodepng::save_file(PngBuffer, filePath); return filePath; }
            if (error) { return lodepng_error_text(error); }
        }
        else { return "Not saved"; }
    }

    void drawPoint(pair<double,double> position, double Thickness, vec4 Color) {
        glColor4f(Color.x, Color.y, Color.z, Color.w);
        glPointSize(Thickness);
        glBegin(GL_POINTS);
        glVertex2d(position.first, position.second);
        glEnd();
    }

    void drawLine(pair<double,double> position1, pair<double,double> position2, double Thickness, vec4 Color) {
        glColor4f(Color.x, Color.y, Color.z, Color.w);
        glLineWidth(Thickness);
        glBegin(GL_LINES);
        glVertex2d(position1.first, position1.second);
        glVertex2d(position2.first, position2.second);
        glEnd();
    }

    void drawSquare(pair<double,double> pos, double Size, double Thickness, vec4 Color) {
        double x = pos.first;
        double y = pos.second;
        pair<double,double> a = pair<double,double>(x - Size / 2.0f, y + Size / 2.0f);
        pair<double,double> b = pair<double,double>(x + Size / 2.0f, y + Size / 2.0f);
        pair<double,double> c = pair<double,double>(x + Size / 2.0f, y - Size / 2.0f);
        pair<double,double> d = pair<double,double>(x - Size / 2.0f, y - Size / 2.0f);
        drawPoint(a, Thickness, Color);
        drawLine(a, b, Thickness, Color);
        drawPoint(b, Thickness, Color);
        drawLine(b, c, Thickness, Color);
        drawPoint(c, Thickness, Color);
        drawLine(c, d, Thickness, Color);
        drawPoint(d, Thickness, Color);
        drawLine(d, a, Thickness, Color);
    }

    void drawRect(pair<double,double> inferiorLeft, pair<double,double> superiorRight, double Thickness, vec4 Color)
    {
        glColor4f(Color.x, Color.y, Color.z, Color.w);
        glLineWidth(Thickness);
        glBegin(GL_LINE_LOOP);
        glVertex2d(inferiorLeft.first, superiorRight.second);
        glVertex2d(superiorRight.first, superiorRight.second);
        glVertex2d(superiorRight.first, inferiorLeft.second);
        glVertex2d(inferiorLeft.first, inferiorLeft.second);
        glEnd();
		
    }

    void drawCircle(pair<double,double> pos, double radius, int numSegments, double Thickness, vec4 Color) {
        glColor4f(Color.x, Color.y, Color.z, Color.w);
        glLineWidth(Thickness);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < numSegments; i++) {
            double theta = 2.0f * 3.14159265358979323846264338327950f * static_cast<double>(i) / static_cast<double>(numSegments);
            double x = radius * cosf(theta);
            double y = radius * sinf(theta);
            glVertex2d(pos.first + x, pos.second + y);
        }
        glEnd();
    }

    void alphaClear() {
        glBindFramebuffer(GL_FRAMEBUFFER, GLContext::fbo);
        glClear(GL_COLOR_BUFFER_BIT); 
        glClearColor(background.x, background.y, background.z, background.w);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}