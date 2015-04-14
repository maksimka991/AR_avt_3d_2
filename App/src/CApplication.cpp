#include <cstdio>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <SDL.h>

#include "CApplication.hpp"
#include "video/CVideoCamera.hpp"
#include "video/CVideoFile.hpp"
#include "detector/CDetector.hpp"
#include "renderer/CRenderer.hpp"

namespace NApp
{
 
const unsigned int CApplication::WINDOW_HEIGHT = 480u;
const unsigned int CApplication::WINDOW_WIDTH = 640u;
const unsigned int CApplication::COLORBITS = 32u;
const unsigned int CApplication::MULTISAMPLING = 4u;
const std::string CApplication::MODELS_CONFIGURATION_PATH("my_file.txt");
 
CApplication::CApplication()
   : mSurface(0)
   , mStartTime(0)
{
}

int CApplication::run(int argc, const char argv[])
{
   if (false == initialize())
   {
      std::cerr << "initialize() failed." << std::endl;
      return EXIT_FAILURE;
   }

   if (false == createComponents(argc, argv))
   {
      std::cerr << "createComponents() failed." << std::endl;
      return EXIT_FAILURE;
   }

   runMainLoop();

   return EXIT_SUCCESS;
}

bool CApplication::initialize()
{
   if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0 ) {
     std::cerr << "SDL_Init() failed." << std::endl;
     return false;
   }

   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
   SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
   SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
   SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
   SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, MULTISAMPLING);
 
   SDL_WM_SetCaption("Window", "");

   mSurface = SDL_SetVideoMode(
         WINDOW_WIDTH,
         WINDOW_HEIGHT,
         COLORBITS,
         SDL_OPENGL | SDL_RESIZABLE | SDL_HWSURFACE | SDL_DOUBLEBUF );

   if (0 == mSurface) {
     std::cerr << "SDL_SetVideoMode() failed." << std::endl;
     return false;
   }

   mStartTime = SDL_GetTicks();

   return true;
}

bool CApplication::createComponents(int argc, const char argv[])
{
   //mVideo = std::make_shared<CVideoFile>("path to file");
   mVideo = std::make_shared<CVideoCamera>();
   if (false == mVideo->initialize())
   {
      std::cerr << "mVideo->initialize() failed." << std::endl;
      return false;
   }

   mDetector = std::make_shared<CDetector>();
   if (false == mDetector->initialize())
   {
      std::cerr << "mDetector->initialize() failed." << std::endl;
      return false;
   }

   std::vector<std::string> modelPaths;
   std::fstream modelsConfig(MODELS_CONFIGURATION_PATH);
   if (modelsConfig.is_open())
   {
      std::string path;
      while (std::getline(modelsConfig, path))
      {
         modelPaths.push_back(path);
      }
   }
   else
   {
      std::cerr << "modelsConfig.is_open() failed." << std::endl;
      return false;
   }

   mRenderer = std::make_shared<CRenderer>(
      WINDOW_WIDTH, WINDOW_HEIGHT,
      modelPaths);

   if (false == mRenderer->initialize())
   {
      std::cerr << "mRenderer->initialize() failed." << std::endl;
      return false;
   }

   return true;
}
 

