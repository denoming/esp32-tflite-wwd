#pragma once

#include <memory>

class HammingWindow {
public:
    HammingWindow(int windowSize);

    void
    applyWindow(float* input);

private:
    std::unique_ptr<float[]> _coefficients;
    int _windowSize;
};