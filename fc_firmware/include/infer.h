// Description: Use TVM to do inference
#include <tvm/runtime/module.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class Infer {
public:
    Infer();
    ~Infer();
    void init(const std::string& model_path, const std::string& params_path);
    void run(const cv::Mat& img, float* out_data);
    size_t get_array_size(tvm::runtime::NDArray array);
private:
    // TVM related
    tvm::runtime::Module mod_;
    tvm::runtime::PackedFunc run_;
    tvm::runtime::PackedFunc get_input_;
    tvm::runtime::PackedFunc get_output_;
    tvm::runtime::PackedFunc set_input_;
    tvm::runtime::PackedFunc set_output_;

    // TVM input/output related
    tvm::runtime::NDArray input_;
    tvm::runtime::NDArray output_;
    std::vector<int64_t> input_shape_;
    std::vector<int64_t> output_shape_;
};