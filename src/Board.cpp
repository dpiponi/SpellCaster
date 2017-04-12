#include <ft2build.h>
#include FT_FREETYPE_H

#include <SDL_image.h>

#include "SpellCaster.h"
#include "Board.h"

GLuint create_texture(const char *filename) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);

    SDL_Surface *texture_data = IMG_Load(filename);
    if (!texture_data) {
         printf("IMG_Load: %s\n", IMG_GetError());
         exit(1);
    }

#if 0
    std::vector<unsigned char> image;
    unsigned int width, height;
    unsigned int error = lodepng::decode(image, width, height, filename);
    if (error) {
        return 0; // I think 0 is reserved for other use so OK
    }
#endif
    //cout << error << endl;

    // Give the image to OpenGL
    SDL_LockSurface(texture_data);

    GLint nOfColors = texture_data->format->BytesPerPixel;
    GLenum texture_format;
    if (nOfColors == 4)     // contains an alpha channel
    {
            if (texture_data->format->Rmask == 0x000000ff)
                    texture_format = GL_RGBA;
            //else
                    //texture_format = GL_BGRA;
    } else if (nOfColors == 3)     // no alpha channel
    {
            if (texture_data->format->Rmask == 0x000000ff)
                    texture_format = GL_RGB;
            //else
                    //texture_format = GL_BGR;
    } else {
            printf("warning: the image is not truecolor..  this will probably break\n");
            // this error should not go unhandled
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_data->w, texture_data->h, 0, texture_format, GL_UNSIGNED_BYTE, texture_data->pixels);
    SDL_UnlockSurface(texture_data);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
    glGenerateMipmap(GL_TEXTURE_2D);
    SDL_FreeSurface(texture_data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    return textureID;
}

Letter::Letter(char c, FT_Face &face) : character(c) {
    FT_GlyphSlot g = face->glyph;

    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        cout << "FONT error" << endl;
        exit(1);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        g->bitmap.width,
        g->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        g->bitmap.buffer
    );

    width = g->bitmap.width;
    height = g->bitmap.rows;
    advance = g->advance.x;
    left = g->bitmap_left;
    top = g->bitmap_top;

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    texture = textureID;
}
