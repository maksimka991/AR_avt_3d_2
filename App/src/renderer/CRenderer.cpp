#include "renderer/CRenderer.hpp"
#include "renderer/CUtils.hpp"
#include "renderer/CFpsCounter.hpp"
#include "renderer/CShader.hpp"
#include "renderer/CCamera.hpp"
#include "renderer/CLight.hpp"
#include "renderer/CModel.hpp"
#include "renderer/ShaderData.hpp"

namespace NApp
{

const std::string CRenderer::STANDART_MODEL_PATH = "data/model/dwarf.x"; 
const std::string CRenderer::FONT_PATH = "data/ARIAL.TTF";

const std::string CRenderer::EXTENSIONS[] = {
   "GL_EXT_framebuffer_object",
   "GL_EXT_framebuffer_sRGB",
   "GL_EXT_vertex_array",
   "GL_ARB_vertex_program",
   "GL_ARB_fragment_program",
   "GL_ARB_vertex_buffer_object"
};

CRenderer::CRenderer(
      int width, int height,
      const std::vector<std::string> & modelPaths)
   : mModelPaths(modelPaths)
   , mWidth(width)
   , mHeight(height)
   , mBGTexture(0)
   , mScale(1.f)
   , mRotation(0.f)
   , mTransition(0.f)
{
}

CRenderer::~CRenderer()
{
   if (0 != TTF_WasInit())
   {
      TTF_Quit();
   }
}

bool CRenderer::initialize()
{
   GLenum err = glewInit();
   if (GLEW_OK != err)
   {
      std::cerr << "glewInit() failed." << std::endl;
      return false;
   }

//   foreach (const std::string & extension, EXTENSIONS)
//   {
//      if (false == CUtils::isExtensionSupported(extension.c_str()))
//      {
//         std::cerr << extension << " isn't supported." << std::endl;
//         return false;
//      }
//   }

   if (false == initGl())
   {
      std::cerr << "initGl() failed." << std::endl;
      return false;
   }

   if (false == initFont())
   {
      std::cerr << "initFont() failed." << std::endl;
      return false;
   }

   if (false == initScene())
   {
      std::cerr << "initScene() failed." << std::endl;
      return false;
   }

   if (false == loadModels())
   {
      std::cerr << "loadModels() failed." << std::endl;
      return false;
   }

   mFpsCounter = std::make_shared<CFpsCounter>();

   return true;
}

CModel & CRenderer::getModel(unsigned int markerId)
{

		if (markerId >= mModels.size())
			return *mDefaultModel;
		else
			return *mModels[markerId];
	/*
   if ( markerId < mModels.size()    || 0 != mModels[markerId]	 )
   {
      return *mModels[markerId];
	   
   }
     
    assert(0 != mDefaultModel);
    return *mDefaultModel;*/


}

bool CRenderer::initGl()
{
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_NORMALIZE);
   glEnable(GL_BLEND);
   glEnable(GL_POLYGON_SMOOTH);
   glEnable(GL_LINE_SMOOTH);
   glEnable(GL_MULTISAMPLE);

   glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
   glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
   glClearDepth(1.0f);
   glDepthFunc(GL_LEQUAL);
   glFrontFace(GL_CCW);
   glCullFace(GL_BACK);

   return CUtils::checkGLErrors();
}

bool CRenderer::initFont()
{
   if (0 == TTF_WasInit() && -1 == TTF_Init())
   {
      return false;
   }

   mFonts[FontSize::SMALL]  = TTF_OpenFont(FONT_PATH.c_str(), (int)FontSize::SMALL);
   mFonts[FontSize::MIDDLE] = TTF_OpenFont(FONT_PATH.c_str(), (int)FontSize::MIDDLE);
   mFonts[FontSize::BIG]    = TTF_OpenFont(FONT_PATH.c_str(), (int)FontSize::BIG);

   return
        (0 != mFonts[FontSize::SMALL])
     && (0 != mFonts[FontSize::MIDDLE])
     && (0 != mFonts[FontSize::BIG]);
}

