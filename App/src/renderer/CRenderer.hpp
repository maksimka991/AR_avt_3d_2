#pragma once

#include <GL/glew.h>
#include <SDL_ttf.h>
#include <memory>
#include <map>
#include <string>
#include <vector>
#include "renderer/IRenderer.hpp"

namespace NApp
{

class CShader;
class CModel;
class CCamera;
class CLight;
class CFpsCounter;

/** Interface of renderer. */
class CRenderer : public IRenderer
{
public:
   /** Constructor */
   CRenderer(
      int width, int height,
      const std::vector<std::string> & modelPaths);

   /** Destructor */
   virtual ~CRenderer();

   /** @copydoc IRenderer::initialize() */
   virtual bool initialize();

   /** @copydoc IRenderer::getFps() */
   virtual int getFps() const;

   /** @copydoc IRenderer::render() */
   virtual void render(
      unsigned int ellapsedTime,
      const CFrame & frame,
      const CMarkersData & markers);

   /** @copydoc IRenderer::renderText() */
   virtual void renderText(
      FontSize::ESize size,
      const std::string & text,
      const glm::ivec2 & position,
      const glm::ivec3 & color = glm::ivec3(255));

   /** @copydoc IRenderer::renderIcon() */
   virtual void renderIcon(
      const IconData & icon,
      const glm::ivec2 & position);

   /** @copydoc IRenderer::resize() */
   virtual void resize(int width, int height);

   /** @copydoc IRenderer::scaleModel() */
   virtual void scaleModel(const glm::vec3 & scaling);

   /** @copydoc IRenderer::rotateModel() */
   virtual void rotateModel(const glm::vec3 & rotation);

   /** @copydoc IRenderer::translateModel() */
   virtual void translateModel(const glm::vec3 & transition);

   /** @copydoc IRenderer::resetTransform() */
   virtual void resetTransform();

   virtual void screenshot();

private:
   bool initScene();
   bool loadModels();
   bool initGl();
   bool initFont();

   void createBackground(
      unsigned int width,
      unsigned int height);

   CModel & getModel(unsigned int markerId);

   void renderBackground(const CFrame & frame);

   void renderSurface(SDL_Surface * surface, const glm::ivec2 & position);

   void renderModel(unsigned int ellapsedTime, const CMarkersData & markers);

private:
   static const std::string EXTENSIONS[];
   static const std::string STANDART_MODEL_PATH;
   static const std::string FONT_PATH;

private:
   typedef std::shared_ptr<CModel> tModel;
   typedef std::vector<tModel> tModelList;

   typedef std::map<FontSize::ESize, TTF_Font *> tFontsList;
private:
   std::vector<std::string> mModelPaths;
   int mWidth;
   int mHeight;
   unsigned int mBGTexture;
   std::shared_ptr<CFpsCounter> mFpsCounter;
   std::shared_ptr<CShader> mShader;
   std::shared_ptr<CCamera> mCamera;
   std::shared_ptr<CLight> mLight;
   tFontsList mFonts;
   tModelList mModels;
   tModel mDefaultModel;

   glm::vec3 mScale;
   glm::vec3 mRotation;
   glm::vec3 mTransition;
};

inline
void CRenderer::scaleModel(const glm::vec3 & scale)
{
	mScale = mScale*scale;
  
}

inline
void CRenderer::rotateModel(const glm::vec3 & rotation)
{
	mRotation = mRotation+rotation;
}

inline
void CRenderer::translateModel(const glm::vec3 & transition)
{
	mTransition = transition;
}

inline
void CRenderer::resetTransform()
{
   mScale = glm::vec3(1.f);
   mRotation = glm::vec3(0.f);
   mTransition = glm::vec3(0.f);
}

} /* namespace NApp */
