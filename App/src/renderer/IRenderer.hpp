#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include "video/CFrame.hpp"
#include "detector/CMarkersData.hpp"

namespace NApp
{

struct IconData
{
   cv::Mat img;

   static IconData loadFromFile(const std::string & filename)
   {
      IconData icon;
      icon.img = cv::imread(filename, CV_LOAD_IMAGE_UNCHANGED);

      return icon;
   }
};

struct FontSize
{
   enum ESize
   {
      SMALL = 20,
      MIDDLE = 30,
      BIG = 40
   };
};

/** Interface of renderer. */
class IRenderer
{
public:
   /** Destructor */
   virtual ~IRenderer();

   /**
    * Initialize renderer (create resources, etc).
    * @return true if succes, false - otherwise.
    */
   virtual bool initialize() = 0;

   /** Get current FPS value. */
   virtual int getFps() const = 0;

   /**
    * @brief render
    * @param ellapsedTime ellaspsed time from previous call
    * @param frame frame from camera
    * @param markers markers info
    */
   virtual void render(
      unsigned int ellapsedTime,
      const CFrame & frame,
      const CMarkersData & markers) = 0;

   /**
    * @brief Render text at position.
    * @param[in] text text for rendering
    * @param[in] position position of text
    * @param[in] color color of text
    */
   virtual void renderText(
      FontSize::ESize size,
      const std::string & text,
      const glm::ivec2 & position,
      const glm::ivec3 & color = glm::ivec3(255)) = 0;

   /**
    * @brief Render icon at position.
    * Support only icon with alpha channel (RGBA)
    * @param[in] icon data of icon
    * @param[in] position position of icon
    */
   virtual void renderIcon(
      const IconData & icon,
      const glm::ivec2 & position) = 0;

   /** @brief Notify render about window resizing. */
   virtual void  resize(int width, int height) = 0;

   /** @{ Methods for setting transformation of model. */
   virtual void scaleModel(const glm::vec3 & scale) = 0;
   virtual void rotateModel(const glm::vec3 & rotation) = 0;
   virtual void translateModel(const glm::vec3 & transition) = 0;
   /** @} */

   /** @brief Reset all transformation. */
   virtual void resetTransform() = 0;

   virtual void  screenshot() = 0;
};

} /* namespace NApp */
