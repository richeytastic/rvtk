cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)
project( rVTK)

include( "../LibCMakeCommon.cmake")
set( BUILD_SHARED_LIBS TRUE)

set( SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set( INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
include_directories( "${INCLUDE_DIR}")
# Include the build directory because generated header files sit here.
include_directories( "${PROJECT_BINARY_DIR}")

# Boost
if(WIN32)
    set( BOOST_ROOT "D:/boost_1_56_0")
    set( BOOST_LIBRARYDIR "${BOOST_ROOT}/lib64-msvc-11.0")
endif(WIN32)
find_package( Boost 1.5 REQUIRED COMPONENTS filesystem system thread random regex)
include_directories( ${Boost_INCLUDE_DIRS})
link_directories( ${Boost_LIBRARY_DIRS})

# OpenCV
if(WIN32)
    set( OpenCV_DIR "${LOCALLIBS}/opencv")
endif(WIN32)
find_package( OpenCV 2.4 REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS})

# VTK
set( VTK_DIR "${LOCALLIBS}/VTK64/lib/cmake/vtk-6.1")
find_package( VTK REQUIRED)
include( "${VTK_USE_FILE}")
include_directories( ${VTK_INCLUDE_DIRS})

# Qt
if(WIN32)
    # Build 64-bit using the MSVC11 64-bit compiler
    set( CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "C:/Program Files (x86)/Windows Kits/8.0/Lib/win8/um/x64")
    set( Qt5_LIBRARY_DIR "D:/Qt/5.2.1/msvc2012_64_opengl/lib")
    find_package( Qt5 REQUIRED Widgets OpenGL Sql WebKitWidgets)
    link_libraries( Qt5::Widgets Qt5::OpenGL Qt5::Sql Qt5::WebKitWidgets)
endif()

set( RFeatures_DIR "${LOCALLIBS}/rFeatures")
find_package( RFeatures REQUIRED)
include_directories( "${RFeatures_INCLUDE_DIR}")

set( RLIB_DIR "${LOCALLIBS}/rlib")
find_package( RLIB REQUIRED)

set( LibLAS_DIR "${LOCALLIBS}/libLAS-1.7.0")
find_package( LibLAS REQUIRED)


set( INCLUDE_FILES
    "${INCLUDE_DIR}/ClosestPointFinder.h"
    "${INCLUDE_DIR}/DataReader.h"
    "${INCLUDE_DIR}/DijkstraPath.h"
    #"${INCLUDE_DIR}/FlightInteractor.h"
    #"${INCLUDE_DIR}/ImagingInteractor.h"
    "${INCLUDE_DIR}/InteractorC1.h"
    "${INCLUDE_DIR}/KeyPresser.h"
    "${INCLUDE_DIR}/LinePlotter.h"
    "${INCLUDE_DIR}/LinesMapper.h"
    "${INCLUDE_DIR}/ModelBounder.h"
    "${INCLUDE_DIR}/PointCloudMapper.h"
    "${INCLUDE_DIR}/PointPlacer.h"
    "${INCLUDE_DIR}/SnapshotKeyPresser.h"
    "${INCLUDE_DIR}/Viewer.h"
    "${INCLUDE_DIR}/ViewerProjector.h"
    "${INCLUDE_DIR}/VtkActorFactory.h"
    "${INCLUDE_DIR}/VtkDisplayActor.h"
    "${INCLUDE_DIR}/VtkModel.h"
    "${INCLUDE_DIR}/VtkTools.h"
    "${INCLUDE_DIR}/VTKTypes.h"
    )

set( SRC_FILES
    ${SRC_DIR}/ClosestPointFinder
    ${SRC_DIR}/DataReader
    ${SRC_DIR}/DijkstraPath
    #${SRC_DIR}/FlightInteractor
    #${SRC_DIR}/ImagingInteractor
    ${SRC_DIR}/InteractorC1
    ${SRC_DIR}/KeyPresser
    ${SRC_DIR}/LinePlotter
    ${SRC_DIR}/LinesMapper
    ${SRC_DIR}/ModelBounder
    ${SRC_DIR}/PointCloudMapper
    ${SRC_DIR}/PointPlacer
    ${SRC_DIR}/SnapshotKeyPresser
    ${SRC_DIR}/Viewer
    ${SRC_DIR}/ViewerProjector
    ${SRC_DIR}/VtkActorFactory
    ${SRC_DIR}/VtkDisplayActor
    ${SRC_DIR}/VtkModel
    ${SRC_DIR}/VtkTools
    )


# Require generate_export_header for .lib import libraries associated with Windows DLLs (if BUILD_SHARED_LIBS is true)
include(GenerateExportHeader)
add_library( ${PROJECT_NAME} ${SRC_FILES} ${INCLUDE_FILES})
generate_export_header( ${PROJECT_NAME}
    BASE_NAME ${PROJECT_NAME}
    EXPORT_MACRO_NAME ${PROJECT_NAME}_EXPORT
    EXPORT_FILE_NAME ${PROJECT_NAME}_Export.h
    STATIC_DEFINE ${PROJECT_NAME}_BUILT_AS_STATIC)

target_link_libraries( ${PROJECT_NAME} ${VTK_LIBRARIES})
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries( ${PROJECT_NAME} ${RFeatures_LIBRARY})
target_link_libraries( ${PROJECT_NAME} ${RLIB_LIBRARY})
target_link_libraries( ${PROJECT_NAME} ${LibLAS_LIBRARY})

set( CMAKE_INSTALL_PREFIX "${LOCALLIBS}/${PROJECT_NAME}")
install( TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "bin"
                                 LIBRARY DESTINATION "lib"
                                 ARCHIVE DESTINATION "lib/static")

install( FILES ${INCLUDE_FILES} DESTINATION "include")
install( FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}_Export.h" DESTINATION "include")
# Copy across this library's Find cmake module to ${LOCALLIBS}/cmake
install( FILES "${PROJECT_SOURCE_DIR}/../CMakeModules/FindRVTK.cmake" DESTINATION "${LOCALLIBS}/cmake")
