#include "FilterShift.h"
#include "FilterUtils.h"

FilterShift::FilterShift(ShiftDirection direction):
    m_shiftDir(direction)
{
    // TODO: Task 19 Initialize kernel for shift
    m_kernel.resize(9);
    switch(direction){
    case SHIFT_LEFT:
        for (int i = 0; i < 9; i ++){
            if (i == 5){
                m_kernel[i] = 1.f;
            }
            else
                m_kernel[i] = 0.f;
        }
        break;
    case SHIFT_RIGHT:
        for (int i = 0; i < 9; i ++){
            if (i == 3){
                m_kernel[i] = 1.f;
            }
            else
                m_kernel[i] = 0.f;
        }
        break;
    }
}

FilterShift::~FilterShift()
{
}

void FilterShift::apply(Canvas2D *canvas) {
    // TODO: Task 18 Call convolve function from superclass
    RGBA *data = canvas->data();
    FilterUtils::Convolve2D(data, canvas->width(), canvas->height(), m_kernel);
}
