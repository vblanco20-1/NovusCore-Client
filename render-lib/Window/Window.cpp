#include "Window.h"
#include <cassert>
#include <GLFW/glfw3.h>


bool Window::_glfwInitialized = false;

Window::Window()
    : _window(nullptr), _inputManager(nullptr)
{
    
}

Window::~Window()
{
    if (_window != nullptr)
    {
        glfwDestroyWindow(_window);
    }
    if (_inputManager != nullptr)
    {
        delete _inputManager;
    }
}

void error_callback(i32 error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 modifiers)
{
    Window* userWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    userWindow->GetInputManager()->KeyboardInputChecker(userWindow, key, scancode, action, modifiers);
}
void mouse_callback(GLFWwindow* window, i32 button, i32 action, i32 modifiers)
{
    Window* userWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    userWindow->GetInputManager()->MouseInputChecker(userWindow, button, action, modifiers);
}
void cursor_position_callback(GLFWwindow* window, f64 x, f64 y)
{
    Window* userWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    userWindow->GetInputManager()->MousePositionUpdate(userWindow, x, y);
}

bool Window::Init(u32 width, u32 height)
{
    if (!_glfwInitialized)
    {
        if (!glfwInit())
        {
            assert(false);
            return false;
        }
        glfwSetErrorCallback(error_callback);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window = glfwCreateWindow(width, height, "CNovusCore", NULL, NULL);
    if (!_window)
    {
        assert(false);
        return false;
    }
    glfwSetWindowUserPointer(_window, this);

    _inputManager = new InputManager();
    _inputManager->Setup();
    glfwSetKeyCallback(_window, key_callback);
    glfwSetMouseButtonCallback(_window, mouse_callback);
    glfwSetCursorPosCallback(_window, cursor_position_callback);

    return true;
}

bool Window::Update(f32 deltaTime)
{
    glfwPollEvents();

    if (glfwWindowShouldClose(_window))
    {
        return false;
    }
    return true;
}

void Window::Present()
{
    //glfwSwapBuffers(_window);
}