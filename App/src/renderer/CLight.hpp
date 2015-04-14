#pragma once

#include <glm/glm.hpp>

namespace NApp
{

class CLight
{
public:
   CLight(
      const glm::vec3 & position,
      const glm::vec4 & ambient,
      const glm::vec4 & diffuse);

   void setPosition(const glm::vec3 & position);
   const glm::vec3 & getPosition();

   void setAmbient(const glm::vec4 & ambient);
   const glm::vec4 & getAmbient();

   void setDiffuse(const glm::vec4 & diffuse);
   const glm::vec4 & getDiffuse();

private:
   glm::vec3 mPosition;
   glm::vec4 mAmbient;
   glm::vec4 mDiffuse;
};

inline
CLight::CLight(
      const glm::vec3 & position,
      const glm::vec4 & ambient,
      const glm::vec4 & diffuse)
   : mPosition(position)
   , mAmbient(ambient)
   , mDiffuse(diffuse)
{
}

inline
void CLight::setPosition(const glm::vec3 & position)
{
   mPosition = position;
}

inline
const glm::vec3 & CLight::getPosition()
{
   return mPosition;
}

inline
void CLight::setAmbient(const glm::vec4 & ambient)
{
   mAmbient = ambient;
}

inline
const glm::vec4 & CLight::getAmbient()
{
   return mAmbient;
}

inline
void CLight::setDiffuse(const glm::vec4 & diffuse)
{
   mDiffuse = diffuse;
}

inline
const glm::vec4 & CLight::getDiffuse()
{
   return mDiffuse;
}

} /* namespace NApp */
