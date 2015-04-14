#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace NApp
{

class CShader
{
public:
   CShader(const std::string & vertex, const std::string & fragment);
   ~CShader();

   void bind();
   void unbind();

   GLuint getAttribLocation(const std::string & name);
   GLuint getUniformLocation(const std::string & name);

   void setUniform(const std::string & name, const glm::mat4 & matrix);
   void setUniform(const std::string & name, const std::vector<glm::mat4> & matrices);
   void setUniform(const std::string & name, const glm::vec3 & vector);
   void setUniform(const std::string & name, const glm::vec4 & vector);
   void setUniform(const std::string & name, int value);

   void enableAttributeArray(const std::string & name);
   void disableAttributeArray(const std::string & name);

   void setAttributeArray(const std::string & name, const float * values, unsigned int tupleSize);

/*private:*/
   GLuint mVertexShader;
   GLuint mFragmentShader;
   GLuint mProgram;
};

} /* namespace NApp */
