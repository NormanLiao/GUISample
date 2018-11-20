#ifndef GLSLPROGRAM_H
#define GLSLPROGRAM_H

#include <nanogui/opengl.h>
#include <nanogui/glutil.h>
#include <string>
#include <glm/glm.hpp>


namespace GLSLShader {
    enum GLSLShaderType {
        VERTEX, FRAGMENT, GEOMETRY
    };
};

class GLSLProgram
{
private:
    int  handle;
    bool linked;
    std::string logString;

    int  getUniformLocation(const char * name );
    bool fileExists( const std::string & fileName );

public:
    GLSLProgram();

    bool   compileShaderFromFile( const char * fileName, GLSLShader::GLSLShaderType type );
    bool   compileShaderFromString( const std::string & source, GLSLShader::GLSLShaderType type );
    bool   link();
    bool   validate();
    void   use();

    std::string log();

    int    getHandle();
    bool   isLinked();

    void   bindAttribLocation( GLuint location, const char * name);
    void   bindFragDataLocation( GLuint location, const char * name );

    void   setUniform( const char *name, float x, float y, float z);
    void   setUniform( const char *name, const glm::vec2 & v);
    void   setUniform( const char *name, const glm::vec3 & v);
    void   setUniform( const char *name, const glm::vec4 & v);
    void   setUniform( const char *name, const glm::mat4 & m);
    void   setUniform( const char *name, const glm::mat3 & m);
    void   setUniform( const char *name, float val );
    void   setUniform( const char *name, int val );
    void   setUniform( const char *name, bool val );

    void   printActiveUniforms();
    void   printActiveAttribs();
};

#endif // GLSLPROGRAM_H
