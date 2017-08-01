#ifndef RVTK_AXES_H
#define RVTK_AXES_H

#include <vtkRenderer.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkRenderWindowInteractor.h>
#include "rVTK_Export.h"

namespace RVTK
{

class rVTK_EXPORT Axes
{
public:
    explicit Axes( vtkRenderer*);
    virtual ~Axes();

    void setInteractor( vtkRenderWindowInteractor*);

    void show();
    void hide();

private:
    vtkRenderer* _ren;
    vtkAxesActor* _actor;
    vtkOrientationMarkerWidget* _widget;
};  // end class

}   // end namespace

#endif
