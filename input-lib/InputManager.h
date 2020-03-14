#include <NovusTypes.h>
#include <robin_hood.h>
#include "InputBinding.h"

class InputManager
{
public:
    void Setup();
    void KeyboardInputChecker(Window* window, i32 key, i32 scancode, i32 action, i32 modifiers);
    void MouseInputChecker(Window* window, i32 button, i32 action, i32 modifiers);
    void MousePositionUpdate(Window* window, f64 x, f64 y);
    bool RegisterBinding(std::string bindingName, i32 key, i32 actionMask, i32 modifierMask, std::function<InputBindingFunc> callback);
    bool UnregisterBinding(std::string bindingName, i32 key);

    f64 GetMousePositionX() { return _mousePositionX; }
    f64 GetMousePositionY() { return _mousePositionY; }
private:
    robin_hood::unordered_map<i32, std::vector<InputBinding>> _inputBindingMap;
    f64 _mousePositionX = 0;
    f64 _mousePositionY = 0;
};