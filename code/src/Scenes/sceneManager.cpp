#include "SceneManager.h"
#include "../components/PhysicsCollider.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "../util/fileReader.h"

/* 
    nlohmann::json j = nlohmann::json{{"tew"}};
    std::filesystem::path srcPath = std::filesystem::current_path();
    fileReader::trimDownPathToWorkingDirectory(srcPath);
    srcPath.append("src/Test.json");
    std::ofstream file(srcPath);
    file << j.dump(4); 
*/

void SceneManager::loadLevel(std::string levelName, std::vector<std::unique_ptr<Entity>>& entities)
{
}

void SceneManager::saveLevel(std::string levelName, std::vector<std::unique_ptr<Entity>>& entities)
{
    if((entities.size() == 0)|| (levelName.size() == 0))
        return;
    nlohmann::json level = nlohmann::json{{"levelName", levelName},
                                          {"entities", nlohmann::json::array()}};
    for(auto& entry: entities)
    {
        nlohmann::json entity_json;

        // Add entity information first
        entity_json["name"] = entry->getEntityName();
        entity_json["position"] = {entry->getPosition().x, entry->getPosition().y, entry->getPosition().z};
        entity_json["scale"] = {entry->getScale().x, entry->getScale().y, entry->getScale().z};
        entity_json["rotation"] = entry->getRotation();
        entity_json["Type"] = entry->getEntityType();
        entity_json["ShaderName"] = entry->getShaderName();

        // Add Physics information if it exists
        if(entry->getComponent("Physics"))
        {
            auto physics = dynamic_cast<PhysicsCollider*>(entry->getComponent("Physics"));
            nlohmann::json physics_json = nlohmann::json{ 
                                                    {"Velocity", {physics->getVelocity().x, physics->getVelocity().y, physics->getVelocity().z}},
                                                    {"Acceleration", {physics->getAcceleration().x, physics->getAcceleration().y, physics->getAcceleration().z}},
                                                    {"Mass", physics->getMass()},
                                                    {"IsStatic", physics->getIsStatic()},
                                                    {"IsTrigger", physics->getIsTrigger()},
                                                    {"IsResting", physics->getIsResting()},
                                                    {"IsGrounded", physics->getIsGrounded()},
                                                    {"IsInContact", physics->getIsInContact()},
                                                    {"Elasticity", physics->getElascicity()},
                                                    {"Rot", physics->getRot()}
            };
            entity_json["Physics"] = physics_json;
        }   
        level["entities"].push_back(entity_json);
    }
    std::filesystem::path srcPath = std::filesystem::current_path();
    fileReader::trimDownPathToWorkingDirectory(srcPath);
    srcPath.append("src/Scenes/LevelConfigurations/"+levelName+".json");
    std::ofstream file(srcPath);
    file << level.dump(4);
}
