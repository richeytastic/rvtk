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

/**
 * Create different VTK Actors from an ObjModel supporting the mapping of the different vertex and face indices to VTK.
 */

#ifndef RVTK_VTK_ACTOR_CREATOR_H
#define RVTK_VTK_ACTOR_CREATOR_H

#include <ObjModel.h>
#include "rVTK_Export.h"
typedef unsigned char byte;
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkTexture.h>
#include <vtkCellArray.h>
typedef std::unordered_set<int> IntSet;
typedef std::unordered_map<int, int> IntIntMap;
#include <vector>
#include <list>


namespace RVTK {

class rVTK_EXPORT VtkActorCreator
{
public:
    VtkActorCreator();

    void setVtkUniqueVertexIdSet( IntSet *s) { _uvidxs = s;}
    void setVtkUniqueFaceIdSet( IntSet *s) { _ufidxs = s;}

    // Forward maps
    void setObjToVTKUniqueVertexMap( IntIntMap *m) { _uvmappings = m;}
    void setObjToVTKVertexMap( IntIntMap *m) { _vmappings = m;}
    void setObjToVTKUniqueFaceMap( IntIntMap *m) { _ufmappings = m;}
    void setObjToVTKFaceMap( IntIntMap *m) { _fmappings = m;}

    // The reverse maps
    void setObjToVTKUniqueVertexRMap( IntIntMap *m) { _ruvmappings = m;}
    void setObjToVTKVertexRMap( IntIntMap *m) { _rvmappings = m;}
    void setObjToVTKUniqueFaceRMap( IntIntMap *m) { _rufmappings = m;}
    void setObjToVTKFaceRMap( IntIntMap *m) { _rfmappings = m;}

    // Return a texture mapped actor for the given model as long as the
    // model has no more than a single material defined. If no materials
    // are defined, this function is equivalent to calling generateSurfaceActor.
    // On return, if textured, lighting is set to 100% ambient, 0% diffuse and specular.
    // Returns null if there's more than one material defined on the object.
    vtkActor* generateActor( const RFeatures::ObjModel*, vtkSmartPointer<vtkTexture>&);

    // Call the actor generation functions only AFTER setting the needed lookup maps.
    // The surface actor uses only the unique vertices from the provided object.
    // This makes it possible to treat the surface as a graph.
    // The returned actor has 100% diffuse lighting set and 0% ambient and specular.
    vtkActor* generateSurfaceActor( const RFeatures::ObjModel*);

    // Generate a simple points actor.
    static vtkActor* generatePointsActor( const std::vector<cv::Vec3f>&);
    static vtkActor* generatePointsActor( const RFeatures::ObjModel*);

    // Generate a single line where the given points are joined in sequence.
    // Set joinLoop to true if the first point should be joined to the last.
    static vtkActor* generateLineActor( const std::vector<cv::Vec3f>&, bool joinLoop=false);
    static vtkActor* generateLineActor( const std::list<cv::Vec3f>&, bool joinLoop=false);

    // Generate an actor that is a set of line segments where lps is a sequence of line segment
    // endpoints. (lps.size() must be even).
    static vtkActor* generateLinePairsActor( const std::vector<cv::Vec3f>& lps);

private:
    IntSet* _uvidxs;  // VTK unique vertex (surface model) IDs
    IntSet* _ufidxs;  // VTK unique face (surface model) IDs

    IntIntMap *_uvmappings, *_vmappings, *_ufmappings, *_fmappings; // Obj to VTK unique indices (for lookup)
    IntIntMap *_ruvmappings, *_rvmappings, *_rufmappings, *_rfmappings; // Reverse mappings from VTK to obj indices (for lookup)

    vtkSmartPointer<vtkPoints> createVertices( const RFeatures::ObjModel*, IntIntMap*, vtkCellArray*);
    vtkSmartPointer<vtkCellArray> createPolygons( const RFeatures::ObjModel*, const IntIntMap*);
};  // end class

}   // end namespace

#endif
