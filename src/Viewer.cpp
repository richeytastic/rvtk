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

#include "Viewer.h"
#include "VtkTools.h"
using RVTK::Viewer;
#include <vtkFollower.h>
#include <vtkOpenGLRenderWindow.h>


class Viewer::Deleter
{
public:
    void operator()( Viewer *p)
    {
        delete p;
    }   // end operator()
};  // end Deleter


Viewer::Ptr Viewer::create()
{
    Viewer::Ptr ptr( new Viewer(), Viewer::Deleter());
    return ptr;
}   // end create


Viewer::Viewer() :
	_ren( vtkSmartPointer<vtkOpenGLRenderer>::New()),
    _renWin( vtkSmartPointer<vtkRenderWindow>::New()),
    _headlight( vtkSmartPointer<vtkLight>::New())
{
	_ren->SetBackground( 0.0, 0.0, 0.0);
    _renWin->SetPointSmoothing( false);
	_renWin->AddRenderer( _ren);

    _ren->SetTwoSidedLighting( true);  // Don't light occluded sides
    _ren->SetAutomaticLightCreation( true);

    // Make a headlight and add it to the renderer
    _headlight->SetLightTypeToHeadlight();
    _ren->AddLight( _headlight);
    _ren->SetLightFollowCamera(true);   // Set the headlight to follow the camera
    setHeadlightEnabled(false);
}  // end ctor


Viewer::~Viewer()
{
}   // end dtor


bool Viewer::getSupportsMultiTexturing() const
{
    return false;
    /*
    vtkOpenGLHardwareSupport* hardware = vtkOpenGLRenderWindow::SafeDownCast(_renWin)->GetHardwareSupport();
    return hardware->GetSupportsMultiTexturing();
    */
}   // end getSupportsMultiTexturing


int Viewer::getNumFixedTextureUnits() const
{
    return 1;
    /*
    vtkOpenGLHardwareSupport* hardware = vtkOpenGLRenderWindow::SafeDownCast(_renWin)->GetHardwareSupport();
    return hardware->GetNumberOfFixedTextureUnits();
    */
}   // end getNumFixedTextureUnits



void Viewer::addActor( const vtkSmartPointer<vtkActor> actor)
{
    double *pos = actor->GetPosition();
	actor->SetPosition( pos[0], pos[1], pos[2]);
	_ren->AddActor( actor);
    if (actor->IsA( "vtkFollower"))
        vtkFollower::SafeDownCast(actor)->SetCamera( _ren->GetActiveCamera());
}  // end addActor


void Viewer::removeActor( const vtkSmartPointer<vtkActor> actor)
{
    _ren->RemoveActor( actor);
}   // end removeActor


void Viewer::clear()
{
	_ren->RemoveAllViewProps();
}	// end clear


void Viewer::resetCamera( const cv::Vec3f& camPos, const cv::Vec3f& focus, const cv::Vec3f& vUp, double fov)
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    cam->SetFocalPoint( focus[0], focus[1], focus[2]);
    cam->SetPosition( camPos[0], camPos[1], camPos[2]);
    cam->ComputeViewPlaneNormal();
    cam->SetViewUp( vUp[0], vUp[1], vUp[2]);
    _ren->ResetCameraClippingRange();
    cam->SetViewAngle( fov);
}   // end resetCamera


void Viewer::resetSceneLights( int numLights, const cv::Vec3f* pos, const cv::Vec3f* focus, const double* intensities)
{
    _ren->RemoveAllLights();
    for ( int i = 0; i < numLights; ++i)
    {
        vtkSmartPointer<vtkLight> light = vtkSmartPointer<vtkLight>::New();
        light->SetLightTypeToSceneLight();
        light->SetColor( 1, 1, 1);
        light->SetAmbientColor( 1, 1, 1);
        light->SetIntensity( intensities[i]);
        light->SetPosition( pos[i][0], pos[i][1], pos[i][2]);
        light->SetFocalPoint( focus[i][0], focus[i][1], focus[i][2]);
        _ren->AddLight(light);
    }   // end for
    _ren->AddLight( _headlight);
}   // end resetSceneLights


void Viewer::setHeadlightEnabled( bool enabled)
{
    _headlight->SetSwitch( enabled);
}   // end setHeadlightEnabled


double Viewer::getClipNear() const
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    return cam->GetClippingRange()[0];
}   // end getClipNear


double Viewer::getClipFar() const
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    return cam->GetClippingRange()[1];
}   // end getClipFar


void Viewer::setCameraOrientation( double pitch, double yaw, double roll)
{
    addCameraPitch( pitch);
    addCameraYaw( yaw);
    addCameraRoll( roll);
}   // end setCameraOrientation


