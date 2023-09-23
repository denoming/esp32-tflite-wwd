#pragma once

#include <memory>

class HammingWindow {
public:
    explicit HammingWindow(int windowSize);

    void
    applyWindow(float* input);

private:
    std::unique_ptr<float[]> _coefficients;
    int _windowSize;
};