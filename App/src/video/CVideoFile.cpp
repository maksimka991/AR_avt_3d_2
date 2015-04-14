#include "video/CVideoFile.hpp"

namespace NApp
{

CVideoFile::CVideoFile(const std::string & path)
   : mPath(path)
{
}

CVideoFile::~CVideoFile()
{
   mVideoCapture.release();
}

bool CVideoFile::initialize()
{
   mVideoCapture.open(mPath);

   return mVideoCapture.isOpened();
}

std::shared_ptr<CFrame> CVideoFile::captureFrame()
{
   cv::Mat frameBGR;
   mVideoCapture >> frameBGR;

   if (true == frameBGR.empty())
   {
      return std::shared_ptr<CFrame>();
   }

   cv::Mat frameRGB;
   cv::cvtColor(frameBGR, frameRGB, CV_BGR2RGB);

   return std::shared_ptr<CFrame>(new CFrame(frameRGB));
}

} /* namespace NApp */
