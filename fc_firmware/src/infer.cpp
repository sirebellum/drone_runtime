// Description: Use TVM to do inference
#include "infer.h"
#include <fstream>

// Infer constructor
Infer::Infer(std::string model_path) {
    init(model_path);
}

// Infer destructor
Infer::~Infer() {
}

// Initialize the inference
void Infer::init(const std::string& model_path) {
    // Load the module
    mod_factory = tvm::runtime::Module::LoadFromFile(model_path);

    // Get the graph runtime module
    mod_ = mod_factory.GetFunction("default")(dev_);

    // Get the functions
    set_input_ = mod_.GetFunction("set_input");
    get_output_ = mod_.GetFunction("get_output");
    run_ = mod_.GetFunction("run");
}

// Run the inference
void Infer::run(const cv::Mat& img, float* out_data) {

    // Convert input from cv::Mat to tvm::runtime::NDArray
    cv::Mat img_float;
    img.convertTo(img_float, CV_32F);
    cv::Mat img_norm = img_float / 255.0;
    cv::Mat img_norm_t = img_norm.t();
    cv::Mat img_norm_t_flat = img_norm_t.reshape(1, img_norm_t.total());
    tvm::runtime::NDArray img_tvm = tvm::runtime::NDArray::Empty(
        {1, 3, 224, 224}, {kDLFloat, 32, 1}, {kDLCPU, 0});
    memcpy(img_tvm.ToDLPack()->dl_tensor.data, img_norm_t_flat.data, get_array_size(img_tvm));

    set_input_("input", img_tvm);
    run_();
    tvm::runtime::NDArray out = get_output_(0);
    memcpy(out_data, out.ToDLPack()->dl_tensor.data, get_array_size(out));
}

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
