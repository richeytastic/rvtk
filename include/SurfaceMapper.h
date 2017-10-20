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
    // Will create a new vtkActor from the given model. This method is not prefered (though is simpler)
    // because the lookup map allowing further value mappings of the ObjModel to the vtkActor is lost.
    SurfaceMapper( const RFeatures::ObjModel::Ptr m,
                   const std::string& metricName,
                   const MetricMapper::Ptr mmapper);    // PolygonMetricMapper -or- VertexMetricMapper

    // Use existing actor - also requires existing lookup map (saved from RVTK::VtkActorCreator).
    SurfaceMapper( const RFeatures::ObjModel::Ptr m,
                   vtkSmartPointer<vtkActor> actor,     // Actor to which a new data array will be added
                   const IntIntMap* lookupMap,          // Lookup map to match poly or vertex metric mapper
                   const std::string& metricName,
                   const MetricMapper::Ptr mmapper);    // PolygonMetricMapper -or- VertexMetricMapper

    virtual ~SurfaceMapper(){}

    virtual std::string getMetricName() const;

    // Creates a new actor in case of first constructor, returns existing actor in case of second constructor.
    // In both cases, the returned actor has a new array added to its data set attributes made the
    // currently active dataset. Given the actor's polydata, get its dataset attributes using:
    // da = polydata->GetCellData() (PolygonMetricMapper), or da = polydata->GetPointData (VertexMetricMapper).
    // Then, set the appropriate scalar or vector mapping using:
    // da->SetActiveScalars( metricName.c_str()) (for MetricMappers created with  1 component), or
    // da->SetActiveVectors( metricName.c_str()) (for MetricMappers created with >1 components).
    // Also, ensure that the following are set as needed:
    // actor->GetProperty()->SetRepresentationToSurface() (obviously)
    // actor->GetMapper()->SetScalarModelToUseCellData()
    // actor->GetMapper()->SetScalarVisibility(true)
    virtual vtkSmartPointer<vtkActor> mapActor();

    virtual float getMin( int c) const;
    virtual float getMax( int c) const;
    virtual int getNumMetricComponents() const;

protected:
    virtual float getMetric( int id, int c) = 0;  // Implement me!

private:
    const RFeatures::ObjModel::Ptr _model;
    const std::string _mname;
    const MetricMapper::Ptr _mmapper;
    vtkSmartPointer<vtkActor> _actor;
    const IntIntMap *_lmap;
    IntIntMap _clookup; // For creating new actors
    std::vector<float> _min, _max;
    void init();
    virtual float val( int id, int c);
};  // end class


class rVTK_EXPORT PolygonMetricMapper : public MetricMapper
{
public:
    static MetricMapper::Ptr create( size_t numComponents);
protected:
    virtual vtkDataSetAttributes* getDataSet( vtkPolyData*) const;
    virtual const IntSet* getMappingIds( const RFeatures::ObjModel::Ptr) const;
    virtual void setLookupMap( VtkActorCreator*, IntIntMap*) const;
    explicit PolygonMetricMapper( size_t numComponents);
};  // end class


class rVTK_EXPORT VertexMetricMapper : public MetricMapper
{
public:
    static MetricMapper::Ptr create( size_t numComponents);
protected:
    virtual vtkDataSetAttributes* getDataSet( vtkPolyData* pd) const;
    virtual const IntSet* getMappingIds( const RFeatures::ObjModel::Ptr) const;
    virtual void setLookupMap( VtkActorCreator*, IntIntMap*) const;
    explicit VertexMetricMapper( size_t numComponents);
};  // end class

}   // end namespace

#endif
