# rVTK
C++ wrapper functionality for VTK.

## Prerequisites
- [VTK](http://www.vtk.org/)

  Requires at least VTK version 8.1.1
  For integration with [QTools](../../../QTools), it's necessary to ensure VTK references
  the installed Qt5 distribution. When configuring VTK with CMake, ensure that `WITH_QT` is
  checked and that the correct Qt5 import libraries are being referenced.
  Some applications place themselves on the PATH and this can sometimes cause older versions
  of the Qt shared libraries to be found. This is pretty much a Windows only problem.
  Also note that when upgrading Qt and later rebuilding VTK that VTK maintains its CMake
  variable cache correctly i.e. that it points to the correct Qt libraries and not some other
  Qt libraries found in the environment.
  
  In addition, ensure that separate debug and release builds are compiled. When integrating with
  Qt, QApplication will whinge if mixing build types (app will fail to start).
  
- [rFeatures](../../../rFeatures)
