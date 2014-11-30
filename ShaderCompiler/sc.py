import binascii
import os
import subprocess
import string
import struct
import sys
from d3dcompiler import *


class ShaderType:
  Vertex   = "/Tvs_5_0"
  Geometry = "/Tgs_5_0"
  Pixel    = "/Tps_5_0"
  Compute  = "/Tcs_5_0"

class Technique(object):
  VertexShader = None
  GeometryShader = None
  PixelShader = None
  ComputeShader = None
  Defines = {}

# Return the absolute path to fxc.exe (it should be side by side this file)
def FXC():
  path, filename = os.path.split( os.path.realpath(__file__) )
  path = os.path.abspath(path)
  return path + "\\fxc.exe"

def CompileShader(InputFilename, Type, EntryPoint, Defines={}):
  TempOutputFilename = "temp.fxo"

  define_string = ""
  for k,v in Defines.iteritems():
    define_string += " /D" + k + "=" + v

  cmdline = FXC() + \
            " " + InputFilename + \
            " " + Type + \
            " /E" + EntryPoint + \
            define_string + \
            " /Fo " + TempOutputFilename
  
  #print cmdline

  fxc = subprocess.Popen(cmdline, stderr=subprocess.PIPE, stdout=subprocess.PIPE)
  
  output, errors = fxc.communicate()
  
  if fxc.returncode != 0:
    print errors
    print "Failure"
    exit(1)
  
  f = open(TempOutputFilename, "rb")
  fxo = f.read()
  f.close()
  
  os.remove(TempOutputFilename)
  
  return fxo

def ImportModule(fullpath):
  path, filename = os.path.split(fullpath)
  path = os.path.abspath(path)
  filename, ext = os.path.splitext(filename)
  sys.path.append(path)
  module = __import__(filename)
  del sys.path[-1]
  return module

def FullPath(relpath):
  return os.getcwd() + "\\" + relpath

