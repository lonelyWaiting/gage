from ShaderDefinition import *

ShaderRender = Technique()
ShaderRender.VertexShader = "BasicVS"
ShaderRender.PixelShader = "BasicPS"

ShaderRenderAlphaTest = Technique()
ShaderRenderAlphaTest.VertexShader = "BasicVS"
ShaderRenderAlphaTest.PixelShader = "BasicAlphaTestPS"
ShaderRenderAlphaTest.Defines = {"DIFFUSE":"1", "SPECULAR":"1"}

ShaderRenderUAV = Technique()
ShaderRenderUAV.VertexShader = "BasicVS"
ShaderRenderUAV.PixelShader = "BasicPS_UAV"

ShaderParticles = Technique()
ShaderParticles.VertexShader = "ParticleVS"
ShaderParticles.GeometryShader = "ParticleGS"
ShaderParticles.PixelShader = "ParticlePS"
