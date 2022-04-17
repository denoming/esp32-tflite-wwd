#include "NeuralNetwork.hpp"

#include <tensorflow/lite/micro/all_ops_resolver.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>

#include "Model.hpp"

static const int kArenaSize = 25000;

NeuralNetwork::NeuralNetwork()
    : _arena{nullptr}
    , _model{nullptr}
    , _resolver{nullptr}
    , _interpreter{nullptr}
    , _reporter{nullptr}
    , _input{nullptr}
    , _output{nullptr}
{
    _reporter = tflite::GetMicroErrorReporter();
}

bool NeuralNetwork::setUp()
{
    if (!_arena) {
        _arena = new (std::nothrow) uint8_t[kArenaSize];
        if (!_arena) {
            TF_LITE_REPORT_ERROR(_reporter, "Failed to allocate memory for arena");
            return false;
        }
    }

    if (!_model) {
        _model = tflite::GetModel(TF_MODEL);
        if (_model->version() != TFLITE_SCHEMA_VERSION) {
            TF_LITE_REPORT_ERROR(_reporter, "Model provided is schema version %d not equal to supported version %d.",
                                _model->version(), TFLITE_SCHEMA_VERSION);
            _model = nullptr;
            return false;
        }
    }

    _resolver = new tflite::MicroMutableOpResolver<10>();
    _resolver->AddConv2D();
    _resolver->AddMaxPool2D();
    _resolver->AddFullyConnected();
    _resolver->AddMul();
    _resolver->AddAdd();
    _resolver->AddLogistic();
    _resolver->AddReshape();
    _resolver->AddQuantize();
    _resolver->AddDequantize();

    _interpreter = new (std::nothrow) tflite::MicroInterpreter(_model, *_resolver, _arena, kArenaSize, _reporter);
    if (!_interpreter) {
        TF_LITE_REPORT_ERROR(_reporter, "Failed to instantiate interpreter");
        return false;
    }

    const auto status = _interpreter->AllocateTensors();
    if (status != kTfLiteOk) {
        TF_LITE_REPORT_ERROR(_reporter, "AllocateTensors() failed");
        tearDown();
        return false;
    }
    size_t usedBytes = _interpreter->arena_used_bytes();
    TF_LITE_REPORT_ERROR(_reporter, "Used bytes %d\n", usedBytes);

    _input = _interpreter->input(0);
    _output = _interpreter->output(0);

    return true;
}

void NeuralNetwork::tearDown()
{
    _input = _output = nullptr;

    delete _interpreter;
    delete _resolver;
    // free(m_tensor_arena);
    // delete m_error_reporter;
}

float* NeuralNetwork::getInputBuffer()
{
    return _input->data.f;
}

float NeuralNetwork::predict()
{
    _interpreter->Invoke();
    return _output->data.f[0];
}