#include <iostream>
#include <string>

#include "Letter.h"

using std::cout;
using std::endl;
using std::string;

Letter letters[256];

GLuint create_letter(FT_Face &face) {
  FT_GlyphSlot slot = face->glyph;

    if (FT_Load_Char(face, 'A', FT_LOAD_RENDER)) {
	cout << "FONT error" << endl;
	exit(1);
    }
 
    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      slot->bitmap.width, slot->bitmap.rows,
      0,
      GL_RED, GL_UNSIGNED_BYTE,
      slot->bitmap.buffer
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return textureID;
}

void makeFont(string font_name) {
    FT_Library ft;

    if (FT_Init_FreeType(&ft)) {
        fprintf(stderr, "Could not init freetype library\n");
        exit(1);
    }

    FT_Face face;

    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
    //if (FT_New_Face(ft, "/Library/Fonts/Luminari.ttf", 0, &face)) {
    //if (FT_New_Face(ft, "Luminari.ttf", 0, &face)) {
        fprintf(stderr, "Could not open font\n");
        exit(1);
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    for (int c = 32; c <= 255; ++c) {
        letters[c] = Letter(c, face);
    }
}
