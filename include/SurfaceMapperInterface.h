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

#ifndef RVTK_SURFACE_MAPPER_INTERFACE_H
#define RVTK_SURFACE_MAPPER_INTERFACE_H

#include "rVTK_Export.h"
#include <vtkDataSetAttributes.h>
#include <vtkPolyData.h>
#include <vtkCellData.h>
#include <vtkActor.h>
#include <ObjModel.h>   // RFeatures
typedef boost::unordered_map<int,int> IntIntMap;


namespace RVTK
{

class MetricMapper;

// Don't inherit from directly - use SurfaceMapper.
class rVTK_EXPORT MetricInterface
{
public:
    virtual void mapActor() = 0;
    virtual std::string getMetricName() const = 0;

    virtual float getMin( int) const = 0;
    virtual float getMax( int) const = 0;
    virtual int getNumMetricComponents() const = 0;

protected:
    virtual float val( int, int) = 0;
    friend class MetricMapper;
};  // end class


class VtkActorCreator;
class SurfaceMapper;

class rVTK_EXPORT MetricMapper
{
public:
    typedef boost::shared_ptr<MetricMapper> Ptr;
protected:
    explicit MetricMapper( size_t nc) : _nc(nc) {}
    virtual const IntSet* getMappingIds( const RFeatures::ObjModel::Ptr) const { return NULL;}
    virtual vtkDataSetAttributes* getDataSet( vtkPolyData*) const { return NULL;}
    virtual void setLookupMap( VtkActorCreator*, IntIntMap*) const {}
    size_t getNumComponents() const { return _nc;}
    friend class SurfaceMapper;
private:
    const size_t _nc;
    MetricMapper( const MetricMapper&);     // No copy
    void operator=( const MetricMapper&);   // No copy
};  // end class


}   // end namespace

#endif
