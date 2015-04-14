#pragma once

#include <memory> // for std::shared_ptr
#include "video/CFrame.hpp"
#include "detector/CMarkersData.hpp"

namespace NApp
{

/** Interface of markers detector. */
class IDetector
{
public:
   /** Destructor */
   virtual ~IDetector();

   /**
    * Initialize detector (load some info, etc).
    * @return true if succes, false - otherwise.
    */
   virtual bool initialize() = 0;

   /**
    * @brief detect markers on frame.
    * @param frame from a video source.
    * @return smart pointer to markers data.
    */
   virtual std::shared_ptr<CMarkersData> detect(const CFrame & frame) = 0;
};

} /* namespace NApp */
