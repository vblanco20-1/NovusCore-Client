#pragma once
#include <NovusTypes.h>
#include "angelscript.h"
#include <assert.h>

class ScriptEngine
{
public:
    static void Initialize();

    // GetScriptEngine will initialize the thread local engine object if needed
    static asIScriptEngine* GetScriptEngine();
    static asIScriptContext* GetScriptContext();

    static i32 RegisterScriptClass(std::string name, i32 byteSize, u32 flags);
    static i32 RegisterScriptClassFunction(std::string declaration, const asSFuncPtr& functionPointer, bool isStatic = false, void* auxiliary = 0, i32 compositeOffset = 0, bool isCompositeIndirect = false);
    static i32 RegisterScriptClassProperty(std::string declaration, i32 byteOffset, i32 compositeOffset = 0, bool isCompositeIndirect = false);
    static i32 RegisterScriptFunction(std::string declaration, const asSFuncPtr& functionPointer, void* auxiliary = 0);
    
    template <class Base, class Derived>
    static i32 RegisterScriptInheritance(std::string baseClass)
    {
        i32 r = 0;

        std::string declarationB = _scriptCurrentObjectName + "@ opCast()";
        std::string declarationD = baseClass + "@ opImplCast()";

        r = _scriptEngine->RegisterObjectMethod(baseClass.c_str(), declarationB.c_str(), asFUNCTION((refCast<Base, Derived>)), asCALL_CDECL_OBJLAST); assert(r >= 0);
        r = _scriptEngine->RegisterObjectMethod(_scriptCurrentObjectName.c_str(), declarationD.c_str(), asFUNCTION((refCast<Derived, Base>)), asCALL_CDECL_OBJLAST); assert(r >= 0);

        // Also register the const overloads so the cast works also when the handle is read only 
        declarationB = "const " + _scriptCurrentObjectName + "@ opCast() const";
        declarationD = "const " + baseClass + "@ opImplCast() const";

        r = _scriptEngine->RegisterObjectMethod(baseClass.c_str(), declarationB.c_str(), asFUNCTION((refCast<Base, Derived>)), asCALL_CDECL_OBJLAST); assert(r >= 0);
        r = _scriptEngine->RegisterObjectMethod(_scriptCurrentObjectName.c_str(), declarationD.c_str(), asFUNCTION((refCast<Derived, Base>)), asCALL_CDECL_OBJLAST); assert(r >= 0);

        Base::RegisterBase<Derived>();
        return r;
    }

    template <class A, class B>
    static B* refCast(A* a)
    {
        // If the handle already is a null handle, then just return the null handle
        if (!a) return 0;

        // Now try to dynamically cast the pointer to the wanted type
        return dynamic_cast<B*>(a);
    }

     
    static void RegisterFunctions();
    static void MessageCallback(const asSMessageInfo* msg, void* param);
    static void Print(std::string& message);

private:
private:
    static thread_local asIScriptEngine* _scriptEngine;
    static thread_local asIScriptContext* _scriptContext;
    static thread_local std::string _scriptCurrentObjectName;
};