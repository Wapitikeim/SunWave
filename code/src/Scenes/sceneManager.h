#pragma once
#include <vector>
#include <string>
#include <glm/vec3.hpp>
#include "../Entities/Entity.h"
#include "../physics/PhysicsEngine.h"

#include <nlohmann/json.hpp>
#include <fstream>


class SceneManager
{
    private:
        bool applyPhysicsWhenLoading = true;

    public:
        SceneManager() = default;
        const bool& getApplyPhysicsWhenLoading()const{return applyPhysicsWhenLoading;};
        void setApplyPhysicsWhenLoading(const bool& newApply){applyPhysicsWhenLoading = newApply;};
        
        void loadLevel(const std::string& levelName, std::vector<std::unique_ptr<Entity>>& entities, PhysicsEngine* physicsEngine);
        void saveLevel(const std::string& levelName, std::vector<std::unique_ptr<Entity>>& entities, PhysicsEngine* physicsEngine , const glm::vec3& cameraPos, const float& fov);

};