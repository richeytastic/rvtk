#include "DijkstraShortestPathLineInterpolator.h"
#include <iostream>
#include <algorithm>
#include <vtkCommand.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <DijkstraShortestPathFinder.h> // RFeatures
using RVTK::DijkstraShortestPathLineInterpolator;


// private
DijkstraShortestPathLineInterpolator::DijkstraShortestPathLineInterpolator() : _spfinder(NULL), _pathVtxs(NULL), _cpf(NULL)
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
bool DijkstraShortestPathLineInterpolator::setModel( const RVTK::ClosestPointFinder* cpf)
{
    if ( !cpf->canReverseMapToObjectIndices())
        return false;

    _cpf = cpf;
    if ( _spfinder)
    {
        delete _spfinder;
        delete _pathVtxs;
    }   // end if
    _spfinder = new RFeatures::DijkstraShortestPathFinder( _cpf->getObject());
    _pathVtxs = new std::vector<int>;
    return true;
}   // end setModel


// public
int DijkstraShortestPathLineInterpolator::InterpolateLine( vtkRenderer* ren, vtkContourRepresentation* rep, int n0, int n1)
{
    if ( !_cpf)
    {
        std::cerr << "[ERROR] DijkstraShortestPathLineInterpolator::InterpolateLine: Must setModel() first!" << std::endl;
        return 0;
    }   // end if

    const int numNodes = rep->GetNumberOfNodes();
    if ( n0 < 0 || n1 < 0 || n0 >= numNodes || n1 >= numNodes)
        return 0;

    // Get the actual world positions of the contour nodes
    double p0[3], p1[3];
    if (!rep->GetNthNodeWorldPosition( n0, p0) || !rep->GetNthNodeWorldPosition( n1, p1))
    {
        std::cerr << "[ERROR] DijkstraShortestPathLineInterpolator::InterpolateLine: Failed to get Nth node world position!\n";
        return 0;
    }   // end if

    // Get the closest vertices on the model to the contour nodes
    const int uv0 = _cpf->getClosestObjVertexId( p0);
    const int uv1 = _cpf->getClosestObjVertexId( p1);
    if ( !_spfinder->setEndPointVertexIndices( uv0, uv1))
    {
        std::cerr << "[ERROR] DijkstraShortestPathLineInterpolator::InterpolateLine: "
                  << "Failed to set end point uvidxs in _spfinder of " << uv0 << " and " << uv1 << std::endl;
        assert(false);
    }   // end if

    // Get the path vertices (inclusive of uv0 and uv1)
    const int pathLen = _spfinder->findShortestPath( *_pathVtxs);

    int retVal = 0;
    if ( pathLen < 0)
    {
        std::cerr << "[ERROR] DijkstraShortestPathLineInterpolator::InterpolateLine: "
                  << "No path found between unqiue vertices " << uv0 << " and " << uv1 << std::endl;
    }   // end if
    else
    {
        // Add the intermediate points
        // (Reverse the shortest path (since its actually uv1 to uv0 inclusive))
        const RFeatures::ObjModel::Ptr obj = _cpf->getObject();
        double wpos[3];
        for ( int i = pathLen-1; i > 0; --i)
        {
            const int uvidx = _pathVtxs->at(i);
            const cv::Vec3f& v = obj->getVertex( uvidx);
            wpos[0] = v[0]; wpos[1] = v[1]; wpos[2] = v[2];
            rep->AddIntermediatePointWorldPosition( n0, wpos);
        }   // end for
        retVal = 1;
    }   // end else

    return retVal;
}   // end InterpolateLine


