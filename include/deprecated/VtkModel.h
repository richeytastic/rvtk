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

#ifndef RVTK_VTK_MODEL_H
#define RVTK_VTK_MODEL_H

#include <ObjModel.h>   // RFeatures
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include "rVTK_Export.h"
#include <boost/unordered_set.hpp>
typedef boost::unordered_set<int> IntSet;

namespace RVTK
{

class rVTK_EXPORT ClosestPointFinder;

class rVTK_EXPORT VtkModel
{
public:
    typedef boost::shared_ptr<VtkModel> Ptr;
    static Ptr create( const RFeatures::ObjModel::Ptr model);

    const RFeatures::ObjModel::Ptr getObjectModel() const { return _omodel;}
    const vtkSmartPointer<vtkActor> getSurfaceActor() const { return _surfaceActor;}
    const size_t getNumTexturedActors() const { return _texturedActors.size();}
    const vtkSmartPointer<vtkActor> getTexturedActor( int i) const { return _texturedActors.at(i);}

    int getClosestObjUniqueVertexIdx( const cv::Vec3f&) const;
    int getClosestObjUniqueVertexIdx( const double p[3]) const;
    int getClosestObjFaceIdx( const cv::Vec3f&) const;
    int getClosestObjFaceIdx( const double p[3]) const;

    // Get the closest point in space on the model (not necessarily a vertex).
    cv::Vec3f getClosestPoint( const cv::Vec3f&) const;

    // Straight up mapping to get the VTK basic surface face index
    // (i.e. not a texture polygon) from an object face index.
    int getVtkBasicIdxFromObjFaceIdx( int objFaceIdx) const;
    int getVtkIdxFromObjUniqueVtx( int objUvidx) const;

    int getObjIdxFromVtkUniqueVtx( int vtkUvidx) const;
    int getObjIdxFromVtkUniqueFace( int vtkFaceId) const;

    const IntSet& getVtkUniqueVertexIds() const { return _uvidxs;}
    const IntSet& getVtkUniqueFaceIds() const { return _ufidxs;}

    ~VtkModel();
private:
    const RFeatures::ObjModel::Ptr _omodel;
    vtkSmartPointer<vtkActor> _surfaceActor;
    std::vector< vtkSmartPointer<vtkActor> > _texturedActors;

    ClosestPointFinder* _unqcpf;    // Closest point finder for unique vertices

    IntSet _uvidxs;  // VTK unique vertex (surface model) IDs
    IntSet _ufidxs;  // VTK unique face (surface model) IDs

    // RFeatures::Object to VTK unique indices
    boost::unordered_map<int, int> _uvmappings, _ufmappings;
    // Reverse mappings from VTK to RFeatures::Object indices (for lookup)
    boost::unordered_map<int, int> _ruvmappings, _rufmappings;

    explicit VtkModel( const RFeatures::ObjModel::Ptr model);
    VtkModel( const VtkModel&); // No copy
    void operator=( const VtkModel&);   // No copy
};  // end class

}   // end namespace

#endif

