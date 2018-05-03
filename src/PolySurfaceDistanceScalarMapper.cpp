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

#include <PolySurfaceDistanceScalarMapper.h>
#include <algorithm>
using RVTK::PolySurfaceDistanceScalarMapper;
using RFeatures::ObjModel;


// public
PolySurfaceDistanceScalarMapper::PolySurfaceDistanceScalarMapper( const ObjModel::Ptr model,
                                                                  vtkActor* actor,
                                                                  const IntIntMap* lookupMap,
                                                                  const std::unordered_map<int,double>& dvals,
                                                                  const std::string& mname)
    : RVTK::SurfaceMapper( model, actor, lookupMap, mname, RVTK::PolygonMetricMapper::create(1)), _model(model), _dvals(dvals)
{
}   // end ctor


// private
float PolySurfaceDistanceScalarMapper::getMetric( int faceIdx, int notused)
{
    const int* vidxs = _model->getFaceVertices(faceIdx);
    const double v = (_dvals.at(vidxs[0]) + _dvals.at(vidxs[1]) + _dvals.at(vidxs[2]))/3;
    return float(v);
}   // end getMetric


// public
float PolySurfaceDistanceScalarMapper::getMappedRange( float* minv, float* maxv) const
{
    const float lowv = getMin(0);
    const float highv = getMax(0);
    if ( minv)
        *minv = lowv;
    if ( maxv)
        *maxv = highv;
    return highv - lowv;
}   // end getMappedRange


