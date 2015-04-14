#include "renderer/CFpsCounter.hpp"
#include <SDL.h>

namespace NApp
{

CFpsCounter::CFpsCounter()
  : mFrameTimeLast(SDL_GetTicks())
  , mFrameCount(0)
  , mFramesPerSecond(0u)
{
   memset(mFrameTimes, 0, sizeof(mFrameTimes));
}

void CFpsCounter::onNewFrame()
{
   unsigned int frameTimesIndex = mFrameCount % FRAME_VALUES;
   unsigned int getTicksValue = SDL_GetTicks();
   mFrameTimes[frameTimesIndex] = getTicksValue - mFrameTimeLast;
   mFrameTimeLast = getTicksValue;
   mFrameCount++;

   unsigned int count = FRAME_VALUES;
   if (mFrameCount < FRAME_VALUES)
   {
      count = mFrameCount;
   }

   mFramesPerSecond = 0;
   for (unsigned int i = 0; i < count; i++)
   {
      mFramesPerSecond += mFrameTimes[i];
   }

   mFramesPerSecond /= count;

   // now to make it an actual frames per second value...
   mFramesPerSecond = (int) (1000.f / mFramesPerSecond);
}

} /* namespace NApp */
