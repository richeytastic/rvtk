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

#ifndef RVTK_CLOSEST_POINT_FINDER_H
#define RVTK_CLOSEST_POINT_FINDER_H

#include <ObjModel.h>   // RFeatures
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkOctreePointLocator.h>
#include <vtkCellLocator.h>
#include <boost/unordered_map.hpp>
#include "rVTK_Export.h"

namespace RVTK
{

class rVTK_EXPORT ClosestPointFinder
{
public:
    explicit ClosestPointFinder( const vtkSmartPointer<vtkActor>);
    explicit ClosestPointFinder( const RFeatures::ObjModel::Ptr);
    ~ClosestPointFinder();

    const vtkSmartPointer<vtkActor> getSurfaceActor() const { return _sactor;}

    // Returns VTK index of closest vertex (i.e., from the poly data)
    int getClosestVertex( const cv::Vec3f& pos) const;
    int getClosestVertex( const double pos[3]) const;

    // Returns VTK index of closest cell. Also sets the closest
    // point and the squared distance to the closest point if not NULL.
    int getClosestCell( const cv::Vec3f& pos, cv::Vec3f* closestPoint=NULL, double* squaredDistance=NULL) const;
    int getClosestCell( const double pos[3], cv::Vec3f* closestPoint=NULL, double* squaredDistance=NULL) const;

    // Returns true if constructed using the RFeatures::ObjModel constructor.
    // In this case, the following functions can be used to lookup the object model's
    // unique vertex and polygon (ObjPoly) IDs.
    bool canReverseMapToObjectIndices() const { return _obj != NULL;}
    const RFeatures::ObjModel::Ptr& getObject() const { return _obj;}    // Invalid if not constructed using an ObjModel.

    // Returns the vertex ID of the ObjModel vertex closest to the given position.
    // If not constructed using the ObjModel ctor, returns -1.
    int getClosestObjVertexId( const cv::Vec3f& pos) const;
    int getClosestObjVertexId( const double pos[3]) const;

    int getClosestObjPolyId( const cv::Vec3f& pos, cv::Vec3f* closestPoint=NULL, double* squaredDistance=NULL) const;
    int getClosestObjPolyId( const double pos[3], cv::Vec3f* closestPoint=NULL, double* squaredDistance=NULL) const;

private:
    const RFeatures::ObjModel::Ptr _obj;
    boost::unordered_map<int, int> *_ruvmappings;    // Reverse mapped VTK to ObjModel unique vertex IDs
    boost::unordered_map<int, int> *_rufmappings;
    vtkSmartPointer<vtkOctreePointLocator> _plocator;
    vtkSmartPointer<vtkCellLocator> _clocator;
    vtkSmartPointer<vtkActor> _sactor;

    void init( const vtkSmartPointer<vtkActor>);
};   // end class

}   // end namespace

#endif
