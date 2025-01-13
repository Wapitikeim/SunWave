#include "SceneManager.h"
#include "../components/PhysicsCollider.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "../util/fileReader.h"
#include "../entities/Shape.h"
#include "../entities/PlayerShape.h"

// Factory function to create entities based on type
std::unique_ptr<Entity> createEntity(const std::string& type, const std::string& name, const glm::vec3& position, const glm::vec3& scale, float rotation, const std::string& shaderName)
{
    if (type == "Shape")
    {
        return std::make_unique<Shape>(name, position, scale, rotation, true, shaderName);
    }
    else if (type == "PlayerShape")
    {
        return std::make_unique<PlayerShape>(name, position, scale, rotation, true, shaderName);
    }
    // Add more entity types as needed
    else
    {
        throw std::runtime_error("Unknown entity type: " + type);
    }
}

void SceneManager::loadLevel(const std::string& levelName, std::vector<std::unique_ptr<Entity>>& entities, PhysicsEngine* physicsEngine)
{
    std::filesystem::path srcPath = std::filesystem::current_path();
    fileReader::trimDownPathToWorkingDirectory(srcPath);
    srcPath.append("src/Scenes/LevelConfigurations/"+levelName+".json");
    nlohmann::json levelToLoad;
    std::ifstream file(srcPath);
    // Check if the file was successfully opened
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file " << levelName <<".json" << "\n";
        return;
    }
    file >> levelToLoad;
    if (levelToLoad["levelName"] == nullptr)
    {
        std::cout << "Json does not contain the levelName key \n";
        return;
    }
    std::cout << "Loading " << levelToLoad["levelName"] << "\n";
    entities.clear();
    physicsEngine->clearPhysicsObjects();

    int i = 0;
    for(const auto& entity_json : levelToLoad["entities"])
    {
        //Load base Entity data
        std::string name = entity_json["name"];
        glm::vec3 position = glm::vec3(entity_json["position"][0], entity_json["position"][1], entity_json["position"][2]);
        glm::vec3 scale = glm::vec3(entity_json["scale"][0], entity_json["scale"][1], entity_json["scale"][2]);
        float rotation = entity_json["rotation"];
        std::string type = entity_json["Type"];
        std::string shaderName = entity_json["ShaderName"];

        
        // Create the entity
        //auto aDynamicBox = std::make_unique<Shape>("aDynamicBox", glm::vec3(10.f,5.f,0.3f),glm::vec3(1.f), 0, true, "box");
        auto loadedEntity = createEntity(type, name, position, scale, rotation, shaderName);
        entities.push_back(std::move(loadedEntity));
        
        if (entity_json.contains("Physics"))
        {
            entities[i]->addComponent(std::make_unique<PhysicsCollider>(entities[i].get(),entity_json["Physics"]["IsStatic"]));
            physicsEngine->registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[i]->getComponent("Physics")));
            if(applyPhysicsWhenLoading)
            {
                auto& ref = physicsEngine->getActiveColliders()[i];
                ref->setVelocity(glm::vec3(entity_json["Physics"]["Velocity"][0], entity_json["Physics"]["Velocity"][1], entity_json["Physics"]["Velocity"][2]));
                ref->setAcceleration(glm::vec3(entity_json["Physics"]["Acceleration"][0], entity_json["Physics"]["Acceleration"][1], entity_json["Physics"]["Acceleration"][2]));
                ref->setMass(entity_json["Physics"]["Mass"]);
                ref->setIsTrigger(entity_json["Physics"]["IsTrigger"]);
                ref->setIsResting(entity_json["Physics"]["IsResting"]);
                ref->setIsGrounded(entity_json["Physics"]["IsGrounded"]);
                ref->setIsInContact(entity_json["Physics"]["IsInContact"]);
                ref->setElascity(entity_json["Physics"]["Elasticity"]);
                ref->setRot(entity_json["Physics"]["Rot"]);
            }
        }
        if (applyPhysicsWhenLoading)
            physicsEngine->setInitDone(true);
        i++;
    }
}

void SceneManager::saveLevel(const std::string& levelName, std::vector<std::unique_ptr<Entity>>& entities)
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
