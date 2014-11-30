import os
import subprocess
import sys
import sc
from Samplers import *
from d3dcompiler import *

ShaderRender = sc.Technique()
ShaderRender.VertexShader = "BasicVS"
ShaderRender.PixelShader = "BasicPS"

ShaderRenderAlphaTest = sc.Technique()
ShaderRenderAlphaTest.VertexShader = "BasicVS"
ShaderRenderAlphaTest.PixelShader = "BasicAlphaTestPS"
ShaderRenderAlphaTest.Defines = {"DIFFUSE":"1", "SPECULAR":"1"}

ShaderRenderUAV = sc.Technique()
ShaderRenderUAV.VertexShader = "BasicVS"
ShaderRenderUAV.PixelShader = "BasicPS_UAV"

ShaderParticles = sc.Technique()
ShaderParticles.VertexShader = "ParticleVS"
ShaderParticles.GeometryShader = "ParticleGS"
ShaderParticles.PixelShader = "ParticlePS"
