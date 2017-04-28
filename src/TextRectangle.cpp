#include "TextRectangle.h"

void TextRectangle::drawz(float zlo, float zhi, float ratio, float ignore1) {
    if (!visible || tex==0) {
        return;
    }

    float z_now = z.get();

    if (zlo <= z_now && z_now < zhi) {
        drawTextRectangle(ratio, x.get(), y.get(), angle.get(), xsize.get(), ysize.get(), alpha.get(), tex);
    }
}
