#pragma once
#ifndef RVTK_DIJKSTRA_SHORTEST_PATH_LINE_INTERPOLATOR_H
#define RVTK_DIJKSTRA_SHORTEST_PATH_LINE_INTERPOLATOR_H

#include <vtkRenderer.h>
#include <vtkContourRepresentation.h>
#include <vtkContourLineInterpolator.h>
#include "VtkModel.h"
#include <vector>

namespace RFeatures
{
    class DijkstraShortestPathFinder;
}   // end namespace

namespace RVTK
{

/*** Custom line interpolation ***/
//class VTK_WIDGETS_EXPORT DijkstraShortestPathLineInterpolator : public vtkContourLineInterpolator
class DijkstraShortestPathLineInterpolator : public vtkContourLineInterpolator
{
public:
    vtkTypeMacro( DijkstraShortestPathLineInterpolator, vtkContourLineInterpolator);
    static DijkstraShortestPathLineInterpolator* New();

    void setModel( const RVTK::VtkModel::Ptr& vmodel);

    // Interpolate between nodes n0 and n1 on the given vtkContourRepresentation
    virtual int InterpolateLine( vtkRenderer* ren, vtkContourRepresentation* rep, int n0, int n1);

protected:
    virtual ~DijkstraShortestPathLineInterpolator();

private:
    DijkstraShortestPathLineInterpolator();
    DijkstraShortestPathLineInterpolator( const DijkstraShortestPathLineInterpolator&);   // Not implemented
    void operator=( const DijkstraShortestPathLineInterpolator&);    // Not implemented

    RFeatures::DijkstraShortestPathFinder* _spfinder;
    std::vector<int>* _pathVtxs;
    RVTK::VtkModel::Ptr _vmodel;
};  // end class

}   // end namespace

#endif 