void CApplication::runMainLoop()
{
   while (true)
   {
      SDL_PumpEvents();

      /*** PROCESS KEYSTATE ***/

      int numkeys;
      unsigned char* keystate = SDL_GetKeyState(&numkeys);

      if (keystate[SDLK_ESCAPE])
      {
         break;
      }

      /*** WINDOWS EVENTS ***/
      bool isQuit = false;
      SDL_Event sdlEvent; 

      /*** PROCESS EVENTS ***/ 
    
   while (SDL_PollEvent(&sdlEvent))
      {
         switch (sdlEvent.type)
         {
		  		
		 case SDL_MOUSEBUTTONDOWN:		
			 if (sdlEvent.button.button == SDL_BUTTON_LEFT)	 {
				 int x = sdlEvent.button.x;
				 int y = sdlEvent.button.y;
				 if ((x > 10) && (x < 100) && (y > 10) && (y < 100))				
				 ShellExecute(NULL, "open", "help.html", NULL, NULL, SW_SHOWNORMAL);
			 }
			 
			 break;

         case SDL_QUIT:
            isQuit = true;
            break;
         case SDL_VIDEORESIZE: 
			 mRenderer->resize(sdlEvent.resize.w, sdlEvent.resize.h); break;
         case SDL_KEYDOWN:
            switch (sdlEvent.key.keysym.sym)
            {
            //case SDLK_q:     mRenderer->resize(640, 480);  break;
            case SDLK_EQUALS:
            case SDLK_KP8:   mRenderer->scaleModel(glm::vec3(1.2f)); break;
			 
            case SDLK_MINUS:
            case SDLK_KP2:   mRenderer->scaleModel(glm::vec3(0.8f)); break;

			case SDLK_9:
            case SDLK_KP9:   mRenderer->rotateModel(glm::vec3( 15,   0,   0)); break;
			case SDLK_7:
            case SDLK_KP7:   mRenderer->rotateModel(glm::vec3(-15,   0,   0)); break;
			case SDLK_6:
            case SDLK_KP6:   mRenderer->rotateModel(glm::vec3(  0,  15,   0)); break;
			case SDLK_4:
            case SDLK_KP4:   mRenderer->rotateModel(glm::vec3(  0, -15,   0)); break;
			case SDLK_3:
            case SDLK_KP3:   mRenderer->rotateModel(glm::vec3(  0,   0,  15)); break;
			case SDLK_1:
            case SDLK_KP1:   mRenderer->rotateModel(glm::vec3(  0,   0, -15)); break;

			case SDLK_5:
			case SDLK_KP5:   mRenderer->resetTransform(); break;

            case SDLK_RIGHT: mRenderer->translateModel(glm::vec3( 2, 0,   0)); break;
            case SDLK_LEFT:  mRenderer->translateModel(glm::vec3(-2, 0,   0)); break;
            case SDLK_UP:    mRenderer->translateModel(glm::vec3(  0, 0, -2)); break;
            case SDLK_DOWN:  mRenderer->translateModel(glm::vec3(  0, 0,  2)); break;
           
			case SDLK_SPACE: mRenderer->screenshot(); break;
				 

				
				 break;

			default:
               break;
            }
            break;
         default:
            break;
         }
      }

      if (isQuit)
      {
         break;
      }

      /*** PERFORM TASK ***/
      bool result = performTask();
      if (false == result)
      {
         break;
      }

      SDL_GL_SwapBuffers();
      SDL_Delay(10);
   }
}

bool CApplication::performTask()
{ 
   std::shared_ptr<CFrame> frame = mVideo->captureFrame(); 
   if (0 == frame)
   {
      std::cout << "mVideo->captureFrame() returned null." << std::endl;
      return false;
   }

   std::shared_ptr<CMarkersData> markers = mDetector->detect(*frame);
   if (0 == markers)
   {
      std::cout << "mDetector->detect() returned null." << std::endl;
      return false;
   }
   unsigned int ellapsed = SDL_GetTicks() - mStartTime;

   mRenderer->render(ellapsed, *frame, *markers);

   IconData icon = IconData::loadFromFile("data/icon.png");
   mRenderer->renderIcon(icon, glm::ivec2(10, 10));
   

   //mRenderer->renderText(FontSize::SMALL, "SAR!", glm::ivec2(40, 20), glm::ivec3(20, 255, 30));

   std::stringstream sstr;
   sstr << "FPS: " << mRenderer->getFps(); 
   SDL_WM_SetCaption(sstr.str().c_str(), "");

   return true;
}

} /* namespace NApp */
