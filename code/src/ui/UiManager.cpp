#include "UiManager.h"
#include "../GameEnvironment.h"
#include "../factorys/EntityFactory.h"

void UiManager::setupImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

void UiManager::drawImGuiControlPanel()
{
    ImGui::Begin("GUI control panel");
    ImGui::Checkbox("World control", &worldControl);
    ImGui::Checkbox("Info panel", &infoPanel);
    ImGui::Checkbox("Level manager", &levelManager);
    ImGui::Checkbox("Physics engine Control", &physicsEngineControl);
    ImGui::Checkbox("Player info", &playerExtraInfo);
    ImGui::Checkbox("Mouse info", &mouseInformation);
    ImGui::Checkbox("Entity adder", &entityAdder);
    
    ImGui::Text("");
    ImGui::Checkbox("Enable ImGui", &imGuiControlPanel);
    
    ImGui::End();
}

void UiManager::drawImGuiWorldControl()
{
    auto& entities = gameEnv->getEntities();
    auto* physicsEngine = gameEnv->getPhysicsEngine();
    auto* mouseCollider = gameEnv->getCurrentMouseCollider();
    //Needs to be on Top That it dosent crash if other levels are loaded and the Header is collpased
    ImGui::Begin("World Control");
    ImGui::Checkbox("Grid", &showGrid);
    ImGui::SliderFloat("Grid size:", &gridSize, 0.1, 3, "%.3f",0);
    float fov = gameEnv->getFov();
    ImGui::SliderFloat("Fov:", &fov, 30, 120, "%.3f",0);
    gameEnv->setFOV(fov);

    // Vector of entity names
    std::vector<std::string> entityNames;
    for(auto& entry:entities)
        entityNames.push_back(entry->getEntityName());
    entityNames.push_back("");

    static int selectedEntityIndex = 0;
    static char entityToLoad[128] = "";

    // Set entityToLoad to the mouseCollider name if available
    if (mouseCollider != nullptr)
    {
        strncpy(entityToLoad, mouseCollider->getEntityThisIsAttachedTo()->getEntityName().c_str(), sizeof(entityToLoad));
        entityToLoad[sizeof(entityToLoad) - 1] = '\0'; // Ensure null-termination
    }

    // Create a combo box for selecting the level name
    if (ImGui::Combo("Select Entity", &selectedEntityIndex, [](void* data, int idx, const char** out_text) {
        const std::vector<std::string>* items = static_cast<std::vector<std::string>*>(data);
        if (idx < 0 || idx >= items->size()) return false;
        *out_text = items->at(idx).c_str();
        return true;
    }, static_cast<void*>(&entityNames), entityNames.size()))
    {
        // Update the levelName with the selected level
        strncpy(entityToLoad, entityNames[selectedEntityIndex].c_str(), sizeof(entityToLoad));
        entityToLoad[sizeof(entityToLoad) - 1] = '\0'; // Ensure null-termination
    }
    if(entityToLoad != "" && gameEnv->getEntityFromName<Entity>(entityToLoad) != nullptr)
    {
        auto entityRef = gameEnv->getEntityFromName<Entity>(entityToLoad);
        auto* colliderRef = gameEnv->getComponentOfEntity<PhysicsCollider>(entityRef->getEntityName(),"Physics");
        ImGui::Text("Current selected entity: %s ", entityRef->getEntityName().c_str());
        if(colliderRef == nullptr)
            ImGui::Text("Entity %s has no active Physics Collider", entityRef->getEntityName().c_str());
        else
        {
            ImGui::Text("X: %f Y: %f", colliderRef->getPos().x,colliderRef->getPos().y);
            ImGui::Text("Velocity: X: %f Y: %f", colliderRef->getVelocity().x, colliderRef->getVelocity().y);
            ImGui::Text("Elasicity: %f", colliderRef->getElascicity());
            bool iTrigger = colliderRef->getIsTrigger();
            ImGui::Checkbox("Is Trigger", &iTrigger);
            ImGui::SameLine();
            if(ImGui::Button("Change Trigger"))
                colliderRef->setIsTrigger(!iTrigger);
            
            bool iResting = colliderRef->getIsResting();
            ImGui::Checkbox("Is Resting", &iResting);
            
        
            bool iStatic = colliderRef->getIsStatic();
            ImGui::Checkbox("Is Static", &iStatic);
            ImGui::SameLine();
            if(ImGui::Button("Change Static"))
                colliderRef->setIsStatic(!iStatic);

            bool iDontDraw = entityRef->getDontDraw();
            ImGui::Checkbox("Dont Draw", &iDontDraw);
            ImGui::SameLine();
            if(ImGui::Button("Hide"))
                entityRef->setDontDraw(!iDontDraw);

            if(ImGui::Button("ChangeColor"))
                entityRef->getShaderContainer().setUniformVec4("colorChange", glm::vec4(1,0,0,1));
        
            float massRef = colliderRef->getBody().mass;
            ImGui::SliderFloat("Mass:", &massRef, 0.1, 10, "%.3f",0);
            colliderRef->setMass(massRef);

            if(physicsEngine->getHashTableIndicesSize() != 0)
                ImGui::Text("HashTable Index: %i", colliderRef->getTableIndicies()[0]);
        }
        float position[2] = { entityRef->getPosition().x, entityRef->getPosition().y };
        if (ImGui::InputFloat2("Position", position)) 
            entityRef->setPosition(glm::vec3(position[0], position[1], entityRef->getPosition().z));
        float scaleX = entityRef->getScale().x;
        float scaleY = entityRef->getScale().y;
        ImGui::SliderFloat("ScaleX:", &scaleX, 0.1, 30, "%.3f",0);
        ImGui::SliderFloat("ScaleY:", &scaleY, 0.1, 30, "%.3f",0);
        glm::vec3 newScale(scaleX,scaleY, entityRef->getScale().z);
        entityRef->setScale(newScale);

        if(entityRef->getEntityType() == "UiElement")
        {
            UiElement* uiElement = dynamic_cast<UiElement*>(entityRef);
            if (uiElement) 
            {
            // Successfully cast to UiElement, now you can access UiElement-specific members
            static char textBuffer[256];
            strncpy(textBuffer, uiElement->getText().c_str(), sizeof(textBuffer));
            if (ImGui::InputText("Rendered Text", textBuffer, sizeof(textBuffer))) 
                uiElement->setTextToBeRenderd(std::string(textBuffer));
            }

            // Input for text color
            float textColor[4] = { uiElement->getTextColor().r, uiElement->getTextColor().g, uiElement->getTextColor().b, uiElement->getTextColor().a };
            if (ImGui::InputFloat4("Text Color", textColor)) 
                uiElement->setTextColor(glm::vec4(textColor[0], textColor[1], textColor[2], textColor[3]));
            
            
        }
            
        
        if(ImGui::Button("Delete Entity"))
            gameEnv->deleteEntityFromName(entityRef->getEntityName());
    }
    
    ImGui::End();
}

