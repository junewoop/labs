#include "FilterIdentity.h"
#include "FilterUtils.h"
FilterIdentity::FilterIdentity()
{
    // TODO: Task 17 Initialize kernel for identity
    m_kernel.resize(9);
    for (int i = 0; i < 9; i ++){
        if (i == 4) m_kernel[i] = 1.f;
        else m_kernel[i] = 0.f;
    }
}

FilterIdentity::~FilterIdentity()
{
}

void FilterIdentity::apply(Canvas2D *canvas) {
    // TODO: Task 16 Call convolve function from superclass
    RGBA *data = canvas->data();
    FilterUtils::Convolve2D(data, canvas->width(), canvas->height(), m_kernel);
}
