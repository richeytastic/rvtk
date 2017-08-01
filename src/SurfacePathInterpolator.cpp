#include "SurfacePathInterpolator.h"
#include <iostream>
#include <algorithm>
#include <vtkCommand.h>
#include <vtkOrientedGlyphContourRepresentation.h>
#include <DijkstraShortestPathFinder.h> // RFeatures
#include <FeatureUtils.h>   // RFeatures
using RVTK::SurfacePathInterpolator;


// private
SurfacePathInterpolator::SurfacePathInterpolator()
{}   // end ctor


// public
SurfacePathInterpolator* SurfacePathInterpolator::New()
{
    return new SurfacePathInterpolator;
}   // end New


// public
void SurfacePathInterpolator::setModel( const RVTK::VtkModel::Ptr& vmodel)
{
    _vmodel = vmodel;
}   // end setModel



// public
int SurfacePathInterpolator::InterpolateLine( vtkRenderer* ren, vtkContourRepresentation* rep, int n0, int n1)
{
    if ( !_vmodel)
    {
        std::cerr << "[ERROR] SurfacePathInterpolator::InterpolateLine: Must setModel() first!" << std::endl;
        return 0;
    }   // end if

    const int numNodes = rep->GetNumberOfNodes();
    if ( n0 < 0 || n1 < 0 || n0 >= numNodes || n1 >= numNodes)
        return 0;

    // Get the actual world positions of the contour nodes
    double p0[3], p1[3];
    if (!rep->GetNthNodeWorldPosition( n0, p0) || !rep->GetNthNodeWorldPosition( n1, p1))
    {
        std::cerr << "[ERROR] SurfacePathInterpolator::InterpolateLine: Failed to get Nth node world position!" << std::endl;
        return 0;
    }   // end if

    // Get the closest vertices on the model to the contour nodes
    const int uv0 = _vmodel->getClosestObjUniqueVertexIdx( p0);
    const int uv1 = _vmodel->getClosestObjUniqueVertexIdx( p1);

    RFeatures::DijkstraShortestPathFinder* spfinder = new RFeatures::DijkstraShortestPathFinder( _vmodel->getObjectModel());
    spfinder->setEndPointUniqueVertexIndices( uv0, uv1);
    // Get the path vertices (inclusive of uv0 and uv1)
    std::vector<int>* pathVtxs = new std::vector<int>;
    const int pathLen = spfinder->findShortestPath( *pathVtxs);
    delete spfinder;

    int retVal = 0;
    if ( pathLen < 0)
    {
        std::cerr << "[ERROR] SurfacePathInterpolator::InterpolateLine:"
                  << "\n\tNo path found between unqiue vertices " << uv0 << " and " << uv1 << std::endl;
    }   // end if
    else
    {
        std::vector<cv::Vec3f>* posVecs = new std::vector<cv::Vec3f>;
        getSmoothedPathFromVertexIndices( *pathVtxs, p0, p1, *posVecs);

        // Add the intermediate points
        // (Reverse the shortest path (since its actually uv1 to uv0 inclusive))
        double wpos[3];
        for ( int i = pathLen-1; i > 0; --i)
        {
            const cv::Vec3f& v = posVecs->at(i);
            wpos[0] = v[0]; wpos[1] = v[1]; wpos[2] = v[2];
            rep->AddIntermediatePointWorldPosition( n0, wpos);
        }   // end for

        delete posVecs;
        retVal = 1;
    }   // end else

    delete pathVtxs;
    return retVal;
}   // end InterpolateLine


// private
void SurfacePathInterpolator::getSmoothedPathFromVertexIndices( const std::vector<int>& uvidxs,
                                                                double* s0, double* s1, std::vector<cv::Vec3f>& spos) const
{
    const cv::Vec3f p0( s0[0], s0[1], s0[2]);
    const cv::Vec3f p1( s1[0], s1[1], s1[2]);
    const cv::Vec3f baseVec = p1 - p0;

    spos.push_back( p0); // Insert the first actual position in spos.

    const RFeatures::ObjModel::Ptr omodel = _vmodel->getObjectModel();
    const int nverts = uvidxs.size() - 1;   // First and last vertex indices are ignored since the actual positions are used
    for ( int i = 1; i < nverts; ++i)
    {
        const int uvidx = uvidxs.at(i);
        const cv::Vec3f& p = omodel->getUniqueVertex(uvidx);
        const cv::Vec3f t = RFeatures::project( p, baseVec);    // Calculate the projection of p along the base vector
    }   // end for

    spos.push_back( p1); // Insert the last actual position
}   // end getSmoothedPathFromPathIndices