void UiManager::drawImGuiInfoPanel()
{
    auto& entities = gameEnv->getEntities();
    auto* physicsEngine = gameEnv->getPhysicsEngine();

    ImGui::Begin("Info Panel");
    ImGui::Text("FPS: %i", imGuiFPS);
    ImGui::Text("Entities in Scene: %i", entities.size());
    ImGui::Text("Collisions resolved LastTick: %i", physicsEngine->getCurrentCollisions());
    ImGui::End();
}

void UiManager::drawImGuiLevelManager()
{
    auto& entities = gameEnv->getEntities();
    auto& sceneManager = gameEnv->getSceneManager();
    auto* physicsEngine = gameEnv->getPhysicsEngine();
    auto& cameraPos = gameEnv->getCameraPos();
    auto& fov = gameEnv->getFov();
    auto& mouseX = gameEnv->getMouseX();
    auto& mouseY = gameEnv->getMouseY();
    
    ImGui::Begin("Level Loading");
    // Vector of level names (for demonstration purposes)
    std::vector<std::string> levelNames = fileReader::getAllLevelFileNames();
    static int selectedLevelIndex = 0;
    
    static char levelToLoad[128] = "";
    // Initialize levelToLoad with the first entry if it's not already set
    if (levelToLoad[0] == '\0' && !levelNames.empty())
    {
        strncpy(levelToLoad, levelNames[0].c_str(), sizeof(levelToLoad));
        levelToLoad[sizeof(levelToLoad) - 1] = '\0'; // Ensure null-termination
    }
    // Create a combo box for selecting the level name
    if (ImGui::Combo("Select Level", &selectedLevelIndex, [](void* data, int idx, const char** out_text) {
        const std::vector<std::string>* items = static_cast<std::vector<std::string>*>(data);
        if (idx < 0 || idx >= items->size()) return false;
        *out_text = items->at(idx).c_str();
        return true;
    }, static_cast<void*>(&levelNames), levelNames.size()))
    {
        // Update the levelName with the selected level
        strncpy(levelToLoad, levelNames[selectedLevelIndex].c_str(), sizeof(levelToLoad));
        levelToLoad[sizeof(levelToLoad) - 1] = '\0'; // Ensure null-termination
    }
    ImGui::SameLine();
    if(ImGui::Button("Load"))
        if(levelToLoad != "")
            sceneManager.loadLevel(levelToLoad, entities, physicsEngine);
    
    bool applyPhysics = sceneManager.getApplyPhysicsWhenLoading();
    ImGui::Checkbox("Apply physics when loading", &applyPhysics);
    ImGui::SameLine();
    if(ImGui::Button("Change"))
        sceneManager.setApplyPhysicsWhenLoading(!applyPhysics);
    

    static char levelToSaveName[128] = "";
    ImGui::InputText("Level Name", levelToSaveName, IM_ARRAYSIZE(levelToSaveName));
    ImGui::SameLine();
    if(ImGui::Button("Save"))
    {
        // Convert to std::string for easier manipulation
        std::string levelNameStr(levelToSaveName);

        // Trim whitespace from both ends
        levelNameStr.erase(levelNameStr.find_last_not_of(" \t\n\r\f\v") + 1);
        levelNameStr.erase(0, levelNameStr.find_first_not_of(" \t\n\r\f\v"));

        // Check if the string is empty after trimming
        if (!levelNameStr.empty())
            sceneManager.saveLevel(levelNameStr, entities, physicsEngine, cameraPos, fov);
        else
            std::cerr << "Error: Level name cannot be empty or whitespace only.\n";
        
    }
    ImGui::End();
}

