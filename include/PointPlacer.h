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

/***
 * Query 3D position on surface of a poly data object from a 2D render window pixel position.
 *
 * Richard Palmer
 * March 2015
 ***/

#ifndef RVTK_POINT_PLACER_H
#define RVTK_POINT_PLACER_H

#include <boost/shared_ptr.hpp>
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkPolygonalSurfacePointPlacer.h>
#include "rVTK_Export.h"

namespace RVTK
{

enum rVTK_EXPORT DisplayOrigin
{
    TOP_LEFT_DISPLAY_ORIGIN,    // OpenCV 2D
    BOTTOM_LEFT_DISPLAY_ORIGIN  // VTK 2D
};  // end enum


class rVTK_EXPORT PointPlacer
{
public:
    typedef boost::shared_ptr<PointPlacer> Ptr;
    static Ptr create( const vtkSmartPointer<vtkRenderer>);
    static Ptr create( const vtkSmartPointer<vtkRenderer>, vtkSmartPointer<vtkPolygonalSurfacePointPlacer>);

    void set( const vtkProp*);    // Set prop as the only one
    void add( const vtkProp*);    // Add a prop
    void clear(); // Remove all props

    bool hasProps() const;

    // Calculate the surface position on the model from x,y display coords and set worldPos with the result.
    // Returns true if x,y is a valid display coordinate on the model.
    // Default display origin uses VTK 2D display coords (bottom left).
    // OpenCV coords uses TOP_LEFT_DISPLAY_ORIGIN.
    bool calcSurfacePosition( int x, int y, float* worldPos, DisplayOrigin displayOrigin=BOTTOM_LEFT_DISPLAY_ORIGIN) const;

    const vtkSmartPointer<vtkRenderer> getRenderer() const { return _renderer;}
    const vtkSmartPointer<vtkPolygonalSurfacePointPlacer> getVtkPointPlacer() const { return _pplacer;}

protected:
    const vtkSmartPointer<vtkRenderer> _renderer;
    vtkSmartPointer<vtkPolygonalSurfacePointPlacer> _pplacer;

private:
    explicit PointPlacer( const vtkSmartPointer<vtkRenderer>);
    PointPlacer( const vtkSmartPointer<vtkRenderer>, vtkSmartPointer<vtkPolygonalSurfacePointPlacer>);
    PointPlacer( const PointPlacer&);       // No copy
    void operator=( const PointPlacer&);    // No copy
};  // end class

}   // end namespace

#endif
