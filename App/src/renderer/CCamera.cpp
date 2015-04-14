#include "renderer/CCamera.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace NApp
{

CCamera::CCamera(float near, float far, float angle, float aspectRatio)
   : mView()
   , mProjection(glm::perspective(angle, aspectRatio, near, far))
{
}

void CCamera::setPosition(
   const glm::vec3 & position,
   const glm::quat & orientation)
{
   /// NEED REWRITE IT BY GLM FUNTIONS (WITHOUT OPENCV)

   glm::mat4 mView = glm::toMat4(orientation);
   mView = glm::transpose(mView);
   

   mView[0][1] = -mView[0][1];
   mView[0][2] = -mView[0][2];
   mView[1][0] = -mView[1][0];
   mView[2][0] = -mView[2][0];

   // convert translation vector to opengl coordinate system
   mView[3][0] =  position[0];
   mView[3][1] = -position[1];
   mView[3][2] = -position[2];
   mView[3][3] = 1.f;
}

} /* namespace NApp */
