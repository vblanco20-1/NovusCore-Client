#include "InputManager.h"
#include <Utils/StringUtils.h>
#include <GLFW/glfw3.h>

InputManager::InputManager() : _inputBindingMap(), _bindingToInputBindingMap()
{
    // Mouse
    _inputBindingMap[GLFW_MOUSE_BUTTON_1] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_MOUSE_BUTTON_2] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_MOUSE_BUTTON_3] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_MOUSE_BUTTON_4] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_MOUSE_BUTTON_5] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_MOUSE_BUTTON_6] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_MOUSE_BUTTON_7] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_MOUSE_BUTTON_8] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();

    // Keyboard
    _inputBindingMap[GLFW_KEY_SPACE] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_APOSTROPHE] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_COMMA] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_MINUS] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_PERIOD] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_SLASH] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_0] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_1] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_2] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_3] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_4] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_5] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_6] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_7] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_8] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_9] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_SEMICOLON] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_EQUAL] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_A] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_B] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_C] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_D] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_E] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_G] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_H] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_I] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_J] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_K] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_L] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_M] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_N] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_O] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_P] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_Q] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_R] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_S] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_T] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_U] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_V] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_W] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_X] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_Y] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_Z] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_LEFT_BRACKET] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_BACKSLASH] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_RIGHT_BRACKET] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_GRAVE_ACCENT] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_WORLD_1] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_WORLD_2] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_ESCAPE] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_ENTER] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_TAB] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_BACKSPACE] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_INSERT] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_DELETE] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_RIGHT] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_LEFT] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_DOWN] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_UP] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_PAGE_UP] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_PAGE_DOWN] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_HOME] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_END] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_CAPS_LOCK] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_SCROLL_LOCK] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_NUM_LOCK] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_PRINT_SCREEN] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_PAUSE] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F1] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F2] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F3] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F4] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F5] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F6] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F7] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F8] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F9] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F10] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F11] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F12] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F13] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F14] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F15] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F16] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F17] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F18] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F19] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F20] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F21] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F22] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F23] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F24] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_F25] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_0] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_1] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_2] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_3] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_4] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_5] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_6] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_7] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_8] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_9] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_DECIMAL] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_DIVIDE] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_MULTIPLY] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_SUBTRACT] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_ADD] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_ENTER] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_KP_EQUAL] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_LEFT_SHIFT] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_LEFT_CONTROL] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_LEFT_ALT] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_LEFT_SUPER] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_RIGHT_SHIFT] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_RIGHT_CONTROL] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_RIGHT_ALT] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_RIGHT_SUPER] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();
    _inputBindingMap[GLFW_KEY_MENU] = robin_hood::unordered_map<u32, std::shared_ptr<InputBinding>>();

    // Micro Optimization (Pre Allocate Memory for 8 InputBindings)
    for (auto inputBinding : _inputBindingMap)
    {
        inputBinding.second.reserve(8);
    }

    _bindingToInputBindingMap.reserve(64);
}

void InputManager::KeyboardInputChecker(Window* window, i32 key, i32 /*scanCode*/, i32 action, i32 modifiers)
{
    for (auto kv : _inputBindingMap[key])
    {
        auto inputBinding = kv.second;

        // We always want to update the state of the binding on release as we cannot be certain that the binding has bound release as an action
        if (action == GLFW_RELEASE)
            inputBinding->state = 0;

        // Validate ActionMask and then check Modifier Mask
        bool validModifier = inputBinding->modifierMask == BINDING_MOD_ANY || inputBinding->modifierMask & modifiers || inputBinding->modifierMask == 0 && modifiers == 0;
        if ((inputBinding->actionMask & (1 << action)) && validModifier)
        {
            inputBinding->state = action == GLFW_RELEASE ? 0 : 1;

            if (!inputBinding->callback)
                continue;

            inputBinding->callback(window, inputBinding);
        }
    }
}
void InputManager::MouseInputChecker(Window* window, i32 button, i32 action, i32 modifiers)
{
    _mouseState = action == GLFW_RELEASE ? 0 : 1;

    for (auto kv : _inputBindingMap[button])
    {
        auto inputBinding = kv.second;

        // Validate ActionMask and then check Modifier Mask
        bool validModifier = inputBinding->modifierMask == BINDING_MOD_ANY || inputBinding->modifierMask & modifiers || inputBinding->modifierMask == 0 && modifiers == 0;
        if ((inputBinding->actionMask & (1 << action)) && validModifier)
        {
            inputBinding->state = action == GLFW_RELEASE ? 0 : 1;

            if (!inputBinding->callback)
                continue;

            inputBinding->callback(window, inputBinding);
        }
    }
}
void InputManager::MousePositionUpdate(Window* window, f32 x, f32 y)
{
    _mousePositionX = x;
    _mousePositionY = y;

    for (auto kv : _mousePositionUpdateCallbacks)
    {
        kv.second(window, x, y);
    }
}

