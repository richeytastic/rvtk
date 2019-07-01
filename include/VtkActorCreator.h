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

#ifndef RVTK_VTK_ACTOR_CREATOR_H
#define RVTK_VTK_ACTOR_CREATOR_H

#include <ObjModel.h>
#include "rVTK_Export.h"
#include <vtkSmartPointer.h>
#include <vtkActor.h>
#include <vtkPoints.h>
#include <vtkTexture.h>
#include <vtkCellArray.h>
#include <vector>
#include <list>


namespace RVTK {

class rVTK_EXPORT VtkActorCreator
{
public:
    // Return a texture mapped actor for the given model as long as only a single material is defined.
    // If no materials are defined, this function is equivalent to calling generateSurfaceActor.
    // The provided model must have all its vertex/face IDs in sequential order so they can
    // be treated as indices. On return, lighting is set to 100% ambient, 0% diffuse and 0% specular
    // so that the texture is lit properly. Returns null if more than one material defined on the object.
    // On return, the internal matrix of the actor will match ObjModel::transformMatrix.
    static vtkActor* generateActor( const RFeatures::ObjModel&, vtkSmartPointer<vtkTexture>&);

    // Returns a non-textured actor for the given model. Model must have all its vertex/face IDs
    // stored in sequential order so they can be treated as indices.
    // On return, the internal matrix of the actor will match ObjModel::transformMatrix.
    static vtkActor* generateSurfaceActor( const RFeatures::ObjModel&);

    // Generate a simple points actor. All vertices must be in sequential order.
    // On return, the internal matrix of the actor will match ObjModel::transformMatrix.
    static vtkActor* generatePointsActor( const RFeatures::ObjModel&);

    // Generate a points actor from raw vertices.
    static vtkActor* generatePointsActor( const std::vector<cv::Vec3f>&);

    // Generate a single line where the given points are joined in sequence.
    // Set joinLoop to true if the first point should be joined to the last.
    static vtkActor* generateLineActor( const std::vector<cv::Vec3f>&, bool joinLoop=false);
    static vtkActor* generateLineActor( const std::list<cv::Vec3f>&, bool joinLoop=false);

    // Generate an actor that is a set of line segments where lps is a sequence of line segment
    // endpoints. (lps.size() must be even).
    static vtkActor* generateLinePairsActor( const std::vector<cv::Vec3f>& lps);
};  // end class

}   // end namespace

#endif
