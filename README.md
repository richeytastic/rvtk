# rVTK
C++ wrapper functionality for VTK.

## Prerequisites
- [VTK](http://www.vtk.org/)

  Tested with version 7.1.1
  For integration with [QTools](../../../QTools), it's necessary to ensure VTK references
  the installed Qt5 distribution. Therefore, when configuring VTK with CMake,
  ensure that WITH_QT is checked and that the correct Qt5 import libraries are being referenced
  (many apps. carry with them Qt shared libraries which can screw with automatic detection
  if those apps are on the system PATH).
  
  In addition, ensure that separate debug and release builds are compiled. When integrating with
  Qt, QApplication will whinge if mixing build types (app will fail to start).
  
- [rFeatures](../../../rFeatures)
