/**
 * Create different VTK Actors from an ObjModel supporting
 * the mapping of the different vertex and face indices to VTK.
 *
 * Richard Palmer
 * August 2014
 */

#pragma once
#ifndef RVTK_VTK_ACTOR_FACTORY_H
#define RVTK_VTK_ACTOR_FACTORY_H

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <ObjModel.h>
#include "VTKTypes.h"
typedef unsigned char byte;
#include "VtkTools.h"
#include "rVTK_Export.h"


namespace RVTK
{

class rVTK_EXPORT VtkActorFactory
{
public:
    static void init();

    explicit VtkActorFactory( const RFeatures::ObjModel::Ptr&);

    const RFeatures::ObjModel::Ptr getObjectModel() const { return _omodel;}

    vtkSmartPointer<vtkActor> generateBasicActor();
    vtkSmartPointer<vtkActor> generateTexturedActor();

    // Given found indices of vertices or faces (such as might be returned by
    // the RVTK::ClosestPointFinder), return the object index that maps to the VTK index.
    int getObjIdxFromVtkUniqueVtx( int vtkUvidx) const;
    int getObjIdxFromVtkVtx( int vtkUvidx) const;
    int getObjIdxFromVtkUniqueFace( int vtkFaceId) const;
    int getObjIdxFromVtkFace( int vtkFaceId) const;

    int getVtkBasicIdxFromObjFaceIdx( int objFaceId) const;
    int getVtkTextureIdxFromObjFaceIdx( int objFaceId) const;
    int getVtkIdxFromObjUniqueVtx( int objUvidx) const;

    const boost::unordered_set<int>& getVtkUniqueVertexIds() const { return _uvidxs;}
    const boost::unordered_set<int>& getVtkUniqueFaceIds() const { return _ufidxs;}

private:
    const RFeatures::ObjModel::Ptr _omodel;
    boost::unordered_map<int, int> _uvmappings, _vmappings, _ufmappings, _fmappings; // Obj to VTK unique indices
    // Reverse mappings from VTK to obj indices (for lookup)
    boost::unordered_map<int, int> _ruvmappings, _rvmappings, _rufmappings, _rfmappings;

    boost::unordered_set<int> _uvidxs;  // VTK unique vertex (surface model) IDs
    boost::unordered_set<int> _ufidxs;  // VTK unique face (surface model) IDs

    vtkSmartPointer<vtkPoints> generateUniqueVertices();
    vtkSmartPointer<vtkCellArray> generateUniqueVertexFaces();
    vtkSmartPointer<vtkPoints> generateNonUniqueVertices( vtkSmartPointer<vtkFloatArray>&);
    vtkSmartPointer<vtkCellArray> generateNonUniqueVertexFaces();
};  // end class

}   // end namespace

#endif
