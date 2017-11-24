#include <CurvatureSurfaceMapper.h>
using RVTK::CurvatureSurfaceMapper;
using RVTK::PolySurfaceCurvScalarMapper;

namespace
{

class KP1CurvMapper : public PolySurfaceCurvScalarMapper
{
public:
    KP1CurvMapper( const RFeatures::ObjModelCurvatureMetrics::Ptr cm,
                   vtkActor* actor, const IntIntMap* lmap, const std::string& metricName)
        : PolySurfaceCurvScalarMapper(cm, actor, lmap, metricName){}
protected:
    virtual float getCurvMetric( int fid) const { return _cmetrics->getFaceKP1FirstOrder(fid);}
};  // end class


class KP1FirstCurvMapper : public PolySurfaceCurvScalarMapper
{
public:
    KP1FirstCurvMapper( const RFeatures::ObjModelCurvatureMetrics::Ptr cm,
                   vtkActor* actor, const IntIntMap* lmap, const std::string& metricName)
        : PolySurfaceCurvScalarMapper(cm, actor, lmap, metricName){}
protected:
    virtual float getCurvMetric( int fid) const { return _cmetrics->getFaceKP1SecondOrder(fid);}
};  // end class


class KP2CurvMapper : public PolySurfaceCurvScalarMapper
{
public:
    KP2CurvMapper( const RFeatures::ObjModelCurvatureMetrics::Ptr cm,
                   vtkActor* actor, const IntIntMap* lmap, const std::string& metricName)
        : PolySurfaceCurvScalarMapper(cm, actor, lmap, metricName){}
protected:
    virtual float getCurvMetric( int fid) const { return _cmetrics->getFaceKP2FirstOrder(fid);}
};  // end class


class KP2FirstCurvMapper : public PolySurfaceCurvScalarMapper
{
public:
    KP2FirstCurvMapper( const RFeatures::ObjModelCurvatureMetrics::Ptr cm,
                   vtkActor* actor, const IntIntMap* lmap, const std::string& metricName)
        : PolySurfaceCurvScalarMapper(cm, actor, lmap, metricName){}
protected:
    virtual float getCurvMetric( int fid) const { return _cmetrics->getFaceKP2SecondOrder(fid);}
};  // end class


class AbsCurvMapper : public PolySurfaceCurvScalarMapper
{
public:
    AbsCurvMapper( const RFeatures::ObjModelCurvatureMetrics::Ptr cm,
                   vtkActor* actor, const IntIntMap* lmap, const std::string& metricName)
        : PolySurfaceCurvScalarMapper(cm, actor, lmap, metricName){}
protected:
    virtual float getCurvMetric( int fid) const {
        return sqrt( (pow(_cmetrics->getFaceKP2FirstOrder(fid),2) + pow(_cmetrics->getFaceKP1FirstOrder(fid),2)) / 2);}
};  // end class


class MeanCurvMapper : public PolySurfaceCurvScalarMapper
{
public:
    MeanCurvMapper( const RFeatures::ObjModelCurvatureMetrics::Ptr cm,
                    vtkActor* actor, const IntIntMap* lmap, const std::string& metricName)
        : PolySurfaceCurvScalarMapper(cm, actor, lmap, metricName){}
protected:
    virtual float getCurvMetric( int fid) const {
        return (_cmetrics->getFaceKP2FirstOrder(fid) + _cmetrics->getFaceKP1FirstOrder(fid)) / 2;}
};  // end class


class DeterminantCurvMapper : public PolySurfaceCurvScalarMapper
{
public:
    DeterminantCurvMapper( const RFeatures::ObjModelCurvatureMetrics::Ptr cm,
                    vtkActor* actor, const IntIntMap* lmap, const std::string& metricName)
        : PolySurfaceCurvScalarMapper(cm, actor, lmap, metricName){}
protected:
    virtual float getCurvMetric( int fid) const { return _cmetrics->getFaceDeterminant(fid);}
};  // end class


class GaussianCurvMapper : public PolySurfaceCurvScalarMapper
{
public:
    GaussianCurvMapper( const RFeatures::ObjModelCurvatureMetrics::Ptr cm,
                    vtkActor* actor, const IntIntMap* lmap, const std::string& metricName)
        : PolySurfaceCurvScalarMapper(cm, actor, lmap, metricName){}
protected:
    virtual float getCurvMetric( int fid) const { return _cmetrics->getFaceKP1FirstOrder(fid) * _cmetrics->getFaceKP2FirstOrder(fid);}
};  // end class


std::pair<float,float> map( PolySurfaceCurvScalarMapper& mapper)
{
    mapper.mapActor();
    float lv, hv;
    mapper.getMappedRange( &lv, &hv);
    return std::pair<float,float>(lv,hv);
}   // end map

}   // end namespace


// public
CurvatureSurfaceMapper::CurvatureSurfaceMapper( RFeatures::ObjModelCurvatureMetrics::Ptr cmetrics,
                                                vtkActor* prop, const IntIntMap* lmap)
    : _cmetrics(cmetrics), _actor(prop), _lmap(lmap)
{
}   // end ctor


std::pair<float,float> CurvatureSurfaceMapper::mapKP1( const std::string& mname)
{
    KP1CurvMapper mapper( _cmetrics, _actor, _lmap, mname);
    return map( mapper);
}   // end mapKP1


std::pair<float,float> CurvatureSurfaceMapper::mapKP1First( const std::string& mname)
{
    KP1FirstCurvMapper mapper( _cmetrics, _actor, _lmap, mname);
    return map( mapper);
}   // end mapKP1First


std::pair<float,float> CurvatureSurfaceMapper::mapKP2( const std::string& mname)
{
    KP2CurvMapper mapper( _cmetrics, _actor, _lmap, mname);
    return map( mapper);
}   // end mapKP2


std::pair<float,float> CurvatureSurfaceMapper::mapKP2First( const std::string& mname)
{
    KP2FirstCurvMapper mapper( _cmetrics, _actor, _lmap, mname);
    return map( mapper);
}   // end mapKP2First


std::pair<float,float> CurvatureSurfaceMapper::mapAbs( const std::string& mname)
{
    AbsCurvMapper mapper( _cmetrics, _actor, _lmap, mname);
    return map( mapper);
}   // end mapAbs


std::pair<float,float> CurvatureSurfaceMapper::mapMean( const std::string& mname)
{
    MeanCurvMapper mapper( _cmetrics, _actor, _lmap, mname);
    return map( mapper);
}   // end mapMean


std::pair<float,float> CurvatureSurfaceMapper::mapDeterminant( const std::string& mname)
{
    DeterminantCurvMapper mapper( _cmetrics, _actor, _lmap, mname);
    return map( mapper);
}   // end mapDeterminant


std::pair<float,float> CurvatureSurfaceMapper::mapGaussian( const std::string& mname)
{
    GaussianCurvMapper mapper( _cmetrics, _actor, _lmap, mname);
    return map( mapper);
}   // end mapGaussian

