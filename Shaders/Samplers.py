from d3dcompiler import *

font_sampler_state = D3D11_SAMPLER_DESC()

heightmap_sampler_state = D3D11_SAMPLER_DESC()

tex_sampler_state = D3D11_SAMPLER_DESC()
tex_sampler_state.Filter = Filter.MinMagMipLinear

diffuse_sampler_state = D3D11_SAMPLER_DESC()
diffuse_sampler_state.AddressU = AddressMode.Wrap
diffuse_sampler_state.AddressV = AddressMode.Wrap
diffuse_sampler_state.Filter = Filter.MinMagMipLinear

