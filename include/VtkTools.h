/************************************************************************
 * Copyright (C) 2017 Richard Palmer
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ************************************************************************/

#ifndef RVTK_VTK_TOOLS_H
#define RVTK_VTK_TOOLS_H

#include <ObjModel.h>
#include <vector>
#include <iostream>
#include <vtkActor.h>
#include <vtkColor.h>
#include <vtkLight.h>
#include <vtkCamera.h>
#include <vtkTexture.h>
#include <vtkPolyData.h>
#include <vtkMatrix4x4.h>
#include <vtkImageImport.h>
#include <vtkLookupTable.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include "rVTK_Export.h"

namespace RVTK {

// Set a provided lookup table of colours from startCol to endCol (inclusive).
// Colour values should be specified in RGB order.
rVTK_EXPORT void setColoursLookupTable( vtkSmartPointer<vtkLookupTable>,
                                        int numColours, const vtkColor3ub& startCol, const vtkColor3ub& endCol);

// Make an object (no texture) from an actor's polydata.
rVTK_EXPORT RFeatures::ObjModel::Ptr makeObject( const vtkActor*);

// Return poly data from actor
rVTK_EXPORT vtkPolyData* getPolyData( const vtkActor*);

// Transform the point data on the given actor using the given matrix.
// If the given matrix is null, the actor's internal (GPU) matrix is used.
rVTK_EXPORT void transform( vtkActor*, const vtkMatrix4x4 *m=nullptr);

rVTK_EXPORT vtkSmartPointer<vtkImageImport> makeImageImporter( const cv::Mat img);

// Converts a CV_8UC3 or CV_8UC1 image into a texture object ready for
// addition to an actor (as long as the actor's mapper has poly data
// with texture coordinates set).
// An empty (null) pointer is returned if img is not suitable.
// NB: object texture coordinates typically use the bottom left corner
// as the origin of the image texture whereas OpenCV uses the top left.
// By default, this function flips the image vertically before processing.
// If image flipping is not needed (because the texture coords use the
// top left as origin), ensure XFLIP is set to false.
// For CV_8UC3 images, byte order colours should be BGR (normal OpenCV style).
rVTK_EXPORT vtkSmartPointer<vtkTexture> convertToTexture( const cv::Mat& img, bool XFLIP=true);
rVTK_EXPORT vtkSmartPointer<vtkTexture> loadTexture( const std::string& fname, bool XFLIP=true);

// Convert matrix to VTK format.
rVTK_EXPORT vtkSmartPointer<vtkMatrix4x4> toVTK( const cv::Matx44d&);

// Convert VTK matrix to OpenCV format.
rVTK_EXPORT cv::Matx44d toCV( const vtkMatrix4x4*);

// Print the given matrix to the given stream.
rVTK_EXPORT void print( std::ostream&, const vtkMatrix4x4*);

// Extract the vertex IDs of points in pdata that lie on edges used only by one polygon.
rVTK_EXPORT void extractBoundaryVertices( const vtkSmartPointer<vtkPolyData>& pdata, std::vector<int>& pts);

// Generate a set of normals from a vtkPolyData object having point and cell data.
rVTK_EXPORT vtkSmartPointer<vtkPolyData> generateNormals( vtkSmartPointer<vtkPolyData> pdata);

// Dump a colour or Z buffer image from the provided render window.
rVTK_EXPORT cv::Mat_<cv::Vec3b> extractImage( const vtkRenderWindow*);
rVTK_EXPORT cv::Mat_<float> extractZBuffer( const vtkRenderWindow*);

rVTK_EXPORT void printCameraDetails( vtkCamera*, std::ostream&);    // Print camera details to the given stream

// Replace the lights in the given renderer with the given lights.
// The default light is a scene light that's white with intensity of 1.
struct rVTK_EXPORT Light
{
    Light();    // Default is a headlight
    Light( const cv::Vec3f& pos, const cv::Vec3f& foc); // Creates a scene light with given position and focus
    void toCamera(bool);    // Switch between camera(true) and scene(false)
    vtkSmartPointer<vtkLight> light;
};  // end struct

rVTK_EXPORT void resetLights( vtkRenderer*, const std::vector<Light>&);

// Create six lights all of which are d units from the origin (which is the focus).
// By default, the lights are of the scene type, but can be set as camera lights.
rVTK_EXPORT void createBoxLights( float d, std::vector<Light>&, bool toCamera=false);
}   // end namespace

#endif
