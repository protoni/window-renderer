// Disable console window
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")


#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui/imgui_stdlib.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string.h>
#include <chrono>
#include <thread>

#include "Window.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "windows.h"
#include <crtdbg.h>

#include "DebugMacros.h"

#include <thread>

#define _CRTDBG_MAP_ALLOC //to get more details

// Memory leak detection
_CrtMemState sOld;
_CrtMemState sNew;
_CrtMemState sDiff;

bool showWindow = false;

void task1(Window* window);

// Console text buf
ImVector<char*> Items;

// Time
double deltaTime = 0.0f;   // Time between current frame and last frame
double lastFrame = 0.0f;   // Time of last frame
double secondFrame = 0.0f; // Count seconds
double fpsCounter = 0.0f;  // Count frames per second
double fpsLimit = 120.0f;   // MAX FPS
double frame_time = (1.0f / fpsLimit) * 1000;
double sleep_time = 0.0f;
double fps = 0.0f;

static void Strtrim(char* s) { char* str_end = s + strlen(s); while (str_end > s && str_end[-1] == ' ') str_end--; *str_end = 0; }
static char* Strdup(const char* s)
{
    IM_ASSERT(s);
    size_t len = strlen(s) + 1;
    void* buf = malloc(len);
    IM_ASSERT(buf);
    return (char*)memcpy(buf, (const void*)s, len);
}

void AddLog(const std::string data, ...);// IM_FMTARGS(2

// In C++11 you'd be better off using lambdas for this sort of forwarding callbacks
static int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
    //ExampleAppConsole* console = (ExampleAppConsole*)data->UserData;
    //return console->TextEditCallback(data);
    return 1;
}

