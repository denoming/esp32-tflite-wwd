#pragma once

#include "HammingWindow.hpp"

#include <kissfft/kiss_fftr.h>

class AudioBuffer;

class AudioProcessor final {
public:
    AudioProcessor(int audioLength, int windowSize, int stepSize, int poolingSize);

    ~AudioProcessor();

    void
    getSpectrogram(AudioBuffer& buffer, float* outputSpectrogram);

private:
    void
    getSpectrogramSegment(float* outputSpectrogramRow);

private:
    int _audioLength;
    int _windowSize;
    int _stepSize;
    int _poolingSize;

    int _fftSize;
    std::unique_ptr<float[]> _fftInput;
    std::unique_ptr<kiss_fft_cpx[]> _fftOutput;
    std::unique_ptr<float[]> _energy;
    int _energySize;
    int _pooledEnergySize;

    kiss_fftr_cfg _cfg;

    HammingWindow _hammingWindow;
};