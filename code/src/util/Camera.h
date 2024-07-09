#pragma once
#include <glm/glm.hpp> 
#include <glm/vec4.hpp>
#include <glm/gtx/io.hpp>

class Camera 
{
    //Just here for Easy Access "globally" to the current CameraRelated Matrices 
    //Needs to be changed / discarded / replaced by an Scene System e.g.
    
    private:
    
        static glm::mat4 projection;
        static glm::mat4 view;

    
    public:

        static glm::mat4 getCurrentCameraView();
        static glm::mat4 getCurrentCameraProjection();

        static void setCurrentCameraView(const glm::mat4 &viewToSet);
        static void setCurrentCameraProjection(const glm::mat4 &projectionToSet);
};
