#include "TextRectangle.h"

void TextRectangle::draw(float ratio, float ignore1) {
    if (!visible || tex==0) {
        return;
    }

    drawTextRectangle(ratio, x.get(), y.get(), angle.get(), xsize.get(), ysize.get(), alpha.get(), tex);
}
