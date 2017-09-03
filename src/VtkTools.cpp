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

#include "VtkTools.h"
#include <cassert>
#include <vtkOctreePointLocator.h>
#include <vtkFeatureEdges.h>
#include <vtkFloatArray.h>
#include <vtkPolyDataNormals.h>
#include <vtkWindowToImageFilter.h>
#include <vtkImageShiftScale.h>
#include <vtkImageExport.h>
#include <vtkRenderWindow.h>


void RVTK::setColoursLookupTable( vtkSmartPointer<vtkLookupTable> lut,
                            int numColours, const vtkColor3ub& scol, const vtkColor3ub& fcol)
{
    lut->SetNumberOfTableValues( numColours);
    lut->Build();

    float cstep[3];
    cstep[0] = (float(fcol[0]) - float(scol[0]))/(numColours-1);
    cstep[1] = (float(fcol[1]) - float(scol[1]))/(numColours-1);
    cstep[2] = (float(fcol[2]) - float(scol[2]))/(numColours-1);
    for ( int i = 0; i < numColours; ++i)
    {
        float rgb[3];
        rgb[0] = (scol[0] + i*cstep[0])/255.0;
        rgb[1] = (scol[1] + i*cstep[1])/255.0;
        rgb[2] = (scol[2] + i*cstep[2])/255.0;
        lut->SetTableValue( i, rgb[0], rgb[1], rgb[2], 1);
    }   // end for
}   // end createColoursLookupTable


vtkPolyData* RVTK::getPolyData( const vtkActor* actor)
{
    vtkPolyDataMapper* mapper = (vtkPolyDataMapper*)const_cast<vtkActor*>(actor)->GetMapper();
    return mapper->GetInput();
}   // end getPolyData


vtkSmartPointer<vtkPolyDataMapper> RVTK::createMapper( const vtkSmartPointer<vtkPolyData>& data)
{
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION >= 6
    mapper->SetInputData( data);
#else
    mapper->SetInput( data);
#endif
    return mapper;
}   // end createMapper


vtkSmartPointer<vtkImageImport> RVTK::makeImageImporter( const cv::Mat img)
{
    const int rows = img.rows;
    const int cols = img.cols;
    const int nchannels = img.channels();

    vtkSmartPointer<vtkImageImport> imageImporter = vtkSmartPointer<vtkImageImport>::New();
    imageImporter->SetDataScalarTypeToUnsignedChar();
    imageImporter->CopyImportVoidPointer( img.data, rows * cols * nchannels * sizeof(unsigned char));
    imageImporter->SetNumberOfScalarComponents( nchannels);
    imageImporter->SetWholeExtent( 0, cols-1, 0, rows-1, 0, 0);
    imageImporter->SetDataExtentToWholeExtent();
    imageImporter->Update();
    return imageImporter;
}   // end makeImageImporter


// Converts the given image into a format suitable as a texture.
// Flips the image vertically and transposes the first and second
// bytes if image is triple channel.
vtkSmartPointer<vtkTexture> RVTK::convertToTexture( const cv::Mat& image, bool XFLIP)
{
    vtkSmartPointer<vtkTexture> texture = vtkSmartPointer<vtkTexture>::New();

    // Only accepts either cv::Mat_<cv::Vec3b> or cv::Mat_<byte> images for now
    const int numChannels = image.channels();
    if ( (image.depth() != CV_8U) || !image.isContinuous() || ( numChannels != 1 && numChannels != 3))
    {
        std::cerr << "[WARNING] RVTK::convertToTexture(): Unable to create texture from image!" << std::endl;
        return vtkSmartPointer<vtkTexture>();
    }   // end if

    // Flip the input image vertically.
    cv::Mat img;
    if ( XFLIP)
        cv::flip( image, img, 0/*flip about x axis*/);
    else
        img = image;

    if ( numChannels == 3)  // Swap byte order from BGR to RGB
    {
        std::vector<cv::Mat> channels;
        cv::split( img, channels);
        cv::Mat swappedChannels[3] = {channels[2], channels[1], channels[0]};
        cv::Mat nimg;
        cv::merge( swappedChannels, 3, nimg);
        img = nimg;
    }   // end if

    // Make the texture from the image
    vtkSmartPointer<vtkImageImport> imageImporter = RVTK::makeImageImporter( img);
    texture->SetInputConnection( imageImporter->GetOutputPort());
    texture->Update();
    return texture;
}   // end convertToTexture



vtkSmartPointer<vtkTexture> RVTK::loadTexture( const std::string& fname, bool XFLIP)
{
    cv::Mat m = cv::imread( fname, true);
    if ( m.empty())
        return vtkSmartPointer<vtkTexture>();
    return convertToTexture( m, XFLIP);
}   // end loadTexture



void RVTK::extractBoundaryVertices( const vtkSmartPointer<vtkPolyData>& pdata, std::vector<int>& bvids)
{
    vtkSmartPointer<vtkFeatureEdges> vfe = vtkSmartPointer<vtkFeatureEdges>::New();
#if VTK_MAJOR_VERSION >= 6
    vfe->SetInputData( pdata);
#else
    vfe->SetInput( pdata);
#endif
    vfe->BoundaryEdgesOn(); // Extract edges used by exactly one polygon
    vfe->FeatureEdgesOff();
    vfe->ManifoldEdgesOff();
    vfe->NonManifoldEdgesOff();
    vfe->Update();
    vtkPolyData* odata = vfe->GetOutput();

    vtkOctreePointLocator* plocator = vtkOctreePointLocator::New();
    plocator->SetDataSet( pdata);

    double x[3];
    const int npts = odata->GetNumberOfPoints();
    for ( int i = 0; i < npts; ++i)
    {
        odata->GetPoint( i, x);
        bvids.push_back( plocator->FindClosestPoint(x));
    }   // end for

    plocator->Delete();
}   // end extractBoundaryVertices


