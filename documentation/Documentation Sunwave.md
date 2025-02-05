This documentation should give an overview of the codebase of Sunwave.
It lists every class, a short descrption and if applicable use cases and other examples of usage of the API.

What to do when try to develop new minigames
-> look at minigameManager and go from there

What to do to further implement on engine changes
-> look at uimanager try to build on that explore new options

What features are missing and could be easily implemented?
- A Soundmanger (currently purely handeld by Gameenvoirement)
- A Mousemanger (")
- A more fleshed out CameraSystem
- Sandboxmode could be his own executable

Codequality concerns
- Physics engine has some glaring issues regarding sinking of colliders
- Currently although partily implemented smart pointers could be more strictly enforced
- Namespace(s)

## Classes Overview

### Entry Point
- [main](Class Files\main.md)

### Most Relevant Classes:
- [[GameEnvironment]]  
  - [[SceneManager]] - Level loading/saving  
  - [[UiManager]] - ImGui/Sandbox  

- [[Entity]]  
  - [[PlayerShape]] - Implementation of player controls  
  - [[UiElement]] - UI Elements class  

- [[MinigameManager]] - Logic for all the minigames  

### Physics-Related Classes:
- [[PhysicsEngine]]  
  - [[PhysicsCollider]] - Component used by the Physics Engine  
  - [[MortonHashTableLogic]] - Morton encoded hash table logic  
  - [[CollisionTester]] - SAT  

### Utility Classes:
- [[MeshContainer]]  
- [[ShaderContainer]]  
- [[EntityFactory]]  
- [[Component]]  
  - [[Shape]]  
- [[FontLoader]]  
- [[Camera]]  
- [[FileReader]]  
- [[glfwPrep]]  
- [[HelperFunctions]]  
- [[InfiniteGrid]]  


