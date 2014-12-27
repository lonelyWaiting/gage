import struct
from ctypes import *
from ctypes.wintypes import *
from comtypes import * 

class ID3D11ShaderReflectionType(IUnknown):  _iid_ = GUID("{6E6FFA6A-9BAE-4613-A51E-91652D508C21}")
class ID3D11ShaderReflectionVariable(IUnknown): _iid_ = GUID("{51F23923-F3E5-4BD1-91CB-606177D8DB4C}")
class ID3D11ShaderReflectionConstantBuffer(IUnknown): _iid_ = GUID("{EB62D63D-93DD-4318-8AE8-C6F83AD371B8}")
class ID3D11ShaderReflection(IUnknown): _iid_ = GUID("{8d536ca1-0cca-4956-a837-786963755584}")
    
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

class D3D11_SHADER_VARIABLE_DESC(Structure):
  _fields_ = [
    ("Name", c_char_p),
    ("StartOffset", c_uint),
    ("Size", c_uint),
    ("uFlags", c_uint),
    ("DefaultValue", POINTER(None)),
    ("StartTexture", c_uint),
    ("TextureSize", c_uint),
    ("StartSampler", c_uint),
    ("SamplerSize", c_uint) ]

class D3D_PRIMITIVE:
    D3D_PRIMITIVE_UNDEFINED                = 0,
    D3D_PRIMITIVE_POINT                    = 1,
    D3D_PRIMITIVE_LINE                     = 2,
    D3D_PRIMITIVE_TRIANGLE                 = 3,
    D3D_PRIMITIVE_LINE_ADJ                 = 6,
    D3D_PRIMITIVE_TRIANGLE_ADJ             = 7,
    D3D_PRIMITIVE_1_CONTROL_POINT_PATCH    = 8,
    D3D_PRIMITIVE_2_CONTROL_POINT_PATCH    = 9,
    D3D_PRIMITIVE_3_CONTROL_POINT_PATCH    = 10,
    D3D_PRIMITIVE_4_CONTROL_POINT_PATCH    = 11,
    D3D_PRIMITIVE_5_CONTROL_POINT_PATCH    = 12,
    D3D_PRIMITIVE_6_CONTROL_POINT_PATCH    = 13,
    D3D_PRIMITIVE_7_CONTROL_POINT_PATCH    = 14,
    D3D_PRIMITIVE_8_CONTROL_POINT_PATCH    = 15,
    D3D_PRIMITIVE_9_CONTROL_POINT_PATCH    = 16,
    D3D_PRIMITIVE_10_CONTROL_POINT_PATCH   = 17,
    D3D_PRIMITIVE_11_CONTROL_POINT_PATCH   = 18,
    D3D_PRIMITIVE_12_CONTROL_POINT_PATCH   = 19,
    D3D_PRIMITIVE_13_CONTROL_POINT_PATCH   = 20,
    D3D_PRIMITIVE_14_CONTROL_POINT_PATCH   = 21,
    D3D_PRIMITIVE_15_CONTROL_POINT_PATCH   = 22,
    D3D_PRIMITIVE_16_CONTROL_POINT_PATCH   = 23,
    D3D_PRIMITIVE_17_CONTROL_POINT_PATCH   = 24,
    D3D_PRIMITIVE_18_CONTROL_POINT_PATCH   = 25,
    D3D_PRIMITIVE_19_CONTROL_POINT_PATCH   = 26,
    D3D_PRIMITIVE_20_CONTROL_POINT_PATCH   = 28,
    D3D_PRIMITIVE_21_CONTROL_POINT_PATCH   = 29,
    D3D_PRIMITIVE_22_CONTROL_POINT_PATCH   = 30,
    D3D_PRIMITIVE_23_CONTROL_POINT_PATCH   = 31,
    D3D_PRIMITIVE_24_CONTROL_POINT_PATCH   = 32,
    D3D_PRIMITIVE_25_CONTROL_POINT_PATCH   = 33,
    D3D_PRIMITIVE_26_CONTROL_POINT_PATCH   = 34,
    D3D_PRIMITIVE_27_CONTROL_POINT_PATCH   = 35,
    D3D_PRIMITIVE_28_CONTROL_POINT_PATCH   = 36,
    D3D_PRIMITIVE_29_CONTROL_POINT_PATCH   = 37,
    D3D_PRIMITIVE_30_CONTROL_POINT_PATCH   = 38,
    D3D_PRIMITIVE_31_CONTROL_POINT_PATCH   = 39,
    D3D_PRIMITIVE_32_CONTROL_POINT_PATCH   = 40,
    D3D10_PRIMITIVE_UNDEFINED              = D3D_PRIMITIVE_UNDEFINED,
    D3D10_PRIMITIVE_POINT                  = D3D_PRIMITIVE_POINT,
    D3D10_PRIMITIVE_LINE                   = D3D_PRIMITIVE_LINE,
    D3D10_PRIMITIVE_TRIANGLE               = D3D_PRIMITIVE_TRIANGLE,
    D3D10_PRIMITIVE_LINE_ADJ               = D3D_PRIMITIVE_LINE_ADJ,
    D3D10_PRIMITIVE_TRIANGLE_ADJ           = D3D_PRIMITIVE_TRIANGLE_ADJ,
    D3D11_PRIMITIVE_UNDEFINED              = D3D_PRIMITIVE_UNDEFINED,
    D3D11_PRIMITIVE_POINT                  = D3D_PRIMITIVE_POINT,
    D3D11_PRIMITIVE_LINE                   = D3D_PRIMITIVE_LINE,
    D3D11_PRIMITIVE_TRIANGLE               = D3D_PRIMITIVE_TRIANGLE,
    D3D11_PRIMITIVE_LINE_ADJ               = D3D_PRIMITIVE_LINE_ADJ,
    D3D11_PRIMITIVE_TRIANGLE_ADJ           = D3D_PRIMITIVE_TRIANGLE_ADJ,
    D3D11_PRIMITIVE_1_CONTROL_POINT_PATCH  = D3D_PRIMITIVE_1_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_2_CONTROL_POINT_PATCH  = D3D_PRIMITIVE_2_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_3_CONTROL_POINT_PATCH  = D3D_PRIMITIVE_3_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_4_CONTROL_POINT_PATCH  = D3D_PRIMITIVE_4_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_5_CONTROL_POINT_PATCH  = D3D_PRIMITIVE_5_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_6_CONTROL_POINT_PATCH  = D3D_PRIMITIVE_6_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_7_CONTROL_POINT_PATCH  = D3D_PRIMITIVE_7_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_8_CONTROL_POINT_PATCH  = D3D_PRIMITIVE_8_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_9_CONTROL_POINT_PATCH  = D3D_PRIMITIVE_9_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_10_CONTROL_POINT_PATCH = D3D_PRIMITIVE_10_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_11_CONTROL_POINT_PATCH = D3D_PRIMITIVE_11_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_12_CONTROL_POINT_PATCH = D3D_PRIMITIVE_12_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_13_CONTROL_POINT_PATCH = D3D_PRIMITIVE_13_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_14_CONTROL_POINT_PATCH = D3D_PRIMITIVE_14_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_15_CONTROL_POINT_PATCH = D3D_PRIMITIVE_15_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_16_CONTROL_POINT_PATCH = D3D_PRIMITIVE_16_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_17_CONTROL_POINT_PATCH = D3D_PRIMITIVE_17_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_18_CONTROL_POINT_PATCH = D3D_PRIMITIVE_18_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_19_CONTROL_POINT_PATCH = D3D_PRIMITIVE_19_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_20_CONTROL_POINT_PATCH = D3D_PRIMITIVE_20_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_21_CONTROL_POINT_PATCH = D3D_PRIMITIVE_21_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_22_CONTROL_POINT_PATCH = D3D_PRIMITIVE_22_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_23_CONTROL_POINT_PATCH = D3D_PRIMITIVE_23_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_24_CONTROL_POINT_PATCH = D3D_PRIMITIVE_24_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_25_CONTROL_POINT_PATCH = D3D_PRIMITIVE_25_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_26_CONTROL_POINT_PATCH = D3D_PRIMITIVE_26_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_27_CONTROL_POINT_PATCH = D3D_PRIMITIVE_27_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_28_CONTROL_POINT_PATCH = D3D_PRIMITIVE_28_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_29_CONTROL_POINT_PATCH = D3D_PRIMITIVE_29_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_30_CONTROL_POINT_PATCH = D3D_PRIMITIVE_30_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_31_CONTROL_POINT_PATCH = D3D_PRIMITIVE_31_CONTROL_POINT_PATCH,
    D3D11_PRIMITIVE_32_CONTROL_POINT_PATCH = D3D_PRIMITIVE_32_CONTROL_POINT_PATCH

