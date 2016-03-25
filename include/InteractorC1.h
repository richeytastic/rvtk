#ifndef RVTK_INTERACTORC1_H
#define RVTK_INTERACTORC1_H

#include "Viewer.h"
using RVTK::Viewer;
#include "KeyPresser.h"
using RVTK::KeyPresser;
#include <vtkInteractorStyleTrackballCamera.h>


namespace RVTK
{

class rVTK_EXPORT InteractorC1 : public vtkInteractorStyleTrackballCamera
{
public:
    static InteractorC1 *New();
    vtkTypeMacro( InteractorC1, vtkInteractorStyleTrackballCamera);

    virtual void OnKeyPress();
    void setKeyPresser( KeyPresser::Ptr);

private:
    KeyPresser::Ptr keyPresser;
    InteractorC1();
}; // end class

}   // end namespace

#endif
