import os
import subprocess
import sys
from Samplers import *
from d3dcompiler import *

ParticleUpdate = Technique()
ParticleUpdate.ComputeShader = "ParticleUpdateCS"
