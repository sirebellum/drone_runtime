// Description: Use TVM to do inference
#include <tvm/runtime/module.h>
#include <dlpack/dlpack.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class Infer {
public:
    Infer(std::string model_path);
    ~Infer();
    void init(const std::string& model_path);
    void run(const cv::Mat& img, float* out_data);
    size_t get_array_size(tvm::runtime::NDArray array);
private:
    // TVM related
    tvm::runtime::Module mod_factory;
    tvm::runtime::Module mod_;
    DLDevice dev_ = {kDLCPU, 0}; // TODO: Make configurable
    tvm::runtime::PackedFunc set_input_;
    tvm::runtime::PackedFunc get_output_;
    tvm::runtime::PackedFunc run_;
};