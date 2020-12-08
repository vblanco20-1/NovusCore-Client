#include "Editor.h"
#include "../Utils/ServiceLocator.h"
#include "../Utils/MapUtils.h"

#include "../Rendering/ClientRenderer.h"
#include "../Rendering/TerrainRenderer.h"
#include "../Rendering/MapObjectRenderer.h"
#include "../Rendering/CModelRenderer.h"
#include "../Rendering/DebugRenderer.h"
#include "../Rendering/CameraFreelook.h"
#include "../ECS/Components/Singletons/NDBCSingleton.h"
#include "../ECS/Components/Singletons/MapSingleton.h"
#include "../ECS/Components/Singletons/TextureSingleton.h"
#include <CVar/CVarSystem.h>
#include <InputManager.h>
#include <GLFW/glfw3.h>
#include <entt.hpp>

#include <glm/gtx/matrix_decompose.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include "imgui/misc/cpp/imgui_stdlib.h"

namespace Editor
{
    AutoCVar_Int CVAR_EditorEnabled("editor.Enable", "enable editor mode for the client", 1, CVarFlags::EditCheckbox);

    AutoCVar_Int CVAR_EditorRayEnabled("editor.ray.Enable", "enable ray for debugging screen to world", 0, CVarFlags::EditCheckbox);
    AutoCVar_VecFloat CVAR_EditorRayStartPosition("editor.ray.StartPosition", "", vec4(0, 0, 0, 0), CVarFlags::Noedit);
    AutoCVar_VecFloat CVAR_EditorRayEndPosition("editor.ray.EndPosition", "", vec4(0, 0, 0, 0), CVarFlags::Noedit);

    Editor::Editor()
    {
        InputManager* inputManager = ServiceLocator::GetInputManager();
        inputManager->RegisterKeybind("Editor: Mouse Left Click", GLFW_MOUSE_BUTTON_LEFT, KEYBIND_ACTION_RELEASE, KEYBIND_MOD_NONE | KEYBIND_MOD_SHIFT, std::bind(&Editor::OnMouseClickLeft, this, std::placeholders::_1, std::placeholders::_2));
    }

    void Editor::Update(f32 deltaTime)
    {
        ClientRenderer* clientRenderer = ServiceLocator::GetClientRenderer();
        DebugRenderer* debugRenderer = clientRenderer->GetDebugRenderer();

        if (CVAR_EditorRayEnabled.Get())
        {
            debugRenderer->DrawLine3D(CVAR_EditorRayStartPosition.Get(), CVAR_EditorRayEndPosition.Get(), 0x00FF00FF);
        }

        if (ImGui::Begin("Editor Info"))
        {
            if (_selectedBoundingBox.type != SelectedBoundingBoxType::NONE)
            {
                if (_selectedBoundingBox.type == SelectedBoundingBoxType::TERRAIN)
                {
                    HandleTerrainBoundingBox(debugRenderer);
                }
                else if (_selectedBoundingBox.type == SelectedBoundingBoxType::MAP_OBJECT)
                {

                    ClientRenderer* clientRenderer = ServiceLocator::GetClientRenderer();
                    TerrainRenderer* terrainRenderer = clientRenderer->GetTerrainRenderer();
                    MapObjectRenderer* mapObjectRenderer = terrainRenderer->GetMapObjectRenderer();
                    const std::vector<Terrain::PlacementDetails>& mapObjectPlacementDetails = mapObjectRenderer->GetPlacementDetails();
                    const std::vector<MapObjectRenderer::LoadedMapObject>& loadedMapObjects = mapObjectRenderer->GetLoadedMapObjects();

                    const Terrain::PlacementDetails& placementDetails = mapObjectPlacementDetails[_selectedMapObjectData.placementDetailsIndex];
                    const MapObjectRenderer::LoadedMapObject& loadedMapObject = loadedMapObjects[placementDetails.loadedIndex];
                    const mat4x4& instanceMatrix = mapObjectRenderer->GetInstances()[placementDetails.instanceIndex].instanceMatrix;

                    glm::vec3 scale;
                    glm::quat rotation;
                    glm::vec3 translation;
                    glm::vec3 skew;
                    glm::vec4 perspective;
                    glm::decompose(instanceMatrix, scale, rotation, translation, skew, perspective);

                    glm::vec3 euler = glm::eulerAngles(rotation);
                    glm::vec3 eulerAsDeg = glm::degrees(euler);

                    ImGui::Text("Map Object");
                    ImGui::Text("Model: %s", loadedMapObject.debugName.c_str());
                    ImGui::Text("Position: X: %.2f, Y: %.2f, Z: %.2f", translation.x, translation.y, translation.z);
                    ImGui::Text("Scale: X: %.2f, Y: %.2f, Z: %.2f", scale.x, scale.y, scale.z);
                    ImGui::Text("Rotation: X: %.2f, Y: %.2f, Z: %.2f", eulerAsDeg.x, eulerAsDeg.y, eulerAsDeg.z);
                }
                else if (_selectedBoundingBox.type == SelectedBoundingBoxType::COMPLEX_MODEL)
                {

                    ImGui::Text("COMPLEX MODEL");
                }

                debugRenderer->DrawAABB3D(_selectedBoundingBox.aabb.min, _selectedBoundingBox.aabb.max, 0xFFFF00FF);
            }
            else
            {
                ImGui::TextWrapped("Welcome to the editor window. In the editor window you can see information about what you are currently viewing. To start viewing, click on a map tile, map object or complex model.");
            }

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::TextWrapped("You can clear your selection by using 'Shift + Mouse Left'");
        }

        ImGui::End();

        _ndbcEditorHandler.Draw();
    }

