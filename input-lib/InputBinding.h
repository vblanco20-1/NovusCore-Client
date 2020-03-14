#include <NovusTypes.h>
#include <functional>

enum BindingAction
{
    BINDING_ACTION_RELEASE = 0x01 << 0,
    BINDING_ACTION_PRESS = 0x01 << 1,
    BINDING_ACTION_REPEAT = 0x01 << 2,

    // Combinations
    BINDING_ACTION_CLICK = BINDING_ACTION_RELEASE | BINDING_ACTION_PRESS,
    BINDING_ACTION_ANY = BINDING_ACTION_RELEASE | BINDING_ACTION_PRESS | BINDING_ACTION_REPEAT
};
enum BindingModifier
{
    BINDING_MOD_NONE = 0,
    BINDING_MOD_SHIFT = 0x01 << 0,
    BINDING_MOD_CONTROL = 0x01 << 1,
    BINDING_MOD_ALT = 0x01 << 2,

    // Combinations
    BINDING_MOD_ANY = BINDING_MOD_NONE | BINDING_MOD_SHIFT | BINDING_MOD_CONTROL | BINDING_MOD_ALT
};

class Window;
class InputBinding;
typedef void InputBindingFunc(Window*, InputBinding*);
class InputBinding
{
public:
    InputBinding() : name("invalid"), actionMask(0), key(-1), modifierMask(0), callback(nullptr) { }
    InputBinding(std::string inName, i32 inActionMask, i32 inKey, i32 inModifierMask, std::function<InputBindingFunc> inCallback) : name(inName), actionMask(inActionMask), key(inKey), modifierMask(inModifierMask), callback(inCallback) { }

public:
    std::string name;
    i32 actionMask;
    i32 key;
    i32 modifierMask;
    std::function<InputBindingFunc> callback;
};