#pragma once

#include <opencv2/opencv.hpp>

namespace NApp
{

/** Represents frame from a video source. */
class CFrame
{
public:
   /**
    * Constructor
    * @param frame frame data
    */
   explicit CFrame(const cv::Mat & frame);

   /**
    * Get frame data.
    * @return frame data (Mat).
    */
   const cv::Mat getMat() const;

private:
  cv::Mat mFrame;

};

inline
CFrame::CFrame(const cv::Mat & frame)
   : mFrame(frame)
{
}

inline
const cv::Mat CFrame::getMat() const
{
   return mFrame;
}

} /* namespace NApp */
