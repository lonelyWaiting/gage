import os
import subprocess
import sys
from Samplers import *
from d3dcompiler import *

ShaderText = Technique()
ShaderText.VertexShader = "TextVS"
ShaderText.PixelShader = "TextPS"
