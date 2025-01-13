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
        struct EntityData 
        {
            std::string name;
            glm::vec3 position;
            glm::vec3 scale;
            float rotation;
        };

        struct LevelData 
        {
            std::vector<EntityData> entities;
        };

    public:
        SceneManager() = default;
        
        void loadLevel(const std::string& levelName, std::vector<std::unique_ptr<Entity>>& entities, PhysicsEngine* physicsEngine);
        void saveLevel(const std::string& levelName, std::vector<std::unique_ptr<Entity>>& entities);

};