vtkSmartPointer<vtkPolyData> RVTK::generateNormals( vtkSmartPointer<vtkPolyData> pdata)
{
    vtkSmartPointer<vtkPolyDataNormals> normalsGenerator = vtkPolyDataNormals::New();
#if VTK_MAJOR_VERSION <= 5
    normalsGenerator->SetInput( pdata);
#else
    normalsGenerator->SetInputData( pdata);
#endif
    normalsGenerator->ConsistencyOff();
    normalsGenerator->ComputePointNormalsOn();
    normalsGenerator->ComputeCellNormalsOn();
    normalsGenerator->NonManifoldTraversalOn();
    normalsGenerator->Update();
    return normalsGenerator->GetOutput();
}   // end generateNormals



cv::Mat_<cv::Vec3b> RVTK::extractImage( const vtkRenderWindow* renWin)
{
    vtkRenderWindow* rw = const_cast<vtkRenderWindow*>( renWin);
    vtkSmartPointer<vtkWindowToImageFilter> filter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    filter->SetInput( rw);
    filter->SetMagnification(1);
    filter->SetInputBufferTypeToRGB();  // Extract RGB info

    vtkSmartPointer<vtkImageShiftScale> scale = vtkSmartPointer<vtkImageShiftScale>::New();
    scale->SetOutputScalarTypeToUnsignedChar();
    scale->SetInputConnection( filter->GetOutputPort());

    vtkSmartPointer<vtkImageExport> exporter = vtkSmartPointer<vtkImageExport>::New();
    exporter->SetInputConnection( scale->GetOutputPort());
    exporter->SetImageLowerLeft(false); // Flip vertically for OpenCV

    const int cols = rw->GetSize()[0];
    const int rows = rw->GetSize()[1];
    cv::Mat_<cv::Vec3b> img( rows, cols);
    exporter->SetExportVoidPointer( img.ptr());
    exporter->Export();

    // Need to swap the red and blue channels for OpenCV
    cv::cvtColor( img, img, CV_RGB2BGR);

    return img;
}   // end extractImage



cv::Mat_<float> RVTK::extractZBuffer( const vtkRenderWindow* renWin)
{
    vtkRenderWindow* rw = const_cast<vtkRenderWindow*>( renWin);
    vtkSmartPointer<vtkWindowToImageFilter> filter = vtkSmartPointer<vtkWindowToImageFilter>::New();
    filter->SetInput( rw);
    filter->SetMagnification(1);
    filter->SetInputBufferTypeToZBuffer();

    vtkSmartPointer<vtkImageShiftScale> scale = vtkSmartPointer<vtkImageShiftScale>::New();
    scale->SetOutputScalarTypeToFloat();
    scale->SetInputConnection( filter->GetOutputPort());

    vtkSmartPointer<vtkImageExport> exporter = vtkSmartPointer<vtkImageExport>::New();
    exporter->SetInputConnection( scale->GetOutputPort());
    exporter->SetImageLowerLeft(false); // Flip vertically for OpenCV

    const int cols = rw->GetSize()[0];
    const int rows = rw->GetSize()[1];
    cv::Mat_<float> rngMap( rows, cols);
    exporter->SetExportVoidPointer( rngMap.ptr());
    exporter->Export();

    return rngMap;
}   // end extractZBuffer



void RVTK::printCameraDetails( vtkCamera* cam, std::ostream &os)
{
    using std::endl;
    os << "*********************************************" << endl;
	double *camPos = cam->GetPosition();
	os << "Camera Position (x,y,z):          " << camPos[0] << ", " << camPos[1] << ", " << camPos[2] << endl;
	double *camFoc = cam->GetFocalPoint();
	os << "Camera Focus (x,y,z):             " << camFoc[0] << ", " << camFoc[1] << ", " << camFoc[2] << endl;
    double focDist = cam->GetDistance();  // Distance of focal point to camera position
    os << "Focal point distance (from cam):  " << focDist << endl;
    double *normal = cam->GetViewPlaneNormal();
    os << "Camera View Plane Normal (x,y,z): " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
    double *projDir = cam->GetDirectionOfProjection();
    os << "Direction of projection:          " << projDir[0] << ", " << projDir[1] << ", " << projDir[2] << endl;
    double *camUp = cam->GetViewUp();
    os << "Camera View Up Vector (x,y,z):    " << camUp[0] << ", " << camUp[1] << ", " << camUp[2] << endl;
    double *camOr = cam->GetOrientationWXYZ();
    os << "Camera Orientation (w,x,y,z):     " << camOr[0] << ", " << camOr[1] << ", " << camOr[2] << ", " << camOr[3] << endl;

	if ( cam->GetParallelProjection())
	{
		os << "Cam Projection:                   ORTHOGONAL" << endl;
		double ps = cam->GetParallelScale();
		os << "Cam Parallel Scale:           " << ps << " metres (viewport height)" << endl;
	}	// end if
	else
	{
		os << "Cam Projection:                   PERSPECTIVE" << endl;
		double va = cam->GetViewAngle();
		os << "Cam View Angle:                   " << va << " degrees" << endl;
	}	// end else
    os << "*********************************************" << endl;
}	// end printCameraDetails

