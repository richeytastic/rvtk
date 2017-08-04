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

#include "VtkDisplayActor.h"
using RVTK::VtkDisplayActor;


VtkDisplayActor::Ptr VtkDisplayActor::create( const Viewer::Ptr &pv)
{
    VtkDisplayActor::Ptr ptr( new VtkDisplayActor( pv));
    return ptr;
}   // end create



VtkDisplayActor::VtkDisplayActor( const Viewer::Ptr &pv)
    : viewer(pv), displayed(false), actor( vtkSmartPointer<vtkLODActor>::New())
{
}   // end ctor



VtkDisplayActor::~VtkDisplayActor()
{
    hide();
}   // end dtor



void VtkDisplayActor::setMapper( const vtkSmartPointer<vtkPolyDataMapper> &mapper)
{
    actor->SetMapper( mapper);
}   // end setMapper



void VtkDisplayActor::setPosition( const cv::Vec3d &pos)
{
    actor->SetPosition( pos[0], pos[1], pos[2]);
}   // end setPosition



void VtkDisplayActor::setOpacity( float val)
{
    actor->GetProperty()->SetOpacity( val);
}   // end setOpacity



void VtkDisplayActor::setColour( byte r, byte g, byte b)
{
    actor->GetProperty()->SetColor( r, g, b);
}   // end setColour


#ifndef M_PI
#define M_PI 3.14159265359
#endif

void VtkDisplayActor::setOrientation( double xd, double yd, double zd)
{
    xd *= M_PI/180;
    yd *= M_PI/180;
    zd *= M_PI/180;
    actor->SetOrientation( xd, yd, zd);
}   // end setOrientation



void VtkDisplayActor::show()
{
    if ( !displayed)
        viewer->addActor( actor);
    displayed = true;
}   // end show



void VtkDisplayActor::hide()
{
    if ( displayed)
        viewer->removeActor( actor);
    displayed = false;
}   // end hide
