#include "LinesMapper.h"
using RVTK::LinesMapper;


LinesMapper::Ptr LinesMapper::create( const Lines3d &lns)
{
    LinesMapper::Ptr ptr( new LinesMapper( lns));
    return ptr;
}   // end create



LinesMapper::Ptr LinesMapper::create()
{
    LinesMapper::Ptr ptr( new LinesMapper());
    return ptr;
}   // end create



LinesMapper::LinesMapper( const Lines3d &lns)
    : mapper( vtkSmartPointer<vtkPolyDataMapper>::New())
{
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    polyData->SetPoints( points);
    polyData->SetLines( lines);

#if VTK_MAJOR_VERSION >= 6
    mapper->SetInputData( polyData);
#else
    mapper->SetInput( polyData);
#endif

    int cellIdx = 0;
    for ( size_t i = 0; i < lns.size(); ++i)
    {
        cv::Vec6f ln = lns.at(i);
        lines3d.push_back(ln);

        points->InsertNextPoint( ln[0], ln[1], ln[2]);
        points->InsertNextPoint( ln[3], ln[4], ln[5]);
        lines->InsertNextCell( 2);
        lines->InsertCellPoint( cellIdx++);
        lines->InsertCellPoint( cellIdx++);
    }   // end for

    lines->Modified();
    points->Modified();
}   // end ctor



LinesMapper::LinesMapper()
    : mapper( vtkSmartPointer<vtkPolyDataMapper>::New())
{
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    polyData->SetPoints( points);
    polyData->SetLines( lines);
#if VTK_MAJOR_VERSION >= 6
    mapper->SetInputData( polyData);
#else
    mapper->SetInput( polyData);
#endif
}   // end ctor



void LinesMapper::add( const Lines3d &lns)
{
    vtkPolyData* polyData = (vtkPolyData*)mapper->GetInput();
    vtkPoints* points = polyData->GetPoints();
    vtkCellArray* lines = polyData->GetLines();

    int cellIdx = lines->GetNumberOfCells();
    for ( size_t i = 0; i < lns.size(); ++i)
    {
        cv::Vec6f ln = lns.at(i);
        lines3d.push_back(ln);

        points->InsertNextPoint( ln[0], ln[1], ln[2]);
        points->InsertNextPoint( ln[3], ln[4], ln[5]);
        lines->InsertNextCell( 2);
        lines->InsertCellPoint( cellIdx++);
        lines->InsertCellPoint( cellIdx++);
    }   // end for

    lines->Modified();
    points->Modified();
}   // end add
