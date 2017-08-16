#include <PolySurfaceCurvScalarMapper.h>
#include <algorithm>
#include <cfloat>
using RVTK::PolySurfaceCurvScalarMapper;
using RFeatures::ObjModelCurvatureMetrics;
using RFeatures::ObjModelCurvatureMap;
using RFeatures::ObjModel;


// public
PolySurfaceCurvScalarMapper::PolySurfaceCurvScalarMapper( const ObjModelCurvatureMetrics::Ptr cm, const std::string& mname)
    : RVTK::PolyMapper( cm->getObject(), mname, 1), _cmetrics(cm), _maxv(-FLT_MAX), _minv(FLT_MAX)
{
}   // end ctor


// public
PolySurfaceCurvScalarMapper::PolySurfaceCurvScalarMapper( const ObjModel::Ptr model, const std::string& mname)
    : RVTK::PolyMapper( model, mname, 1),
    _cmetrics( ObjModelCurvatureMetrics::create( ObjModelCurvatureMap::create(model, *model->getFaceIds().begin()))),
    _maxv(-FLT_MAX), _minv(FLT_MAX)
{
}   // end ctor


// private
float PolySurfaceCurvScalarMapper::operator()( int faceIdx, int notused)
{
    if ( _cvals.count(faceIdx) == 0)
    {
        const float v = _cvals[faceIdx] = getCurvMetric(faceIdx);
        _maxv = std::max(v,_maxv);
        _minv = std::min(v,_minv);
    }   // end if
    return _cvals.at(faceIdx);
}   // end operator()


// public
float PolySurfaceCurvScalarMapper::getMappedRange( float* minv, float* maxv) const
{
    if ( minv)
        *minv = _minv;
    if ( maxv)
        *maxv = _maxv;
    return _maxv - _minv;
}   // end getMappedRange
