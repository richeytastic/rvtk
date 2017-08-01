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

