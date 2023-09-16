// Description: Use TVM to do inference
#include "infer.h"
#include <fstream>

// Infer constructor
Infer::Infer(std::string model_path,
             std::string model_json,
             std::string model_params) {
    init(model_path, model_json, model_params);
}

// Infer destructor
Infer::~Infer() {
}

// Initialize the inference
void Infer::init(const std::string& model_path,
                 const std::string& model_json,
                 const std::string& model_params) {
    // Load files
    tvm::runtime::Module mod_syslib = tvm::runtime::Module::LoadFromFile(model_path);

    std::ifstream json_in(model_json, std::ios::in);
    std::string json_data((std::istreambuf_iterator<char>(json_in)), std::istreambuf_iterator<char>());
    json_in.close();

    std::ifstream params_in(model_params, std::ios::binary);
    std::string params_data((std::istreambuf_iterator<char>(params_in)), std::istreambuf_iterator<char>());
    params_in.close();

    // Convert params to TVMByteArray
    TVMByteArray params_arr;
    params_arr.data = params_data.c_str();
    params_arr.size = params_data.length();

    mod_ = (*tvm::runtime::Registry::Get("tvm.graph_executor.create"))(json_data, mod_syslib, dev_, dev_id_);

    set_input_ = mod_.GetFunction("set_input");
    get_output_ = mod_.GetFunction("get_output");
    load_params_ = mod_.GetFunction("load_params");
    run_ = mod_.GetFunction("run");

    load_params_(params_arr);
}

// Run the inference
void Infer::run(const cv::Mat& img, float* out_data) {

    // Resize input image
    cv::Mat img_resized;
    cv::resize(img, img_resized, cv::Size(input_height, input_width));

    // Convert input to single channel
    cv::Mat img_gray;
    cv::cvtColor(img_resized, img_gray, cv::COLOR_BGR2GRAY);

    // Convert input from cv::Mat to tvm::runtime::NDArray
    cv::Mat img_float;
    img_gray.convertTo(img_float, CV_32FC1);
    cv::Mat img_norm = img_float / 255.0;
    cv::Mat img_flat = img_norm.reshape(1, img_norm.total());
    tvm::runtime::NDArray img_tvm = tvm::runtime::NDArray::Empty(
        {1, 1, input_height, input_width}, {kDLFloat, 32, 1}, {kDLCPU, 0});
    memcpy(img_tvm.ToDLPack()->dl_tensor.data, img_flat.data, get_array_size(img_tvm));

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
