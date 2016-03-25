#pragma once
#ifndef RVTK_DIJKSTRA_PATH_H
#define RVTK_DIJKSTRA_PATH_H

#include <vtkActor.h>
#include <vtkColor.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkDijkstraGraphGeodesicPath.h>
#include "rVTK_Export.h"

namespace RVTK
{

// Find the shortest path between two vertices on some poly data (with triangular cells).
class rVTK_EXPORT DijkstraPath
{
public:
    explicit DijkstraPath( const vtkSmartPointer<vtkPolyData>& data);

    // Set line start and end points and update the calculation.
    void setEndPoints( int startVertexId, int endVertexId);

    vtkSmartPointer<vtkPoints> getPathVertexPositions() const;

    // For actor
    void setColour( const vtkColor3ub& col);
    void setLineWidth( float);
    const vtkSmartPointer<vtkActor> getActor() const;

private:
    vtkSmartPointer<vtkDijkstraGraphGeodesicPath> _dijkstra;
    mutable vtkSmartPointer<vtkActor> _actor;
    vtkSmartPointer<vtkPolyData> _pdata;    // Triangle filtered poly data
};  // end class

}   // end namespace

#endif
