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
 * Parent for all VTK objects that know how to draw themselves to a given viewer.
 *
 * Richard Palmer
 * February 2012
 */

#pragma once
#ifndef RVTK_VTK_DISPLAY_ACTOR_H
#define RVTK_VTK_DISPLAY_ACTOR_H

#include "VTKTypes.h"
#include "Viewer.h"
using RVTK::Viewer;

#include <opencv2/opencv.hpp>
#include <boost/shared_ptr.hpp>

typedef unsigned char byte;


namespace RVTK
{

class rVTK_EXPORT VtkDisplayActor
{
public:
    typedef boost::shared_ptr<VtkDisplayActor> Ptr;
    static Ptr create( const Viewer::Ptr &viewer);
    VtkDisplayActor( const Viewer::Ptr &viewer);
    virtual ~VtkDisplayActor();

    inline bool isDisplayed() const { return displayed;}

    void setMapper( const vtkSmartPointer<vtkPolyDataMapper>&);
    void setPosition( const cv::Vec3d &pos);
    virtual void setOpacity( float val);
    virtual void setColour( byte r, byte g, byte b);    // Set colour of the actor

    // Orient the actor about x,y,z axes by the given degrees
    virtual void setOrientation( double xd, double yd, double zd);

    virtual void show();   // Display self to viewer (does not update viewer rendering!)
    virtual void hide();   // Remove self from viewer (does not update viewer rendering!)

protected:
    Viewer::Ptr viewer;
    bool displayed;          // True if the actor is currently being displayed
    vtkSmartPointer<vtkActor> actor;
};  // end class VtkDisplayActor

}   // end namespace

#endif
