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
//#include <vtkOpenGLRenderWindow.h>
//#include <vtkGraphicsFactory.h>
using RVTK::Viewer;


class Viewer::Deleter
{
public:
    void operator()( Viewer *p)
    {
        delete p;
    }   // end operator()
};  // end Deleter


Viewer::Ptr Viewer::create( bool offscreen)
{
    Viewer::Ptr ptr( new Viewer( offscreen), Viewer::Deleter());
    return ptr;
}   // end create


Viewer::Viewer( bool offscreen) :
	_ren( vtkSmartPointer<vtkRenderer>::New()),
    _renWin( vtkSmartPointer<vtkRenderWindow>::New())
{
    //vtkGraphicsFactory::SetOffScreenOnlyMode(offscreen);
    //vtkGraphicsFactory::setUseMesaClasses(true);
    _renWin->SetOffScreenRendering(offscreen);
    // For temporary offscreen, use:
    //_renWin->SetUseOffScreenBuffers(offscreen);

	_ren->SetBackground( 0.0, 0.0, 0.0);
    _renWin->SetPointSmoothing( false);
	_renWin->AddRenderer( _ren);

    _ren->SetTwoSidedLighting( true);  // Don't light occluded sides
    _ren->SetAutomaticLightCreation( true);
}  // end ctor


Viewer::~Viewer()
{
    //_renWin->SetUseOffScreenBuffers(false);
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


void Viewer::setCamera( const RFeatures::CameraParams& cp)
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    cam->SetFocalPoint( cp.focus[0], cp.focus[1], cp.focus[2]);
    cam->SetPosition( cp.pos[0], cp.pos[1], cp.pos[2]);
    cam->SetViewUp( cp.up[0], cp.up[1], cp.up[2]);
    _ren->ResetCameraClippingRange();
    cam->SetViewAngle( cp.fov);
}   // end setCamera


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


void Viewer::setClippingRange( double near, double far)
{
    assert( near <= far);
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    cam->SetClippingRange( near, far);
}   // end setClippingRange


void Viewer::resetClippingRange()
{
    _ren->ResetCameraClippingRange();
}   // end resetClippingRange


RFeatures::CameraParams Viewer::getCamera() const
{
    RFeatures::CameraParams cp;
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    double *arr = cam->GetPosition();
    cp.pos = cv::Vec3f( arr[0], arr[1], arr[2]);
    arr = cam->GetFocalPoint();
    cp.focus = cv::Vec3f( arr[0], arr[1], arr[2]);
    arr = cam->GetViewUp();
    cv::normalize( cv::Vec3f( arr[0], arr[1], arr[2]), cp.up);  // Ensure normalized
    cp.fov = cam->GetViewAngle();
    return cp;
}   // end getCamera


void Viewer::setPerspective( bool enabled)
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();

	if (!enabled && !cam->GetParallelProjection())
		cam->ParallelProjectionOn();
	else if (enabled && cam->GetParallelProjection())
		cam->ParallelProjectionOff();
}	// end setPerspective


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


cv::Size Viewer::getSize() const
{
    const int* sz = _renWin->GetSize();
    return cv::Size( sz[0], sz[1]);
}   // end getSize


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
