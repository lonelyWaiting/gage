import os
import subprocess
import string
import struct

class Blob:
  """Helper class to manage binary data with relocatable pointers
  The first sixteen bytes are a header, then the payload, then the fixup offsets"""
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
  
