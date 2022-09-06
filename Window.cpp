#include "Window.h"

#include <iostream>

#include "DebugMacros.h"

/*
Window position's ( 0, 0 ) coordinates are in top left corner
*/

Window::Window(ScreenSettings* settings) :
    m_window(NULL), m_windowSettings(settings),
    m_firstMouse(true), m_lastX(m_windowSettings->width / 2.0), m_lastY(m_windowSettings->height / 2.0),
    m_debugModeOn(false), m_wireframeModeOn(false), m_showWindow(false)
{

}

Window::~Window()
{

}

bool Window::init()
{
    std::cout << "Init OpenGLRenderer" << std::endl;

    glfwInit();

    // Get screen size
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    // Update default screen width
    m_windowSettings->screenWidth = mode->width;
    m_windowSettings->screenHeight = mode->height;

    // Disable window borders
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    
    

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
    // Create a window
    m_window = glfwCreateWindow(m_windowSettings->width, m_windowSettings->height, m_windowSettings->title.c_str(), NULL, NULL);
    if (m_window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    
    // Init GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD!" << std::endl;
        return false;
    }

    // Set window viewport dimensions
    glViewport(0, 0, m_windowSettings->width, m_windowSettings->height);

    // Enable blending ( transparency ) and set window transparent
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
    glfwSetWindowOpacity(m_window, m_windowSettings->opacity);

    // Set window resize callback
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

    // Disable cursor
    //glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set mouse input callback
    glfwSetCursorPosCallback(m_window, mouseCallback);

    // Set mouse scroll callback
    glfwSetScrollCallback(m_window, scrollCallback);

    // Set pointer to this object
    glfwSetWindowUserPointer(m_window, this);

    // Set initial window position
    m_windowSettings->positionX = ((mode->width / 2) - (m_windowSettings->width / 2)) - m_windowSettings->offsetX;
    m_windowSettings->positionY = m_windowSettings->offsetY;
    glfwSetWindowPos(m_window, m_windowSettings->positionX, m_windowSettings->positionY);
    
    return true;
}

void Window::mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    Window* windowObject = static_cast<Window*>(glfwGetWindowUserPointer(window));

    if (windowObject->m_debugModeOn) {
        windowObject->m_firstMouse = true;
        return;
    }

}

bool Window::shouldExit()
{
    return glfwWindowShouldClose(m_window);
}

void Window::swapBuffers()
{
    glfwSwapBuffers(m_window);
}

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

}

void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

    theWindow->m_windowSettings->width = width;
    theWindow->m_windowSettings->height = height;

    glViewport(0, 0, width, height);
}

GLFWwindow* Window::get()
{
    return m_window;
}

