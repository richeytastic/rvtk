#include "DijkstraShortestPathLineInterpolator.h"
#include <iostream>
#include <algorithm>
#include <vtkCommand.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <DijkstraShortestPathFinder.h> // RFeatures
using RVTK::DijkstraShortestPathLineInterpolator;


// private
DijkstraShortestPathLineInterpolator::DijkstraShortestPathLineInterpolator() : _spfinder(NULL), _pathVtxs(NULL)
{}   // end ctor


// protected
DijkstraShortestPathLineInterpolator::~DijkstraShortestPathLineInterpolator()
{
    if ( _spfinder)
    {
        delete _spfinder;
        delete _pathVtxs;
    }   // end if
}   // end ctor


// public
DijkstraShortestPathLineInterpolator* DijkstraShortestPathLineInterpolator::New()
{
    return new DijkstraShortestPathLineInterpolator;
}   // end New


// public
void DijkstraShortestPathLineInterpolator::setModel( const RVTK::VtkModel::Ptr& vmodel)
{
    _vmodel = vmodel;
    if ( _spfinder)
    {
        delete _spfinder;
        delete _pathVtxs;
    }   // end if
    _spfinder = new RFeatures::DijkstraShortestPathFinder( _vmodel->getObjectModel());
    _pathVtxs = new std::vector<int>;
}   // end setModel


// public
int DijkstraShortestPathLineInterpolator::InterpolateLine( vtkRenderer* ren, vtkContourRepresentation* rep, int n0, int n1)
{
    if ( !_vmodel)
    {
        std::cerr << "[ERROR] DijkstraShortestPathLineInterpolator::InterpolateLine:"
                  << "\n\tMust setModel() first!" << std::endl;
        return 0;
    }   // end if

    const int numNodes = rep->GetNumberOfNodes();
    if ( n0 < 0 || n1 < 0 || n0 >= numNodes || n1 >= numNodes)
        return 0;

    // Get the actual world positions of the contour nodes
    double p0[3], p1[3];
    if (!rep->GetNthNodeWorldPosition( n0, p0) || !rep->GetNthNodeWorldPosition( n1, p1))
    {
        std::cerr << "[ERROR] DijkstraShortestPathLineInterpolator::InterpolateLine:"
                  << "\n\tFailed to get Nth node world position!" << std::endl;
        return 0;
    }   // end if

    // Get the closest vertices on the model to the contour nodes
    const int uv0 = _vmodel->getClosestObjUniqueVertexIdx( p0);
    const int uv1 = _vmodel->getClosestObjUniqueVertexIdx( p1);
    if ( !_spfinder->setEndPointUniqueVertexIndices( uv0, uv1))
    {
        std::cerr << "[ERROR] DijkstraShortestPathLineInterpolator::InterpolateLine:"
                  << "\n\tFailed to set end point uvidxs in _spfinder of " << uv0 << " and " << uv1 << std::endl;
        assert(false);
    }   // end if

    // Get the path vertices (inclusive of uv0 and uv1)
    const int pathLen = _spfinder->findShortestPath( *_pathVtxs);

    int retVal = 0;
    if ( pathLen < 0)
    {
        std::cerr << "[ERROR] DijkstraShortestPathLineInterpolator::InterpolateLine:"
                  << "\n\tNo path found between unqiue vertices " << uv0 << " and " << uv1 << std::endl;
    }   // end if
    else
    {
        // Add the intermediate points
        // (Reverse the shortest path (since its actually uv1 to uv0 inclusive))
        const RFeatures::ObjModel::Ptr omodel = _vmodel->getObjectModel();
        double wpos[3];
        for ( int i = pathLen-1; i > 0; --i)
        {
            const int uvidx = _pathVtxs->at(i);
            const cv::Vec3f& v = omodel->getUniqueVertex( uvidx);
            wpos[0] = v[0]; wpos[1] = v[1]; wpos[2] = v[2];
            rep->AddIntermediatePointWorldPosition( n0, wpos);
        }   // end for
        retVal = 1;
    }   // end else

    return retVal;
}   // end InterpolateLine