    void Editor::DrawImguiMenuBar()
    {
        if (ImGui::BeginMenu("Editor"))
        {
            _ndbcEditorHandler.DrawImGuiMenuBar();

            ImGui::EndMenu();
        }
    }

    void Editor::HandleTerrainBoundingBox(DebugRenderer* debugRenderer)
    {
        entt::registry* registry = ServiceLocator::GetGameRegistry();
        MapSingleton& mapSingleton = registry->ctx<MapSingleton>();

        if (!_selectedObjectDataInitialized)
        {
            _selectedObjectDataInitialized = true;
            _selectedTerrainData.center = (_selectedBoundingBox.aabb.min + _selectedBoundingBox.aabb.max) / vec3(2.0f, 2.0f, 2.0f);

            _selectedTerrainData.adtCoords = Terrain::MapUtils::WorldPositionToADTCoordinates(_selectedTerrainData.center);
            _selectedTerrainData.chunkCoords = Terrain::MapUtils::GetChunkFromAdtPosition(_selectedTerrainData.adtCoords);

            vec2 chunkCoordsFloored = glm::floor(_selectedTerrainData.chunkCoords);
            vec2 chunkRemainder = _selectedTerrainData.chunkCoords - chunkCoordsFloored;

            _selectedTerrainData.chunkWorldPos.x = Terrain::MAP_HALF_SIZE - (chunkCoordsFloored.y * Terrain::MAP_CHUNK_SIZE);
            _selectedTerrainData.chunkWorldPos.y = Terrain::MAP_HALF_SIZE - (chunkCoordsFloored.x * Terrain::MAP_CHUNK_SIZE);
            _selectedTerrainData.chunkId = Terrain::MapUtils::GetChunkIdFromChunkPos(_selectedTerrainData.chunkCoords);

            _selectedTerrainData.cellCoords = (chunkRemainder * Terrain::MAP_CHUNK_SIZE) / Terrain::MAP_CELL_SIZE;
            _selectedTerrainData.cellId = Terrain::MapUtils::GetCellIdFromCellPos(_selectedTerrainData.cellCoords);

            auto itr = mapSingleton.currentMap.chunks.find(_selectedTerrainData.chunkId);
            if (itr == mapSingleton.currentMap.chunks.end())
                return;

            _selectedTerrainData.chunk = &itr->second;
            _selectedTerrainData.cell = &_selectedTerrainData.chunk->cells[_selectedTerrainData.cellId];

            _selectedTerrainData.zone = _selectedTerrainData.cell ? mapSingleton.areaIdToDBC[_selectedTerrainData.cell->areaId] : nullptr;
            _selectedTerrainData.area = nullptr;
            
            if (_selectedTerrainData.zone && _selectedTerrainData.zone->parentId)
            {
                _selectedTerrainData.area = _selectedTerrainData.zone;
                _selectedTerrainData.zone = mapSingleton.areaIdToDBC[_selectedTerrainData.area->parentId];
            }
        }

        NDBCSingleton& ndbcSingleton = registry->ctx<NDBCSingleton>();
        const NDBC::File& areaTableFile = ndbcSingleton.nameHashToDBCFile["AreaTable"_h];

        // Draw AABB of current Chunk
        {
            if (_selectedTerrainData.chunk)
            {
                Geometry::AABoundingBox chunkAABB;
                chunkAABB.min = vec3(_selectedTerrainData.chunkWorldPos.x, _selectedTerrainData.chunkWorldPos.y, _selectedTerrainData.chunk->heightHeader.gridMinHeight);
                chunkAABB.max = vec3(_selectedTerrainData.chunkWorldPos.x - Terrain::MAP_CHUNK_SIZE, _selectedTerrainData.chunkWorldPos.y - Terrain::MAP_CHUNK_SIZE, _selectedTerrainData.chunk->heightHeader.gridMaxHeight);

                debugRenderer->DrawAABB3D(chunkAABB.min, chunkAABB.max, 0xFF0000FF);
            }
        }

        Terrain::Chunk* chunk = _selectedTerrainData.chunk;
        Terrain::Cell* cell = _selectedTerrainData.cell;

        if (!chunk || !cell)
            return;

        const NDBC::AreaTable* zone = _selectedTerrainData.zone;
        const NDBC::AreaTable* area = _selectedTerrainData.area;

        if (zone && zone->parentId)
        {
            area = zone;
            zone = mapSingleton.areaIdToDBC[area->parentId];
        }

        ImGui::Text("Selected Chunk (%u)", _selectedTerrainData.chunkId);
        ImGui::BulletText("Zone: %s", zone ? areaTableFile.stringTable->GetString(zone->name).c_str() : "No Zone Name");
        ImGui::BulletText("Map Object Placements: %u", chunk->mapObjectPlacements.size());
        ImGui::BulletText("Complex Model Placements: %u", chunk->complexModelPlacements.size());

        ImGui::Spacing();
        ImGui::Spacing();

        bool hasLiquid = chunk->liquidHeaders.size() > 0 ? chunk->liquidHeaders[_selectedTerrainData.cellId].packedData != 0 : false;
        ImGui::Text("Selected Cell (%u)", _selectedTerrainData.cellId);
        ImGui::BulletText("Area: %s", area ? areaTableFile.stringTable->GetString(area->name).c_str() : "No Area Name");
        ImGui::BulletText("Area Id: %u, Has Holes: %u, Has Liquid: %u", cell->areaId, cell->hole > 0, hasLiquid);

        ImGui::Spacing();
        ImGui::Spacing();

        TextureSingleton& textureSingleton = registry->ctx<TextureSingleton>();
        for (u32 i = 0; i < 4; i++)
        {
            if (cell)
            {
                Terrain::LayerData& layerData = cell->layers[i];
                if (layerData.textureId != layerData.TextureIdInvalid)
                {
                    const std::string& texture = textureSingleton.textureHashToPath[layerData.textureId];
                    ImGui::BulletText("Texture %u: %s", i, texture.c_str());
                    continue;
                }
            }
                
            ImGui::BulletText("Texture %u: Unused", i);
        }
    }

