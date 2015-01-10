import gc
import struct
from ctypes import *
from ctypes.wintypes import *
from comtypes import * 

class ID3D11ShaderReflectionType(IUnknown): _iid_ = GUID("{6E6FFA6A-9BAE-4613-A51E-91652D508C21}")
class ID3D11ShaderReflectionVariable(IUnknown): _iid_ = GUID("{51F23923-F3E5-4BD1-91CB-606177D8DB4C}")
class ID3D11ShaderReflectionConstantBuffer(IUnknown): _iid_ = GUID("{EB62D63D-93DD-4318-8AE8-C6F83AD371B8}")
class ID3D11ShaderReflection(IUnknown): _iid_ = GUID("{8d536ca1-0cca-4956-a837-786963755584}")
class ID3DBlob(IUnknown): _iid_ = GUID("{8BA5FB08-5195-40e2-AC58-0D989C3A0102}")
    
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

ID3DBlob._methods_ = [
  STDMETHOD(c_void_p, 'GetBufferPointer'),
  STDMETHOD(c_uint, 'GetBufferSize')
]

def D3DCompileFromFile(filename, entrypoint, target):
  """Convenience function to call D3DCompileFromFile()"""
  D3DCompileFromFile = windll.D3DCompiler_47.D3DCompileFromFile
  D3DCompileFromFile.argtypes = [LPCWSTR, c_void_p, c_void_p, LPCSTR, LPCSTR, c_uint, c_uint, POINTER(POINTER(ID3DBlob)), POINTER(POINTER(ID3DBlob))]
  D3DCompileFromFile.restype = c_uint

  pCodeBlob = POINTER(ID3DBlob)()
  pErrorBlob = POINTER(ID3DBlob)()

  ppCodeBlob = byref(pCodeBlob)
  ppErrorBlob = byref(pErrorBlob)
  
  hresult = D3DCompileFromFile(filename, 0, 0, entrypoint, target, 0, 0, ppCodeBlob, ppErrorBlob)

  if hresult == 0:
    # I don't understand this. When I say:
    #   fxo = pCodeBlob.GetBufferPointer()
    # then the buffer that fxo points to becomes corrupted when I return to the calling function.
    # I assume that c_void_p implicitly manages the buffer that it points to, but its size is never set? But why would that corrupt data?
    # In any event, calling c_types.create_string_buffer() and copying the data into that buffer does work.
    fxo_size = pCodeBlob.GetBufferSize()
    fxo = create_string_buffer(fxo_size)
    memmove(fxo, pCodeBlob.GetBufferPointer(), fxo_size)
    return fxo, fxo_size
  else:
    print "D3DCompileFromFile() failed:"
    print cast(pErrorBlob.GetBufferPointer(), c_char_p).value
    exit(1)

def D3DReflect(shader_blob, shader_blob_size):
  """Convenience function to call D3DReflect() given a shader binary blob and return the reflection object"""
  D3DReflect = windll.D3DCompiler_47.D3DReflect
  D3DReflect.argtypes = [c_void_p, c_uint, POINTER(GUID), POINTER(POINTER(ID3D11ShaderReflection))]
  D3DReflect.restype = c_uint

  pReflectionInterface = POINTER(ID3D11ShaderReflection)()
  hresult = D3DReflect(shader_blob, shader_blob_size, byref(ID3D11ShaderReflection._iid_), byref(pReflectionInterface))
  if hresult == 0:
    return pReflectionInterface
  else:
    print "D3DReflect() failed"
    print "Error code = %d" % hresult
    exit(1)

