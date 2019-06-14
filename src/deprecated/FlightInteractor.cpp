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