void UiManager::drawImGuiPhysicsEngineControl()
{
    auto* physicsEngine = gameEnv->getPhysicsEngine();

    ImGui::Begin("Physics Engine Control");
    ImGui::SliderFloat("Tickrate:", &physicsEngine->tickrateOfSimulation, 60, 300, "%.3f",0);
    ImGui::SliderFloat("Speed:", &physicsEngine->speedOfSimulation, 0, 3, "%.3f",0);
    bool engineHalting = physicsEngine->getIsHalting();
    ImGui::Checkbox("Is Halting", &engineHalting);
    ImGui::SameLine();
    if(ImGui::Button("Halt Engine"))
        physicsEngine->setIsHalting(!engineHalting);
    /* if(ImGui::Button("Reset Level"))
        resetLevel();
    if(ImGui::Button("Load just walls"))
        loadWallLevel(); */
    ImGui::SliderInt("Entity Count", &gameEnv->entitiesToFill, 20, 200);
    if(ImGui::Button("Test Stuff"))
    {
        gameEnv->fillSceneWithEntitys();
    }

    float newBounce = physicsEngine->getBounceMultiplier();
    ImGui::SliderFloat("Bounce multiplier:", &newBounce, 0.5f, 4.0f, "%.3f",0);
    physicsEngine->setBounceMultiplier(newBounce);  
    ImGui::Text("Hash Table Size: %i ", physicsEngine->getHashTableIndicesSize());
    bool initDone = physicsEngine->getInitDone();
    ImGui::Checkbox("Init done", &initDone);
    ImGui::SameLine();
    if(ImGui::Button("Re init"))
        physicsEngine->setInitDone(!initDone);
    ImGui::End();
}

