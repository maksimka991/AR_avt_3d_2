#pragma once

#include <opencv2/opencv.hpp>
#include "video/IVideo.hpp"

namespace NApp
{

/** Implementation video source based on camera. */
class CVideoCamera : public IVideo
{
public:
   /** Constructor */
   explicit CVideoCamera();

   /** Destructor. */
   virtual ~CVideoCamera();

   /** @copydoc IVideo::initialize() */
   virtual bool initialize();

   /** @copydoc IVideo::captureFrame() */
   virtual std::shared_ptr<CFrame> captureFrame();

private:
   cv::VideoCapture mVideoCapture;
};

} /* namespace NApp */
