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

#include "Axes.h"
using RVTK::Axes;


Axes::Axes( vtkRenderer* ren) : _ren(ren), _actor( vtkAxesActor::New()), _widget(NULL)
{
}   // end ctor


Axes::~Axes()
{
    if ( _widget)
        _widget->Delete();
    _actor->Delete();
}   // end dtor


void Axes::setInteractor( vtkRenderWindowInteractor* i)
{
    _widget = vtkOrientationMarkerWidget::New();
    _widget->SetOutlineColor( 0.9, 0.5, 0.1);
    _widget->SetOrientationMarker(_actor);
    _widget->SetInteractor(i);
    _widget->SetViewport( 0.0, 0.0, 0.4, 0.4);
    _widget->SetEnabled( 1);
}   // end setInteractor


// public
void Axes::show()
{
    if ( _widget)
    {
        _actor->SetVisibility(true);
        _widget->InteractiveOn();
    }   // end if
}   // end show


// public
void Axes::hide()
{
    if ( _widget)
    {
        _actor->SetVisibility(false);
        _widget->InteractiveOff();
    }   // end if
}   // end hide

