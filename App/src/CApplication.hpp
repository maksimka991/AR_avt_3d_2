#pragma once

#include <memory> // for std::shared_ptr
#include <string>
 

struct SDL_Surface;

namespace NApp
{

class IVideo;
class IDetector;
class IRenderer;

/** Implements application logic. */
class CApplication 
{
public:
   /** Constructor. */
   CApplication();

   /**
    * Run application.
    * @param argc count of command line arguments
    * @param argv array of command line arguments
    */
   int run(int argc, const char argv[]);

private:
   bool initialize();

   bool createComponents(int argc, const char argv[]);

   void runMainLoop();
   void Screenshot(int w, int h);

   bool performTask();

private:
   static const unsigned int WINDOW_HEIGHT;
   static const unsigned int WINDOW_WIDTH;
   static const unsigned int COLORBITS;
   static const unsigned int MULTISAMPLING;
   static const std::string  MODELS_CONFIGURATION_PATH;

private:
   SDL_Surface * mSurface;
   unsigned int mStartTime;

   std::shared_ptr<IVideo>    mVideo;
   std::shared_ptr<IDetector> mDetector;
   std::shared_ptr<IRenderer> mRenderer;
};

} /* namespace NApp */
