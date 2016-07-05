#include "PointCloudMapper.h"
using RVTK::PointCloudMapper;
#include <cassert>


PointCloudMapper::Ptr PointCloudMapper::create( const cv::Mat_<cv::Vec3f> pts, const cv::Mat_<cv::Vec3b> cimg)
{
    return PointCloudMapper::Ptr( new PointCloudMapper( pts, cimg));
}   // end create


PointCloudMapper::Ptr PointCloudMapper::create( const PointCloud::Ptr pts)
{
    return PointCloudMapper::Ptr( new PointCloudMapper( pts));
}   // end create



PointCloudMapper::Ptr PointCloudMapper::create()
{
    PointCloudMapper::Ptr ptr( new PointCloudMapper());
    return ptr;
}   // end create



void addPoints( const cv::Mat_<cv::Vec3f> pts, const cv::Mat_<cv::Vec3b> cimg,
        vtkSmartPointer<vtkPoints> points,
        vtkSmartPointer<vtkUnsignedCharArray> cols,
        vtkSmartPointer<vtkCellArray> cells, int cellIdx)
{
    assert( pts.size() == cimg.size());
    const int rws = pts.rows;
    const int cls = pts.cols;

    for ( int i = 0; i < rws; ++i)
    {
        const cv::Vec3f* pptr = pts.ptr<cv::Vec3f>(i);
        const cv::Vec3b* cptr = cimg.ptr<cv::Vec3b>(i);

        for ( int j = 0; j < cls; ++j)
        {
            const cv::Vec3f& p = pptr[j];
            const cv::Vec3b& c = cptr[j];
            points->InsertNextPoint( p[0], p[1], p[2]);
            cols->InsertNextTuple3( c[2], c[1], c[0]);
            cells->InsertNextCell( 1);
            cells->InsertCellPoint( cellIdx++);
        }   // end for - cols
    }   // end for - rows

    points->Modified();
}   // end addPoints



void addPoints( const PointCloud::Ptr pts,
        vtkSmartPointer<vtkPoints> points,
        vtkSmartPointer<vtkUnsignedCharArray> cols,
        vtkSmartPointer<vtkCellArray> cells, int cellIdx)
{
    const int npts = (int)pts->size();

    float x, y, z;
    byte r, g, b;
    for ( int i = 0; i < npts; ++i)
    {
        const RFeatures::PointXYZRGB& p = pts->at(i);
        PointCloud::unpackPoint( p, x, y, z, r, g, b);
        points->InsertNextPoint( x, y, z);
        cols->InsertNextTuple3( r, g, b);
        cells->InsertNextCell( 1);
        cells->InsertCellPoint( cellIdx++);
    }   // end for

    points->Modified();
}   // end addPoints




PointCloudMapper::PointCloudMapper( const cv::Mat_<cv::Vec3f> pts, const cv::Mat_<cv::Vec3b> cimg)
    : mapper( vtkSmartPointer<vtkPolyDataMapper>::New())
{
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkUnsignedCharArray> cols = vtkSmartPointer<vtkUnsignedCharArray>::New();
    cols->SetNumberOfComponents(3);
    vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();

    addPoints( pts, cimg, points, cols, cells, 0);

    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints( points);
    polyData->GetPointData()->SetScalars( cols);
    polyData->SetVerts( cells);
#if VTK_MAJOR_VERSION >= 6
    mapper->SetInputData( polyData);
#else
    mapper->SetInput( polyData);
#endif
}   // end ctor



PointCloudMapper::PointCloudMapper( const PointCloud::Ptr pts)
    : mapper( vtkSmartPointer<vtkPolyDataMapper>::New())
{
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkUnsignedCharArray> cols = vtkSmartPointer<vtkUnsignedCharArray>::New();
    cols->SetNumberOfComponents(3);
    vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();

    addPoints( pts, points, cols, cells, 0);

    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints( points);
    polyData->GetPointData()->SetScalars( cols);
    polyData->SetVerts( cells);
#if VTK_MAJOR_VERSION >= 6
    mapper->SetInputData( polyData);
#else
    mapper->SetInput( polyData);
#endif
}   // end ctor



PointCloudMapper::PointCloudMapper()
    : mapper( vtkSmartPointer<vtkPolyDataMapper>::New())
{
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkUnsignedCharArray> cols = vtkSmartPointer<vtkUnsignedCharArray>::New();
    cols->SetNumberOfComponents(3);
    vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();

    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints( points);
    polyData->GetPointData()->SetScalars( cols);
    polyData->SetVerts( cells);
#if VTK_MAJOR_VERSION >= 6
    mapper->SetInputData( polyData);
#else
    mapper->SetInput( polyData);
#endif
}   // end ctor



void PointCloudMapper::add( const cv::Mat_<cv::Vec3f> pts, const cv::Mat_<cv::Vec3b> cimg)
{
    vtkPolyData* polyData = (vtkPolyData*)mapper->GetInput();
    vtkPoints* points = polyData->GetPoints();
    vtkUnsignedCharArray* cols = (vtkUnsignedCharArray*)polyData->GetPointData()->GetScalars();
    vtkCellArray* cells = polyData->GetVerts();
    addPoints( pts, cimg, points, cols, cells, cells->GetNumberOfCells());
}   // end add



void PointCloudMapper::add( const PointCloud::Ptr pts)
{
    vtkPolyData* polyData = (vtkPolyData*)mapper->GetInput();
    vtkPoints* points = polyData->GetPoints();
    vtkUnsignedCharArray* cols = (vtkUnsignedCharArray*)polyData->GetPointData()->GetScalars();
    vtkCellArray* cells = polyData->GetVerts();
    addPoints( pts, points, cols, cells, cells->GetNumberOfCells());
}   // end add



/*
void PointCloudMapper::setColour( byte r, byte g, byte b)
{
    vtkUnsignedCharArray* cols = (vtkUnsignedCharArray*)polyData->GetPointData()->GetScalars();

    byte col[3];
    col[0] = r;
    col[1] = g;
    col[2] = b;
    size_t nvals = cols->GetNumberOfTuples();
    for ( size_t i = 0; i < nvals; ++i)
    {
        cols->SetTupleValue( i, col);
        pcl::PointXYZRGB &pt = pcloud->at( 0, i);
        PointCloud::packPoint( pt, pt.x, pt.y, pt.z, r, g, b);
    }   // end for

    polyData->GetPoints()->Modified();
}   // end setColour
*/
