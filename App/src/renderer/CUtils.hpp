#pragma once

#include <string>

namespace NApp
{

/** Rendering utils. */
class CUtils
{
public:
   static bool isExtensionSupported(const char * extensionsString);
   static bool checkGLErrors();

   static std::string getFullPath(const std::string & parent, const std::string & relative);
   static std::string replaceString(const std::string & str, const std::string & pattern, const std::string & replacement);

private:
   CUtils();
};

} /* namespace NApp */
