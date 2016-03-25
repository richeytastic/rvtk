#include <FlightInteractor.h>
using RVTK::FlightInteractor;


vtkStandardNewMacro( FlightInteractor);


FlightInteractor::FlightInteractor()
{
	SetAutoAdjustCameraClippingRange( true);
}  // end ctor



void FlightInteractor::OnKeyPress()
{
   char ckey = this->Interactor->GetKeyCode();  // Get key press value
   if ( keyPresser != NULL)
       keyPresser->handleKeyPress( ckey);
   vtkInteractorStyleFlight::OnKeyPress();   // Forward key press
}  // end OnKeyPress



void FlightInteractor::setViewer( Viewer *pv)
{
    keyPresser = KeyPresser::create(pv);
}   // end setViewer