void UiManager::drawImGuiPlayerExtraInfo()
{
    ImGui::Begin("Player Extra Info");
    if(gameEnv->getEntityFromName<Entity>("Player") != nullptr)
    {
        ImGui::Text("Player Pos: X: %f Y: %f", gameEnv->getEntityFromName<PlayerShape>("Player")->getPosition().x, gameEnv->getEntityFromName<PlayerShape>("Player")->getPosition().y);
        ImGui::Text("Player is grounded: %s", gameEnv->getComponentOfEntity<PhysicsCollider>("Player", "Physics")->getIsGrounded() ? "true" : "false");
        ImGui::Text("Player Velocity X:%f Y: %f", gameEnv->getComponentOfEntity<PhysicsCollider>("Player", "Physics")->getVelocity().x, gameEnv->getComponentOfEntity<PhysicsCollider>("Player", "Physics")->getVelocity().y);
        ImGui::Text("Player Acceleration X: %f Y:%f",  gameEnv->getComponentOfEntity<PhysicsCollider>("Player", "Physics")->getAcceleration().x, gameEnv->getComponentOfEntity<PhysicsCollider>("Player", "Physics")->getAcceleration().y);
        ImGui::Text("Corner left bottom: X:%f Y:%f", gameEnv->getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().leftBottom.x, gameEnv->getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().leftBottom.y);
        ImGui::Text("Corner right bottom: X:%f Y:%f", gameEnv->getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().rightBottom.x, gameEnv->getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().rightBottom.y);
        ImGui::Text("Corner left top: X:%f Y:%f", gameEnv->getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().leftTop.x, gameEnv->getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().leftTop.y);
        ImGui::Text("Corner right top: X:%f Y:%f", gameEnv->getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().rightTop.x, gameEnv->getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().rightTop.y);
        //ImGui::Text("TestSDF d=%f",CollisionTester::signedDistance2DBoxAnd2DBox(getComponentOfEntity<PhysicsCollider>("aRandomTriggerBox","Physics"),getComponentOfEntity<PhysicsCollider>("Player","Physics")));
        ImGui::SliderFloat("Speed", &gameEnv->getEntityFromName<PlayerShape>("Player")->velocity,1, 20,"%.3f",0);
        float scaleX = gameEnv->getEntityFromName<Entity>("Player")->getScale().x;
        float scaleY = gameEnv->getEntityFromName<Entity>("Player")->getScale().y;
        ImGui::SliderFloat("ScaleX:", &scaleX, 0.1, 30, "%.3f",0);
        ImGui::SliderFloat("ScaleY:", &scaleY, 0.1, 30, "%.3f",0);
        glm::vec3 newScale(scaleX,scaleY, gameEnv->getEntityFromName<Entity>("Player")->getScale().z);
        gameEnv->getEntityFromName<Entity>("Player")->setScale(newScale);
        ImGui::Text("Player rot %f", gameEnv->getEntityFromName<Entity>("Player")->getRotation());
        
    }
    else
        ImGui::Text("Player not found");
    ImGui::End();
}

