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

#ifndef RVTK_POLY_SURFACE_DISTANCE_SCALAR_MAPPER_H
#define RVTK_POLY_SURFACE_DISTANCE_SCALAR_MAPPER_H

#include "SurfaceMapper.h"


namespace RVTK
{

class rVTK_EXPORT PolySurfaceDistanceScalarMapper : public SurfaceMapper
{
public:
    PolySurfaceDistanceScalarMapper( const RFeatures::ObjModel::Ptr model,
                                     vtkActor* actor,
                                     const IntIntMap* lookupMap,
                                     const boost::unordered_map<int,double>& dvals,
                                     const std::string& metricName);

    float getMappedRange( float* minv=NULL, float* maxv=NULL) const;

private:
    const RFeatures::ObjModel::Ptr _model;
    const boost::unordered_map<int,double>& _dvals;
    virtual float getMetric( int, int);
};   // end class

}   // end namespace

#endif

