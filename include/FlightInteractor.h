/**
 * Customised Interactor to allow fly throughs.
 * Richard Palmer
 * 2012
 */

#pragma once
#ifndef RVTK_FLIGHT_INTERACTOR_H
#define RVTK_FLIGHT_INTERACTOR_H

#include <vtkInteractorStyleFlight.h>
#include "Viewer.h"
using RVTK::Viewer;
#include "KeyPresser.h"
using RVTK::KeyPresser;


namespace RVTK
{

class FlightInteractor : public vtkInteractorStyleFlight
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
