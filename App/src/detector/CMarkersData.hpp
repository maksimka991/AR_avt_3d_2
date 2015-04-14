#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

namespace NApp
{

struct Marker
{
   Marker(
         unsigned int id,
         const glm::mat4 & view,
         const glm::quat & rvec,
         const glm::vec3 & tvec)
      : Id(id)
      , View(view)
      , R(rvec)
      , T(tvec)
   {
   }

   unsigned int Id;
   glm::mat4 View;
   glm::quat R;
   glm::vec3 T;
};

/** Markers data. */
class CMarkersData
{
public:
   /**
    * Constructor
    * @param markers list of recognized markers
    */
   explicit CMarkersData(const std::vector<Marker> & markers);

   /** Get list of recognized markers. */
   const std::vector<Marker> & getMarkers() const;

private:
   std::vector<Marker> mMarkers;
};

inline
CMarkersData::CMarkersData(const std::vector<Marker> & markers)
   : mMarkers(markers)
{
}

inline
const std::vector<Marker> & CMarkersData::getMarkers() const
{
   return mMarkers;
}

} /* namespace NApp */
