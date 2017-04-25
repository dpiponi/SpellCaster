#include "TextRectangle.h"

void TextRectangle::drawz(float zlo, float zhi, float ratio, float ignore1) {
    if (!visible || tex==0) {
        return;
    }

    if (zlo <= 0.0 && 0.0 < zhi) {
        // I think text is drawn at z=0.0
        drawTextRectangle(ratio, x.get(), y.get(), angle.get(), xsize.get(), ysize.get(), alpha.get(), tex);
    }
}
