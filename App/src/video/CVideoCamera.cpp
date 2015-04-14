#include "video/CVideoCamera.hpp"

namespace NApp
{

CVideoCamera::CVideoCamera()
{
}

CVideoCamera::~CVideoCamera()
{
}

bool CVideoCamera::initialize()
{
   mVideoCapture.open(0);
   
   return mVideoCapture.isOpened();
}

std::shared_ptr<CFrame> CVideoCamera::captureFrame()
{
   cv::Mat frameBGR;
   mVideoCapture >> frameBGR;

   if (true == frameBGR.empty())
   {
      return std::shared_ptr<CFrame>();
   }

   cv::Mat frameRGB;
    cv::cvtColor(frameBGR, frameRGB, CV_BGR2RGB);

   /// fix issue in camera driver
   cv::flip(frameRGB, frameRGB, 1); // 0 - around x

   return std::shared_ptr<CFrame>(new CFrame(frameRGB));
}

} /* namespace NApp */
