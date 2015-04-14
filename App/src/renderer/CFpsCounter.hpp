#pragma once

#include <string>

namespace NApp
{

class CFpsCounter
{
public:
   CFpsCounter();

   void onNewFrame();

   int getFPS() const;

private:
   static const unsigned int FRAME_VALUES = 10u;

private:
   std::string mFpsString;
   unsigned int mFrameTimes[FRAME_VALUES]; // An array to store frame times:
   unsigned int mFrameTimeLast;  // Last calculated SDL_GetTicks
   unsigned int mFrameCount;     // total frames rendered
   int mFramesPerSecond;       // the value you want
};

inline
int CFpsCounter::getFPS() const
{
   return mFramesPerSecond;
}

} /* namespace NApp */
