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

#include <PointPlacer.h>
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


void PointPlacer::set( const vtkProp* p)
{
    _pplacer->RemoveAllProps();
    _pplacer->AddProp( const_cast<vtkProp*>(p));
}   // end set


void PointPlacer::add( const vtkProp* p)
{
    _pplacer->AddProp( const_cast<vtkProp*>(p));
}   // end add


void PointPlacer::clear()
{
    _pplacer->RemoveAllProps();
}   // end clear


bool PointPlacer::hasProps() const
{
    return _pplacer->GetNumberOfProps() > 0;
}   // end hasProps


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

