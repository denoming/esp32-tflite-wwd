#include "HammingWindow.hpp"

#include <cmath>

namespace {

/**
 * Create the constants for a hamming window
 */
std::unique_ptr<float[]> getCoefficients(int windowSize)
{
    std::unique_ptr<float[]> coefficients{new float[windowSize]};
    const float arg = M_PI * 2.0 / windowSize;
    for (int i = 0; i < windowSize; ++i) {
        // Scale it to fixed point and round it.
        coefficients[i] = 0.5f - (0.5f * std::cos(arg * (float(i) + 0.5f)));
    }
    return coefficients;
}

}

HammingWindow::HammingWindow(int windowSize)
    : _coefficients{getCoefficients(windowSize)}
    , _windowSize{windowSize}
{
}

void HammingWindow::applyWindow(float* input)
{
    for (int i = 0; i < _windowSize; ++i) {
        input[i] = input[i] * _coefficients[i];
    }
}