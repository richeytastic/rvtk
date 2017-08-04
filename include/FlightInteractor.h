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

/**
 * Customised Interactor to allow fly throughs.
 * Richard Palmer
 * 2012
 */

#ifndef RVTK_FLIGHT_INTERACTOR_H
#define RVTK_FLIGHT_INTERACTOR_H

#include <vtkInteractorStyleFlight.h>
#include "Viewer.h"
using RVTK::Viewer;
#include "KeyPresser.h"
using RVTK::KeyPresser;
#include "rVTK_Export.h"


namespace RVTK
{

class rVTK_EXPORT FlightInteractor : public vtkInteractorStyleFlight
{
public:
    static FlightInteractor *New();
    vtkTypeMacro( FlightInteractor, vtkInteractorStyleFlight);

    virtual void OnKeyPress();

    void setViewer( Viewer *pv);

private:
    KeyPresser::Ptr keyPresser;

    FlightInteractor();
}; // end class

}   // end namespace

#endif
