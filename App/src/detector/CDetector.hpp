#pragma once

#include <ALVAR/Camera.h>
#include <ALVAR/MarkerDetector.h>
#include <ALVAR/Marker.h>
#include "detector/IDetector.hpp"

namespace NApp
{

/** Implementation of markers detector. */
class CDetector : public IDetector
{
public:
   CDetector();

   /** @copydoc IDetector::initialize() */
   virtual bool initialize();

   /** @copydoc IDetector::detect() */
   virtual std::shared_ptr<CMarkersData> detect(const CFrame & frame);

private:
   alvar::Camera mCamera;
   alvar::MarkerDetector<alvar::MarkerData> mMarkerDetector;
};

} /* namespace NApp */
