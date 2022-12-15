import onnx
import numpy as np

import tvm
from tvm import te
import tvm.relay as relay
from tvm.relay.transform import ToMixedPrecision
from tvm.contrib import clang

# FLight controller
print("Compiling flight controller...")
model_path = "fc.onnx"
onnx_model = onnx.load(model_path)

target = tvm.target.arm_cpu(options=["-mattr=+neon,+vfp4",
                                     "-mcpu=cortex-a7",
                                     "-mtriple=armv7a-linux-gnueabihf"])

input_name = "input"
shape_dict = {input_name: (1, 6, 7)}
mod, params = relay.frontend.from_onnx(onnx_model, shape_dict)

with tvm.transform.PassContext(opt_level=3, config={"tir.disable_assert": False}):
    mod = relay.build(mod, target=target)
    with open("fc_firmware/models/fc.json", "w") as json:
        json.write(mod.get_graph_json())
    mod.export_library("fc_firmware/models/fc.so", options=["-fuse-ld=lld",
                                                     "--target=armv7a-linux-gnueabihf"])
