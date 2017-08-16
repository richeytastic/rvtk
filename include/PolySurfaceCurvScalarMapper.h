#ifndef RVTK_POLY_SURFACE_CURV_SCALAR_MAPPER_H
#define RVTK_POLY_SURFACE_CURV_SCALAR_MAPPER_H

#include "MetricMapper.h"
#include <ObjModelCurvatureMetrics.h>   // RFeatures


namespace RVTK
{

class rVTK_EXPORT PolySurfaceCurvScalarMapper : public PolyMapper
{
public:
    PolySurfaceCurvScalarMapper( const RFeatures::ObjModelCurvatureMetrics::Ptr cm, const std::string& metricName);

    // Curvature metrics generated from provided model by initially parsing the model.
    // Inefficient if curvature metrics already exist. Also, model parsing starts using
    // the first available face ID and is not guaranteed to succeed if the model is
    // composed of many unconnected components.
    PolySurfaceCurvScalarMapper( const RFeatures::ObjModel::Ptr model, const std::string& metricName);

    virtual ~PolySurfaceCurvScalarMapper(){}

    float getMappedRange( float* minv=NULL, float* maxv=NULL) const;

protected:
    const RFeatures::ObjModelCurvatureMetrics::Ptr _cmetrics;
    virtual float getCurvMetric( int faceIdx) const = 0;

private:
    float _maxv, _minv;
    boost::unordered_map<int,float> _cvals;
    virtual float operator()( int faceIdx, int notused);
};   // end class

}   // end namespace

#endif
