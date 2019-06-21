cmake_minimum_required(VERSION 3.12.2 FATAL_ERROR)

project( rVTK)

set( WITH_RFEATURES TRUE)
set( WITH_VTK TRUE)
include( "cmake/FindLibs.cmake")

set( INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set( SRC_DIR "${PROJECT_SOURCE_DIR}/src")
include_directories( ${INCLUDE_DIR})

set( INCLUDE_FILES
    "${INCLUDE_DIR}/Axes.h"
    "${INCLUDE_DIR}/DataReader.h"
    #"${INCLUDE_DIR}/DijkstraShortestPathLineInterpolator.h"
    "${INCLUDE_DIR}/ImageGrabber.h"
    "${INCLUDE_DIR}/InteractorC1.h"
    "${INCLUDE_DIR}/KeyPresser.h"
    "${INCLUDE_DIR}/LookupTable.h"
    "${INCLUDE_DIR}/OffscreenModelViewer.h"
    "${INCLUDE_DIR}/PointPlacer.h"
    "${INCLUDE_DIR}/RendererPicker.h"
    "${INCLUDE_DIR}/ScalarLegend.h"
    "${INCLUDE_DIR}/SnapshotKeyPresser.h"
    "${INCLUDE_DIR}/SurfaceMapper.h"
    "${INCLUDE_DIR}/Viewer.h"
    "${INCLUDE_DIR}/ViewerProjector.h"
    "${INCLUDE_DIR}/VtkActorCreator.h"
    "${INCLUDE_DIR}/VtkTools.h"
    "${INCLUDE_DIR}/VTKTypes.h"
    )

set( SRC_FILES
    ${SRC_DIR}/Axes
    ${SRC_DIR}/DataReader
    #${SRC_DIR}/DijkstraShortestPathLineInterpolator
    ${SRC_DIR}/ImageGrabber
    ${SRC_DIR}/InteractorC1
    ${SRC_DIR}/KeyPresser
    ${SRC_DIR}/LookupTable
    ${SRC_DIR}/OffscreenModelViewer
    ${SRC_DIR}/PointPlacer
    ${SRC_DIR}/RendererPicker
    ${SRC_DIR}/ScalarLegend
    ${SRC_DIR}/SnapshotKeyPresser
    ${SRC_DIR}/SurfaceMapper
    ${SRC_DIR}/Viewer
    ${SRC_DIR}/ViewerProjector
    ${SRC_DIR}/VtkActorCreator
    ${SRC_DIR}/VtkTools
    )

add_library( ${PROJECT_NAME} ${SRC_FILES} ${INCLUDE_FILES})
include( "cmake/LinkLibs.cmake")
