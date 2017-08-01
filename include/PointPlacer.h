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

    void setModel( const vtkSmartPointer<vtkActor>);    // Set provided actor as the only one
    void addModel( const vtkSmartPointer<vtkActor>);    // Add an actor
    void clearModels(); // Remove all actors

    bool hasModel() const { return _pplacer->GetNumberOfProps() > 0;}

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

    PointPlacer( const PointPlacer&);   // Not implemented
    void operator=( const PointPlacer&);    // Not implemented
};  // end class

}   // end namespace

#endif