if __name__ == '__main__':
  if len(sys.argv) != 3:
    print "Usage: sc.py InputFilename OutputFilename"
    print "       InputFilename is the filename of the .py shader definition file"
    print "       OutputFilename is the shader binary output file"
    exit()
  
  ShaderDefinitionFilename = FullPath(sys.argv[1])
  OutputFilename = FullPath(sys.argv[2])
  
  defs = ImportModule(ShaderDefinitionFilename)

  # Find the techniques and samplers (and soon, render state blocks) in the shader's Python script
  techniques = []
  samplers = []
  for name,object in defs.__dict__.items():
    if type(object) == defs.sc.Technique:
      object.Name = name
      techniques.append(object)
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
      shaderblobs[name] = CompileShader(inputshaderfilename, ShaderType.Vertex, t.VertexShader, t.Defines)

    if t.GeometryShader:
      name = t.Name + "." + t.GeometryShader
      print "Compiling " + name
      shaderblobs[name] = CompileShader(inputshaderfilename, ShaderType.Geometry, t.GeometryShader, t.Defines)

    if t.PixelShader:
      name = t.Name + "." + t.PixelShader
      print "Compiling " + name
      shaderblobs[name] = CompileShader(inputshaderfilename, ShaderType.Pixel, t.PixelShader, t.Defines)

    if t.ComputeShader:
      name = t.Name + "." + t.ComputeShader
      print "Compiling " + name
      shaderblobs[name] = CompileShader(inputshaderfilename, ShaderType.Compute, t.ComputeShader, t.Defines)

  # For each of the shader blobs, insert the reflection information into the dictionary
  shaderblobs_reflection = {}        
  for k,v in shaderblobs.iteritems():
    pReflectionInterface = POINTER(ID3D11ShaderReflection)()
    result = windll.D3DCompiler_43.D3DReflect(v, len(v), byref(IID_ID3D11ShaderReflection), byref(pReflectionInterface))
    shaderblobs_reflection[k] = pReflectionInterface

  # Print out interesting information
  for k,v in shaderblobs.iteritems():
    print k + ":"
    shaderdesc = D3D11_SHADER_DESC()
    result = pReflectionInterface.contents.GetDesc(byref(shaderdesc))

    for i in range(shaderdesc.BoundResources):
      resourcedesc = D3D11_SHADER_INPUT_BIND_DESC()
      pReflectionInterface.contents.GetResourceBindingDesc(i, byref(resourcedesc))
      print "  Res %d (%s): %s(%d)" % (i, D3D10_SHADER_INPUT_TYPE.String(resourcedesc.Type), resourcedesc.Name, resourcedesc.BindPoint)

    for i in range(shaderdesc.InputParameters):
      inputparam = D3D11_SIGNATURE_PARAMETER_DESC()
      pReflectionInterface.contents.GetInputParameterDesc(i, byref(inputparam))
      print "  INPUT %s[%d]: %d" % (inputparam.SemanticName, inputparam.SemanticIndex, inputparam.Register)

    for i in range(shaderdesc.OutputParameters):
      outputparam = D3D11_SIGNATURE_PARAMETER_DESC()
      pReflectionInterface.contents.GetOutputParameterDesc(i, byref(outputparam))
      print "  OUTPUT %s[%d]: %d" % (outputparam.SemanticName, outputparam.SemanticIndex, outputparam.Register)

  # Helper class to manage binary data with relocatable pointers
  # The first sixteen bytes are a header, then the payload, then the fixup offsets
  class Blob:
    def __init__(self):
      self.Labels = {}
      self.References = {}
      self.Data = bytearray()

    def Pack(self, *args):
      self.Data += struct.pack(*args)

    def Copy(self, _bytearray):
      self.Data += _bytearray

    def Align(self, alignment):
      pad = alignment - (len(self.Data) % alignment)
      for i in range(pad):
        self.Data += bytearray(b'\0')

    def Label(self, name):
      if name in self.Labels:
        raise NameError("Duplicate label %s at %d and %d" % (name, self.Labels[name], len(self.Data)))
      self.Labels[name] = len(self.Data)

    def Reference(self, dst_name):
      if len(self.Data) % 8 != 0:
        raise NameError("The pointer at %d to %s is not eight byte aligned" % (len(self.Data), dst_name))
      self.References[dst_name] = len(self.Data)
      self.Data += bytearray(b'\0\0\0\0\0\0\0\0')    #64 bit pointer

    def Resolve(self):
      fixups = bytearray()
      for name,ptroffset in self.References.iteritems():
        if not(name in self.Labels):
          raise NameError("Reference %s at %d cannot be resolved" % (name, ptroffset))
        labeloffset = self.Labels[name]
        self.Data[ptroffset:ptroffset+8] = bytearray(struct.pack("Q", labeloffset))[0:8]
        fixups += struct.pack("I", ptroffset)
      header = struct.pack("IIII", len(self.References), len(self.Data), 0, 0)
      self.Data = header + self.Data + fixups
      
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
      #pReflectionInterface = POINTER(ID3D11ShaderReflection)()
      #result = windll.D3DCompiler_43.D3DReflect(shaderblob, len(shaderblob), byref(IID_ID3D11ShaderReflection), byref(pReflectionInterface))
      #print "%x" % result
      #print len(shaderblob)
      shaderdesc = D3D11_SHADER_DESC()
      result = pReflectionInterface.contents.GetDesc(byref(shaderdesc))

      constantbuffers = []
      samplerz = []
      shaderresources = []
      uavs = []
      for i in range(shaderdesc.BoundResources):
        resourcedesc = D3D11_SHADER_INPUT_BIND_DESC()
        pReflectionInterface.contents.GetResourceBindingDesc(i, byref(resourcedesc))
        if   resourcedesc.Type == D3D10_SHADER_INPUT_TYPE.D3D10_SIT_CBUFFER:                       constantbuffers.append(resourcedesc)
        elif resourcedesc.Type == D3D10_SHADER_INPUT_TYPE.D3D10_SIT_TBUFFER:                       shaderresources.append(resourcedesc)
        elif resourcedesc.Type == D3D10_SHADER_INPUT_TYPE.D3D10_SIT_TEXTURE:                       shaderresources.append(resourcedesc)
        elif resourcedesc.Type == D3D10_SHADER_INPUT_TYPE.D3D10_SIT_SAMPLER:                       samplerz.append(resourcedesc)
        elif resourcedesc.Type == D3D10_SHADER_INPUT_TYPE.D3D11_SIT_UAV_RWTYPED:                   uavs.append(resourcedesc)
        elif resourcedesc.Type == D3D10_SHADER_INPUT_TYPE.D3D11_SIT_STRUCTURED:                    shaderresources.append(resourcedesc)
        elif resourcedesc.Type == D3D10_SHADER_INPUT_TYPE.D3D11_SIT_UAV_RWSTRUCTURED:              uavs.append(resourcedesc)
        elif resourcedesc.Type == D3D10_SHADER_INPUT_TYPE.D3D11_SIT_BYTEADDRESS:                   shaderresources.append(resourcedesc)
        elif resourcedesc.Type == D3D10_SHADER_INPUT_TYPE.D3D11_SIT_UAV_RWBYTEADDRESS:             uavs.append(resourcedesc)
        elif resourcedesc.Type == D3D10_SHADER_INPUT_TYPE.D3D11_SIT_UAV_APPEND_STRUCTURED:         uavs.append(resourcedesc)
        elif resourcedesc.Type == D3D10_SHADER_INPUT_TYPE.D3D11_SIT_UAV_CONSUME_STRUCTURED:        uavs.append(resourcedesc)
        elif resourcedesc.Type == D3D10_SHADER_INPUT_TYPE.D3D11_SIT_UAV_RWSTRUCTURED_WITH_COUNTER: uavs.append(resourcedesc)

      blob.Pack("I", len(constantbuffers))
      blob.Pack("I", len(samplerz))
      blob.Pack("I", len(shaderresources))
      blob.Pack("I", len(uavs))
      blob.Pack("I", len(shaderblob))
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
      blob.Pack("%ds" % len(shaderblob), shaderblob)

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
