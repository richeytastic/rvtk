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

#ifndef RVTK_AXES_H
#define RVTK_AXES_H

#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include "rVTK_Export.h"

namespace RVTK
{

class rVTK_EXPORT Axes
{
public:
    explicit Axes( vtkRenderWindowInteractor *rwint=NULL);

    void setEnabled( bool);
    void setVisible( bool);
    bool isVisible() const;
    bool isEnabled() const;

private:
    vtkSmartPointer<vtkAxesActor> _actor;
    vtkSmartPointer<vtkOrientationMarkerWidget> _widget;
};  // end class

}   // end namespace

#endif
