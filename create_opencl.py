import onnx
import numpy as np

import tvm
from tvm import te
import tvm.relay as relay
from tvm.relay.transform import ToMixedPrecision
from tvm.contrib import clang
from tvm.relay.backend import Executor, Runtime

# FLight controller
print("Compiling flight controller...")
model_path = "fc.onnx"
onnx_model = onnx.load(model_path)

target = tvm.target.mali(model="mali400")
target_host = tvm.target.arm_cpu(options=["-mattr=+neon,+vfp4",
                                     "-mcpu=cortex-a7",
                                     "-mtriple=armv7a-linux-gnueabihf"])
input_name = "input"
shape_dict = {input_name: (1, 6*6)}
mod, params = relay.frontend.from_onnx(onnx_model, shape_dict)
mod = ToMixedPrecision("float16")(mod)

with tvm.transform.PassContext(opt_level=4, config={"tir.disable_assert": False}):
    mod = relay.build(mod, target=target, target_host=target_host, mod_name="flightcontrol",
        executor=Executor("graph"), runtime=Runtime("cpp"))
    with open("fc_firmware/fc.json", "w") as json:
        json.write(mod.get_graph_json())
    mod.export_library("fc_firmware/fc.so", options=["-fuse-ld=lld",
                                                     "--target=armv7a-linux-gnueabihf",
                                                     "-fno-short-wchar"])
# People detection
# print("Compiling people detection...")
# model_path = "ppl.onnx"
# onnx_model = onnx.load(model_path)

# input_name = "images"
# shape_dict = {input_name: (1, 1, 256, 256)}
# mod, params = relay.frontend.from_onnx(onnx_model, shape_dict)
# mod = ToMixedPrecision("float16")(mod)

# with tvm.transform.PassContext(opt_level=3):
#     print("Compiling so")
#     mod = relay.build(mod, target=target)
#     with open("fc_firmware/ppl.json", "w") as json:
#         json.write(mod.get_graph_json())
#     mod.export_library("fc_firmware/ppl.so", options=["-fuse-ld=lld",
#                                                      "--target=aarch64-linux-gnueabihf"])
# with tvm.transform.PassContext(opt_level=3):
#     print("Compiling cl")
#     mod = relay.build(mod, target=target)
#     mod.export_library("fc_firmware/ppl.cl",)