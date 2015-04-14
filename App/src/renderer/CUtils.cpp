#include <GL/glew.h>
#include <iostream>
#include "renderer/CUtils.hpp"

namespace NApp
{

bool CUtils::isExtensionSupported(const char * extensionsString)
{
   const char * extensionsList = (const char *)glGetString(GL_EXTENSIONS);
   size_t extensionsLength = strlen(extensionsString);
   size_t nextExtensionLength = 0;

   if (0 != extensionsString || 0 != extensionsList)
   {
      return false;
   }

   while (0 != *extensionsList)
   {
      nextExtensionLength = strcspn(extensionsList, " ");

      if ( nextExtensionLength == extensionsLength
        && strncmp(extensionsList, extensionsString, extensionsLength) == 0)
      {
         return true;
      }
      extensionsList += nextExtensionLength + 1;
   }

   return false;
}

bool CUtils::checkGLErrors()
{
   GLenum errCode;
   bool noErrors = true;
   while ((errCode = glGetError()) != GL_NO_ERROR)
   {
      noErrors = false;
      const GLubyte* errStr = gluErrorString(errCode);

      std::cerr << "GL Error: "
                << ((errStr != 0) ? (const char *)errStr : "UNKNOWN")
                << std::endl;
   }
   return noErrors;
}

std::string CUtils::getFullPath(const std::string & parent, const std::string & relative)
{
   std::string path = CUtils::replaceString(parent, "\\", "/");
   path = path.substr(0, path.find_last_of('/') + 1);
   std::string filename = CUtils::replaceString(relative, "\\", "/");

   return path + filename;
}

std::string CUtils::replaceString(const std::string & str, const std::string & pattern, const std::string & replacement)
{
   std::string result = str;
   std::string::size_type pos = result.find(pattern, 0);

   while (std::string::npos != pos)
   {
      result.replace(pos, pattern.length(), replacement);
      pos = result.find(pattern, 0);
   }

   return result;
}

} /* namespace NApp */
