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
