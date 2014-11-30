import os
import subprocess
import sys
import sc
from Samplers import *
from d3dcompiler import *

ShaderText = sc.Technique()
ShaderText.VertexShader = "TextVS"
ShaderText.PixelShader = "TextPS"
