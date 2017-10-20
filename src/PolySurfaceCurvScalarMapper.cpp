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

#include <PolySurfaceCurvScalarMapper.h>
using RVTK::PolySurfaceCurvScalarMapper;
using RFeatures::ObjModelCurvatureMetrics;
using RFeatures::ObjModelCurvatureMap;
using RFeatures::ObjModel;


// public
PolySurfaceCurvScalarMapper::PolySurfaceCurvScalarMapper( const ObjModelCurvatureMetrics::Ptr cm,
                                                          vtkSmartPointer<vtkActor> actor,
                                                          const IntIntMap* lookupMap,
                                                          const std::string& mname)
    : RVTK::SurfaceMapper( cm->getObject(), actor, lookupMap, mname, RVTK::PolygonMetricMapper::create(1)), _cmetrics(cm)
{
}   // end ctor


// private
float PolySurfaceCurvScalarMapper::getMetric( int faceIdx, int notused)
{
    if ( _cvals.count(faceIdx) == 0)
        _cvals[faceIdx] = getCurvMetric(faceIdx);
    return _cvals.at(faceIdx);
}   // end getMetric


// public
float PolySurfaceCurvScalarMapper::getMappedRange( float* minv, float* maxv) const
{
    const float lowv = getMin(0);
    const float highv = getMax(0);
    if ( minv)
        *minv = lowv;
    if ( maxv)
        *maxv = highv;
    return highv - lowv;
}   // end getMappedRange

