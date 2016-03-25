#include "ClosestPointFinder.h"
using RVTK::ClosestPointFinder;


RVTK::ClosestPointFinder::ClosestPointFinder( const vtkSmartPointer<vtkPolyData>& data)
    : _plocator( vtkSmartPointer<vtkOctreePointLocator>::New()),
      _clocator( vtkSmartPointer<vtkCellLocator>::New())
{
    _plocator->SetDataSet( data);
    _clocator->SetDataSet( data);
    _plocator->BuildLocator();
    _clocator->BuildLocator();
}   // end ctor


int RVTK::ClosestPointFinder::getClosestCell( const cv::Vec3f& pos, cv::Vec3f* pv, double* sd) const
{
    const double v[3] = {pos[0], pos[1], pos[2]};
    return getClosestCell( v, pv, sd);
}   // end getClosestCell


int RVTK::ClosestPointFinder::getClosestCell( const double v[3], cv::Vec3f* pv, double* sd) const
{
    double cp[3];   // closest point
    vtkIdType cellId;
    double sqdist;
    int subid;

    _clocator->FindClosestPoint( const_cast<double*>(v), cp, cellId, subid, sqdist);

    if ( pv)
    {
        (*pv)[0] = float(cp[0]);
        (*pv)[1] = float(cp[1]);
        (*pv)[2] = float(cp[2]);
    }   // end if

    if ( sd)
        *sd = sqdist;

    return cellId;
}   // end getClosestCell


int RVTK::ClosestPointFinder::getClosestVertex( const cv::Vec3f& pos) const
{
    double v[3] = {pos[0], pos[1], pos[2]};
    return _plocator->FindClosestPoint( v);
}   // end getClosestVertex


int RVTK::ClosestPointFinder::getClosestVertex( const double v[3]) const
{
    return _plocator->FindClosestPoint( v);
}   // end getClosestVertex