    bool Editor::IsRayIntersectingComplexModel(const vec3& rayOrigin, const vec3& oneOverRayDir, const Geometry::AABoundingBox* terrainAABB, f32& t)
    {
        entt::registry* registry = ServiceLocator::GetGameRegistry();
        MapSingleton& mapSingleton = registry->ctx<MapSingleton>();

        ClientRenderer* clientRenderer = ServiceLocator::GetClientRenderer();
        TerrainRenderer* terrainRenderer = clientRenderer->GetTerrainRenderer();
        CModelRenderer* cmodelRenderer = terrainRenderer->GetComplexModelRenderer();

        vec3 aabbCenter = (_selectedBoundingBox.aabb.min + _selectedBoundingBox.aabb.max) / vec3(2.0f, 2.0f, 2.0f);
        vec2 adtCoords = Terrain::MapUtils::WorldPositionToADTCoordinates(aabbCenter);
        vec2 chunkCoords = Terrain::MapUtils::GetChunkFromAdtPosition(adtCoords);
        u32 chunkId = Terrain::MapUtils::GetChunkIdFromChunkPos(chunkCoords);

        const Terrain::Chunk& chunk = mapSingleton.currentMap.chunks[chunkId];
        u32 numPlacementsOnChunk = static_cast<u32>(chunk.complexModelPlacements.size());
        u32 chunkMapPlacementDetailsOffset = cmodelRenderer->GetChunkPlacementDetailsOffset(chunkId);
        const std::vector<Terrain::PlacementDetails>& complexModelPlacementDetails = cmodelRenderer->GetPlacementDetails();

        std::vector<Geometry::AABoundingBox> intersectedBoundingBoxes;
        std::vector<f32> timeToIntersection;
        std::vector<u32> placementDetailIndices;

        intersectedBoundingBoxes.reserve(16);
        timeToIntersection.reserve(16);
        placementDetailIndices.reserve(16);

        Geometry::AABoundingBox* selectedBoundingBox = nullptr;

        f32 timeToIntersect = 0;

        const std::vector<CModelRenderer::LoadedComplexModel>& loadedComplexModels = cmodelRenderer->GetLoadedComplexModels();
        const std::vector<CModelRenderer::Instance>& complexModelInstances = cmodelRenderer->GetInstances();
        const std::vector<CModel::CullingData>& complexModelCullingDatas = cmodelRenderer->GetCullingData();

        for (u32 i = chunkMapPlacementDetailsOffset; i < numPlacementsOnChunk + chunkMapPlacementDetailsOffset; i++)
        {
            const Terrain::PlacementDetails& placementDetails = complexModelPlacementDetails[i];

            const CModelRenderer::LoadedComplexModel& loadedComplexModel = loadedComplexModels[placementDetails.loadedIndex];
            const CModelRenderer::Instance& instanceData = complexModelInstances[placementDetails.instanceIndex];

            const mat4x4& m = instanceData.instanceMatrix;

            // Transform extents (take maximum)
            glm::mat3x3 absMatrix = glm::mat3x3(glm::abs(vec3(m[0])), glm::abs(vec3(m[1])), glm::abs(vec3(m[2])));

            Geometry::AABoundingBox complexModelAABB;
            complexModelAABB.min = vec3(Terrain::MAP_SIZE, Terrain::MAP_SIZE, Terrain::MAP_SIZE);
            complexModelAABB.max = vec3(-Terrain::MAP_SIZE, -Terrain::MAP_SIZE, -Terrain::MAP_SIZE);
            f32 smallestT = 10000.0f;
            bool didPush = false;

            for (const CModelRenderer::DrawCallData& opaqueDrawCallData : loadedComplexModel.opaqueDrawCallDataTemplates)
            {
                const CModel::CullingData& cullingData = complexModelCullingDatas[opaqueDrawCallData.cullingDataID];

                vec3 center = (cullingData.minBoundingBox + cullingData.maxBoundingBox) * f16(0.5f);
                vec3 extents = vec3(cullingData.maxBoundingBox) - center;

                // transform center
                vec3 transformedCenter = vec3(m * vec4(center, 1.0f));
                vec3 transformedExtents = absMatrix * extents;

                // Transform to min/max box representation
                Geometry::AABoundingBox aabb;
                aabb.min = transformedCenter - transformedExtents;
                aabb.max = transformedCenter + transformedExtents;

                for (u32 j = 0; j < 3; j++)
                {
                    if (aabb.min[j] < complexModelAABB.min[j])
                        complexModelAABB.min[j] = aabb.min[j];

                    if (aabb.max[j] > complexModelAABB.max[j])
                        complexModelAABB.max[j] = aabb.max[j];
                }

                if (IsRayIntersectingAABB(rayOrigin, oneOverRayDir, aabb, t))
                {
                    if (!didPush)
                    {
                        didPush = true;

                        if (t < smallestT)
                            smallestT = t;
                    }
                }
            }

            for (const CModelRenderer::DrawCallData& transparentDrawCallData : loadedComplexModel.transparentDrawCallDataTemplates)
            {
                const CModel::CullingData& cullingData = complexModelCullingDatas[transparentDrawCallData.cullingDataID];

                vec3 center = (cullingData.minBoundingBox + cullingData.maxBoundingBox) * f16(0.5f);
                vec3 extents = vec3(cullingData.maxBoundingBox) - center;

                // transform center
                vec3 transformedCenter = vec3(m * vec4(center, 1.0f));
                vec3 transformedExtents = absMatrix * extents;

                // Transform to min/max box representation
                Geometry::AABoundingBox aabb;
                aabb.min = transformedCenter - transformedExtents;
                aabb.max = transformedCenter + transformedExtents;

                for (u32 j = 0; j < 3; j++)
                {
                    if (aabb.min[j] < complexModelAABB.min[j])
                        complexModelAABB.min[j] = aabb.min[j];

                    if (aabb.max[j] > complexModelAABB.max[j])
                        complexModelAABB.max[j] = aabb.max[j];
                }

                if (IsRayIntersectingAABB(rayOrigin, oneOverRayDir, aabb, t))
                {
                    if (!didPush)
                    {
                        didPush = true;

                        if (t < smallestT)
                            smallestT = t;
                    }
                }
            }

            if (didPush)
            {
                intersectedBoundingBoxes.push_back(complexModelAABB);
                timeToIntersection.push_back(smallestT);
                placementDetailIndices.push_back(i);
            }
        }

        f32 smallestT = std::numeric_limits<f32>().max();
        for (u32 i = 0; i < intersectedBoundingBoxes.size(); i++)
        {
            f32& currentT = timeToIntersection[i];
            if (currentT < smallestT)
            {
                smallestT = currentT;
                selectedBoundingBox = &intersectedBoundingBoxes[i];
            }
        }

        if (selectedBoundingBox)
        {
            _selectedBoundingBox.type = SelectedBoundingBoxType::COMPLEX_MODEL;
            _selectedBoundingBox.aabb.min = selectedBoundingBox->min;
            _selectedBoundingBox.aabb.max = selectedBoundingBox->max;
        }

        return selectedBoundingBox != nullptr;
    }
    bool Editor::IsRayIntersectingMapObject(const vec3& rayOrigin, const vec3& oneOverRayDir, const Geometry::AABoundingBox* terrainAABB, f32& t)
    {
        entt::registry* registry = ServiceLocator::GetGameRegistry();
        MapSingleton& mapSingleton = registry->ctx<MapSingleton>();

        ClientRenderer* clientRenderer = ServiceLocator::GetClientRenderer();
        TerrainRenderer* terrainRenderer = clientRenderer->GetTerrainRenderer();
        MapObjectRenderer* mapObjectRenderer = terrainRenderer->GetMapObjectRenderer();

        vec3 aabbCenter = (_selectedBoundingBox.aabb.min + _selectedBoundingBox.aabb.max) / vec3(2.0f, 2.0f, 2.0f);
        vec2 adtCoords = Terrain::MapUtils::WorldPositionToADTCoordinates(aabbCenter);
        vec2 chunkCoords = Terrain::MapUtils::GetChunkFromAdtPosition(adtCoords);
        u32 chunkId = Terrain::MapUtils::GetChunkIdFromChunkPos(chunkCoords);

        const Terrain::Chunk& chunk = mapSingleton.currentMap.chunks[chunkId];
        u32 numPlacementsOnChunk = static_cast<u32>(chunk.mapObjectPlacements.size());
        u32 chunkMapPlacementDetailsOffset = mapObjectRenderer->GetChunkPlacementDetailsOffset(chunkId);
        const std::vector<Terrain::PlacementDetails>& mapObjectPlacementDetails = mapObjectRenderer->GetPlacementDetails();

        std::vector<Geometry::AABoundingBox> intersectedBoundingBoxes;
        std::vector<f32> timeToIntersection;
        std::vector<u32> placementDetailIndices;

        intersectedBoundingBoxes.reserve(16);
        timeToIntersection.reserve(16);
        placementDetailIndices.reserve(16);

        Geometry::AABoundingBox* selectedBoundingBox = nullptr;

        f32 timeToIntersect = 0;

        const std::vector<MapObjectRenderer::LoadedMapObject>& loadedMapObjects = mapObjectRenderer->GetLoadedMapObjects();
        const std::vector<MapObjectRenderer::InstanceData>& mapObjectInstanceDatas = mapObjectRenderer->GetInstances();

        for (u32 i = chunkMapPlacementDetailsOffset; i < numPlacementsOnChunk + chunkMapPlacementDetailsOffset; i++)
        {
            const Terrain::PlacementDetails& placementDetails = mapObjectPlacementDetails[i];

            const MapObjectRenderer::LoadedMapObject& loadedMapObject = loadedMapObjects[placementDetails.loadedIndex];
            const std::vector<Terrain::CullingData>& mapObjectCullingData = loadedMapObject.cullingData;
            const MapObjectRenderer::InstanceData& instanceData = mapObjectInstanceDatas[placementDetails.instanceIndex];

            const mat4x4& m = instanceData.instanceMatrix;

            // Transform extents (take maximum)
            glm::mat3x3 absMatrix = glm::mat3x3(glm::abs(vec3(m[0])), glm::abs(vec3(m[1])), glm::abs(vec3(m[2])));

            Geometry::AABoundingBox mapObjectAABB;
            mapObjectAABB.min = vec3(Terrain::MAP_SIZE, Terrain::MAP_SIZE, Terrain::MAP_SIZE);
            mapObjectAABB.max = vec3(-Terrain::MAP_SIZE, -Terrain::MAP_SIZE, -Terrain::MAP_SIZE);
            f32 smallestT = 10000.0f;
            bool didPush = false;

            for (const Terrain::CullingData& cullingData : mapObjectCullingData)
            {
                vec3 center = (cullingData.minBoundingBox + cullingData.maxBoundingBox) * f16(0.5f);
                vec3 extents = vec3(cullingData.maxBoundingBox) - center;

                // transform center
                vec3 transformedCenter = vec3(m * vec4(center, 1.0f));
                vec3 transformedExtents = absMatrix * extents;

                // Transform to min/max box representation
                Geometry::AABoundingBox aabb;
                aabb.min = transformedCenter - transformedExtents;
                aabb.max = transformedCenter + transformedExtents;

                for (u32 j = 0; j < 3; j++)
                {
                    if (aabb.min[j] < mapObjectAABB.min[j])
                        mapObjectAABB.min[j] = aabb.min[j];

                    if (aabb.max[j] > mapObjectAABB.max[j])
                        mapObjectAABB.max[j] = aabb.max[j];
                }

                if (IsRayIntersectingAABB(rayOrigin, oneOverRayDir, aabb, t))
                {
                    if (!didPush)
                    {
                        didPush = true;

                        if (t < smallestT)
                            smallestT = t;
                    }
                }
            }

            if (didPush)
            {
                intersectedBoundingBoxes.push_back(mapObjectAABB);
                timeToIntersection.push_back(smallestT);
                placementDetailIndices.push_back(i);
            }
        }

        f32 smallestT = std::numeric_limits<f32>().max();
        u32 detailsIndex = std::numeric_limits<u32>().max();
        for (u32 i = 0; i < intersectedBoundingBoxes.size(); i++)
        {
            f32& currentT = timeToIntersection[i];
            if (currentT < smallestT)
            {
                smallestT = currentT;
                selectedBoundingBox = &intersectedBoundingBoxes[i];
                detailsIndex = placementDetailIndices[i];
            }
        }

        if (selectedBoundingBox)
        {
            _selectedBoundingBox.type = SelectedBoundingBoxType::MAP_OBJECT;
            _selectedBoundingBox.aabb.min = selectedBoundingBox->min;
            _selectedBoundingBox.aabb.max = selectedBoundingBox->max;

            _selectedMapObjectData.placementDetailsIndex = detailsIndex;
        }

        return selectedBoundingBox != nullptr;
    }
    bool Editor::IsRayIntersectingTerrain(const vec3& rayOrigin, const vec3& oneOverRayDir)
    {
        ClientRenderer* clientRenderer = ServiceLocator::GetClientRenderer();
        TerrainRenderer* terrainRenderer = clientRenderer->GetTerrainRenderer();

        std::vector<Geometry::AABoundingBox> terrainBoundingBoxes = terrainRenderer->GetBoundingBoxes();
        std::vector<Geometry::AABoundingBox*> intersectedBoundingBoxes;
        std::vector<f32> timeToIntersection;

        intersectedBoundingBoxes.reserve(16);
        timeToIntersection.reserve(16);

        Geometry::AABoundingBox* selectedBoundingBox = nullptr;

        f32 t = 0;
        for (Geometry::AABoundingBox& boundingBox : terrainBoundingBoxes)
        {
            if (IsRayIntersectingAABB(rayOrigin, oneOverRayDir, boundingBox, t))
            {
                intersectedBoundingBoxes.push_back(&boundingBox);
                timeToIntersection.push_back(t);
            }
        }

        f32 smallestT = std::numeric_limits<f32>().max();
        for (u32 i = 0; i < intersectedBoundingBoxes.size(); i++)
        {
            f32& currentT = timeToIntersection[i];
            if (currentT < smallestT)
            {
                smallestT = currentT;
                selectedBoundingBox = intersectedBoundingBoxes[i];
            }
        }

        if (selectedBoundingBox)
        {
            _selectedBoundingBox.type = SelectedBoundingBoxType::TERRAIN;
            _selectedBoundingBox.aabb.min = selectedBoundingBox->min;
            _selectedBoundingBox.aabb.max = selectedBoundingBox->max;

            /* Check if we have hit any WMO before the terrain */
            f32 timeForRayIntersection = std::numeric_limits<f32>().max();
            if (IsRayIntersectingComplexModel(rayOrigin, oneOverRayDir, selectedBoundingBox, timeForRayIntersection))
                return true;

            if (IsRayIntersectingMapObject(rayOrigin, oneOverRayDir, selectedBoundingBox, timeForRayIntersection))
                return true;
        }

        return selectedBoundingBox != nullptr;
    }
    bool Editor::IsRayIntersectingAABB(const vec3& rayOrigin, const vec3& oneOverRayDir, const Geometry::AABoundingBox& aabb, f32& t)
    {
        f32 t1 = (aabb.min.x - rayOrigin.x) * oneOverRayDir.x;
        f32 t2 = (aabb.max.x - rayOrigin.x) * oneOverRayDir.x;
        f32 t3 = (aabb.min.y - rayOrigin.y) * oneOverRayDir.y;
        f32 t4 = (aabb.max.y - rayOrigin.y) * oneOverRayDir.y;
        f32 t5 = (aabb.min.z - rayOrigin.z) * oneOverRayDir.z;
        f32 t6 = (aabb.max.z - rayOrigin.z) * oneOverRayDir.z;

        f32 tMin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
        f32 tMax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));

        if (tMax < 0 || tMin > tMax)
        {
            t = tMax;
            return false;
        }

        t = tMin;
        return true;
    }

    bool Editor::OnMouseClickLeft(Window* window, std::shared_ptr<Keybind> keybind)
    {
        if (!CVAR_EditorEnabled.Get())
            return false;

        CameraFreeLook* camera = ServiceLocator::GetCameraFreeLook();
        if (!camera->IsActive())
            return false;

        if (camera->IsMouseCaptured())
            return false;

        if (ImGui::GetCurrentContext()->HoveredWindow)
            return false;

        // Shift Click clears selection
        if (keybind->currentModifierMask & KEYBIND_MOD_SHIFT)
        {
            if (CVAR_EditorRayEnabled.Get())
            {
                CVAR_EditorRayStartPosition.Set(vec4(0, 0, 0, 0));
                CVAR_EditorRayEndPosition.Set(vec4(0, 0, 0, 0));
            }

            _selectedBoundingBox.type = SelectedBoundingBoxType::NONE;
            return false;
        }

        ClientRenderer* clientRenderer = ServiceLocator::GetClientRenderer();
        InputManager* inputManager = ServiceLocator::GetInputManager();
        hvec2 mousePosition = inputManager->GetMousePosition();

        f32 mousePosX = Math::Map(mousePosition.x, 0, static_cast<f32>(clientRenderer->WIDTH), -1, 1);
        f32 mousePosY = -Math::Map(mousePosition.y, 0, static_cast<f32>(clientRenderer->HEIGHT), -1, 1);

        const mat4x4 m = glm::inverse(camera->GetViewProjectionMatrix());

        const vec3 start = DebugRenderer::UnProject(vec3(mousePosX, mousePosY, 1.0f), m);
        const vec3 end = DebugRenderer::UnProject(vec3(mousePosX, mousePosY, 0.0f), m);
        const vec3 dir = glm::normalize(end - start);
        const vec3 dirFrac = vec3(1.0f, 1.0f, 1.0f) / dir;

        if (CVAR_EditorRayEnabled.Get())
        {
            CVAR_EditorRayStartPosition.Set(vec4(start, 0));
            CVAR_EditorRayEndPosition.Set(vec4(end, 0));
        }

        if (IsRayIntersectingTerrain(start, dirFrac))
        {
            _selectedObjectDataInitialized = false;
            return true;
        }

        return false;
    }
}