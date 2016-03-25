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
