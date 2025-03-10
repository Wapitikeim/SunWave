#include "SceneManager.h"
#include "../components/PhysicsCollider.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "../util/fileReader.h"
#include "../entities/Shape.h"
#include "../entities/PlayerShape.h"
#include "../entities/UiElement.h"
#include "../factorys/EntityFactory.h"

void SceneManager::loadLevel(const std::string& levelName, std::vector<std::unique_ptr<Entity>>& entities, PhysicsEngine* physicsEngine)
{
    std::filesystem::path srcPath = std::filesystem::current_path();
    fileReader::trimDownPathToWorkingDirectory(srcPath);
    srcPath.append("src/scenes/LevelConfigurations/"+levelName+".json");
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
    if (levelToLoad.contains("physicsEngine"))
        for(const auto& physicsEntry_json : levelToLoad["physicsEngine"])
            physicsEngine->setBounceMultiplier(physicsEntry_json["BounceM"]);
    else
        std::cerr << "Error: BounceM key not found in physicsEngine section of the JSON.\n";

    int i = 0;
    int j = 0; //for Physics if objects are loaded that dont have physics
    for(const auto& entity_json : levelToLoad["entities"])
    {
        //Load base Entity data
        std::string name = entity_json["name"];
        glm::vec3 position = glm::vec3(entity_json["position"][0], entity_json["position"][1], entity_json["position"][2]);
        glm::vec3 scale = glm::vec3(entity_json["scale"][0], entity_json["scale"][1], entity_json["scale"][2]);
        float rotation = entity_json["rotation"];
        std::string type = entity_json["Type"];
        std::string shaderName = entity_json["ShaderName"];
        
        std::unique_ptr<Entity> loadedEntity;

        if(entity_json.contains("UiElement"))
        {
            std::string renderedText = entity_json["UiElement"]["RenderedText"];
            std::string fontName = entity_json["UiElement"]["Font"]; 
            unsigned int fontSize = entity_json["UiElement"]["FontSize"]; 
            glm::vec4 textColor = glm::vec4(entity_json["UiElement"]["TextColor"][0], entity_json["UiElement"]["TextColor"][1], entity_json["UiElement"]["TextColor"][2], entity_json["UiElement"]["TextColor"][3]);
            
            loadedEntity = createUiEntity(type, name, position, scale, rotation, renderedText, fontName, fontSize, textColor);
        }
        else
            loadedEntity = createEntity(type, name, position, scale, rotation, shaderName);
        
        entities.push_back(std::move(loadedEntity));

        if (entity_json.contains("Physics"))
        {
            entities[i]->addComponent(std::make_unique<PhysicsCollider>(entities[i].get(),entity_json["Physics"]["IsStatic"]));
            physicsEngine->registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[i]->getComponent("Physics")));
            if(applyPhysicsWhenLoading)
            {
                auto& ref = physicsEngine->getActiveColliders()[j];
                ref->setVelocity(glm::vec3(entity_json["Physics"]["Velocity"][0], entity_json["Physics"]["Velocity"][1], entity_json["Physics"]["Velocity"][2]));
                ref->setAcceleration(glm::vec3(entity_json["Physics"]["Acceleration"][0], entity_json["Physics"]["Acceleration"][1], entity_json["Physics"]["Acceleration"][2]));
                ref->setMass(entity_json["Physics"]["Mass"]);
                ref->setIsTrigger(entity_json["Physics"]["IsTrigger"]);
                ref->setIsResting(entity_json["Physics"]["IsResting"]);
                ref->setIsGrounded(entity_json["Physics"]["IsGrounded"]);
                ref->setIsInContact(entity_json["Physics"]["IsInContact"]);
                ref->setElascity(entity_json["Physics"]["Elasticity"]);
                ref->setRot(entity_json["Physics"]["Rot"]);
                ref->setLockX(entity_json["Physics"]["LockX"]);
                ref->setLockY(entity_json["Physics"]["LockY"]);
                ref->setUnaffectedByGravity(entity_json["Physics"]["IgnoreGravity"]);
            }
            j++;
        }
        
        i++;
    }
    if (applyPhysicsWhenLoading)
            physicsEngine->setInitDone(true);
}

void SceneManager::saveLevel(const std::string& levelName, std::vector<std::unique_ptr<Entity>>& entities, PhysicsEngine* physicsEngine, const glm::vec3& cameraPos, const float& fov)
{
    if((entities.size() == 0)|| (levelName.size() == 0))
        return;
    nlohmann::json level = nlohmann::json{{"levelName", levelName},
                                          {"entities", nlohmann::json::array()},
                                          {"worldData", nlohmann::json::array()},
                                          {"physicsEngine", nlohmann::json::array()}};
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
                                                    {"Rot", physics->getRot()},
                                                    {"LockX", physics->getLockX()},
                                                    {"LockY", physics->getLockY()},
                                                    {"IgnoreGravity", physics->getUnaffectedByGravity()}

            };
            entity_json["Physics"] = physics_json;
        }   
        
        //UIElementInfo
        if(entry->getEntityType()=="UiElement")
        {
            UiElement* uiElement = dynamic_cast<UiElement*>(entry.get());
            if (uiElement) 
            {
                nlohmann::json uiElement_json = nlohmann::json
                {
                    {"Font", uiElement->getCurrentFontName()},
                    {"FontSize", uiElement->getCurrentFontSize()},
                    {"RenderedText", uiElement->getText()},
                    {"TextColor", {uiElement->getTextColor().r, uiElement->getTextColor().g, uiElement->getTextColor().b, uiElement->getTextColor().a}}
                };
                entity_json["UiElement"] = uiElement_json;
            }
            
        }

        level["entities"].push_back(entity_json);
    }
    
    //World Data just Camera Info for Now
    nlohmann::json worlddata_json;
    worlddata_json["CameraPos"] = {cameraPos.x,cameraPos.y,cameraPos.z};
    worlddata_json["Fov"] =  fov;
    level["worldData"].push_back(worlddata_json);

    //PhysicsEngine extra info - just bounce Multipliert for now
    nlohmann::json physicsEngine_json;
    physicsEngine_json["BounceM"] = physicsEngine->getBounceMultiplier();
    level["physicsEngine"].push_back(physicsEngine_json);
    
    std::filesystem::path srcPath = std::filesystem::current_path();
    fileReader::trimDownPathToWorkingDirectory(srcPath);
    srcPath.append("src/scenes/LevelConfigurations/"+levelName+".json");
    std::ofstream file(srcPath);
    file << level.dump(4);
}