void UiManager::drawImGuiMouseInformation()
{
    auto& mouseX = gameEnv->getMouseX();
    auto& mouseY = gameEnv->getMouseY();
    auto* refColliderForMouseCurrent = gameEnv->getCurrentMouseCollider();
    
    ImGui::Begin("Mouse Information");
    ImGui::Text("Mouse X: %f", mouseX);
    ImGui::Text("Mouse Y: %f", mouseY);
    if(refColliderForMouseCurrent == nullptr)
        ImGui::Text("Mouse in contact with: none");
    else
        ImGui::Text("Mouse in contact with: %s", refColliderForMouseCurrent->getEntityThisIsAttachedTo()->getEntityName().c_str());
    ImGui::End();
}

void UiManager::drawImGuiEnitityAdder()
{
    //What Do I need for the entity?
    auto& entities = gameEnv->getEntities();
    auto* physicsEngine = gameEnv->getPhysicsEngine();

    static char entityType[128] = "Shape";
    static char entityName[128] = "NewEntity";
    static float position[2] = {10.0f, 10.0f};
    static float scale[2] = {1.0f, 1.0f};
    static float rotation = 0.0f;
    static char shaderName[128] = "box";
    static bool isStatic = "true";

    static int selectedShaderIndex = 0; // Index for the selected shader
    static int selectedShapeIndex = 0; // Index for the selected shape

    // List of shader names
    const char* shaderNames[] = { "box", "circle", "cross", "sTriangle" };
    // List of shape names
    const char* shapeNames[] = { "Shape", "PlayerShape"};


    ImGui::Begin("Entity Adder");

    // Dropdown for shape names
    if (ImGui::Combo("Shape Name", &selectedShapeIndex, shapeNames, IM_ARRAYSIZE(shapeNames)))
    {
        // Update entityType based on the selected index
        strncpy(entityType, shapeNames[selectedShapeIndex], sizeof(entityType));
        entityType[sizeof(entityType) - 1] = '\0'; // Ensure null-termination
    }
    ImGui::InputText("Name", entityName, IM_ARRAYSIZE(entityName));
    ImGui::InputFloat2("Position", position);
    ImGui::InputFloat2("Scale", scale);
    ImGui::InputFloat("Rotation", &rotation);
    ImGui::Checkbox("Is static", &isStatic);
    // Dropdown for shader names
    if (ImGui::Combo("Shader Name", &selectedShaderIndex, shaderNames, IM_ARRAYSIZE(shaderNames)))
    {
        // Update shaderName based on the selected index
        strncpy(shaderName, shaderNames[selectedShaderIndex], sizeof(shaderName));
        shaderName[sizeof(shaderName) - 1] = '\0'; // Ensure null-termination
    }

    if (ImGui::Button("Add"))
    {
        try
        {
            auto newEntity = createEntity(entityType, entityName, glm::vec3(position[0], position[1], 0.f), glm::vec3(scale[0], scale[1], 1.0f), rotation, shaderName);
            gameEnv->addEntity(std::move(newEntity));
            std::cout << "Trying to add entity: " << entityName << "\n";
            auto* addedEntity = gameEnv->getEntityFromName<Entity>(entityName);
            addedEntity->addComponent(std::make_unique<PhysicsCollider>(addedEntity,isStatic));
            physicsEngine->registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(gameEnv->getEntityFromName<Entity>(entityName)->getComponent("Physics")));
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    ImGui::End();
}

void UiManager::prepFrames()
{
    //Imgui Setup
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UiManager::draw()
{   
    if(imGuiControlPanel)
        drawImGuiControlPanel();

    if(worldControl&&imGuiControlPanel)
        drawImGuiWorldControl();

    if(infoPanel&&imGuiControlPanel)
        drawImGuiInfoPanel();

    if(levelManager&&imGuiControlPanel)
        drawImGuiLevelManager();

    if(physicsEngineControl&&imGuiControlPanel)
        drawImGuiPhysicsEngineControl();
    
    if(playerExtraInfo&&imGuiControlPanel)
        drawImGuiPlayerExtraInfo();

    if(mouseInformation&&imGuiControlPanel)
        drawImGuiMouseInformation();
    
    if(entityAdder&&imGuiControlPanel)
        drawImGuiEnitityAdder();

    //Nessecary
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