void Viewer::addCameraPitch( double pitch)
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    cam->Pitch(pitch);
    cam->OrthogonalizeViewUp();
}   // end addCameraPitch


void Viewer::addCameraYaw( double yaw)
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    cam->Yaw(yaw);
    cam->OrthogonalizeViewUp();
}   // end addCameraYaw


void Viewer::addCameraRoll( double roll)
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    cam->Roll(roll);
    cam->OrthogonalizeViewUp();
}   // end addCameraRoll


void Viewer::setCameraPosition( const cv::Vec3f &pos)
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
	cam->SetPosition( pos[0], pos[1], pos[2]);
}   // end setCameraPosition


void Viewer::setCameraFocus( const cv::Vec3f& foc)
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    cam->SetClippingRange(0,500);
    cam->SetFocalPoint( foc[0], foc[1], foc[2]);
    cam->ComputeViewPlaneNormal();
}   // end setCameraFocus


void Viewer::setCameraViewUp( const cv::Vec3f& vu)
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
	cam->SetViewUp( vu[0], vu[1], vu[2]);
}   // end setCameraViewUp


void Viewer::getCamera( cv::Vec3f &absPos, cv::Vec3f &focus, cv::Vec3f &vu)
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    double *arr = cam->GetPosition();
    absPos = cv::Vec3f( arr[0], arr[1], arr[2]);
    arr = cam->GetFocalPoint();
    focus = cv::Vec3f( arr[0], arr[1], arr[2]);
    arr = cam->GetViewUp();
    vu = cv::Vec3f( arr[0], arr[1], arr[2]);
}   // end getCamera


cv::Vec3f Viewer::getCameraPosition() const
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    const double *arr = cam->GetPosition();
    return cv::Vec3f( arr[0], arr[1], arr[2]);
}   // end getCameraPosition


cv::Vec3f Viewer::getCameraViewUp() const
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    const double *arr = cam->GetViewUp();
    const cv::Vec3f vup = cv::Vec3f( arr[0], arr[1], arr[2]);
    return vup / cv::norm(vup); // Ensure normalised
}   // end getCameraViewUp


cv::Vec3f Viewer::getCameraFocus() const
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    const double *arr = cam->GetFocalPoint();
    return cv::Vec3f( arr[0], arr[1], arr[2]);
}   // end getCameraFocus


cv::Vec3f Viewer::getCameraDirection() const
{
    const cv::Vec3f dirVec = getCameraFocus() - getCameraPosition();
    return dirVec / cv::norm(dirVec);
}   // end getCameraDirection


void Viewer::setPerspective( bool enabled)
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();

	if (!enabled && !cam->GetParallelProjection())
		cam->ParallelProjectionOn();
	else if (enabled && cam->GetParallelProjection())
		cam->ParallelProjectionOff();
}	// end setPerspective


void Viewer::setFieldOfView( double angle)
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    cam->SetViewAngle( angle);
}   // end setFieldOfView


double Viewer::getFieldOfView() const
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    return cam->GetViewAngle();
}   // end getFieldOfView


void Viewer::setParallelScale( double scale)
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    cam->SetParallelScale( scale);
}   // end setParallelScale


void Viewer::setInteractor( vtkSmartPointer<vtkRenderWindowInteractor> interactor)
{
    interactor->SetRenderWindow( _renWin);
	//_renWin->SetInteractor( interactor);
}  // end setInteractor


void Viewer::setInteractorStyle( vtkSmartPointer<vtkInteractorStyle> style)
{
    vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetInteractorStyle( style);
    setInteractor( interactor);
}  // end setInteractorStyle


void Viewer::changeBackground( double c)
{
	_ren->SetBackground( c,c,c);
}  // end changeBackground


void Viewer::setStereoRendering( bool opt)
{
	_renWin->SetStereoRender( opt);
}	// end setStereoRendering


bool Viewer::getStereoRendering() const
{
    return _renWin->GetStereoRender() > 0;
}	// end getStereoRendering


void Viewer::setSize( int width, int height)
{
    _renWin->SetSize( width, height);
}   // end setSize


int Viewer::getWidth() const
{
    return _renWin->GetSize()[0];
}   // end getWidth


int Viewer::getHeight() const
{
    return _renWin->GetSize()[1];
}   // end getHeight


void Viewer::updateRender()
{
	_renWin->Render();
}	// end updateRender


cv::Mat_<cv::Vec3b> Viewer::extractImage() const
{
    return RVTK::extractImage( _renWin);
}   // end extractImage


cv::Mat_<float> Viewer::extractZBuffer() const
{
    return RVTK::extractZBuffer( _renWin);
}   // end extractZBuffer
