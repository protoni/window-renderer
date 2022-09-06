#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

enum WindowPosition
{
    Top,
    RightSide,
    LeftSide,
    Bottom
};

enum AnimationDirection
{
    Up,
    Down,
    Right,
    Left
};

struct ScreenSettings
{
    // Default screen size
    unsigned int screenWidth = 1920;
    unsigned int screenHeight = 1080;

    // Default window size
    unsigned int width = 900;
    unsigned int height = 200;

    const std::string title = "OpenGLRenderer";
    float opacity = 0.9;

    // Offset in pixels from the default position ( top right corner )
    int offsetX = 0;
    int offsetY = 0;

    // Default window position
    int windowPosition = WindowPosition::Top;

    // Is window animated ( sliding in and out of view )
    bool animated = true;

    // Animating going down by default
    int animationDirection = AnimationDirection::Down;

    // Current window position
    int positionX = 0;
    int positionY = 0;

    // How many pixels / frame to move when animating
    int animationSpeed = 10;

    // Is window currently doing animation move
    bool animating = false;
};

class Window
{
public:
    Window(ScreenSettings* settings);
    ~Window();

    bool init();
    GLFWwindow* get();
    bool shouldExit();
    void swapBuffers();
    void debugMode();
    void wireframeMode();
    bool getWireframeMode();
    void setWireframeMode(bool state);
    void update(bool showWindow);
    bool isAnimating();

private:
    // Callback functions
    static void mouseCallback(GLFWwindow* window, double xposIn, double yposIn);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* m_window;
    ScreenSettings* m_windowSettings;

    bool m_debugModeOn;
    bool m_wireframeModeOn;

    // Mouse
    bool m_firstMouse;
    double m_lastX;
    double m_lastY;

    bool m_showWindow;
    

    ScreenSettings* m_settings;
};

#endif // WINDOW_H