bool CRenderer::initScene()
{
   mShader = std::make_shared<CShader>(
      SHADER_VERTEX_SOURCE,
      SHADER_FRAGMENT_SOURCE);

   mCamera = std::make_shared<CCamera>(
      0.01f,
      100.0f,
      45.0f,
      (float)mWidth / mHeight);

   mLight = std::make_shared<CLight>(
      glm::vec3(500.0, 30.0, 50.0),
      glm::vec4(0.6f, 0.6f, 0.56f, 1.0f),
      glm::vec4(0.75f, 0.75f, 0.68f, 1.0f));

   return true;
}

void CRenderer::resize(int width, int height)
{
   mWidth = width;
   mHeight = height;

   mCamera = std::make_shared<CCamera>(
      0.01f,
      100.0f,
      45.0f,
      (float)mWidth / mHeight);
}

bool CRenderer::loadModels()
{
   float percent = 100.f / (mModelPaths.size()+1 );

   for (size_t i = 0; i < mModelPaths.size(); ++i)
   {
      tModel model = CModel::load(mModelPaths[i]);
      std::cout << i << "] " << ((0 != model) ? "Success" : "Failure")
                << " '" << mModelPaths[i] << "' "
                << percent * i << "%"
                << std::endl;

      mModels.push_back(model);
   }

   mDefaultModel = CModel::load(STANDART_MODEL_PATH);
   std::cout << "D] " << ((0 != mDefaultModel) ? "Success" : "Failure")
             << " '" << STANDART_MODEL_PATH << "' "
             << "100%"
             << std::endl;

   return (0 != mDefaultModel);
}

void CRenderer::createBackground(
   unsigned int width,
   unsigned int height)
{
   // Create Texture
   glGenTextures(1, &mBGTexture);
   glBindTexture(GL_TEXTURE_2D, mBGTexture);

   glTexImage2D(GL_TEXTURE_2D,
      0,
      GL_RGB,
      width, height,
      0,
      GL_RGB, GL_UNSIGNED_BYTE,
      0);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   glBindTexture(GL_TEXTURE_2D, 0);
}

int CRenderer::getFps() const
{
   if (0 != mFpsCounter)
   {
      return mFpsCounter->getFPS();
   }
   return 0u;
}

void CRenderer::render(
   unsigned int ellapsedTime,
   const CFrame & frame,
   const CMarkersData & markers)
{
   glViewport(0, 0, mWidth, mHeight);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   CUtils::checkGLErrors();

   renderBackground(frame);
   CUtils::checkGLErrors();

   renderModel(ellapsedTime, markers);
   CUtils::checkGLErrors();

   mFpsCounter->onNewFrame();
}

void CRenderer::renderBackground(const CFrame & frame)
{
   const cv::Mat & img = frame.getMat();
   if (0 == mBGTexture)
   {
      createBackground(img.cols, img.rows);
   }

   glDisable(GL_DEPTH_TEST);
   glDisable(GL_CULL_FACE);

   glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

   // change coordinate system according aspect ratio of window rect
   // Update model-view matrix (inside glOrtho)
   float wa = (float)mWidth / mHeight;
   if (wa > 1.0) {
     glOrtho(
        -wa,    wa,
        -1.0,  1.0,
         1.0, -1.0);
   } else {
     glOrtho(
        -1.0,  1.0,
        -1.0 / wa, 1.0 / wa,
         1.0, -1.0);
   }

   glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();

   glBindTexture(GL_TEXTURE_2D, mBGTexture);
   glTexSubImage2D(
      GL_TEXTURE_2D,
      0,
      0, 0, img.cols, img.rows,
      GL_RGB, GL_UNSIGNED_BYTE,
      img.data);

   float fa = (float)img.cols / img.rows;
   // assume that aspect of frame always more then 1.0
   glBegin(GL_QUADS);
      glTexCoord2f(0.f, 1.f); glVertex3f(-fa, -1.f, 0.f);
      glTexCoord2f(1.f, 1.f); glVertex3f( fa, -1.f, 0.f);
      glTexCoord2f(1.f, 0.f); glVertex3f( fa, 1.f, 0.f);
      glTexCoord2f(0.f, 0.f); glVertex3f(-fa, 1.f, 0.f);
   glEnd();

   glMatrixMode(GL_PROJECTION); glPopMatrix();
   glMatrixMode(GL_MODELVIEW);  glPopMatrix();
}

