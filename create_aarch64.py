import onnx
import numpy as np

import tvm
from tvm import te
import tvm.relay as relay
from tvm.relay.transform import ToMixedPrecision
from tvm.contrib import clang

model_path = "model.onnx"
onnx_model = onnx.load(model_path)

target = tvm.target.arm_cpu(options=["-mattr=+neon",
                                     "-mcpu=cortex-a53",
                                     "-mtriple=aarch64-linux-gnueabihf"])

input_name = "input"
shape_dict = {input_name: (1, 12)}
print("ONNX to relay")
mod, params = relay.frontend.from_onnx(onnx_model, shape_dict)
mod = ToMixedPrecision("float16")(mod)

with tvm.transform.PassContext(opt_level=3):
    print("Compiling")
    mod = relay.build(mod, target=target)
    with open("firmware/model.json", "w") as json:
        json.write(mod.get_graph_json())
    mod.export_library("firmware/model.so", options=["-fuse-ld=lld",
                                                     "--target=aarch64-linux-gnueabihf"])
