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

#ifndef RVTK_DIJKSTRA_SHORTEST_PATH_LINE_INTERPOLATOR_H
#define RVTK_DIJKSTRA_SHORTEST_PATH_LINE_INTERPOLATOR_H

#include "rVTK_Export.h"
#include <vtkRenderer.h>
#include <vtkContourRepresentation.h>
#include <vtkContourLineInterpolator.h>
#include <ObjModelKDTree.h>              // RFeatures

namespace RVTK
{

/*** Custom line interpolation ***/
//class VTK_WIDGETS_EXPORT DijkstraShortestPathLineInterpolator : public vtkContourLineInterpolator
class rVTK_EXPORT DijkstraShortestPathLineInterpolator : public vtkContourLineInterpolator
{
public:
    vtkTypeMacro( DijkstraShortestPathLineInterpolator, vtkContourLineInterpolator);
    static DijkstraShortestPathLineInterpolator* New();

    void setModel( RFeatures::ObjModelKDTree::Ptr);

    // Interpolate between nodes n0 and n1 on the given vtkContourRepresentation
    virtual int InterpolateLine( vtkRenderer* ren, vtkContourRepresentation* rep, int n0, int n1);

protected:
    virtual ~DijkstraShortestPathLineInterpolator(){}

private:
    RFeatures::ObjModelKDTree::Ptr _kdtree;

    DijkstraShortestPathLineInterpolator();
    DijkstraShortestPathLineInterpolator( const DijkstraShortestPathLineInterpolator&);   // Not implemented
    void operator=( const DijkstraShortestPathLineInterpolator&);    // Not implemented
};  // end class

}   // end namespace

#endif 
