import onnx
import numpy as np
import tvm
from tvm import te
import tvm.relay as relay
from tvm.relay.transform import ToMixedPrecision

model_path = "model.onnx"
onnx_model = onnx.load(model_path)

target = "llvm"

input_name = "input"
shape_dict = {input_name: (1, 12+3+1)}
print("ONNX to relay")
mod, params = relay.frontend.from_onnx(onnx_model, shape_dict)
mod = ToMixedPrecision("float16")(mod)

with tvm.transform.PassContext(opt_level=3):
    print("Compiling")
    mod = relay.build(mod, target=target)
    mod.export_library("x86_model.so")
