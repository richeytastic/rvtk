cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project( rVTK)

set( WITH_RFEATURES TRUE)
set( WITH_VTK TRUE)
include( "cmake/FindLibs.cmake")

set( INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set( SRC_DIR "${PROJECT_SOURCE_DIR}/src")
include_directories( ${INCLUDE_DIR})

set( INCLUDE_FILES
    "${INCLUDE_DIR}/Axes.h"
    #"${INCLUDE_DIR}/ClosestPointFinder.h"
    "${INCLUDE_DIR}/CurvatureSurfaceMapper.h"
    "${INCLUDE_DIR}/DataReader.h"
    "${INCLUDE_DIR}/DijkstraShortestPathLineInterpolator.h"
    #"${INCLUDE_DIR}/FlightInteractor.h"
    #"${INCLUDE_DIR}/ImagingInteractor.h"
    "${INCLUDE_DIR}/ImageGrabber.h"
    "${INCLUDE_DIR}/InteractorC1.h"
    "${INCLUDE_DIR}/KeyPresser.h"
    "${INCLUDE_DIR}/LinePlotter.h"
    "${INCLUDE_DIR}/LinesMapper.h"
    "${INCLUDE_DIR}/ModelPathDrawer.h"
    "${INCLUDE_DIR}/PointCloudMapper.h"
    "${INCLUDE_DIR}/PointPlacer.h"
    "${INCLUDE_DIR}/PolySurfaceCurvScalarMapper.h"
    "${INCLUDE_DIR}/PolySurfaceDistanceScalarMapper.h"
    "${INCLUDE_DIR}/RendererPicker.h"
    "${INCLUDE_DIR}/ScalarLegend.h"
    "${INCLUDE_DIR}/SnapshotKeyPresser.h"
    "${INCLUDE_DIR}/SurfaceMapper.h"
    "${INCLUDE_DIR}/SurfaceMapperInterface.h"
    #"${INCLUDE_DIR}/SurfacePathInterpolator.h"
    "${INCLUDE_DIR}/Viewer.h"
    "${INCLUDE_DIR}/ViewerProjector.h"
    "${INCLUDE_DIR}/VtkActorCreator.h"
    #"${INCLUDE_DIR}/VtkDisplayActor.h"
    #"${INCLUDE_DIR}/VtkModel.h"
    "${INCLUDE_DIR}/VtkTools.h"
    "${INCLUDE_DIR}/VTKTypes.h"
    )

set( SRC_FILES
    ${SRC_DIR}/Axes
    #${SRC_DIR}/ClosestPointFinder
    ${SRC_DIR}/CurvatureSurfaceMapper
    ${SRC_DIR}/DataReader
    ${SRC_DIR}/DijkstraShortestPathLineInterpolator
    #${SRC_DIR}/FlightInteractor
    #${SRC_DIR}/ImagingInteractor
    ${SRC_DIR}/ImageGrabber
    ${SRC_DIR}/InteractorC1
    ${SRC_DIR}/KeyPresser
    ${SRC_DIR}/LinePlotter
    ${SRC_DIR}/LinesMapper
    ${SRC_DIR}/ModelPathDrawer
    ${SRC_DIR}/PointCloudMapper
    ${SRC_DIR}/PointPlacer
    ${SRC_DIR}/PolySurfaceCurvScalarMapper
    ${SRC_DIR}/PolySurfaceDistanceScalarMapper
    ${SRC_DIR}/RendererPicker
    ${SRC_DIR}/ScalarLegend
    ${SRC_DIR}/SnapshotKeyPresser
    ${SRC_DIR}/SurfaceMapper
    #${SRC_DIR}/SurfacePathInterpolator
    ${SRC_DIR}/Viewer
    ${SRC_DIR}/ViewerProjector
    ${SRC_DIR}/VtkActorCreator
    #${SRC_DIR}/VtkDisplayActor
    #${SRC_DIR}/VtkModel
    ${SRC_DIR}/VtkTools
    )

add_library( ${PROJECT_NAME} ${SRC_FILES} ${INCLUDE_FILES})
include( "cmake/LinkLibs.cmake")
