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

#ifndef RVTK_SURFACE_MAPPER_H
#define RVTK_SURFACE_MAPPER_H

#include "SurfaceMapperInterface.h"


namespace RVTK
{

class rVTK_EXPORT SurfaceMapper : public MetricInterface
{
public:
    SurfaceMapper( const RFeatures::ObjModel::Ptr m,
                   const std::string& metricName,
                   const MetricMapper mmapper);    // PolygonMetricMapper -or- VertexMetricMapper

    virtual ~SurfaceMapper(){}

    vtkSmartPointer<vtkActor> makeActor();
    virtual std::string getMetricName() const;

protected:
    virtual float getMetric( int id, int c) = 0;  // Implement me!

    virtual int getNumMetricComponents() const;
    virtual const RFeatures::ObjModel::Ptr getObject();
    virtual float getMin( int c) const;
    virtual float getMax( int c) const;
    virtual float operator()( int id, int c);

private:
    const RFeatures::ObjModel::Ptr _model;
    const std::string _mname;
    const MetricMapper _mmapper;
    std::vector<float> _min, _max;
};  // end class


class rVTK_EXPORT PolygonMetricMapper : public MetricMapper
{
public:
    explicit PolygonMetricMapper( size_t numComponents);
protected:
    virtual vtkDataSetAttributes* getDataSet( vtkPolyData*) const;
    virtual const IntSet* getMappingIds( const RFeatures::ObjModel::Ptr) const;
    virtual void setLookup( VtkActorCreator*, IntIntMap*) const;
};  // end class


class rVTK_EXPORT VertexMetricMapper : public MetricMapper
{
public:
    explicit VertexMetricMapper( size_t numComponents);
protected:
    virtual vtkDataSetAttributes* getDataSet( vtkPolyData* pd) const;
    virtual const IntSet* getMappingIds( const RFeatures::ObjModel::Ptr) const;
    virtual void setLookup( VtkActorCreator*, IntIntMap*) const;
};  // end class

}   // end namespace

#endif
