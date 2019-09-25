/************************************************************************
 * Copyright (C) 2019 Richard Palmer
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
    : _label(label), _metricfn(fn), _mapsPolys(mapPolys), _ndims(d) {}


// private
float SurfaceMapper::val( int id, size_t k) const
{
    const float v = _metricfn(id,k);
    _min[k] = std::min(v,_min[k]);
    _max[k] = std::max(v,_max[k]);
    return v;
}   // end val


// public
void SurfaceMapper::mapMetrics( const ObjModel& model, vtkActor *actor) const
{
    assert( model.hasSequentialIds());
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

    const int nf = model.numPolys();
    const int nv = model.numVtxs();

    if ( nd == 1)  // Scalars
    {
        if ( _mapsPolys)
        {
            cvals->SetNumberOfValues( nf);
            for ( int fid = 0; fid < nf; ++fid)
                cvals->SetValue( fid, val( fid, 0));
        }   // end if
        else
        {
            // For vertex mapping, depending on how the actor's polydata have been created, there could be the
            // same number of points as there are vertices in the model (if texture mapping was not done) or
            // there could be three times the number of triangles (if texture mapping was done). In the first
            // case, setting the value is a straight forward one-to-one mapping. However, in the second case,
            // vertices that share faces will be duplicated and so each unique vertex metric value needs to
            // be mapped to all of these duplicate corresponding points in the array.
            const int np = RVTK::getPolyData(actor)->GetPoints()->GetNumberOfPoints();
            const bool hasDups = np == 3*nf;
            /*
            std::cerr << "numVtxs  = " << nv << std::endl;
            std::cerr << "numPolys = " << nf << std::endl;
            std::cerr << "3*NF     = " << 3*nf << std::endl;
            */

            if ( !hasDups)
            {
                cvals->SetNumberOfValues( nv);
                for ( int vid = 0; vid < nv; ++vid)
                    cvals->SetValue( vid, val( vid, 0));    // One-to-one mapping
            }   // end if
            else
            {
                // Map of vertex ids to their values for local caching since we don't want
                // to ask the client to repeatedly calculate the same metric for a vertex.
                std::unordered_map<int,float> vmap;
                int i = 0;
                cvals->SetNumberOfValues( 3*nf);
                for ( int fid = 0; fid < nf; ++fid)
                {
                    const int* fvidxs = model.fvidxs(fid);
                    for ( int j = 0; j < 3; ++j)
                    {
                        const int vid = fvidxs[j];
                        if ( vmap.count(vid) == 0)
                            vmap[vid] = val( vid, 0);
                        cvals->SetValue( i++, vmap[vid]);
                    }   // end for
                }   // end for
            }   // end else
        }   // end else
    }   // end if
    else    // Vectors
    {
        std::vector<float> mval(nd);
        cvals->SetNumberOfComponents( static_cast<int>(nd));
        
        if ( _mapsPolys)
        {
            cvals->SetNumberOfTuples( nf);
            for ( int fid = 0; fid < nf; ++fid)
            {
                for ( size_t k = 0; k < nd; ++k)
                    mval[k] = val( fid, k);
                cvals->SetTuple( fid, &mval[0]);
            }   // end for
        }   // end if
        else
        {
            const bool hasDups = RVTK::getPolyData(actor)->GetPoints()->GetNumberOfPoints() == 3*nf;

            if ( !hasDups)
            {
                cvals->SetNumberOfTuples( nv);
                for ( int vid = 0; vid < nv; ++vid)
                {
                    for ( size_t k = 0; k < nd; ++k)
                        mval[k] = val( vid, k);
                    cvals->SetTuple( vid, &mval[0]);
                }   // end for
            }   // end if
            else
            {
                std::unordered_map<int, std::vector<float> > vmap;
                int i = 0;
                cvals->SetNumberOfTuples( 3*nf);
                for ( int fid = 0; fid < nf; ++fid)
                {
                    const int* fvidxs = model.fvidxs(fid);
                    for ( int j = 0; j < 3; ++j)
                    {
                        const int vid = fvidxs[j];
                        if ( vmap.count(vid) == 0)
                        {
                            for ( size_t k = 0; k < nd; ++k)
                                vmap[vid][k] = val( vid, k);
                        }   // end if
                        cvals->SetTuple( i++, &vmap[vid][0]);
                    }   // end for
                }   // end for
            }   // end else
        }   // end else
    }   // end else

    vtkPolyData* pd = RVTK::getPolyData(actor);
    vtkDataSetAttributes *ds = _mapsPolys ? (vtkDataSetAttributes*)pd->GetCellData() : (vtkDataSetAttributes*)pd->GetPointData();
    ds->AddArray( cvals);
}   // end mapMetrics
