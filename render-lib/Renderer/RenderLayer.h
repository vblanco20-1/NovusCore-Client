#pragma once
#include <NovusTypes.h>
#include <vector>
#include <robin_hood.h>
#include <unordered_map>
#include "InstanceData.h"
#include "Descriptors/ModelDesc.h"

namespace Renderer
{
    // A RenderLayer is just a collection of models to be drawn at certain positions
    class RenderLayer
    {
    public:
        typedef std::vector<InstanceData*> Instances;
        typedef type_safe::underlying_type<ModelID> _ModelID;

        void RegisterModel(ModelID modelID, InstanceData* instanceData) { _models[static_cast<_ModelID>(modelID)].push_back(instanceData); }
        void Reset() 
        {
            for (auto& model : _models)
            {
                model.second.clear();
            }

            _models.clear(); 
        }

        //robin_hood::unordered_map<_ModelID, Instances>& GetModels() { return _models; }
        robin_hood::unordered_map<_ModelID, Instances>& GetModels() { return _models; }

        RenderLayer() {}
        ~RenderLayer()
        {
            Reset();
        }

    private:
        

    private:
        robin_hood::unordered_map<_ModelID, Instances> _models;
        //std::unordered_map<_ModelID, Instances> _models;
    };
}