#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include "renderer/CShader.hpp"

namespace NApp
{

CShader::CShader(const std::string & vertex, const std::string & fragment)
{
   mVertexShader = glCreateShader(GL_VERTEX_SHADER);
   mFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

   const char * vertexSrc = vertex.c_str();
   const char * fragmentSrc = fragment.c_str();
   glShaderSource(mVertexShader, 1, (const GLchar **) &vertexSrc, 0);
   glShaderSource(mFragmentShader, 1, (const GLchar **) &fragmentSrc, 0);
   glCompileShader(mVertexShader);
   glCompileShader(mFragmentShader);

   GLint compiled;

   glGetShaderiv(mVertexShader, GL_COMPILE_STATUS, &compiled);
   if (GL_FALSE == compiled)
   {
      GLint length = 0;
      glGetShaderiv(mVertexShader, GL_INFO_LOG_LENGTH, &length);
      if (length > 0)
      {
         std::unique_ptr<char[]> log(new char[length]);
         glGetShaderInfoLog(mVertexShader, length, 0, log.get());
         std::cerr << "Errors:\n" << log.get() << std::endl;
      }
      std::cerr << "Vertex shader not compiled" << std::endl;
   }

   glGetShaderiv(mFragmentShader, GL_COMPILE_STATUS, &compiled);
   if (GL_FALSE == compiled)
   {
      GLint length = 0;
      glGetShaderiv(mFragmentShader, GL_INFO_LOG_LENGTH, &length);
      if(length > 0)
      {
         std::unique_ptr<char[]> log(new char[length]);
         glGetShaderInfoLog(mFragmentShader, length, 0, log.get());
         std::cerr << "Errors:\n" << log.get() << std::endl;
      }
      std::cerr << "Fragment shader not compiled" << std::endl;
   }

   mProgram = glCreateProgram();
   glAttachShader(mProgram, mVertexShader);
   glAttachShader(mProgram, mFragmentShader);

   glLinkProgram(mProgram);

   GLint isLinked;
   glGetProgramiv(mProgram, GL_LINK_STATUS, &isLinked);

   if (GL_FALSE == isLinked)
   {
      std::cerr << "Shader not linked." << std::endl;
   }
}

CShader::~CShader()
{
   glDeleteProgram(mProgram);
   glDeleteShader(mVertexShader);
   glDeleteShader(mFragmentShader);
}

void CShader::bind()
{
   glUseProgram(mProgram);
}

void CShader::unbind()
{
   glUseProgram(0);
}

GLuint CShader::getAttribLocation(const std::string & name)
{
   return glGetAttribLocation(mProgram, name.c_str());
}

GLuint CShader::getUniformLocation(const std::string & name)
{
   return glGetUniformLocation(mProgram, name.c_str());
}

void CShader::setUniform(const std::string & name, const glm::mat4 & matrix)
{
   glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void CShader::setUniform(const std::string & name, const std::vector<glm::mat4> & matriсes)
{
   glUniformMatrix4fv(getUniformLocation(name), matriсes.size(), GL_FALSE, (const GLfloat *)matriсes.data());
}

void CShader::setUniform(const std::string & name, const glm::vec3 & vector)
{
   glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(vector));
}

void CShader::setUniform(const std::string & name, const glm::vec4 & vector)
{
   glUniform4fv(getUniformLocation(name), 1, glm::value_ptr(vector));
}

void CShader::setUniform(const std::string & name, int value)
{
   glUniform1i(getUniformLocation(name), value);
}

void CShader::enableAttributeArray(const std::string & name)
{
   glEnableVertexAttribArray(getAttribLocation(name));
}

void CShader::disableAttributeArray(const std::string & name)
{
   glDisableVertexAttribArray(getAttribLocation(name));
}

void CShader::setAttributeArray(const std::string & name, const float * values, unsigned int tupleSize)
{
   glVertexAttribPointer(getAttribLocation(name), tupleSize, GL_FLOAT, GL_FALSE, 0, values);
}

} /* namesapce NApp */
