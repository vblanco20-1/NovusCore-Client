#include "MovementSystem.h"
#include <InputManager.h>
#include <Utils/ByteBuffer.h>
#include <Utils/StringUtils.h>
#include <Networking/Opcode.h>
#include "../../Utils/ServiceLocator.h"
#include "../../Rendering/CameraFreelook.h"
#include "../Components/Singletons/TimeSingleton.h"
#include "../Components/Network/ConnectionSingleton.h"
#include "../Components/LocalplayerSingleton.h"
#include "../Components/Transform.h"

void MovementSystem::Update(entt::registry& registry)
{
    LocalplayerSingleton& localplayerSingleton = registry.ctx<LocalplayerSingleton>();
    if (localplayerSingleton.entity == entt::null)
        return;

    InputManager* inputManager = ServiceLocator::GetInputManager();
    Transform& transform = registry.get<Transform>(localplayerSingleton.entity);

    MovementFlags originalFlags = transform.moveFlags;
    transform.moveFlags = MovementFlags::NONE;

    bool movingForward = transform.HasMoveFlag(MovementFlags::FORWARD);
    if (inputManager->IsKeyPressed("Move Forward"_h) && !movingForward)
    {
        transform.AddMoveFlag(MovementFlags::FORWARD);
    }
    else if (movingForward)
    {
        transform.RemoveMoveFlag(MovementFlags::FORWARD);
    }

    bool movingBackward = transform.HasMoveFlag(MovementFlags::BACKWARD);
    if (inputManager->IsKeyPressed("Move Backward"_h) && !movingBackward)
    {
        transform.AddMoveFlag(MovementFlags::BACKWARD);
    }
    else if (movingBackward)
    {
        transform.RemoveMoveFlag(MovementFlags::BACKWARD);
    }

    bool movingLeft = transform.HasMoveFlag(MovementFlags::LEFT);
    if (inputManager->IsKeyPressed("Move Left"_h) && !movingLeft)
    {
        transform.AddMoveFlag(MovementFlags::LEFT);
    }
    else if (movingLeft)
    {
        transform.RemoveMoveFlag(MovementFlags::LEFT);
    }

    bool movingRight = transform.HasMoveFlag(MovementFlags::RIGHT);
    if (inputManager->IsKeyPressed("Move Right"_h) && !movingRight)
    {
        transform.AddMoveFlag(MovementFlags::RIGHT);
    }
    else if(movingRight)
    {
        
        transform.RemoveMoveFlag(MovementFlags::RIGHT);
    }


    // If we are moving in directions that counter themselves unset them
    if (transform.HasMoveFlag(MovementFlags::ALL))
    {
        transform.RemoveMoveFlag(MovementFlags::ALL);
    }
    else if (transform.HasMoveFlag(MovementFlags::VERTICAL))
    {
        transform.RemoveMoveFlag(MovementFlags::VERTICAL);
    }
    else if (transform.HasMoveFlag(MovementFlags::HORIZONTAL))
    {
        transform.RemoveMoveFlag(MovementFlags::HORIZONTAL);
    }

    if (transform.moveFlags != MovementFlags::NONE)
        transform.RemoveMoveFlag(MovementFlags::NONE);


    // Tell server we stopped moving
    if (transform.HasMoveFlag(MovementFlags::NONE))
    {
        // Make sure we only send stop once
        if (transform.moveFlags != originalFlags)
        {
            ConnectionSingleton& connectionSingleton = registry.ctx<ConnectionSingleton>();

            std::shared_ptr<Bytebuffer> buffer = Bytebuffer::Borrow<128>();
            buffer->Put(Opcode::MSG_MOVE_STOP_ENTITY);
            buffer->PutU16(32);

            buffer->Put(localplayerSingleton.entity);
            buffer->Put(transform.moveFlags);
            buffer->Put(transform.position);
            buffer->Put(transform.rotation);
            connectionSingleton.gameConnection->Send(buffer);

            transform.isDirty = true;
        }
    }
    else
    {
        // Send Packet
        ConnectionSingleton& connectionSingleton = registry.ctx<ConnectionSingleton>();

        std::shared_ptr<Bytebuffer> buffer = Bytebuffer::Borrow<128>();
        if (transform.moveFlags != originalFlags)
        {
            buffer->Put(Opcode::MSG_MOVE_ENTITY);
        }
        else
        {
            buffer->Put(Opcode::MSG_MOVE_HEARTBEAT_ENTITY);
        }

        buffer->PutU16(32);

        buffer->Put(localplayerSingleton.entity);
        buffer->Put(transform.moveFlags);
        buffer->Put(transform.position);
        buffer->Put(transform.rotation);

        connectionSingleton.gameConnection->Send(buffer);

        transform.isDirty = true;
    }
}
