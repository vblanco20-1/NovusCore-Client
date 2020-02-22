#pragma once
#include <NovusTypes.h>
#include <angelscript.h>
#include <Utils/DebugHandler.h>

class Player
{
public:
    Player(f32 x) : x(x), y(x), z(x) {}
    static void RegisterType();

private:
    void Print();

private:
    static const char* _objectName;
    f32 x;
    f32 y;
    f32 z;
};