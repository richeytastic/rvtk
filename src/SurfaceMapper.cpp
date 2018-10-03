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
#include <VtkTools.h>
using RVTK::SurfaceMapper;
using RVTK::MetricFn;
#include <vtkSmartPointer.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <climits>
#include <cassert>
using RFeatures::ObjModel;


SurfaceMapper::CPtr SurfaceMapper::create( const std::string& label, const MetricFn& fn, bool mapPolys, size_t d)
{
    return CPtr( new SurfaceMapper( label, fn, mapPolys, d), [](SurfaceMapper* x){ delete x;});
}   // end create


SurfaceMapper::SurfaceMapper( const std::string& label, const MetricFn& fn, bool mapPolys, size_t d)
    : _label(label), _metricfn(fn), _mapsPolys(mapPolys),
      _mmapper( mapPolys ? RVTK::PolygonMetricMapper::create(d) : RVTK::VertexMetricMapper::create(d))
{
}   // end ctor


// private
float SurfaceMapper::val( int id, size_t k) const
{
    const float v = _metricfn(id,k);
    _min[k] = std::min(v,_min[k]);
    _max[k] = std::max(v,_max[k]);
    return v;
}   // end val


// public
void SurfaceMapper::mapMetrics( const ObjModel *model, const std::unordered_map<int,int> *lmap, vtkActor *actor) const
{
    assert(lmap);
    const size_t nd = ndimensions();
    assert( nd >= 1);

    // Reset the min/max values for this call to mapMetrics.
    _min.resize(nd);
    _max.resize(nd);
    for ( size_t i = 0; i < nd; ++i)
    {
        _min[i] = FLT_MAX;
        _max[i] = -FLT_MAX;
    }   // end for

    vtkSmartPointer<vtkFloatArray> cvals = vtkSmartPointer<vtkFloatArray>::New();
    cvals->SetName( _label.c_str());
    const IntSet& objids = *_mmapper->mappingIds( model);

    if ( nd == 1)  // Scalars
    {
        cvals->SetNumberOfValues( objids.size());
        for ( int objid : objids)
            cvals->SetValue( lmap->at(objid), val( objid, 0));
    }   // end if
    else    // Vectors
    {
        std::vector<float> mval(nd);
        cvals->SetNumberOfComponents( static_cast<int>(nd));
        cvals->SetNumberOfTuples( objids.size());
        for ( int objid : objids)
        {
            for ( size_t k = 0; k < nd; ++k)
                mval[k] = val( objid, k);
            cvals->SetTuple( lmap->at(objid), &mval[0]);
        }   // end for
    }   // end else

    vtkDataSetAttributes *ds = _mmapper->dataSet( RVTK::getPolyData( actor));    // polydata->GetCellData or polydata->GetPointData
    ds->AddArray( cvals);
}   // end mapMetrics
