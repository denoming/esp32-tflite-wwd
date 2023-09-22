#pragma once

#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/schema/schema_generated.h>

struct TfLiteTensor;

class NeuralNetwork {
public:
    NeuralNetwork();

    bool
    setUp();

    void
    tearDown();

    float*
    getInputBuffer();

    float
    predict();

private:
    uint8_t* _arena;
    const tflite::Model* _model;
    tflite::MicroMutableOpResolver<10>* _resolver;
    tflite::MicroInterpreter* _interpreter;
    TfLiteTensor* _input;
    TfLiteTensor* _output;
};