#pragma once
#include <NovusTypes.h>
#include "angelscript.h"

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
    
    static void RegisterFunctions();
    static void MessageCallback(const asSMessageInfo* msg, void* param);
    static void Print(std::string& message);

private:
private:
    static thread_local asIScriptEngine* _scriptEngine;
    static thread_local asIScriptContext* _scriptContext;
    static thread_local std::string _scriptCurrentObjectName;
};