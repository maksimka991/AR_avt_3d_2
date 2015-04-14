#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>

namespace NApp
{

class CCamera
{
public:
   CCamera(float near, float far, float angle, float aspectRatio);

   void setPosition(const glm::vec3 & position, const glm::quat & orientation);

   const glm::mat4 & getProjectionMatrix() const;
   const glm::mat4 & getViewMatrix() const;

private:
   glm::mat4 mView;
   glm::mat4 mProjection;
};

inline
const glm::mat4 & CCamera::getProjectionMatrix() const
{
   return mProjection;
}

inline
const glm::mat4 & CCamera::getViewMatrix() const
{
   return mView;
}

} /* namespace NApp */
