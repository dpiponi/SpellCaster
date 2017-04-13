#ifndef LETTER_H
#define LETTER_H

#include <string>

using std::string;

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/glad.h>

void makeFont(string font_name, int font_size);

class Letter {
public:
    GLuint texture;
    char character;
    int width;
    int height;
    int top;
    int left;
    int advance;
    Letter() { }
    Letter(char c, FT_Face &face);
};

extern Letter letters[256];

#endif
