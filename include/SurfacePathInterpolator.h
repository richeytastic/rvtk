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

#ifndef RVTK_SURFACE_PATH_INTERPOLATOR_H
#define RVTK_SURFACE_PATH_INTERPOLATOR_H

#include <vtkRenderer.h>
#include <vtkContourRepresentation.h>
#include <vtkContourLineInterpolator.h>
#include "VtkModel.h"
#include <vector>

namespace RVTK
{

class SurfacePathInterpolator : public vtkContourLineInterpolator
{
public:
    vtkTypeMacro( SurfacePathInterpolator, vtkContourLineInterpolator);
    static SurfacePathInterpolator* New();

    void setModel( const RVTK::VtkModel::Ptr& vmodel);

    // Interpolate between nodes n0 and n1 on the given vtkContourRepresentation
    virtual int InterpolateLine( vtkRenderer* ren, vtkContourRepresentation* rep, int n0, int n1);

protected:
    virtual ~SurfacePathInterpolator(){}

private:
    SurfacePathInterpolator();
    SurfacePathInterpolator( const SurfacePathInterpolator&);   // Not implemented
    void operator=( const SurfacePathInterpolator&);    // Not implemented

    RVTK::VtkModel::Ptr _vmodel;

    void getSmoothedPathFromVertexIndices( const std::vector<int>&, double*, double*, std::vector<cv::Vec3f>&) const;
};  // end class

}   // end namespace

#endif 
