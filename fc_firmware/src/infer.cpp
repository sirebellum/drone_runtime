// Description: Use TVM to do inference
#include "infer.h"
#include <fstream>

// Infer constructor
Infer::Infer() {
    // Default path and params
    std::string model_path = "model.so";
    std::string params_path = "model.params";
    init(model_path, params_path);
}

// Infer destructor
Infer::~Infer() {
}

// Initialize the inference
void Infer::init(const std::string& model_path, const std::string& params_path) {
    // Load the module
    mod_ = tvm::runtime::Module::LoadFromFile(model_path);

    // Get the functions
    run_ = mod_.GetFunction("run");
    get_input_ = mod_.GetFunction("get_input");
    get_output_ = mod_.GetFunction("get_output");
    set_input_ = mod_.GetFunction("set_input");
    set_output_ = mod_.GetFunction("set_output");

    // Get the input shape
    input_shape_ = get_input_().operator std::vector<int64_t>();
    output_shape_ = get_output_().operator std::vector<int64_t>();

    // Allocate the input/output
    input_ = tvm::runtime::NDArray::Empty(input_shape_, {kDLFloat, 32, 1}, {kDLCPU, 0});
    output_ = tvm::runtime::NDArray::Empty(output_shape_, {kDLFloat, 32, 1}, {kDLCPU, 0});
}

// Run the inference
void Infer::run(const cv::Mat& img, float* out_data) {
    // Set the input
    set_input_(input_);

    // Run the inference

    // Get the output
    output_ = get_output_();

    // Copy the output
    memcpy(out_data, output_->data, get_array_size(output_));
}

// Get the input size
size_t Infer::get_array_size(tvm::runtime::NDArray array) {
    // Calculate total number of bytes in input
    size_t dtypes_bytes = array.DataType().bytes();
    // Calculate total number of elements in input
    size_t num_elements = 1;
    for (auto dim : array.Shape()) {
        num_elements *= dim;
    }
    // Return total number of bytes in input
    return dtypes_bytes * num_elements;
}
