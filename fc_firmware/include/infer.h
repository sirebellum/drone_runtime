// Description: Use TVM to do inference
#include <tvm/runtime/module.h>
#include <tvm/runtime/registry.h>
#include <dlpack/dlpack.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class Infer {
public:
    Infer(std::string model_path,
          std::string model_json,
          std::string model_params);
    ~Infer();
    void init(const std::string& model_path,
              const std::string& model_json,
              const std::string& model_params);
    void run(const cv::Mat& img, float* out_data);
    size_t get_array_size(tvm::runtime::NDArray array);
    // Metadata
    int input_width = 256;
    int input_height = 256;
private:
    // TVM related
    tvm::runtime::Module mod_factory;
    tvm::runtime::Module mod_;
    int dev_ = kDLCPU;
    int dev_id_ = 1;
    tvm::runtime::PackedFunc set_input_;
    tvm::runtime::PackedFunc get_output_;
    tvm::runtime::PackedFunc load_params_;
    tvm::runtime::PackedFunc run_;
};