bool InputManager::RegisterBinding(std::string bindingName, i32 key, i32 actionMask, i32 modifierMask, std::function<InputBindingFunc> callback)
{
    u32 hashedBindingName = StringUtils::fnv1a_32(bindingName.c_str(), bindingName.length());

    for (auto kv : _inputBindingMap)
    {
        auto iterator = kv.second.find(hashedBindingName);
        if (iterator != kv.second.end())
            return false;
    }

    std::shared_ptr<InputBinding> binding = std::make_shared<InputBinding>(bindingName, actionMask, key, modifierMask, callback);
    _inputBindingMap[key][hashedBindingName] = binding;
    _bindingToInputBindingMap[hashedBindingName] = binding;

    return true;
}

bool InputManager::UnregisterBinding(std::string bindingName)
{
    u32 hashedBindingName = StringUtils::fnv1a_32(bindingName.c_str(), bindingName.length());

    auto iterator = _bindingToInputBindingMap.find(hashedBindingName);
    if (iterator == _bindingToInputBindingMap.end())
        return false;

    _inputBindingMap[_bindingToInputBindingMap[hashedBindingName]->key].erase(hashedBindingName);
    _bindingToInputBindingMap.erase(hashedBindingName);
    return true;
}

bool InputManager::RegisterMouseUpdateCallback(std::string callbackName, std::function<MousePositionUpdateFunc> callback)
{
    u32 hashedCallbackName = StringUtils::fnv1a_32(callbackName.c_str(), callbackName.length());

    auto iterator = _mousePositionUpdateCallbacks.find(hashedCallbackName);
    if (iterator != _mousePositionUpdateCallbacks.end())
        return false;

    _mousePositionUpdateCallbacks[hashedCallbackName] = callback;
    return true;
}

bool InputManager::UnregisterMouseUpdateCallback(std::string callbackName, std::function<MousePositionUpdateFunc> callback)
{
    u32 hashedCallbackName = StringUtils::fnv1a_32(callbackName.c_str(), callbackName.length());

    auto iterator = _mousePositionUpdateCallbacks.find(hashedCallbackName);
    if (iterator == _mousePositionUpdateCallbacks.end())
        return false;

    _mousePositionUpdateCallbacks.erase(hashedCallbackName);
    return true;
}

bool InputManager::GetBinding(std::string bindingName, std::shared_ptr<InputBinding>& binding)
{
    u32 hashedBindingName = StringUtils::fnv1a_32(bindingName.c_str(), bindingName.length());

    auto iterator = _bindingToInputBindingMap.find(hashedBindingName);
    if (iterator == _bindingToInputBindingMap.end())
        return false;

    binding = _bindingToInputBindingMap[hashedBindingName];
    return true;
}

bool InputManager::IsPressed(GLFWwindow* window, i32 key)
{
    return glfwGetKey(window, key) == GLFW_RELEASE ? false : true;
}

bool InputManager::IsPressed(std::string bindingName)
{
    u32 hashedBindingName = StringUtils::fnv1a_32(bindingName.c_str(), bindingName.length());
    return IsPressed(hashedBindingName);
}
bool InputManager::IsPressed(u32 bindingNameHash)
{
    return _bindingToInputBindingMap[bindingNameHash]->state;
}
bool InputManager::IsPressed(i32 key, std::string bindingName)
{
    return _inputBindingMap[key][StringUtils::fnv1a_32(bindingName.c_str(), bindingName.length())]->state;
}

