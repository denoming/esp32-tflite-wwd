#include "AudioProcessor.hpp"

#include "AudioBuffer.hpp"

#define EPSILON 1e-6

namespace {

size_t getInputSize(std::size_t windowSize)
{
    size_t size = 1;
    while (size < windowSize) {
        size <<= 1;
    }
    return size;
}

}

AudioProcessor::AudioProcessor(int audioLength, int windowSize, int stepSize, int poolingSize)
    : _audioLength{audioLength}
    , _windowSize{windowSize}
    , _stepSize{stepSize}
    , _poolingSize{poolingSize}
    ,  _fftSize{getInputSize(windowSize)}
    , _hammingWindow{windowSize}
{
    _energySize = _fftSize / 2 + 1;
    _pooledEnergySize = ceilf(_energySize / static_cast<float>(poolingSize));
    _fftInput.reset(new float[_fftSize]);
    _fftOutput.reset(new kiss_fft_cpx[_energySize]);
    _energy.reset(new float[_energySize]);
    
    _cfg = kiss_fftr_alloc(_fftSize, false, 0, 0);
}

AudioProcessor::~AudioProcessor()
{
    free(_cfg);
}

void AudioProcessor::getSpectrogram(AudioBuffer& buffer, float* outputSpectrogram)
{
    int startIndex = buffer.pos();
    float mean = 0;
    for (int i = 0; i < _audioLength; i++) {
        mean += buffer.peek();
        buffer.next();
    }
    mean /= _audioLength;

    buffer.seek(startIndex);
    float max = 0;
    for (int i = 0; i < _audioLength; i++) {
        max = std::max(max, fabsf(static_cast<float>(buffer.peek()) - mean));
        buffer.next();
    }

    for (int windowStart = startIndex; windowStart < startIndex + 16000 - _windowSize; windowStart += _stepSize) {
        buffer.seek(windowStart);
        for (int i = 0; i < _windowSize; i++) {
            _fftInput[i] = (static_cast<float>(buffer.peek()) - mean) / max;
            buffer.next();
        }
        for (int i = _windowSize; i < _fftSize; i++) {
            _fftInput[i] = 0;
        }
        getSpectrogramSegment(outputSpectrogram);
        outputSpectrogram += _pooledEnergySize;
    }
}

void AudioProcessor::getSpectrogramSegment(float* outputSpectrogramRow)
{
    _hammingWindow.applyWindow(_fftInput.get());

    kiss_fftr(
        _cfg,
        _fftInput.get(),
        _fftOutput.get());

    for (int i = 0; i < _energySize; i++) {
        const float real = _fftOutput[i].r;
        const float imag = _fftOutput[i].i;
        const float magSquared = (real * real) + (imag * imag);
        _energy[i] = magSquared;
    }

    float* outputSrc = _energy.get();
    float* outputDst = outputSpectrogramRow;
    for (int i = 0; i < _energySize; i += _poolingSize) {
        float average = 0;
        for (int j = 0; j < _poolingSize; j++) {
            if (i + j < _energySize) {
                average += *outputSrc;
                outputSrc++;
            }
        }
        *outputDst = average / _poolingSize;
        outputDst++;
    }

    for (int i = 0; i < _pooledEnergySize; i++) {
        outputSpectrogramRow[i] = log10f(outputSpectrogramRow[i] + EPSILON);
    }
}