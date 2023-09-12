# Description: Use TVM to create an ARM CPU model
import tvm
import torch
from tvm import relay
from models import IMAGE_SIZE

# Import the torch model
model = torch.jit.load('model.pt').eval()

# Convert the model to TVM
input_shape = [1, 3, IMAGE_SIZE, IMAGE_SIZE]
input_names = ['input0']
shape_list = [(input_names[0], input_shape)]
mod, params = tvm.relay.frontend.from_pytorch(model, shape_list)

# Compile the model
with tvm.transform.PassContext(opt_level=3):
    lib = relay.build(mod, target="llvm", params=params)

# Save the model
lib.export_library('fc_firmware/lib/model_x86.so')
# with open('model_x86.json', 'w') as f_graph_json:
#     f_graph_json.write(mod.to_json())
# with open('model_x86.params', 'wb') as f_params:
#     f_params.write(relay.save_param_dict(params))
