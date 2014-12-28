import struct

# D3D11_FILTER
class Filter:
  MinMagMipPoint = 0
  MinMagPointMipLinear = 0x1
  MinPointMagLinearMipPoint = 0x4
  MinPointMagMipLinear = 0x5
  MinLinearMagMipPoint = 0x10
  MinLinearMagPointMipLinear = 0x11
  MinMagLinearMipPoint = 0x14
  MinMagMipLinear = 0x15
  Anisotropic = 0x55
  ComparisonMinMagMipPoint = 0x80
  ComparisonMinMagPointMipLinear = 0x81
  ComparisonMinPointMagLinearMipPoint = 0x84
  ComparisonMinPointMagMipLinear = 0x85
  ComparisonMinLinearMagMipPoint = 0x90
  ComparisonMinLinearMagPointMipLinear = 0x91
  ComparisonMinMagLinearMipPoint = 0x94
  ComparisonMinMagMipLinear = 0x95
  ComparisonAnisotropic = 0xd5
  Text1bit = 0x80000000

# D3D11_TEXTURE_ADDRESS_MODE:
class AddressMode:
  Wrap = 1
  Mirror = 2
  Clamp = 3
  Border = 4
  MirrorOnce = 5

# D3D11_COMPARISON_FUNC:
class ComparisonFunc:
  Never = 1
  Less  = 2
  Equal = 3
  LessEqual = 4
  Greater = 5
  NotEqual = 6
  GreaterEqual = 7
  Always = 8

class D3D11_SAMPLER_DESC(object):
  def __init__(self):
    self.filter = Filter.MinMagMipPoint
    self.addressu = AddressMode.Wrap
    self.addressv = AddressMode.Wrap
    self.addressw = AddressMode.Wrap
    self.miplodbias = 0
    self.maxanisotropy = 1
    self.comparisonfunc = ComparisonFunc.Less
    self.bordercolor = 0x00000000
    self.minlod = 0
    self.maxlod = 65535

  def pack(self):
    data = bytearray()
    data = struct.pack("4I 1f 2I 6f", \
                       self.filter, \
                       self.addressu, \
                       self.addressv, \
                       self.addressw, \
                       self.miplodbias, \
                       self.maxanisotropy, \
                       self.comparisonfunc, \
                       (self.bordercolor & 0x000000ff) / 256.0, \
                       (self.bordercolor & 0x0000ff00) / 256.0, \
                       (self.bordercolor & 0x00ff0000) / 256.0, \
                       (self.bordercolor & 0xff000000) / 256.0, \
                       self.minlod, \
                       self.maxlod)
    return data

  def __setattr__(self, name, value):
    name = name.lower()
    if name == "name":
      # we add a name attribute to identify the sampler, but this field doesn't exist in the d3d struct
      super(D3D11_SAMPLER_DESC, self).__setattr__("Name", value)
      return
    if name == "filter":
      if value in Filter.__dict__.values():
         super(D3D11_SAMPLER_DESC, self).__setattr__(name, value)
         return
    if name == "addressu" or \
       name == "addressv" or \
       name == "addressw":
      if value in AddressMode.__dict__.values():
        super(D3D11_SAMPLER_DESC, self).__setattr__(name, value)
        return
    if name == "miplodbias":
      if type(value) == float or type(value) == int:
        super(D3D11_SAMPLER_DESC, self).__setattr__(name, value)
        return
    if name == "maxanisotropy":
      if type(value) == int:
        super(D3D11_SAMPLER_DESC, self).__setattr__(name, value)
        return
    if name == "comparisonfunc":
      if value in ComparisonFunc.__dict__.values():
        super(D3D11_SAMPLER_DESC, self).__setattr__(name, value)
        return
    if name == "bordercolor":
      if type(value) == int or type(value) == long:
        super(D3D11_SAMPLER_DESC, self).__setattr__(name, value)
        return
    if name == "minlod" or \
       name == "maxlod":
       if type(value) == int or type(value) == float:
         super(D3D11_SAMPLER_DESC, self).__setattr__(name, value)
         return
    raise NameError("Unknown sampler state [%s = %s]" % (name, value))

class Technique(object):
  VertexShader = None
  GeometryShader = None
  PixelShader = None
  ComputeShader = None
  Defines = {}
