#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include "video/IVideo.hpp"

namespace NApp
{

/** Implementation video source based on file. */
class CVideoFile : public IVideo
{
public:
   /**
    * Constructor.
    * @param path to video file.
    */
   explicit CVideoFile(const std::string & path);

   /** Destructor. */
   virtual ~CVideoFile();

   /** @copydoc IVideo::initialize() */
   virtual bool initialize();

   /** @copydoc IVideo::captureFrame() */
   virtual std::shared_ptr<CFrame> captureFrame();

private:
   std::string mPath;
   cv::VideoCapture mVideoCapture;
};

} /* namespace NApp */
