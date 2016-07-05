cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)
project( rVTK)

include( "../LibCMakeCommon.cmake")

set( BUILD_SHARED_LIBS TRUE)
set( BUILD_USING_SHARED_LIBS TRUE)

set( SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set( INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
include_directories( "${INCLUDE_DIR}")
# Include the build directory because generated header files sit here.
include_directories( "${PROJECT_BINARY_DIR}")

# Boost
find_package( Boost 1.58 REQUIRED COMPONENTS system filesystem thread random regex)
include_directories( ${Boost_INCLUDE_DIRS})
add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})


# OpenCV
find_package( OpenCV 2.4 REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS})

# VTK
find_package( VTK REQUIRED)
include( "${VTK_USE_FILE}")
include_directories( ${VTK_INCLUDE_DIRS})

find_package( RFeatures REQUIRED)
include_directories( "${RFeatures_INCLUDE_DIR}")

find_package( RLIB REQUIRED)

find_package( LibLAS REQUIRED)


set( INCLUDE_FILES
    "${INCLUDE_DIR}/ClosestPointFinder.h"
    "${INCLUDE_DIR}/DataReader.h"
    "${INCLUDE_DIR}/DijkstraShortestPathLineInterpolator.h"
    #"${INCLUDE_DIR}/FlightInteractor.h"
    #"${INCLUDE_DIR}/ImagingInteractor.h"
    "${INCLUDE_DIR}/InteractorC1.h"
    "${INCLUDE_DIR}/KeyPresser.h"
    "${INCLUDE_DIR}/LinePlotter.h"
    "${INCLUDE_DIR}/LinesMapper.h"
    "${INCLUDE_DIR}/ModelPathDrawer.h"
    "${INCLUDE_DIR}/PointCloudMapper.h"
    "${INCLUDE_DIR}/PointPlacer.h"
    "${INCLUDE_DIR}/SnapshotKeyPresser.h"
    #"${INCLUDE_DIR}/SurfacePathInterpolator.h"
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
    ${SRC_DIR}/DijkstraShortestPathLineInterpolator
    #${SRC_DIR}/FlightInteractor
    #${SRC_DIR}/ImagingInteractor
    ${SRC_DIR}/InteractorC1
    ${SRC_DIR}/KeyPresser
    ${SRC_DIR}/LinePlotter
    ${SRC_DIR}/LinesMapper
    ${SRC_DIR}/ModelPathDrawer
    ${SRC_DIR}/PointCloudMapper
    ${SRC_DIR}/PointPlacer
    ${SRC_DIR}/SnapshotKeyPresser
    #${SRC_DIR}/SurfacePathInterpolator
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