class D3D_FEATURE_LEVEL(Structure):
    _fields_ = [ ("value", c_uint) ]
    D3D_FEATURE_LEVEL_9_1  = 0x9100,
    D3D_FEATURE_LEVEL_9_2  = 0x9200,
    D3D_FEATURE_LEVEL_9_3  = 0x9300,
    D3D_FEATURE_LEVEL_10_0 = 0xa000,
    D3D_FEATURE_LEVEL_10_1 = 0xa100,
    D3D_FEATURE_LEVEL_11_0 = 0xb000
  
ID3D11ShaderReflectionType._methods_ = [
    # TODO
]

ID3D11ShaderReflectionVariable._methods_ = [
    STDMETHOD(HRESULT, 'GetDesc', [POINTER(D3D11_SHADER_VARIABLE_DESC)]),
    STDMETHOD(POINTER(ID3D11ShaderReflectionType), 'GetType'),
    STDMETHOD(POINTER(ID3D11ShaderReflectionConstantBuffer), 'GetBuffer'),
    STDMETHOD(c_uint, 'GetInterfaceSlot', [c_uint])
]

ID3D11ShaderReflectionConstantBuffer._methods_ = [
    STDMETHOD(HRESULT, 'GetDesc', [POINTER(D3D11_SHADER_BUFFER_DESC)]),
    STDMETHOD(POINTER(ID3D11ShaderReflectionVariable), 'GetVariableByIndex', [c_uint]),
    STDMETHOD(POINTER(ID3D11ShaderReflectionVariable), 'GetVariableByName', [c_uint])
]

