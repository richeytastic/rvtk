#include "VtkTools.h"
#include "ClosestPointFinder.h"
#include <iostream>
#include <cassert>
#include <vtkTriangle.h>
#include <vtkFloatArray.h>
#include <vtkPolyDataNormals.h>


vtkSmartPointer<vtkLookupTable> RVTK::createColoursLookupTable(
        int numColours, const vtkColor3ub& scol, const vtkColor3ub& fcol)
{
    vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
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

    return lut;
}   // end createColoursLookupTable


void RVTK::setLookupTable( vtkSmartPointer<vtkActor> actor, vtkSmartPointer<vtkLookupTable> lut)
{
    vtkSmartPointer<vtkMapper> mapper = actor->GetMapper();
    mapper->SetScalarRange( 0, lut->GetNumberOfTableValues() - 1);
    mapper->SetLookupTable( lut);
}   // end setLookupTable


vtkSmartPointer<vtkPolyData> RVTK::getPolyData( const vtkSmartPointer<vtkActor>& actor)
{
    vtkSmartPointer<vtkPolyDataMapper> mapper = (vtkPolyDataMapper*)actor->GetMapper();
    return mapper->GetInput();
}   // end getPolyData


vtkSmartPointer<vtkPolyDataMapper> RVTK::createMapper( const vtkSmartPointer<vtkPolyData>& data)
{
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
#if VTK_MAJOR_VERSION == 6
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
        return texture;

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
    return texture;
}   // end convertToTexture


void RVTK::extractBoundaryVertices( const vtkSmartPointer<vtkPolyData>& pdata, std::vector<int>& bvids)
{
    vtkSmartPointer<vtkFeatureEdges> vfe = vtkSmartPointer<vtkFeatureEdges>::New();
#if VTK_MAJOR_VERSION == 6
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

    RVTK::ClosestPointFinder cpfinder( pdata);
    const int npts = odata->GetNumberOfPoints();
    double x[3];
    for ( int i = 0; i < npts; ++i)
    {
        odata->GetPoint( i, x);
        const int uvid = cpfinder.getClosestVertex( x);
        bvids.push_back( uvid);
    }   // end for
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

