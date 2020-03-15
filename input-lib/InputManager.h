#include <NovusTypes.h>
#include <robin_hood.h>
#include "InputBinding.h"

class Window;
struct GLFWwindow;

typedef void MousePositionUpdateFunc(Window*, f32 x, f32 y);
class InputManager
{
public:
    InputManager();
    void KeyboardInputChecker(Window* window, i32 key, i32 scancode, i32 action, i32 modifiers);
    void MouseInputChecker(Window* window, i32 button, i32 action, i32 modifiers);
    void MousePositionUpdate(Window* window, f32 x, f32 y);
    bool RegisterBinding(std::string bindingName, i32 key, i32 actionMask, i32 modifierMask, std::function<InputBindingFunc> callback);
    bool UnregisterBinding(std::string bindingName, i32 key);

    bool RegisterMouseUpdateCallback(std::string callbackName, std::function<MousePositionUpdateFunc> callback);
    bool UnregisterMouseUpdateCallback(std::string callbackName, std::function<MousePositionUpdateFunc> callback);

    bool IsPressed(GLFWwindow* window, i32 key);
    bool IsPressed(i32 key, std::string bindingName);

    f32 GetMousePositionX() { return _mousePositionX; }
    f32 GetMousePositionY() { return _mousePositionY; }
    bool IsMousePressed() { return _mouseState; }
private:
    robin_hood::unordered_map<i32, robin_hood::unordered_map<u32, InputBinding>> _inputBindingMap;
    robin_hood::unordered_map<u32, std::function<MousePositionUpdateFunc>> _mousePositionUpdateCallbacks;
    f32 _mousePositionX = 0;
    f32 _mousePositionY = 0;
    bool _mouseState = false;
};