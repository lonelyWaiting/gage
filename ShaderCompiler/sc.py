import os
import subprocess
import string
import struct
import sys
from blob import *
from d3dcompiler import *
from d3dstate import *

import ctypes

class ShaderType:
  Vertex   = "vs_5_0"
  Geometry = "gs_5_0"
  Pixel    = "ps_5_0"
  Compute  = "cs_5_0"

PROJECT_ROOT = ''

def FindProjectRoot():
  """Find the project root, which is the directory that contains .git. Look backwards from the current directory for .git"""
  global PROJECT_ROOT
  PROJECT_ROOT = os.getcwd()
  while not os.path.isdir(PROJECT_ROOT + "\\.git"):
    PROJECT_ROOT,tail = os.path.split(PROJECT_ROOT)
    if tail == '':
      print "Could not find project root"
      exit(1)

def FXC():
  """The absolute path to fxc.exe, which should be in the PROJECT_ROOT\ShaderCompiler directory"""
  return PROJECT_ROOT + "\\ShaderCompiler\\fxc.exe"

def MCPP():
  """The absolute path to mcpp.exe, which should be in the PROJECT_ROOT\ShaderCompiler directory"""
  return PROJECT_ROOT + "\\ShaderCompiler\\mcpp.exe"

def CallProcess(cmd):
  """Spawn the process with cmd. Blocks until it returns. If return code != 0, then print stderr and exit"""
  #print cmd
  p = subprocess.Popen(cmd, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
  output, errors = p.communicate()
  if p.returncode != 0:
    print errors
    print "Failure"
    exit(1)

def CompileShader(InputFilename, Type, EntryPoint, Defines={}):
  TempPreprocessedFilename = InputFilename + "_preprocessed_temp"

  define_string = ""
  for k,v in Defines.iteritems():
    define_string += " -D " + k + "=" + v

  mcpp_cmd = MCPP() + \
             " " + define_string + \
             " " + InputFilename + \
             " " + TempPreprocessedFilename
  CallProcess(mcpp_cmd)
    
  fxo,fxo_size = D3DCompileFromFile(TempPreprocessedFilename, EntryPoint, Type)

  os.remove(TempPreprocessedFilename)
  return fxo,fxo_size

def ImportModule(fullpath):
  path, filename = os.path.split(fullpath)
  path = os.path.abspath(path)
  filename, ext = os.path.splitext(filename)
  sys.path.append(path)
  print fullpath + " ... " + filename
  module = __import__(filename)
  del sys.path[-1]
  return module

if __name__ == '__main__':
  def FullPath(relpath):
    return os.getcwd() + "\\" + relpath

  # if __file__ exists, we are running non-interactively, so use cmd line args
  # if it doesn't exist, we're running interactively, so use test paths
  if '__file__' in globals():
    if len(sys.argv) != 3:
      print "Usage: sc.py InputFilename OutputFilename"
      print "       InputFilename is the filename of the .py shader definition file"
      print "       OutputFilename is the shader binary output file"
      exit(1)
    ShaderDefinitionFilename = FullPath(sys.argv[1])
    OutputFilename = FullPath(sys.argv[2])
  else:    
    ShaderDefinitionFilename = FullPath("..\\Shaders\\Mesh.py")
    OutputFilename = FullPath("..\\Shaders\\Mesh.fxo")

  FindProjectRoot()
  
  defs = ImportModule(ShaderDefinitionFilename)

  # Find the techniques and samplers (and soon, render state blocks) in the shader's Python script
  techniques = []
  samplers = []
  for name,object in defs.__dict__.items():
    if type(object) == defs.Technique:
      object.Name = name
      techniques.append(object)
      #techniques.insert(0,object)
    if type(object) == D3D11_SAMPLER_DESC:
      object.Name = name
      samplers.append(object)

  # Compile everything. Insert the compiled shader blobs into the dictionary
  # keyed as techniquename.shadername
  shaderblobs = {}        
  for t in techniques:
    inputshaderfilename = string.replace(ShaderDefinitionFilename, ".py", ".fx")

    if t.VertexShader:
      name = t.Name + "." + t.VertexShader
      print "Compiling " + name
      fxo,fxo_size = CompileShader(inputshaderfilename, ShaderType.Vertex, t.VertexShader, t.Defines)
      shaderblobs[name] = (fxo, fxo_size)

    if t.GeometryShader:
      name = t.Name + "." + t.GeometryShader
      print "Compiling " + name
      fxo,fxo_size = CompileShader(inputshaderfilename, ShaderType.Geometry, t.GeometryShader, t.Defines)
      shaderblobs[name] = (fxo, fxo_size)

    if t.PixelShader:
      name = t.Name + "." + t.PixelShader
      print "Compiling " + name
      fxo,fxo_size = CompileShader(inputshaderfilename, ShaderType.Pixel, t.PixelShader, t.Defines)
      shaderblobs[name] = (fxo, fxo_size)

    if t.ComputeShader:
      name = t.Name + "." + t.ComputeShader
      print "Compiling " + name
      fxo,fxo_size = CompileShader(inputshaderfilename, ShaderType.Compute, t.ComputeShader, t.Defines)
      shaderblobs[name] = (fxo, fxo_size)

  # For each of the shader blobs, insert the reflection information into the dictionary
  shaderblobs_reflection = {}        
  for k,v in shaderblobs.iteritems():
    shader_blob = v[0]
    shader_blob_size = v[1]
    shaderblobs_reflection[k] = D3DReflect(shader_blob, shader_blob_size)

  # Print out interesting information
  for k,v in shaderblobs.iteritems():
    print k + ":"
    shaderdesc = D3D11_SHADER_DESC()
    pReflectionInterface = shaderblobs_reflection[k]
    result = pReflectionInterface.GetDesc(byref(shaderdesc))

    def EnumToString(cls, index):
      for k,v in cls.__dict__.items():
        if v == index: 
          return k
        
    for i in range(shaderdesc.BoundResources):
      resourcedesc = D3D11_SHADER_INPUT_BIND_DESC()
      pReflectionInterface.GetResourceBindingDesc(i, byref(resourcedesc))
      print "  Res %d (%s): %s(%d)" % (i, EnumToString(D3D_SHADER_INPUT_TYPE, resourcedesc.Type), resourcedesc.Name, resourcedesc.BindPoint)
      
    for i in range(shaderdesc.InputParameters):
      inputparam = D3D11_SIGNATURE_PARAMETER_DESC()
      pReflectionInterface.GetInputParameterDesc(i, byref(inputparam))
      print "  INPUT %s[%d]: %d" % (inputparam.SemanticName, inputparam.SemanticIndex, inputparam.Register)

    for i in range(shaderdesc.OutputParameters):
      outputparam = D3D11_SIGNATURE_PARAMETER_DESC()
      pReflectionInterface.GetOutputParameterDesc(i, byref(outputparam))
      print "  OUTPUT %s[%d]: %d" % (outputparam.SemanticName, outputparam.SemanticIndex, outputparam.Register)

  # Write the binary data
  blob = Blob()
  blob.Pack("I", len(techniques))
  blob.Pack("I", len(samplers))
  blob.Reference("techniques")
  blob.Reference("samplers")

  blob.Align(16)
  blob.Label("techniques")
  for technique in techniques:
    blob.Label("technique." + technique.Name)
    blob.Pack("64s", technique.Name)
    if technique.VertexShader != None: 
      blob.Reference("technique." + technique.Name + ".vertexshader")
    else:
      blob.Pack("Q", 0)

    if technique.GeometryShader != None: 
      blob.Reference("technique." + technique.Name + ".geometryshader")
    else:
      blob.Pack("Q", 0)

    if technique.PixelShader != None: 
      blob.Reference("technique." + technique.Name + ".pixelshader")
    else:
      blob.Pack("Q", 0 )

    if technique.ComputeShader != None: 
      blob.Reference("technique." + technique.Name + ".computeshader")
    else:
      blob.Pack("Q", 0)

  for technique in techniques:
    def WriteShader(labelpath, shaderblob, pReflectionInterface):
      shaderdesc = D3D11_SHADER_DESC()
      result = pReflectionInterface.GetDesc(byref(shaderdesc))

      constantbuffers = []
      samplerz = []
      shaderresources = []
      uavs = []
      for i in range(shaderdesc.BoundResources):
        resourcedesc = D3D11_SHADER_INPUT_BIND_DESC()
        pReflectionInterface.GetResourceBindingDesc(i, byref(resourcedesc))
        if   resourcedesc.Type == D3D_SHADER_INPUT_TYPE.D3D_SIT_CBUFFER:                       constantbuffers.append(resourcedesc)
        elif resourcedesc.Type == D3D_SHADER_INPUT_TYPE.D3D_SIT_TBUFFER:                       shaderresources.append(resourcedesc)
        elif resourcedesc.Type == D3D_SHADER_INPUT_TYPE.D3D_SIT_TEXTURE:                       shaderresources.append(resourcedesc)
        elif resourcedesc.Type == D3D_SHADER_INPUT_TYPE.D3D_SIT_SAMPLER:                       samplerz.append(resourcedesc)
        elif resourcedesc.Type == D3D_SHADER_INPUT_TYPE.D3D_SIT_UAV_RWTYPED:                   uavs.append(resourcedesc)
        elif resourcedesc.Type == D3D_SHADER_INPUT_TYPE.D3D_SIT_STRUCTURED:                    shaderresources.append(resourcedesc)
        elif resourcedesc.Type == D3D_SHADER_INPUT_TYPE.D3D_SIT_UAV_RWSTRUCTURED:              uavs.append(resourcedesc)
        elif resourcedesc.Type == D3D_SHADER_INPUT_TYPE.D3D_SIT_BYTEADDRESS:                   shaderresources.append(resourcedesc)
        elif resourcedesc.Type == D3D_SHADER_INPUT_TYPE.D3D_SIT_UAV_RWBYTEADDRESS:             uavs.append(resourcedesc)
        elif resourcedesc.Type == D3D_SHADER_INPUT_TYPE.D3D_SIT_UAV_APPEND_STRUCTURED:         uavs.append(resourcedesc)
        elif resourcedesc.Type == D3D_SHADER_INPUT_TYPE.D3D_SIT_UAV_CONSUME_STRUCTURED:        uavs.append(resourcedesc)
        elif resourcedesc.Type == D3D_SHADER_INPUT_TYPE.D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER: uavs.append(resourcedesc)

      blob.Pack("I", len(constantbuffers))
      blob.Pack("I", len(samplerz))
      blob.Pack("I", len(shaderresources))
      blob.Pack("I", len(uavs))
      blob.Pack("I", sizeof(shaderblob[0]))
      blob.Pack("I", 0) # pad
      blob.Reference(labelpath + ".cb")
      blob.Reference(labelpath + ".samplers")
      blob.Reference(labelpath + ".shaderresources")
      blob.Reference(labelpath + ".uavs")
      blob.Reference(labelpath + ".shaderblob")
      blob.Pack("I", 0) # Ptr to the D3D compiled shader (set at load time)

      blob.Align(16)
      blob.Label(labelpath + ".cb")
      for cb in constantbuffers:
        blob.Pack("I", cb.BindPoint)
        blob.Pack("64s", cb.Name)

      blob.Align(16)
      blob.Label(labelpath + ".samplers")
      for s in samplerz:
        blob.Pack("I", s.BindPoint)
        blob.Pack("I", 0)
        blob.Reference("sampler." + s.Name)

      blob.Align(16)
      blob.Label(labelpath + ".shaderresources")
      for res in shaderresources:
        blob.Pack("I", res.BindPoint)
        blob.Pack("64s", res.Name)

      blob.Align(16)
      blob.Label(labelpath + ".uavs")
      for res in uavs:
        blob.Pack("I", res.BindPoint)
        blob.Pack("64s", res.Name)

      blob.Align(16)
      blob.Label(labelpath + ".shaderblob")
      blob.Pack("%ds" % sizeof(shaderblob[0]), shaderblob[0].raw) # sizeof(shaderblob[0]) is == shaderblob[1], the shaderblob size

    if technique.VertexShader != None:
      name = "technique." + technique.Name + ".vertexshader"
      blob.Align(16)
      blob.Label(name)
      blobname = technique.Name + "." + technique.VertexShader
      WriteShader(name, shaderblobs[blobname], shaderblobs_reflection[blobname])

    if technique.GeometryShader != None:
      name = "technique." + technique.Name + ".geometryshader"
      blob.Align(16)
      blob.Label(name)
      blobname = technique.Name + "." + technique.GeometryShader
      WriteShader(name, shaderblobs[blobname], shaderblobs_reflection[blobname])

    if technique.PixelShader != None:
      name = "technique." + technique.Name + ".pixelshader"
      blob.Align(16)
      blob.Label(name)
      blobname = technique.Name + "." + technique.PixelShader
      WriteShader(name, shaderblobs[blobname], shaderblobs_reflection[blobname])

    if technique.ComputeShader != None:
      name = "technique." + technique.Name + ".computeshader"
      blob.Align(16)
      blob.Label(name)
      blobname = technique.Name + "." + technique.ComputeShader
      WriteShader(name, shaderblobs[blobname], shaderblobs_reflection[blobname])
      
  blob.Align(16)
  blob.Label("samplers")
  for sampler in samplers:
    blob.Label("sampler." + sampler.Name)
    blob.Pack("64s", sampler.Name)
    blob.Copy(sampler.pack())
    blob.Pack("I", 0)
    blob.Pack("Q", 0)  # Ptr to the D3D sampler state object (set at load time)

  blob.Resolve()

  f = open(OutputFilename, "wb")
  f.write(blob.Data)
  f.close()
    
  exit(0)