int main(int argc, char** argv)
{
    // Auto dump memory leak info on exit
    //_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // Take a snapshot of heap memory
    _CrtMemCheckpoint(&sOld); 

    // Create main window
    ScreenSettings screenSettings;
    Window* window = new Window(&screenSettings);
    window->init();

    // Check how many vertex attributes are supported
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows


    //ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 0.0f;
    }

    // Setup Platform/Renderer backends
    const char* glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(window->get(), true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    static bool no_titlebar = false;
    static bool no_scrollbar = false;
    static bool no_menu = false;
    static bool no_move = false;
    static bool no_resize = false;
    static bool no_collapse = false;
    static bool no_close = false;
    static bool no_nav = false;
    static bool no_background = false;
    static bool no_bring_to_front = false;
    static bool no_docking = false;
    static bool unsaved_document = false;

    ImGuiWindowFlags window_flags = 0;
    if (no_titlebar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (!no_menu)           window_flags |= ImGuiWindowFlags_MenuBar;
    if (no_move)            window_flags |= ImGuiWindowFlags_NoMove;
    if (no_resize)          window_flags |= ImGuiWindowFlags_NoResize;
    if (no_collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
    if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
    if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
    if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
    if (no_docking)         window_flags |= ImGuiWindowFlags_NoDocking;
    if (unsaved_document)   window_flags |= ImGuiWindowFlags_UnsavedDocument;

    ImVec4 clear_color = ImVec4(0.3f, 0.3f, 0.3f, 0.3f);


    // Start windows keyboard shortcut listener on a thread because it will mess up the event loop
    std::thread t1(task1, window);

    // Hide the window by default
    glfwHideWindow(window->get());

    char InputBuf[256];
    
    std::string input_text;

    bool exitCalled = false;

    // Render loop
    while (!window->shouldExit() && !exitCalled)
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            static float f = 0.0f;
            static int counter = 0;

            static bool opt_fullscreen = true;
            static bool opt_padding = false;
            static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

            // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
            // because it would be confusing to have two docking targets within each others.
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
            if (opt_fullscreen)
            {
                const ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->WorkPos);
                ImGui::SetNextWindowSize(viewport->WorkSize);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }
            else
            {
                dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
            }

            // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
            // and handle the pass-thru hole, so we ask Begin() to not render a background.
            if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

            // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
            // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
            // all active windows docked into it will lose their parent and become undocked.
            // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
            // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
            if (!opt_padding)
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("DockSpace Demo", NULL, window_flags);
            if (!opt_padding)
                ImGui::PopStyleVar();

            if (opt_fullscreen)
                ImGui::PopStyleVar(2);

            // Submit the DockSpace
            //ImGuiIO& io = ImGui::GetIO();
            //if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            //{
            //    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            //    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
            //}

            if (ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Options"))
                {
                    // Disabling fullscreen would allow the window to be moved to the front of other windows,
                    // which we can't undo at the moment without finer window depth/z control.
                    ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                    ImGui::MenuItem("Padding", NULL, &opt_padding);
                    ImGui::Separator();
            
                    if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoSplit; }
                    if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoResize; }
                    if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode; }
                    if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0)) { dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar; }
                    if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0, opt_fullscreen)) { dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode; }
                    ImGui::Separator();
            
                    
                    ImGui::EndMenu();
                }
            
                ImGui::EndMenuBar();
            }

            // Reserve enough left-over height for 1 separator + 1 input text
            //const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
            const float footer_height_to_reserve = ImGui::GetFrameHeightWithSpacing();
            ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar);
            if (ImGui::BeginPopupContextWindow())
            {
                //if (ImGui::Selectable("Clear")) ClearLog();
                ImGui::EndPopup();
            }

            // Create text area
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
            for (int i = 0; i < Items.Size; i++)
            {
                const char* item = Items[i];
                //if (!Filter.PassFilter(item))
                //    continue;

                // Normally you would store more information in your item than just a string.
                // (e.g. make Items[] an array of structure, store color/type etc.)
                ImVec4 color;
                bool has_color = false;
                if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
                else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
                if (has_color)
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(item);
                if (has_color)
                    ImGui::PopStyleColor();
            }

            ImGui::PopStyleVar();
            ImGui::EndChild();
            ImGui::Separator();

            auto windowWidth = ImGui::GetWindowSize().x;
            //auto textWidth = ImGui::CalcTextSize(text.c_str()).x;
            ImGui::SetCursorPosX(screenSettings.width / 7);
            ImGui::SetCursorPosY(screenSettings.height / 1.1); // Move console input field to the bottom of the window

            

            // Command-line
            bool reclaim_focus = false;
            //if (ImGui::InputText("##", InputBuf, IM_ARRAYSIZE(InputBuf))) {
            //    char* s = InputBuf;
            //    if (s[0])
            //        std::cout << s << std::endl;
            //}
            ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
            //if (ImGui::InputText("##", InputBuf, IM_ARRAYSIZE(InputBuf), input_text_flags, &TextEditCallbackStub))
            if (ImGui::InputText("##", &input_text, input_text_flags))
            {
                //char* s = InputBuf;
                //Strtrim(s);
                //if (s[0])
                //    //ExecCommand(s);
                //    std::cout << s << std::endl;
                //strcpy_s(s, IM_ARRAYSIZE(InputBuf), "");

                std::cout << "input_text: " << input_text << std::endl;
                if (input_text.compare("exit") == 0) {
                    exitCalled = true;
                }
                else
                    AddLog("$ " + input_text);
                input_text = "";
                reclaim_focus = true;
            }

            // Auto-focus on window apparition
            ImGui::SetItemDefaultFocus();
            //if (reclaim_focus)
                ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window->get(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        //glClearColor(0.2f, 0.3f, 0.3f, 0.3f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        // Calculate delta time
        double currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Limit FPS
        sleep_time = frame_time - ( deltaTime * 1000);
        std::this_thread::sleep_for(std::chrono::microseconds(static_cast<int>(sleep_time)));

        // Update frame counter
        secondFrame += deltaTime;
        fpsCounter++;

        // Do something every second
        if (secondFrame >= 1.0f) {
            secondFrame = 0.0f;

            // Print out FPS and delta time
            std::cout << "deltaTime: " << deltaTime << std::endl;
            std::cout << "FPS: " << fpsCounter << std::endl;
            fps = fpsCounter;
            fpsCounter = 0.0f;

        }

        window->update(showWindow);

        window->swapBuffers();
        glfwPollEvents();
    }

    // Cleanup
    glfwTerminate();
    delete window;
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    

    std::cout << "Exiting.." << std::endl;

    _CrtMemCheckpoint(&sNew); //take a snapshot 
    if (_CrtMemDifference(&sDiff, &sOld, &sNew)) // if there is a difference
    {
        _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
        _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);

        OutputDebugString(L"-----------_CrtMemDumpStatistics ---------");
        _CrtMemDumpStatistics(&sDiff);
        //OutputDebugString(L"-----------_CrtMemDumpAllObjectsSince ---------");
        //_CrtMemDumpAllObjectsSince(&sOld);
        OutputDebugString(L"-----------_CrtDumpMemoryLeaks ---------");
        _CrtDumpMemoryLeaks();

        
    }

    std::exit(0);
    return 0;
}

bool registered = false;

void task1(Window* window)
{
    if (!registered) {
        if (RegisterHotKey(
            NULL,
            1,
            MOD_ALT | 0x4000, // ( 0x4000: MOD_NOREPEAT )
            0x42))  // 0x42 is 'b'
        {
            //_tprintf(_T("Hotkey 'ALT+b' registered, using MOD_NOREPEAT flag\n"));
            std::cout << "Hotkey 'ALT+b' registered, using MOD_NOREPEAT flag" << std::endl;
        }

        registered = true;
    }

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0) != 0) {
        if (msg.message == WM_HOTKEY)
        {
            //_tprintf(_T("WM_HOTKEY received\n"));
            std::wcout << "WM_HOTKEY received" << std::endl;
            showWindow = !showWindow;

        }
    }
 }

void AddLog(const std::string data, ...)// IM_FMTARGS(2)
{
    const char* fmt = data.c_str();
    // FIXME-OPT
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf) - 1] = 0;
    va_end(args);
    Items.push_back(Strdup(buf));
}


