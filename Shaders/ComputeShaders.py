import os
import subprocess
import sys
import sc
from Samplers import *
from d3dcompiler import *

ParticleUpdate = sc.Technique()
ParticleUpdate.ComputeShader = "ParticleUpdateCS"
