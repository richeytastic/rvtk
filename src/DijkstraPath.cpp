#include "DijkstraPath.h"
using RVTK::DijkstraPath;
#include <vtkTriangleFilter.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkIdList.h>


RVTK::DijkstraPath::DijkstraPath( const vtkSmartPointer<vtkPolyData>& pdata)
    : _dijkstra( vtkSmartPointer<vtkDijkstraGraphGeodesicPath>::New()), _actor(NULL)
{
    vtkSmartPointer<vtkTriangleFilter> tfilter = vtkSmartPointer<vtkTriangleFilter>::New();
#if VTK_MAJOR_VERSION == 6
    tfilter->SetInputData( pdata);
#else
    tfilter->SetInput( pdata);
#endif
    tfilter->Update();
    vtkSmartPointer<vtkPolyData> pd = tfilter->GetOutput();
#if VTK_MAJOR_VERSION == 6
    _dijkstra->SetInputData( pd);
#else
    _dijkstra->SetInput( pd);
#endif
    _pdata = pd;
}   // end ctor


void RVTK::DijkstraPath::setEndPoints( int s, int f)
{
    _dijkstra->SetStartVertex(s);
    _dijkstra->SetEndVertex(f);
    _dijkstra->Update();
}   // end setEndPoints


vtkSmartPointer<vtkPoints> RVTK::DijkstraPath::getPathVertexPositions() const
{
    vtkIdList* idlist = _dijkstra->GetIdList();
    const int n = idlist->GetNumberOfIds();
    vtkSmartPointer<vtkPoints> vpoints = vtkSmartPointer<vtkPoints>::New();
    double x[3];
    for ( int i = 0; i < n; ++i)
    {
        vtkIdType vid = idlist->GetId(i);
        _pdata->GetPoint( vid, x);
        vpoints->InsertNextPoint(x);
    }   // end for
    return vpoints;
}   // end getPathVertexPositions


void RVTK::DijkstraPath::setColour( const vtkColor3ub& col)
{
    if ( !_actor)   // Cause actor to be created if not already
        getActor();
    _actor->GetProperty()->SetColor(float(col[0])/255, float(col[1])/255, float(col[2])/255);
}   // end setColour


void RVTK::DijkstraPath::setLineWidth( float w)
{
    if ( !_actor)   // Cause actor to be created if not already
        getActor();
    _actor->GetProperty()->SetLineWidth(w);
}   // end setLineWidth


const vtkSmartPointer<vtkActor> RVTK::DijkstraPath::getActor() const
{
    if ( !_actor)   // Create the actor if not already being used
    {
        _actor = vtkSmartPointer<vtkActor>::New();
        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection( _dijkstra->GetOutputPort());
        _actor->SetMapper(mapper);
        _actor->GetProperty()->SetColor(0,0,1); // Blue
        _actor->GetProperty()->SetLineWidth(5);
    }   // end if
    return _actor;
}   // end getActor
