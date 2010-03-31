#include "Shader.h"
#include "glew.h"
#include <QtOpenGL>
#include <assert.h>

using namespace RE330;

Shader::Shader(char *vertexFileName, char *fragmentFileName) {
    char *vs = NULL,*fs = NULL;

    vs = readSource(vertexFileName);
    fs = readSource(fragmentFileName);

    // If these assertions fail your shader couldn't be read from file.
    assert(fs!=NULL);
    assert(vs!=NULL);

    const char * ff = fs;
    const char * vv = vs;

    if (glewIsSupported("GL_VERSION_2_0")) {  // OpenGL 2.0 syntax

        v = glCreateShader(GL_VERTEX_SHADER);
        f = glCreateShader(GL_FRAGMENT_SHADER);

        glShaderSource(v, 1, &vv,NULL);
        glShaderSource(f, 1, &ff,NULL);

        free(vs);free(fs);

        glCompileShader(v);
        glCompileShader(f);

        p = glCreateProgram();

        glAttachShader(p,f);
        glAttachShader(p,v);

        glLinkProgram(p);

    }

    else {  // ARB extensions syntax

        v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
        f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

        glShaderSourceARB(v, 1, &vv,NULL);
        glShaderSourceARB(f, 1, &ff,NULL);

        free(vs);free(fs);

        glCompileShaderARB(v);
        glCompileShaderARB(f);

        p = glCreateProgramObjectARB();

        glAttachObjectARB(p,f);
        glAttachObjectARB(p,v);

        glLinkProgramARB(p);

    }

    // If this assertion fails your shader programs couldn't be compiled
    // or linked by OpenGL.
    assert(glGetError()==GL_NO_ERROR);
}

void Shader::use() {
    if (glewIsSupported("GL_VERSION_2_0")) {  // OpenGL 2.0 syntax
        glUseProgram(p);
    }
    else {  // ARB extensions syntax
        glUseProgramObjectARB(p);
    }
    // If this assertion fails your shader program couldn't be activated
    // by OpenGL.

    assert(glGetError()==GL_NO_ERROR);
}

void Shader::disable()
{
    if (glewIsSupported("GL_VERSION_2_0")) {  // OpenGL 2.0 syntax
        glUseProgram(0);  // OpenGL 2.0 syntax
    }
    else {  // ARB extensions syntax
        glUseProgramObjectARB(0);  // ARB extensions syntax
    }
}

char *Shader::readSource(char *fn) {

    FILE *fp;
    char *content = NULL;

    int count=0;

    if (fn != NULL) {
        fp = fopen(fn,"rt");

        if (fp != NULL) {

      fseek(fp, 0, SEEK_END);
      count = ftell(fp);
      rewind(fp);

            if (count > 0) {
                content = (char *)malloc(sizeof(char) * (count+1));
                count = fread(content,sizeof(char),count,fp);
                content[count] = '\0';
            }
            fclose(fp);
        }
    }
    return content;
}
