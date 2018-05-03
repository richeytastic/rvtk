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

#ifndef RVTK_POLY_SURFACE_CURV_SCALAR_MAPPER_H
#define RVTK_POLY_SURFACE_CURV_SCALAR_MAPPER_H

#include "SurfaceMapper.h"
#include <ObjModelCurvatureMetrics.h>   // RFeatures


namespace RVTK {

class rVTK_EXPORT PolySurfaceCurvScalarMapper : public SurfaceMapper
{
public:
    PolySurfaceCurvScalarMapper( const RFeatures::ObjModelCurvatureMetrics::Ptr cm,
                                 vtkActor* actor,
                                 const IntIntMap* lookupMap,    // Lookup actor poly IDs from ObjModel face IDs
                                 const std::string& metricName);
    virtual ~PolySurfaceCurvScalarMapper(){}

    float getMappedRange( float* minv=NULL, float* maxv=NULL) const;

protected:
    const RFeatures::ObjModelCurvatureMetrics::Ptr _cmetrics;
    virtual float getCurvMetric( int faceIdx) const = 0;

private:
    std::unordered_map<int,float> _cvals;
    virtual float getMetric( int, int);   // Implements SufaceMapper::getMetric
};   // end class

}   // end namespace

#endif