ID3D11ShaderReflection._methods_ = [
    STDMETHOD(HRESULT, 'GetDesc', [POINTER(D3D11_SHADER_DESC)]),
    STDMETHOD(POINTER(ID3D11ShaderReflectionConstantBuffer), 'GetConstantBufferByIndex', [c_uint]),
    STDMETHOD(POINTER(ID3D11ShaderReflectionConstantBuffer), 'GetConstantBufferByName', [c_char_p]),
    STDMETHOD(HRESULT, 'GetResourceBindingDesc', [c_uint, POINTER(D3D11_SHADER_INPUT_BIND_DESC)]),
    STDMETHOD(HRESULT, 'GetInputParameterDesc', [c_uint, POINTER(D3D11_SIGNATURE_PARAMETER_DESC)]),
    STDMETHOD(HRESULT, 'GetOutputParameterDesc', [c_uint, POINTER(D3D11_SIGNATURE_PARAMETER_DESC)]),
    STDMETHOD(HRESULT, 'GetPatchConstantParameterDesc', [c_uint, POINTER(D3D11_SIGNATURE_PARAMETER_DESC)]),
    STDMETHOD(POINTER(ID3D11ShaderReflectionVariable), 'GetVariableByName', [c_char_p]),
    STDMETHOD(HRESULT, 'GetResourceBindingDescByName', [c_char_p, POINTER(D3D11_SHADER_INPUT_BIND_DESC)]),
    STDMETHOD(c_uint, 'GetMovInstructionCount'),
    STDMETHOD(c_uint, 'GetMovcInstructionCount'),
    STDMETHOD(c_uint, 'GetConversionInstructionCount'),
    STDMETHOD(c_uint, 'GetBitwiseInstructionCount'),
    STDMETHOD(D3D_PRIMITIVE, 'GetGSInputPrimitive'),
    STDMETHOD(BOOL, 'IsSampleFrequencyShader'),
    STDMETHOD(c_uint, 'GetNumInterfaceSlots'),
    STDMETHOD(HRESULT, 'GetMinFeatureLevel', [POINTER(D3D_FEATURE_LEVEL)]),
    STDMETHOD(c_uint, 'GetThreadGroupSize', [POINTER(c_uint), POINTER(c_uint), POINTER(c_uint)]),
    STDMETHOD(c_uint64, 'GetRequiresFlags')
]

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
