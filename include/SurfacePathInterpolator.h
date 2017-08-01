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
