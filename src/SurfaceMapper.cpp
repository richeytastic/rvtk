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
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <climits>
#include <cassert>
#include <boost/foreach.hpp>
using RFeatures::ObjModel;

// public
MetricMapper::Ptr PolygonMetricMapper::create( size_t nc) { return MetricMapper::Ptr( new PolygonMetricMapper(nc)); }
// protected
PolygonMetricMapper::PolygonMetricMapper( size_t nc) : MetricMapper(nc) {}
vtkDataSetAttributes* PolygonMetricMapper::getDataSet( vtkPolyData* pd) const { return pd->GetCellData();}
const IntSet* PolygonMetricMapper::getMappingIds( const ObjModel::Ptr m) const { return &m->getFaceIds();}
void PolygonMetricMapper::setLookupMap( RVTK::VtkActorCreator* ac, IntIntMap* lmap) const { ac->setObjToVTKUniqueFaceMap( lmap);}


// public
MetricMapper::Ptr VertexMetricMapper::create( size_t nc) { return MetricMapper::Ptr( new VertexMetricMapper(nc)); }
// protected
VertexMetricMapper::VertexMetricMapper( size_t nc) : MetricMapper(nc) {}
vtkDataSetAttributes* VertexMetricMapper::getDataSet( vtkPolyData* pd) const { return pd->GetPointData();}
const IntSet* VertexMetricMapper::getMappingIds( const ObjModel::Ptr m) const { return &m->getVertexIds();}
void VertexMetricMapper::setLookupMap( RVTK::VtkActorCreator* ac, IntIntMap* lmap) const { ac->setObjToVTKUniqueVertexMap( lmap);}


// private
void SurfaceMapper::init()
{
    const size_t nc = _mmapper->getNumComponents();
    _min.resize(nc);
    _max.resize(nc);
    for ( size_t i = 0; i < nc; ++i)
    {
        _min[i] = FLT_MAX;
        _max[i] = -FLT_MAX;
    }   // end for
}   // end init


// public
SurfaceMapper::SurfaceMapper( const ObjModel::Ptr m,
                              const std::string& metricName, const MetricMapper::Ptr mmapper)
    : _model(m), _mname(metricName), _mmapper(mmapper)
{
    init();
    // Make the actor
    RVTK::VtkActorCreator actorCreator;
    _mmapper->setLookupMap( &actorCreator, &_clookup);
    _actor = actorCreator.generateSurfaceActor( m);
    _lmap = &_clookup;
}   // end ctor


// public
SurfaceMapper::SurfaceMapper( const ObjModel::Ptr m, vtkSmartPointer<vtkActor> actor, const IntIntMap* lmap,
                              const std::string& metricName, const MetricMapper::Ptr mmapper)
    : _model(m), _mname(metricName), _mmapper(mmapper), _actor(actor), _lmap(lmap)
{
    init();
    assert( lmap != NULL);
    assert( actor != NULL);
}   // end ctor


// public
float SurfaceMapper::getMin( int c) const { return _min[c];}
float SurfaceMapper::getMax( int c) const { return _max[c];}
int SurfaceMapper::getNumMetricComponents() const { return _mmapper->getNumComponents();}


// protected
std::string SurfaceMapper::getMetricName() const { return _mname;}


// protected
float SurfaceMapper::val( int id, int c)
{
    const float v = getMetric(id,c);
    _min[c] = std::min(v,_min[c]);
    _max[c] = std::max(v,_max[c]);
    return v;
}   // end val


// public
vtkSmartPointer<vtkActor> SurfaceMapper::mapActor()
{
    vtkSmartPointer<vtkFloatArray> cvals = vtkSmartPointer<vtkFloatArray>::New();
    cvals->SetName( _mname.c_str());
    const int nc = _mmapper->getNumComponents();
    const IntSet& objids = *_mmapper->getMappingIds( _model);

    if ( nc > 1)    // Vectors
    {
        float *mval = new float[nc];
        cvals->SetNumberOfComponents( nc);
        cvals->SetNumberOfTuples( objids.size());
        BOOST_FOREACH ( int objid, objids)
        {
            for ( int k = 0; k < nc; ++k)
                mval[k] = val( objid, k);
            cvals->SetTuple( _lmap->at(objid), mval);
        }   // end foreach
        delete[] mval;
    }   // end if
    else if ( nc == 1)  // Scalars
    {
        float mval;
        cvals->SetNumberOfValues( objids.size());
        BOOST_FOREACH ( int objid, objids)
        {
            mval = val( objid, 0);
            cvals->SetValue( _lmap->at(objid), mval);
        }   // end foreach
    }   // end else if

    vtkDataSetAttributes *ds = _mmapper->getDataSet( RVTK::getPolyData( _actor));    // polydata->GetCellData or polydata->GetPointData
    ds->AddArray( cvals);
    if ( nc == 1)
        ds->SetActiveScalars( cvals->GetName());
    else if ( nc > 1)
        ds->SetActiveVectors( cvals->GetName());
    return _actor;
}   // end mapActor
