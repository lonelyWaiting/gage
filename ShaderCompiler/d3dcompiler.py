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

class D3D_SHADER_INPUT_TYPE:
  D3D_SIT_CBUFFER = 0
  D3D_SIT_TBUFFER = 1
  D3D_SIT_TEXTURE = 2
  D3D_SIT_SAMPLER = 3
  D3D_SIT_UAV_RWTYPED = 4
  D3D_SIT_STRUCTURED = 5
  D3D_SIT_UAV_RWSTRUCTURED = 6
  D3D_SIT_BYTEADDRESS = 7
  D3D_SIT_UAV_RWBYTEADDRESS = 8
  D3D_SIT_UAV_APPEND_STRUCTURED = 9
  D3D_SIT_UAV_CONSUME_STRUCTURED = 10
  D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER = 11

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

class D3D_FEATURE_LEVEL(Structure):
    _fields_ = [ ("value", c_uint) ]
    D3D_FEATURE_LEVEL_9_1  = 0x9100,
    D3D_FEATURE_LEVEL_9_2  = 0x9200,
    D3D_FEATURE_LEVEL_9_3  = 0x9300,
    D3D_FEATURE_LEVEL_10_0 = 0xa000,
    D3D_FEATURE_LEVEL_10_1 = 0xa100,
    D3D_FEATURE_LEVEL_11_0 = 0xb000

class D3D11_SHADER_TYPE_DESC(Structure):
  _fields_ = [
    ("Class", c_uint),  # one of D3D_SHADER_VARIABLE_CLASS
    ("Type", c_uint),   # one of D3D_SHADER_VARIABLE_TYPE
    ("Rows", c_uint),
    ("Columns", c_uint),
    ("Elements", c_uint),
    ("Members", c_uint),
    ("Offsets", c_uint),
    ("Name", c_char_p) ]
  
ID3D11ShaderReflectionType._methods_ = [
    STDMETHOD(HRESULT, 'GetDesc', [POINTER(D3D11_SHADER_TYPE_DESC)]),
    STDMETHOD(POINTER(ID3D11ShaderReflectionType), 'GetMemberTypeByIndex', [c_uint]),
    STDMETHOD(POINTER(ID3D11ShaderReflectionType), 'GetMemberTypeByName', [c_char_p]),
    STDMETHOD(c_char_p, 'GetMemberTypeName', [c_uint]),
    STDMETHOD(HRESULT, 'IsEqual', [POINTER(ID3D11ShaderReflectionType)]),
    STDMETHOD(POINTER(ID3D11ShaderReflectionType), 'GetSubType'),
    STDMETHOD(POINTER(ID3D11ShaderReflectionType), 'GetBaseClass'),
    STDMETHOD(c_uint, 'GetNumInterfaces'),
    STDMETHOD(POINTER(ID3D11ShaderReflectionType), 'GetInterfaceByIndex', [c_uint]),
    STDMETHOD(HRESULT, 'IsOfType', [POINTER(ID3D11ShaderReflectionType)]),
    STDMETHOD(HRESULT, 'ImplementsInterface', [POINTER(ID3D11ShaderReflectionType)])
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
    STDMETHOD(c_uint, 'GetGSInputPrimitive'),
    STDMETHOD(BOOL, 'IsSampleFrequencyShader'),
    STDMETHOD(c_uint, 'GetNumInterfaceSlots'),
    STDMETHOD(HRESULT, 'GetMinFeatureLevel', [POINTER(D3D_FEATURE_LEVEL)]),
    STDMETHOD(c_uint, 'GetThreadGroupSize', [POINTER(c_uint), POINTER(c_uint), POINTER(c_uint)]),
    STDMETHOD(c_uint64, 'GetRequiresFlags')
]

def D3DReflect(shader_blob):
  """Convenience function to call D3DReflect() given a shader binary blob and return the reflection object"""
  pReflectionInterface = POINTER(ID3D11ShaderReflection)()
  windll.D3DCompiler_47.D3DReflect(shader_blob, len(shader_blob), byref(ID3D11ShaderReflection._iid_), byref(pReflectionInterface))
  return pReflectionInterface
