#include <InteractorC1.h>
using RVTK::InteractorC1;
#include <iostream>


vtkStandardNewMacro( InteractorC1);


InteractorC1::InteractorC1()
{
	SetAutoAdjustCameraClippingRange( true);
}  // end ctor



void InteractorC1::OnKeyPress()
{
    const string key = this->Interactor->GetKeySym();
    bool handled = false;
    if ( keyPresser != NULL)
        handled = keyPresser->handleKeyPress( key);

    // If not handled, pass up to VTK system for handling
    if (!handled)
        vtkInteractorStyleTrackballCamera::OnKeyPress();   // Forward key press
}  // end OnKeyPress



void InteractorC1::setKeyPresser( KeyPresser::Ptr kp)
{
    keyPresser = kp;    // May be NULL
}   // end setKeyPresser
