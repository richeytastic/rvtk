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
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <ObjModel.h>   // RFeatures
typedef boost::unordered_map<int,int> IntIntMap;


namespace RVTK
{

class MetricMapper;
class VtkActorCreator;

// Don't inherit from directly - use PolygonMapper or VertexMapper and just implement operator().
class rVTK_EXPORT MetricInterface
{
public:
    virtual float operator()( int id, int c=0) = 0;          // Returns component c metric value.

    virtual vtkSmartPointer<vtkActor> makeActor() = 0;       // Implemented in PolyMapper and VertexMapper
    virtual std::string getMetricName() const = 0;           // Implemented in PolyMapper and VertexMapper

protected:
    virtual int getNumMetricComponents() const = 0;          // How many scalars per metric
    virtual const RFeatures::ObjModel::Ptr getObject() = 0;  // Get the object that metrics are mapped against
    friend class MetricMapper;
};  // end class


class rVTK_EXPORT MetricMapper
{
public:
    vtkSmartPointer<vtkActor> createSurfaceActor( MetricInterface* mint);
protected:
    virtual vtkDataSetAttributes* getDataSet( vtkPolyData* pd) const = 0;
    virtual const IntSet& getMappingsIds( const RFeatures::ObjModel::Ptr& model) const = 0;
    virtual void setLookup( VtkActorCreator*, IntIntMap*) const = 0;
    MetricMapper();
};  // end class


class rVTK_EXPORT PolygonMetricMapper : public MetricMapper
{
protected:
    virtual vtkDataSetAttributes* getDataSet( vtkPolyData* pd) const { return pd->GetCellData();}
    virtual const IntSet& getMappingsIds( const RFeatures::ObjModel::Ptr& m) const { return m->getFaceIds();}
    virtual void setLookup( VtkActorCreator*, IntIntMap*) const;
};  // end class


class rVTK_EXPORT VertexMetricMapper : public MetricMapper
{
protected:
    virtual vtkDataSetAttributes* getDataSet( vtkPolyData* pd) const { return pd->GetPointData();}
    virtual const IntSet& getMappingsIds( const RFeatures::ObjModel::Ptr& m) const { return m->getVertexIds();}
    virtual void setLookup( VtkActorCreator*, IntIntMap*) const;
};  // end class


class rVTK_EXPORT PolyMapper : public MetricInterface
{
public:
    PolyMapper( const RFeatures::ObjModel::Ptr m, const std::string& metricName, int nComponents)
        : _model(m), _mname(metricName), _nComponents(nComponents) {}

    vtkSmartPointer<vtkActor> makeActor() { return _mmapper.createSurfaceActor(this);}

protected:
    virtual std::string getMetricName() const { return _mname;}
    virtual int getNumMetricComponents() const { return _nComponents;}
    virtual const RFeatures::ObjModel::Ptr getObject() { return _model;}

private:
    const RFeatures::ObjModel::Ptr _model;
    const std::string _mname;
    int _nComponents;
    PolygonMetricMapper _mmapper;
};  // end class


class rVTK_EXPORT VertexMapper : public MetricInterface
{
public:
    VertexMapper( const RFeatures::ObjModel::Ptr m, const std::string& metricName, int nComponents)
        : _model(m), _mname(metricName), _nComponents(nComponents) {}

    vtkSmartPointer<vtkActor> makeActor() { return _mmapper.createSurfaceActor(this);}

protected:
    virtual std::string getMetricName() const { return _mname;}
    virtual int getNumMetricComponents() const { return _nComponents;}
    virtual const RFeatures::ObjModel::Ptr getObject() { return _model;}

private:
    const RFeatures::ObjModel::Ptr _model;
    const std::string _mname;
    int _nComponents;
    VertexMetricMapper _mmapper;
};  // end class


}   // end namespace

#endif
