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

#ifndef RVTK_METRIC_MAPPER_H
#define RVTK_METRIC_MAPPER_H

#include "rVTK_Export.h"
#include <vtkDataSetAttributes.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkCellData.h>
#include <ObjModel.h>   // RFeatures

namespace RVTK {

class SurfaceMapper;


class rVTK_EXPORT MetricMapper
{
public:
    typedef std::shared_ptr<MetricMapper> Ptr;
protected:
    explicit MetricMapper( size_t nd) : _nd(nd) {}
    virtual const IntSet* mappingIds( const RFeatures::ObjModel*) const = 0;
    virtual vtkDataSetAttributes* dataSet( vtkPolyData*) const = 0;
    size_t ndimensions() const { return _nd;}
    friend class SurfaceMapper;
private:
    const size_t _nd;
    MetricMapper( const MetricMapper&) = delete;
    void operator=( const MetricMapper&) = delete;
};  // end class


class rVTK_EXPORT PolygonMetricMapper : public MetricMapper
{
public:
    static MetricMapper::Ptr create( size_t nd) { return MetricMapper::Ptr( new PolygonMetricMapper(nd));}
protected:
    vtkDataSetAttributes* dataSet( vtkPolyData *pd) const override { return pd->GetCellData();}
    const IntSet* mappingIds( const RFeatures::ObjModel *m) const override { return &m->getFaceIds();}
    explicit PolygonMetricMapper( size_t nd) : MetricMapper(nd) {}
};  // end class


class rVTK_EXPORT VertexMetricMapper : public MetricMapper
{
public:
    static MetricMapper::Ptr create( size_t nd) { return MetricMapper::Ptr( new VertexMetricMapper(nd));}
protected:
    vtkDataSetAttributes* dataSet( vtkPolyData *pd) const override { return pd->GetPointData();}
    const IntSet* mappingIds( const RFeatures::ObjModel *m) const override { return &m->getVertexIds();}
    explicit VertexMetricMapper( size_t nd) : MetricMapper(nd) {}
};  // end class

}   // end namespace

#endif
