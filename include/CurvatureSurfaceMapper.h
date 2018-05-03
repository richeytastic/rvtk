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

#ifndef RVTK_CURVATURE_SURFACE_MAPPER_H
#define RVTK_CURVATURE_SURFACE_MAPPER_H

#include "PolySurfaceCurvScalarMapper.h"  // RVTK
#include <ObjModelCurvatureMetrics.h>     // RFeatures
#include <string>

namespace RVTK {

class rVTK_EXPORT CurvatureSurfaceMapper
{
public:
    // Map must map ObjModel face IDs to VTK poly IDs.
    // Map is generated along with the VTK surface model when using RVTK::VtkActorCreator.
    CurvatureSurfaceMapper( RFeatures::ObjModelCurvatureMetrics::Ptr, vtkActor*, const IntIntMap*);

    // Map surface of the actor with the desired metric.
    // Pass in the lookup name for the surface metric / legend title.
    // Returns the mapped range of values [min,max].
    std::pair<float,float> mapKP1( const std::string&);
    std::pair<float,float> mapKP1First( const std::string&);
    std::pair<float,float> mapKP2( const std::string&);
    std::pair<float,float> mapKP2First( const std::string&);
    std::pair<float,float> mapGaussian( const std::string&);
    std::pair<float,float> mapAbs( const std::string&);
    std::pair<float,float> mapMean( const std::string&);
    std::pair<float,float> mapDeterminant( const std::string&);

private:
    RFeatures::ObjModelCurvatureMetrics::Ptr _cmetrics;
    vtkActor* _actor;
    const IntIntMap* _lmap;
};  // end class

}   // end namespace

#endif


