#include "Texture.h"
#include <glew.h>
#include <assert.h>
#include <iostream>

using namespace RE330;
using namespace std;

Texture::Texture(QImage *tex) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, (GLuint*)&mId);
    glBindTexture(GL_TEXTURE_2D, mId);

    GLvoid *buf = (GLvoid*)getData(tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->width(), tex->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    delete buf;

    // If this assertion fails your texture wasn't passed to OpenGL
    // successfully.
    GLenum error = glGetError();
    switch(error) {
        case GL_INVALID_ENUM :
            cout << "Invalid enum" << endl;
            break;
        case GL_INVALID_VALUE :
            cout << "Invalid value" << endl;
            break;
        case GL_INVALID_OPERATION :
            cout << "Invalid operation" << endl;
            break;
        case GL_STACK_OVERFLOW :
            cout << "Stack overflow" << endl;
            break;
        case GL_STACK_UNDERFLOW :
            cout << "Stack underflow" << endl;
            break;
        case GL_OUT_OF_MEMORY :
            cout << "Out of memory" << endl;
            break;
    }

    assert(glGetError()==GL_NO_ERROR);
}

int Texture::getId() const {
    return mId;
}

int *Texture::getData(QImage *tex) {
    int *buf;
    buf = new int[tex->width()*tex->height()];
    for(int i=0; i<tex->height(); i++) {
        for(int j=0; j<tex->width(); j++) {
            buf[(tex->height()-i-1)*tex->width()+j]=qRed(tex->pixel(j,i)) | qGreen(tex->pixel(j,i)) << 8 | qBlue(tex->pixel(j,i)) << 16;
        }
    }
    return buf;
}
