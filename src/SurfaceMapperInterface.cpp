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

#include <SurfaceMapperInterface.h>
#include <VtkActorCreator.h>
#include <VtkTools.h>
using RVTK::MetricMapper;
using RVTK::MetricInterface;
#include <vtkPointData.h>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkFloatArray.h>
#include <boost/foreach.hpp>
using RFeatures::ObjModel;


// protected
MetricMapper::MetricMapper( size_t nc) : _numComponents(nc)
{
}   // end ctor


// protected
vtkSmartPointer<vtkActor> MetricMapper::createSurfaceActor( MetricInterface* mint) const
{
    const ObjModel::Ptr model = mint->getObject();
    RVTK::VtkActorCreator actorCreator;
    IntIntMap lookupMap;    // Object index to VTK index (for cells or vertices)
    this->setLookup( &actorCreator, &lookupMap);
    vtkSmartPointer<vtkActor> actor = actorCreator.generateSurfaceActor(model);

    vtkSmartPointer<vtkFloatArray> cvals = vtkSmartPointer<vtkFloatArray>::New();
    cvals->SetName( mint->getMetricName().c_str());

    const int nc = (int)getNumComponents();
    const IntSet& objids = *this->getMappingIds(model);

    if ( nc > 1)
    {
        float *mval = new float[nc];
        cvals->SetNumberOfComponents( nc);
        cvals->SetNumberOfTuples( objids.size());
        BOOST_FOREACH ( int objid, objids)
        {
            for ( int k = 0; k < nc; ++k)
                mval[k] = mint->operator()( objid, k);
            cvals->SetTuple( lookupMap.at(objid), mval);
        }   // end foreach
        delete[] mval;
    }   // end if
    else if ( nc == 1)
    {
        float mval;
        cvals->SetNumberOfValues( objids.size());
        BOOST_FOREACH ( int objid, objids)
        {
            mval = mint->operator()( objid, 0);
            cvals->SetValue( lookupMap.at(objid), mval);
        }   // end foreach
    }   // end else if

    vtkDataSetAttributes *ds = this->getDataSet( RVTK::getPolyData( actor));
    ds->AddArray( cvals);
    if ( nc == 1)
        ds->SetActiveScalars( cvals->GetName());
    else if ( nc > 1)
        ds->SetActiveVectors( cvals->GetName());

    actor->GetProperty()->SetRepresentationToSurface();
    actor->GetMapper()->SetScalarModeToUseCellData();
    actor->GetMapper()->SetScalarVisibility(true);
    return actor;
}   // end createSurfaceActor


// protected
vtkDataSetAttributes* MetricMapper::getDataSet( vtkPolyData*) const { return NULL;}
const IntSet* MetricMapper::getMappingIds( const ObjModel::Ptr) const { return NULL;}
void MetricMapper::setLookup( RVTK::VtkActorCreator*, IntIntMap*) const {}
size_t MetricMapper::getNumComponents() const { return _numComponents;}

