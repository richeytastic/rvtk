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
#include <ObjModel.h>
#include "VTKTypes.h"
typedef unsigned char byte;
#include "VtkTools.h"
#include "rVTK_Export.h"


namespace RVTK
{

class rVTK_EXPORT ObjectFaceValuer
{
public:
    virtual double getFaceValue( int objFaceId) const = 0;
};  // end class


class rVTK_EXPORT VtkActorFactory
{
public:
    static void init();

    explicit VtkActorFactory( const RFeatures::ObjModel::Ptr&);

    vtkSmartPointer<vtkActor> generateBasicActor();
    vtkSmartPointer<vtkActor> generateTexturedActor();

    // Apply returned array to actor using actor->GetCellData()->SetScalars()
    vtkSmartPointer<vtkFloatArray> createFaceLookupTableIndices( const ObjectFaceValuer&,
                                       int numLookupTableVals, double minVal, double maxVal);

    // Given found indices of vertices or faces (such as might be returned by
    // the RVTK::ClosestPointFinder), return the object index that maps to the VTK index.
    int getObjIdxFromVtkUniqueVtx( int vtkUvidx) const { return _ruvmappings.at(vtkUvidx);}
    int getObjIdxFromVtkVtx( int vtkUvidx) const { return _rvmappings.at(vtkUvidx);}
    int getObjIdxFromVtkUniqueFace( int vtkFaceId) const { return _rufmappings.at(vtkFaceId);}
    int getObjIdxFromVtkFace( int vtkFaceId) const { return _rfmappings.at(vtkFaceId);}

    int getVtkBasicIdxFromObjFaceIdx( int objFaceId) const { return _ufmappings.at(objFaceId);}
    int getVtkTextureIdxFromObjFaceIdx( int objFaceId) const { return _fmappings.at(objFaceId);}

private:
    const RFeatures::ObjModel::Ptr _omodel;
    boost::unordered_map<int, int> _uvmappings, _vmappings, _ufmappings, _fmappings; // Obj to VTK unique indices
    // Reverse mappings from VTK to obj indices (for lookup)
    boost::unordered_map<int, int> _ruvmappings, _rvmappings, _rufmappings, _rfmappings;

    vtkSmartPointer<vtkPoints> generateUniqueVertices();
    vtkSmartPointer<vtkCellArray> generateUniqueVertexFaces();
    vtkSmartPointer<vtkPoints> generateNonUniqueVertices( vtkSmartPointer<vtkFloatArray>&);
    vtkSmartPointer<vtkCellArray> generateNonUniqueVertexFaces();
};  // end class

}   // end namespace

#endif
