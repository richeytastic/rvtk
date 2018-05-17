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

#include <DijkstraShortestPathLineInterpolator.h>
#include <DijkstraShortestPathFinder.h>
#include <iostream>
#include <algorithm>
using RVTK::DijkstraShortestPathLineInterpolator;

// private
DijkstraShortestPathLineInterpolator::DijkstraShortestPathLineInterpolator() : vtkContourLineInterpolator()
{}   // end ctor

// public
DijkstraShortestPathLineInterpolator* DijkstraShortestPathLineInterpolator::New()
{
    return new DijkstraShortestPathLineInterpolator;
}   // end New

// public
void DijkstraShortestPathLineInterpolator::setModel( const RFeatures::ObjModelKDTree* kdtree) { _kdtree = kdtree;}


// public
int DijkstraShortestPathLineInterpolator::InterpolateLine( vtkRenderer* ren, vtkContourRepresentation* rep, int n0, int n1)
{
    if ( !_kdtree)
    {
        std::cerr << "[ERROR] RVTK::DijkstraShortestPathLineInterpolator::InterpolateLine: Model not set!" << std::endl;
        return 0;
    }   // end if

    const int numNodes = rep->GetNumberOfNodes();
    if ( n0 < 0 || n1 < 0 || n0 >= numNodes || n1 >= numNodes)
        return 0;

    cv::Vec3d p0, p1; // Get the actual world positions of the contour nodes
    if (!rep->GetNthNodeWorldPosition( n0, &p0[0]) || !rep->GetNthNodeWorldPosition( n1, &p1[0]))
    {
        std::cerr << "[ERROR] RVTK::DijkstraShortestPathLineInterpolator::InterpolateLine: Failed to get Nth node world position!\n";
        assert(false);
        return 0;
    }   // end if

    // Get the closest vertices on the model to the contour nodes
    const int uv0 = _kdtree->find(p0);
    const int uv1 = _kdtree->find(p1);

    const RFeatures::ObjModel* model = _kdtree->model();
    RFeatures::DijkstraShortestPathFinder spfinder( model);
    if ( !spfinder.setEndPointVertexIndices( uv0, uv1))
        assert(false);

    // Get the path vertices (inclusive of uv0 and uv1)
    std::vector<int> pathVtxs;
    const int pathLen = spfinder.findShortestPath( pathVtxs);

    int retVal = 0;
    if ( pathLen < 0)
        std::cerr << "[WARNING] RVTK::DijkstraShortestPathLineInterpolator::InterpolateLine: No path between vertices " << uv0 << " and " << uv1 << std::endl;
    else
    {
        // Add the intermediate points: Reverse shortest path (since it's actually uv1 to uv0 inclusive)
        double wpos[3];
        for ( int i = pathLen-1; i > 0; --i)
        {
            const cv::Vec3f& v = model->vtx( pathVtxs[i]);
            wpos[0] = v[0];
            wpos[1] = v[1];
            wpos[2] = v[2];
            rep->AddIntermediatePointWorldPosition( n0, wpos);
        }   // end for
        retVal = 1;
    }   // end else

    return retVal;
}   // end InterpolateLine


