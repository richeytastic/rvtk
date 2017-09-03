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

#include <SurfaceMapper.h>
#include <VtkActorCreator.h>
#include <VtkTools.h>
using RVTK::SurfaceMapper;
using RVTK::MetricMapper;
using RVTK::PolygonMetricMapper;
using RVTK::VertexMetricMapper;
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <climits>
using RFeatures::ObjModel;


// public
PolygonMetricMapper::PolygonMetricMapper( size_t nc) : MetricMapper(nc) {}
// protected
vtkDataSetAttributes* PolygonMetricMapper::getDataSet( vtkPolyData* pd) const { return pd->GetCellData();}
const IntSet* PolygonMetricMapper::getMappingIds( const ObjModel::Ptr m) const { return &m->getFaceIds();}
void PolygonMetricMapper::setLookup( RVTK::VtkActorCreator *actorCreator, IntIntMap* lookupMap) const
{
    actorCreator->setObjToVTKUniqueFaceMap( lookupMap);
}   // end setLookup


// public
VertexMetricMapper::VertexMetricMapper( size_t nc) : MetricMapper(nc) {}
// protected
vtkDataSetAttributes* VertexMetricMapper::getDataSet( vtkPolyData* pd) const { return pd->GetPointData();}
const IntSet* VertexMetricMapper::getMappingIds( const ObjModel::Ptr m) const { return &m->getVertexIds();}
void VertexMetricMapper::setLookup( RVTK::VtkActorCreator *actorCreator, IntIntMap* lookupMap) const
{
    actorCreator->setObjToVTKUniqueVertexMap( lookupMap);
}   // end setLookup



// public
SurfaceMapper::SurfaceMapper( const ObjModel::Ptr m, const std::string& metricName, const MetricMapper mmapper)
    : _model(m), _mname(metricName), _mmapper(mmapper)
{
    const size_t nc = _mmapper.getNumComponents();
    _min.resize(nc);
    _max.resize(nc);
    for ( size_t i = 0; i < nc; ++i)
    {
        _min[i] = FLT_MAX;
        _max[i] = -FLT_MAX;
    }   // end for
}   // end ctor

vtkSmartPointer<vtkActor> SurfaceMapper::makeActor() { return _mmapper.createSurfaceActor(this);}


// protected
float SurfaceMapper::operator()( int id, int c)
{
    const float v = getMetric(id,c);
    _min[c] = std::min(v,_min[c]);
    _max[c] = std::max(v,_max[c]);
    return v;
}   // end operator()


std::string SurfaceMapper::getMetricName() const { return _mname;}
int SurfaceMapper::getNumMetricComponents() const { return _mmapper.getNumComponents();}
const ObjModel::Ptr SurfaceMapper::getObject() { return _model;}
float SurfaceMapper::getMin( int c) const { return _min[c];}
float SurfaceMapper::getMax( int c) const { return _max[c];}

