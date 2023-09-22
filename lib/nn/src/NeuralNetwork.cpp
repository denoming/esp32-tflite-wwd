#include "NeuralNetwork.hpp"

#include "Model.hpp"

#include <esp_err.h>
#include <esp_log.h>

static const int kArenaSize = 25000;

static const char* TAG = "ESP32 - NN";

NeuralNetwork::NeuralNetwork()
    : _arena{nullptr}
    , _model{nullptr}
    , _resolver{nullptr}
    , _interpreter{nullptr}
    , _input{nullptr}
    , _output{nullptr}
{
}

bool
NeuralNetwork::setUp()
{
    if (!_arena) {
        _arena = new (std::nothrow) uint8_t[kArenaSize];
        if (!_arena) {
            ESP_LOGE(TAG, "Failed to allocate memory for arena");
            return false;
        }
    }

    if (!_model) {
        _model = tflite::GetModel(TF_MODEL);
        if (_model->version() != TFLITE_SCHEMA_VERSION) {
            ESP_LOGE(TAG,
                     "Invalid model <%lu> while <%d> expected",
                     _model->version(),
                     TFLITE_SCHEMA_VERSION);
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

    _interpreter
        = new (std::nothrow) tflite::MicroInterpreter(_model, *_resolver, _arena, kArenaSize);
    if (!_interpreter) {
        ESP_LOGE(TAG, "Failed to instantiate interpreter");
        return false;
    }

    const auto status = _interpreter->AllocateTensors();
    if (status != kTfLiteOk) {
        ESP_LOGE(TAG, "AllocateTensors() failed");
        tearDown();
        return false;
    }
    size_t usedBytes = _interpreter->arena_used_bytes();
    ESP_LOGD(TAG, "Used bytes %d\n", usedBytes);

    _input = _interpreter->input(0);
    _output = _interpreter->output(0);

    return true;
}

void
NeuralNetwork::tearDown()
{
    _input = _output = nullptr;

    delete _interpreter;
    delete _resolver;
}

float*
NeuralNetwork::getInputBuffer()
{
    return _input->data.f;
}

float
NeuralNetwork::predict()
{
    _interpreter->Invoke();
    return _output->data.f[0];
}