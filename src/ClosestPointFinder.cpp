#include "ClosestPointFinder.h"
#include "VtkActorCreator.h"
#include "VtkTools.h"
using RVTK::ClosestPointFinder;

// private
void ClosestPointFinder::init( const vtkSmartPointer<vtkActor> actor)
{
    _plocator = vtkSmartPointer<vtkOctreePointLocator>::New();
    _clocator = vtkSmartPointer<vtkCellLocator>::New();
    vtkSmartPointer<vtkPolyData> data = RVTK::getPolyData(actor);
    _plocator->SetDataSet( data);
    _clocator->SetDataSet( data);
    _plocator->BuildLocator();
    _clocator->BuildLocator();
    _sactor = actor;
}   // end init


ClosestPointFinder::ClosestPointFinder( const vtkSmartPointer<vtkActor> actor)
{
    _ruvmappings = NULL;
    _rufmappings = NULL;
    init( actor);
}   // end ctor


ClosestPointFinder::ClosestPointFinder( const RFeatures::ObjModel::Ptr obj) : _obj(obj)
{
    _ruvmappings = new boost::unordered_map<int,int>;
    _rufmappings = new boost::unordered_map<int,int>;
    RVTK::VtkActorCreator acreator;
    acreator.setObjToVTKUniqueVertexRMap( _ruvmappings);
    acreator.setObjToVTKUniqueFaceRMap( _rufmappings);
    init( acreator.generateSurfaceActor( obj));
}   // end ctor


ClosestPointFinder::~ClosestPointFinder()
{
    if ( _ruvmappings)
        delete _ruvmappings;
    if ( _rufmappings)
        delete _rufmappings;
}   // end dtor



int ClosestPointFinder::getClosestCell( const cv::Vec3f& pos, cv::Vec3f* pv, double* sd) const
{
    const double v[3] = {pos[0], pos[1], pos[2]};
    return getClosestCell( v, pv, sd);
}   // end getClosestCell


int ClosestPointFinder::getClosestCell( const double v[3], cv::Vec3f* pv, double* sd) const
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


int ClosestPointFinder::getClosestVertex( const cv::Vec3f& pos) const
{
    double v[3] = {pos[0], pos[1], pos[2]};
    return _plocator->FindClosestPoint( v);
}   // end getClosestVertex


int ClosestPointFinder::getClosestVertex( const double v[3]) const
{
    return _plocator->FindClosestPoint( v);
}   // end getClosestVertex


int ClosestPointFinder::getClosestObjVertexId( const cv::Vec3f& pos) const
{
    const double v[3] = {pos[0], pos[1], pos[2]};
    return getClosestObjVertexId( v);
}   // end getClosestObjVertexId


int ClosestPointFinder::getClosestObjVertexId( const double pos[3]) const
{
    if ( !canReverseMapToObjectIndices())
        return -1;
    const int vtkId = getClosestVertex( pos);;
    return _ruvmappings->at(vtkId);
}   // end getClosestObjVertexId


int ClosestPointFinder::getClosestObjPolyId( const cv::Vec3f& pos, cv::Vec3f* cp, double* sd) const
{
    const double v[3] = {pos[0], pos[1], pos[2]};
    return getClosestObjPolyId( v, cp, sd);
}   // end getClosestObjPolyId


int ClosestPointFinder::getClosestObjPolyId( const double v[3], cv::Vec3f* cp, double* sd) const
{
    if ( !canReverseMapToObjectIndices())
        return -1;
    const int vtkId = getClosestCell( v, cp, sd);
    return _rufmappings->at(vtkId);
}   // end getClosestObjPolyId
