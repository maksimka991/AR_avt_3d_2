#pragma once

#include <memory> // for std::shared_ptr
#include "video/CFrame.hpp"

namespace NApp
{

/** Interface of video source. */
class IVideo
{
public:
   /** Destructor */
   virtual ~IVideo();

   /**
    * Initialize video source (open file, camera).
    * @return true if succes, false - otherwise.
    */
   virtual bool initialize() = 0;

   /**
    * @brief captureFrame
    * @return smart pointer to frame.
    * @note if method returns null it seems that file ended or camera was closed.
    */
   virtual std::shared_ptr<CFrame> captureFrame() = 0;
};

} /* namespace NApp */
