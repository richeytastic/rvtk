#pragma once
#ifndef RVTK_VTK_TOOLS_H
#define RVTK_VTK_TOOLS_H

#include <boost/unordered_set.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <vtkTexture.h>
#include <vtkImageImport.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkPolyDataMapper.h>
#include <vtkLookupTable.h>
#include <vtkSmartPointer.h>
#include <vtkTriangleFilter.h>
#include <vtkColor.h>
#include <vtkIdList.h>
#include <vtkFeatureEdges.h>
#include "rVTK_Export.h"
#include <ObjModel.h>   // RFeatures

namespace RVTK
{

// Set a provided lookup table of colours from startCol to endCol (inclusive).
// Colour values should be specified in RGB order.
rVTK_EXPORT void setColoursLookupTable( vtkSmartPointer<vtkLookupTable>,
                                        int numColours, const vtkColor3ub& startCol, const vtkColor3ub& endCol);

// Return of poly data object from actor
rVTK_EXPORT vtkPolyData* getPolyData( const vtkSmartPointer<vtkActor>& actor);

// VTK version independent creation of mapper object from poly data
rVTK_EXPORT vtkSmartPointer<vtkPolyDataMapper> createMapper( const vtkSmartPointer<vtkPolyData>& data);

rVTK_EXPORT vtkSmartPointer<vtkImageImport> makeImageImporter( const cv::Mat img);

// Converts a CV_8UC3 or CV_8UC1 image into a texture object ready for
// addition to an actor (as long as the actor's mapper has poly data
// with texture coordinates set).
// An empty (NULL) pointer is returned if img is not suitable.
// NB: object texture coordinates typically use the bottom left corner
// as the origin of the image texture whereas OpenCV uses the top left.
// By default, this function flips the image vertically before processing.
// If image flipping is not needed (because the texture coords use the
// top left as origin), ensure XFLIP is set to false.
// For CV_8UC3 images, byte order colours should be BGR (normal OpenCV style).
rVTK_EXPORT vtkSmartPointer<vtkTexture> convertToTexture( const cv::Mat& img, bool XFLIP=true);

// Extract the vertex IDs of points in pdata that lie on edges used only by one polygon.
rVTK_EXPORT void extractBoundaryVertices( const vtkSmartPointer<vtkPolyData>& pdata, std::vector<int>& pts);

// Generate a set of normals from a vtkPolyData object having point and cell data.
rVTK_EXPORT vtkSmartPointer<vtkPolyData> generateNormals( vtkSmartPointer<vtkPolyData> pdata);

// Dump a colour or Z buffer image from the provided render window.
rVTK_EXPORT cv::Mat_<cv::Vec3b> extractImage( vtkRenderWindow*);
rVTK_EXPORT cv::Mat_<float> extractZBuffer( vtkRenderWindow*);

}   // end namespace

#endif