void CRenderer::renderSurface(
   SDL_Surface * surface,
   const glm::ivec2 & position)
{
   GLuint texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA,
      surface->w, surface->h,
      0,
      GL_BGRA, GL_UNSIGNED_BYTE,
      surface->pixels);

   glMatrixMode(GL_MODELVIEW);  glPushMatrix(); glLoadIdentity();
   gluOrtho2D(0, mWidth, mHeight, 0);

   glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();

   glDisable(GL_DEPTH_TEST);
   glDisable(GL_CULL_FACE);

   glBegin(GL_QUADS);
      glTexCoord2f(0.f, 0.f); glVertex2f(position.x,              position.y);
      glTexCoord2f(1.f, 0.f); glVertex2f(position.x + surface->w, position.y);
      glTexCoord2f(1.f, 1.f); glVertex2f(position.x + surface->w, position.y + surface->h);
      glTexCoord2f(0.f, 1.f); glVertex2f(position.x,              position.y + surface->h);
   glEnd();

   glMatrixMode(GL_PROJECTION);   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);    glPopMatrix();

   glDeleteTextures(1, &texture);
}

void CRenderer::renderText(
   FontSize::ESize size,
   const std::string & text,
   const glm::ivec2 & position,
   const glm::ivec3 & color)
{
   TTF_Font * font = mFonts[size];
   if (0 == font)
   {
      return;
   }

   SDL_Color foreground = { color.r, color.g, color.b };
   SDL_Surface * textSurface = TTF_RenderText_Blended(
      font,
      text.c_str(),
      foreground);

   renderSurface(textSurface, position);

   SDL_FreeSurface(textSurface);
}
const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);

	strftime(buf, sizeof(buf), "%Y_%m_%d_%H_%M_%S", &tstruct);

	return buf;
}
void CRenderer::screenshot()
{
	unsigned char * pixels = new unsigned char[mWidth*mHeight * 4]; // 4 bytes for RGBA
	glReadPixels(0, 0, mWidth, mHeight, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
	SDL_Surface * surf = SDL_CreateRGBSurfaceFrom(pixels, mWidth, mHeight, 8 * 4, mWidth * 4, 0, 0, 0, 0);

	 
	std::string PATCH = "c:/" + currentDateTime() + ".bmp";
	SDL_SaveBMP(surf, PATCH.c_str());


	SDL_FreeSurface(surf);

	delete[] pixels;
}
void CRenderer::renderIcon(
   const IconData & icon,
   const glm::ivec2 & position)
{
   SDL_Surface * textSurface = 0;

   IplImage img = (IplImage)icon.img;
   if (4 == img.nChannels)
   {
      textSurface = SDL_CreateRGBSurfaceFrom(
         (void*)img.imageData,
         img.width, img.height,
         img.depth * img.nChannels,
         img.widthStep,
         0x0000ff00, 0x00ff0000, 0xff000000, 0x000000ff);
   }
   else
   {
      return;
   }

   renderSurface(textSurface, position);
}

void CRenderer::renderModel(
   unsigned int ellapsedTime,
   const CMarkersData & markers)
{
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);

   mShader->bind();

   mShader->setUniform("lightPosition", mLight->getPosition());
   mShader->setUniform("lightAmbientColor", mLight->getAmbient());
   mShader->setUniform("lightDiffuseColor", mLight->getDiffuse());

   for (size_t i = 0; i < markers.getMarkers().size(); ++i)
   {
      const Marker & marker = markers.getMarkers()[i];

      //mCamera->setPosition(glm::vec3(0.f, 0.f, 2.f), glm::quat(0.f, 0.f, 0.f, 1.f));
      mCamera->setPosition(marker.T, marker.R);

      //set shader uniforms
      mShader->setUniform("projectionMatrix", mCamera->getProjectionMatrix());
      //mShader->setUniform("viewMatrix", mCamera->getViewMatrix());
      mShader->setUniform("viewMatrix", marker.View);

     CModel & model = getModel(marker.Id);

     // transform model
     model.scale(mScale);
     model.rotate(mRotation);
     model.translate(mTransition);

     model.render(*mShader, ellapsedTime);
   }
   mShader->unbind();
}

} /* namespace NApp */
