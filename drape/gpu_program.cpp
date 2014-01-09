#include "gpu_program.hpp"
#include "glfunctions.hpp"

#include "../base/assert.hpp"

#ifdef DEBUG
class UniformValidator
{
private:
  uint32_t m_programID;
  map<string, UniformTypeAndSize> m_uniformsMap;

public:
  UniformValidator(uint32_t programId)
    : m_programID(programId)
  {
    int32_t numberOfUnis = GLFunctions::glGetProgramiv(m_programID, GLConst::GLActiveUniforms);
    for (size_t unIndex = 0; unIndex < numberOfUnis; ++unIndex)
    {
      string name;
      glConst type;
      UniformSize size;
      GLCHECK(GLFunctions::glGetActiveUniform(m_programID, unIndex, &size, &type, name));
      m_uniformsMap[name] = make_pair(type, size);
    }
  }

  bool HasUniform(string const & name)
  {
    return m_uniformsMap.find(name) != m_uniformsMap.end();
  }

  bool HasValidTypeAndSizeForName(string const & name, glConst type, UniformSize size)
  {
    if (HasUniform(name))
    {
      UniformTypeAndSize actualParams = m_uniformsMap[name];
      return type == actualParams.first && size == actualParams.second;
    }
    else
      return false;
  }
};

bool GpuProgram::HasUniform(string const & name, glConst type, UniformSize size)
{
  return m_validator->HasValidTypeAndSizeForName(name, type, size);
}
#endif

GpuProgram::GpuProgram(RefPointer<Shader> vertexShader, RefPointer<Shader> fragmentShader)
{
  m_programID = GLFunctions::glCreateProgram();
  GLFunctions::glAttachShader(m_programID, vertexShader->GetID());
  GLFunctions::glAttachShader(m_programID, fragmentShader->GetID());

  string errorLog;
  VERIFY(GLFunctions::glLinkProgram(m_programID, errorLog), ());

  GLFunctions::glDetachShader(m_programID, vertexShader->GetID());
  GLFunctions::glDetachShader(m_programID, fragmentShader->GetID());

  //get uniforms info
  m_validator.reset(new UniformValidator(m_programID));
}

GpuProgram::~GpuProgram()
{
  Unbind();
  GLFunctions::glDeleteProgram(m_programID);
}

void GpuProgram::Bind()
{
  GLFunctions::glUseProgram(m_programID);
}

void GpuProgram::Unbind()
{
  GLFunctions::glUseProgram(0);
}

int8_t GpuProgram::GetAttributeLocation(const string & attributeName) const
{
  return GLFunctions::glGetAttribLocation(m_programID, attributeName);
}

int8_t GpuProgram::GetUniformLocation(const string & uniformName) const
{
  return GLFunctions::glGetUniformLocation(m_programID, uniformName);
}

void GpuProgram::ActivateSampler(uint8_t textureBlock, const string & samplerName)
{
  ASSERT(GLFunctions::glGetCurrentProgram() == m_programID, ());
  int8_t location = GLFunctions::glGetUniformLocation(m_programID, samplerName);
  GLFunctions::glUniformValuei(location, textureBlock);
}
