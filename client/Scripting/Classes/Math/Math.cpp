#include "Math.h"
#include <angelscript.h>
#include <Utils/DebugHandler.h>
#include "../../ScriptEngine.h"
#include "GlmRegistrationUtil/GlmRegistrationUtil.h"

namespace ASMath
{
    void RegisterNamespace()
    {
        i32 r = ScriptEngine::SetNamespace("Math");
        assert(r >= 0);
        {

        }

        r = ScriptEngine::ResetNamespace();
        assert(r >= 0);

        asIScriptEngine* scriptEngine = ScriptEngine::GetScriptEngine();
        AngelScriptIntegration::RegisterGlmVectors(scriptEngine);
    }
}
