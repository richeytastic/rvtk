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

#include <Viewer.h>
#include <VtkTools.h>
#include <vtkFollower.h>
using RVTK::Viewer;

Viewer::Ptr Viewer::create( bool offscreen) { return Ptr( new Viewer( offscreen), [](auto d){delete d;});}

Viewer::Viewer( bool offscreen)
{
    _renWin->SetOffScreenRendering(offscreen);
	_ren->SetBackground( 0.0, 0.0, 0.0);
    _renWin->SetPointSmoothing( false);
	_renWin->AddRenderer( _ren);
    _ren->SetTwoSidedLighting( true);  // Don't light occluded sides
    _ren->SetAutomaticLightCreation( true);
}  // end ctor


void Viewer::addActor( vtkActor* actor)
{
    _ren->AddViewProp( actor);
    if (actor->IsA( "vtkFollower"))
        vtkFollower::SafeDownCast(actor)->SetCamera( _ren->GetActiveCamera());
}  // end addActor

void Viewer::removeActor( vtkActor* actor) { _ren->RemoveViewProp( actor);}

void Viewer::clear() { _ren->RemoveAllViewProps();}	// end clear


void Viewer::setCamera( const RFeatures::CameraParams& cp)
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    cam->SetFocalPoint( cp.focus[0], cp.focus[1], cp.focus[2]);
    cam->SetPosition( cp.pos[0], cp.pos[1], cp.pos[2]);
    cam->SetViewUp( cp.up[0], cp.up[1], cp.up[2]);
    _ren->ResetCameraClippingRange();
    cam->SetViewAngle( cp.fov);
}   // end setCamera


double Viewer::clipNear() const { return _ren->GetActiveCamera()->GetClippingRange()[0];}
double Viewer::clipFar() const { return _ren->GetActiveCamera()->GetClippingRange()[1];}


void Viewer::setCameraOrientation( double pitch, double yaw, double roll)
{
    addCameraPitch( pitch);
    addCameraYaw( yaw);
    addCameraRoll( roll);
}   // end setCameraOrientation


void Viewer::addCameraPitch( double pitch)
{
	vtkCamera* cam = _ren->GetActiveCamera();
    cam->Pitch(pitch);
    cam->OrthogonalizeViewUp();
}   // end addCameraPitch


void Viewer::addCameraYaw( double yaw)
{
	vtkCamera* cam = _ren->GetActiveCamera();
    cam->Yaw(yaw);
    cam->OrthogonalizeViewUp();
}   // end addCameraYaw


void Viewer::addCameraRoll( double roll)
{
	vtkCamera* cam = _ren->GetActiveCamera();
    cam->Roll(roll);
    cam->OrthogonalizeViewUp();
}   // end addCameraRoll


void Viewer::setClippingRange( double near, double far)
{
    assert( near <= far);
	_ren->GetActiveCamera()->SetClippingRange( near, far);
}   // end setClippingRange


void Viewer::resetClippingRange() { _ren->ResetCameraClippingRange();}


RFeatures::CameraParams Viewer::camera() const
{
    RFeatures::CameraParams cp;
	vtkCamera* cam = _ren->GetActiveCamera();
    double *arr = cam->GetPosition();
    cp.pos = cv::Vec3f( arr[0], arr[1], arr[2]);
    arr = cam->GetFocalPoint();
    cp.focus = cv::Vec3f( arr[0], arr[1], arr[2]);
    arr = cam->GetViewUp();
    cv::normalize( cv::Vec3f( arr[0], arr[1], arr[2]), cp.up);  // Ensure normalized
    cp.fov = cam->GetViewAngle();
    return cp;
}   // end camera


void Viewer::setPerspective( bool enabled)
{
	vtkCamera* cam = _ren->GetActiveCamera();
	if (!enabled && !cam->GetParallelProjection())
		cam->ParallelProjectionOn();
	else if (enabled && cam->GetParallelProjection())
		cam->ParallelProjectionOff();
}	// end setPerspective


void Viewer::setParallelScale( double scale) { _ren->GetActiveCamera()->SetParallelScale( scale);}
void Viewer::setInteractor( vtkRenderWindowInteractor* interactor) { interactor->SetRenderWindow( _renWin);}


void Viewer::setInteractorStyle( vtkInteractorStyle* style)
{
    vtkRenderWindowInteractor* interactor = vtkRenderWindowInteractor::New();
    interactor->SetInteractorStyle( style);
    setInteractor( interactor);
}  // end setInteractorStyle


void Viewer::changeBackground( double c) { _ren->SetBackground( c,c,c);}
void Viewer::setStereoRendering( bool opt) { _renWin->SetStereoRender( opt);}
bool Viewer::stereoRendering() const { return _renWin->GetStereoRender() > 0;}
void Viewer::setSize( size_t width, size_t height) { _renWin->SetSize( width, height);}
int Viewer::width() const { return _renWin->GetSize()[0];}
int Viewer::height() const { return _renWin->GetSize()[1];}

cv::Size Viewer::size() const
{
    const int* sz = _renWin->GetSize();
    return cv::Size( sz[0], sz[1]);
}   // end size

void Viewer::updateRender() { _renWin->Render();}
cv::Mat_<cv::Vec3b> Viewer::extractImage() const { return RVTK::extractImage( _renWin);}
cv::Mat_<float> Viewer::extractZBuffer() const { return RVTK::extractZBuffer( _renWin);}
