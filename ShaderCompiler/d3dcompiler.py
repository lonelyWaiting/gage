import struct
from ctypes import *

class GUID(Structure):
  _fields_ = [("i0", c_uint),("i1", c_uint),("i2", c_uint),("i3", c_uint)]

IID_ID3D11ShaderReflection = GUID(0x0a233719, 0x45783960, 0x3b207c9d, 0xc19c1d8b)

class D3D11_SHADER_DESC(Structure):
  _fields_ = [
    ("Version", c_uint),
    ("Creator", c_char_p),
    ("Flags", c_uint),
    ("ConstantBuffers", c_uint),
    ("BoundResources", c_uint),
    ("InputParameters", c_uint),
    ("OutputParameters", c_uint),
    ("InstructionCount", c_uint),
    ("TempRegisterCount", c_uint),
    ("TempArrayCount", c_uint),
    ("DefCount", c_uint),
    ("DclCount", c_uint),
    ("TextureNormalInstructions", c_uint),
    ("TextureLoadInstructions", c_uint),
    ("TextureCompInstructions", c_uint),
    ("TextureBiasInstructions", c_uint),
    ("TextureGradientInstructions", c_uint),
    ("FloatInstructionCount", c_uint),
    ("IntInstructionCount", c_uint),
    ("UintInstructionCount", c_uint),
    ("StaticFlowControlCount", c_uint),
    ("DynamicFlowControlCount", c_uint),
    ("MacroInstructionCount", c_uint),
    ("ArrayInstructionCount", c_uint),
    ("CutInstructionCount", c_uint),
    ("EmitInstructionCount", c_uint),
    ("GSOutputTopology", c_uint),
    ("GSMaxOutputVertexCount", c_uint),
    ("InputPrimitive", c_uint),
    ("PatchConstantParameters", c_uint),
    ("cGSInstanceCount", c_uint),
    ("cControlPoints", c_uint),
    ("HSOutputPrimitive", c_uint),
    ("HSPartitioning", c_uint),
    ("TessellatorDomain", c_uint),
    ("cBarrierInstructions", c_uint),
    ("cInterlockedInstructions", c_uint),
    ("cTextureStoreInstructions", c_uint) ]

class D3D10_SHADER_INPUT_TYPE:
  D3D10_SIT_CBUFFER = 0
  D3D10_SIT_TBUFFER = 1
  D3D10_SIT_TEXTURE = 2
  D3D10_SIT_SAMPLER = 3
  D3D11_SIT_UAV_RWTYPED = 4
  D3D11_SIT_STRUCTURED = 5
  D3D11_SIT_UAV_RWSTRUCTURED = 6
  D3D11_SIT_BYTEADDRESS = 7
  D3D11_SIT_UAV_RWBYTEADDRESS = 8
  D3D11_SIT_UAV_APPEND_STRUCTURED = 9
  D3D11_SIT_UAV_CONSUME_STRUCTURED = 10
  D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER = 11
  @classmethod
  def String(cls, index):
    for k,v in cls.__dict__.items():
      if v == index: 
        return k

class D3D11_SHADER_BUFFER_DESC(Structure):
  _fields_ = [
    ("Name", c_char_p),
    ("Type", c_uint),
    ("Variables", c_uint),
    ("Size", c_uint),
    ("uFlags", c_uint) ]

class D3D11_SHADER_INPUT_BIND_DESC(Structure):
  _fields_ = [
    ("Name", c_char_p),
    ("Type", c_uint),
    ("BindPoint", c_uint),
    ("BindCount", c_uint),
    ("uFlags", c_uint),
    ("ReturnType", c_uint),
    ("Dimension", c_uint),
    ("NumSamples", c_uint) ]

class D3D11_SIGNATURE_PARAMETER_DESC(Structure):
  _fields_ = [
    ("SemanticName", c_char_p),
    ("SemanticIndex", c_uint),
    ("Register", c_uint),
    ("SystemValueType", c_uint),
    ("ComponentType", c_uint),
    ("Mask", c_ubyte),
    ("ReadWriteMask", c_ubyte),
    ("Stream", c_uint) ]

class ID3D11ShaderReflectionConstantBuffer(Structure):
  _fields_ = [ ("vtable", POINTER(WINFUNCTYPE(None))) ]
  IGetDesc = 0
  IGetVariableByIndex = 1
  IGetVariableByName = 2
  def GetDesc(self, a):
    GetDescFuncPtrType = WINFUNCTYPE(HRESULT, POINTER(ID3D11ShaderReflectionConstantBuffer), POINTER(D3D11_SHADER_BUFFER_DESC))
    fnptr = cast(self.vtable[ID3D11ShaderReflectionConstantBuffer.IGetDesc], GetDescFuncPtrType)
    return fnptr(self, a)

class ID3D11ShaderReflection(Structure):
  _fields_ = [ ("vtable", POINTER(WINFUNCTYPE(None))) ]
  IAddRef = 0
  IQueryInterface = 1
  IRelease = 2
  IGetDesc = 3
  IGetConstantBufferByIndex = 4
  IGetConstantBufferByName = 5
  IGetResourceBindingDesc = 6
  IGetInputParameterDesc = 7
  IGetOutputParameterDesc = 8
  IGetPatchConstantParameterDesc = 9
  IGetVariableByName = 10
  IGetResourceBindingDescByName = 11
  def GetDesc(self, a):
    GetDescFuncPtrType = WINFUNCTYPE(HRESULT, POINTER(ID3D11ShaderReflection), POINTER(D3D11_SHADER_DESC))
    fnptr = cast(self.vtable[ID3D11ShaderReflection.IGetDesc], GetDescFuncPtrType)
    return fnptr(self, a)
  def GetConstantBufferByIndex(self, a):
    GetConstantBufferByIndexFuncPtrType = WINFUNCTYPE(POINTER(ID3D11ShaderReflectionConstantBuffer), POINTER(ID3D11ShaderReflection), c_uint)
    fnptr = cast(self.vtable[ID3D11ShaderReflection.IGetConstantBufferByIndex], GetConstantBufferByIndexFuncPtrType)
    return fnptr(self, a)
  def GetResourceBindingDesc(self, a, b):
    GetResourceBindingDescFuncPtrType = WINFUNCTYPE(HRESULT, POINTER(ID3D11ShaderReflection), c_uint, POINTER(D3D11_SHADER_INPUT_BIND_DESC))
    fnptr = cast(self.vtable[ID3D11ShaderReflection.IGetResourceBindingDesc], GetResourceBindingDescFuncPtrType)
    return fnptr(self, a, b)
  def GetInputParameterDesc(self, a, b):
    GetInputParameterDescFuncPtrType = WINFUNCTYPE(HRESULT, POINTER(ID3D11ShaderReflection), c_uint, POINTER(D3D11_SIGNATURE_PARAMETER_DESC))
    fnptr = cast(self.vtable[ID3D11ShaderReflection.IGetInputParameterDesc], GetInputParameterDescFuncPtrType)
    return fnptr(self, a, b)
  def GetOutputParameterDesc(self, a, b):
    GetOutputParameterDescFuncPtrType = WINFUNCTYPE(HRESULT, POINTER(ID3D11ShaderReflection), c_uint, POINTER(D3D11_SIGNATURE_PARAMETER_DESC))
    fnptr = cast(self.vtable[ID3D11ShaderReflection.IGetOutputParameterDesc], GetOutputParameterDescFuncPtrType)
    return fnptr(self, a, b)

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
