# Description: Use TVM to create an ARM CPU model
import tvm
import torch
from tvm import relay
# from models import IMAGE_SIZE
import argparse

# Parse arguments
parser = argparse.ArgumentParser(description='TVM model generator')
parser.add_argument('--model', type=str, default='model', help='Model to use')
parser.add_argument('--arch', type=str, default='x86', help='Output file name')
parser.add_argument('--input_size', type=str, default='[1,3,256,256]', help='Input size')

args = parser.parse_args()

# Set the target
if args.arch == 'x86':
    target = tvm.target.Target('llvm', host='llvm')
    output = args.model + '_x86.so'
elif args.arch == 'arm':
    target = 'llvm -mtriple=aarch64-linux-gnu'
    target = tvm.target.Target(target, host=target)
    output = args.model + '_arm.so'
elif args.arch == 'mac':
    target = 'llvm -mtriple=aarch64-apple-darwin'
    target = tvm.target.Target(target, host=target)
    output = args.model + '_mac.so'

# Load the model
model = torch.load(args.model+'.pt')
model.eval()

# Create a dummy input
input_shape = eval(args.input_size)

# Create a Relay model
input_name = 'input'
shape_list = [(input_name, input_shape)]
mod, params = relay.frontend.from_pytorch(model, shape_list)

# Compile the model
with relay.build_config(opt_level=3):
    graph, lib, params = relay.build(mod, target=target, params=params)

# Save the model
lib.export_library("fc_firmware/lib/" + output)

# Save the parameters
with open("fc_firmware/lib/"+args.model+".params", "wb") as fo:
    fo.write(relay.save_param_dict(params))

# Save the graph json
with open("fc_firmware/lib/"+args.model+".json", "w") as fo:
    fo.write(graph)
