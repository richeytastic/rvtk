#include "PointPlacer.h"
using RVTK::PointPlacer;


PointPlacer::Ptr PointPlacer::create( const vtkSmartPointer<vtkRenderer> r)
{
    return Ptr( new PointPlacer(r));
}   // end create


PointPlacer::Ptr PointPlacer::create( const vtkSmartPointer<vtkRenderer> r,
                                      vtkSmartPointer<vtkPolygonalSurfacePointPlacer> p)
{
    return Ptr( new PointPlacer(r,p));
}   // end create


// private
PointPlacer::PointPlacer( const vtkSmartPointer<vtkRenderer> r)
    : _renderer(r), _pplacer( vtkPolygonalSurfacePointPlacer::New())
{}   // end ctor


// private
PointPlacer::PointPlacer( const vtkSmartPointer<vtkRenderer> r, vtkSmartPointer<vtkPolygonalSurfacePointPlacer> p)
    : _renderer(r), _pplacer( p)
{}   // end ctor


void PointPlacer::setModel( const vtkSmartPointer<vtkActor> p)
{
    _pplacer->RemoveAllProps();
    _pplacer->AddProp( p);
}   // end setModel


void PointPlacer::addModel( const vtkSmartPointer<vtkActor> p)
{
    _pplacer->AddProp( p);
}   // end addModel


void PointPlacer::clearModels()
{
    _pplacer->RemoveAllProps();
}   // end clearModels


bool PointPlacer::calcSurfacePosition( int x, int y, float* worldPos, RVTK::DisplayOrigin dispOrig) const
{
    if ( _pplacer->GetNumberOfProps() == 0)
        return false;

    if ( dispOrig == RVTK::TOP_LEFT_DISPLAY_ORIGIN)
        y = _renderer->GetSize()[1] - y - 1;    // Convert to VTK display origin (BOTTOM_LEFT_DISPLAY_ORIGIN)

    double dpos[2] = {(double)x, (double)y};
    if ( !_pplacer->ValidateDisplayPosition( _renderer, dpos))
        return false;

    double wpos[3];
    double worient[9];  // not used
    if (!_pplacer->ComputeWorldPosition( _renderer, dpos, wpos, worient))
        return false;

    worldPos[0] = wpos[0];
    worldPos[1] = wpos[1];
    worldPos[2] = wpos[2];

    return true;
}   // end calcSurfacePosition