void Window::debugMode()
{
    m_debugModeOn = !m_debugModeOn;

    if (m_debugModeOn) {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

bool Window::getWireframeMode()
{
    return m_wireframeModeOn;
}

void Window::setWireframeMode(bool state)
{
    m_wireframeModeOn = state;

    if (m_wireframeModeOn) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Window::update(bool showWindow)
{

    // Handle show/hide window only if the state has changed
    if (showWindow != m_showWindow && !m_windowSettings->animating) {
        if (showWindow) {
            glfwShowWindow(m_window);
            std::cout << "show window!!" << std::endl;
        }
        //else {
        //    glfwHideWindow(m_window);
        //    std::cout << "hide window!!" << std::endl;
        //}
    }

    // Handle animation
    if (m_windowSettings->animated) {
    
        // Showing
        if (showWindow && !m_showWindow) {
            
            // Set animation state on
            m_windowSettings->animating = true;
    
            // Set window to the start of animation position
            if (m_windowSettings->windowPosition == WindowPosition::Top) {
                std::cout << "Set direction down" << std::endl;
                // Set animation direction down
                m_windowSettings->animationDirection = AnimationDirection::Down;

                // Move window up beyond the screen border by the height of the window size
                m_windowSettings->positionY = m_windowSettings->height * -1;
                glfwSetWindowPos(m_window, m_windowSettings->positionX, m_windowSettings->positionY);
            }
            ///else if (m_windowSettings->windowPosition == WindowPosition::RightSide) {
            ///
            ///    // Set animation direction to left
            ///    m_windowSettings->animationDirection = AnimationDirection::Left;
            ///
            ///    // Move window to right beyond the screen border by the height of the window size
            ///    m_windowSettings->positionX = m_windowSettings->screenWidth + m_windowSettings->width;
            ///    glfwSetWindowPos(m_window, m_windowSettings->positionX, m_windowSettings->positionY);
            ///}
            ///else if (m_windowSettings->windowPosition == WindowPosition::Bottom) {
            ///
            ///    // Set animation direction to up
            ///    m_windowSettings->animationDirection = AnimationDirection::Up;
            ///
            ///    // Move window down beyond the screen border by the height of the window size
            ///    m_windowSettings->positionY = m_windowSettings->screenHeight + m_windowSettings->height;
            ///    glfwSetWindowPos(m_window, m_windowSettings->positionX, m_windowSettings->positionY);
            ///}
            ///else if (m_windowSettings->windowPosition == WindowPosition::LeftSide) {
            ///
            ///    // Set animation direction to right
            ///    m_windowSettings->animationDirection = AnimationDirection::Right;
            ///
            ///    // Move window to left beyond the screen border by the width of the window size
            ///    m_windowSettings->positionX = m_windowSettings->width * -1;
            ///    glfwSetWindowPos(m_window, m_windowSettings->positionX, m_windowSettings->positionY);
            ///}
        }
    
        // Hiding
        else if (!showWindow && m_showWindow) {
    
            // Set animation state on
            m_windowSettings->animating = true;

            // Set window to the start of animation position
            if (m_windowSettings->windowPosition == WindowPosition::Top) {
                
                // Set animation direction up
                m_windowSettings->animationDirection = AnimationDirection::Up;

                // Move window down below the screen border by the height of the window size
                m_windowSettings->positionY = 0;
                glfwSetWindowPos(m_window, m_windowSettings->positionX, m_windowSettings->positionY);
            }
            ///else if (m_windowSettings->windowPosition == WindowPosition::RightSide) {
            ///
            ///    // Set animation direction to right
            ///    m_windowSettings->animationDirection = AnimationDirection::Right;
            ///
            ///    // Move window to left of the screen border by the width of the window size
            ///    m_windowSettings->positionX = m_windowSettings->screenWidth - m_windowSettings->width;
            ///    glfwSetWindowPos(m_window, m_windowSettings->positionX, m_windowSettings->positionY);
            ///}
            ///else if (m_windowSettings->windowPosition == WindowPosition::Bottom) {
            ///
            ///    // Set animation direction to down
            ///    m_windowSettings->animationDirection = AnimationDirection::Down;
            ///
            ///    // Move window down beyond the screen border by the height of the window size
            ///    m_windowSettings->positionY = m_windowSettings->screenHeight - m_windowSettings->height;
            ///    glfwSetWindowPos(m_window, m_windowSettings->positionX, m_windowSettings->positionY);
            ///}
            ///else if (m_windowSettings->windowPosition == WindowPosition::LeftSide) {
            ///
            ///    // Set animation direction to left
            ///    m_windowSettings->animationDirection = AnimationDirection::Left;
            ///
            ///    // Move window to left beyond the screen border by the width of the window size
            ///    m_windowSettings->positionX = m_windowSettings->width;
            ///    glfwSetWindowPos(m_window, m_windowSettings->positionX, m_windowSettings->positionY);
            ///}
        }
    
        // If currently animating
        if (m_windowSettings->animating) {

            // Check position and stop animating if position exceeds the animation limit
            if (m_windowSettings->animationDirection == AnimationDirection::Up) {
                int negative = m_windowSettings->height * -1;

                if (m_windowSettings->positionY <= negative) {
                    m_windowSettings->animating = false;
                    
                    std::cout << "limit: " << negative << std::endl;
                    std::cout << "Stop animating" << std::endl;
                    glfwHideWindow(m_window);
                    std::cout << "hide window!" << std::endl;
                }
            }
            else if (m_windowSettings->animationDirection == AnimationDirection::Down) {
                if (m_windowSettings->positionY >= 0) {
                    m_windowSettings->animating = false;
                    std::cout << "Stop animating" << std::endl;
                    glfwShowWindow(m_window);
                    std::cout << "show window!" << std::endl;
                }
            }

            // If still animating
            if (m_windowSettings->animating) {
                if (m_windowSettings->animationDirection == AnimationDirection::Up) {
                    m_windowSettings->positionY -= m_windowSettings->animationSpeed;
                    //std::cout << "Update up" << std::endl;
                }

                else if (m_windowSettings->animationDirection == AnimationDirection::Down) {
                    m_windowSettings->positionY += m_windowSettings->animationSpeed;
                    //std::cout << "Update down" << std::endl;
                }

                glfwSetWindowPos(m_window, m_windowSettings->positionX, m_windowSettings->positionY);
            }
        }

    }


    //std::cout << "windowX: " << m_windowSettings->positionX << ", windowY: " << m_windowSettings->positionY << std::endl;
    
    

    m_showWindow = showWindow;
}

bool Window::isAnimating()
{
    return m_windowSettings->animating;
}
