#include "Viewer.h"
#include "VtkTools.h"
using RVTK::Viewer;
using RVTK::ActorSubset;

#include <ostream>
using std::ostream;
#include <iostream>
using std::endl;

#include <vtkFollower.h>


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



void Viewer::printCameraDetails( ostream &os) const
{
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();

    os << "*********************************************" << endl;
	double *camPos = cam->GetPosition();
	os << "Camera Position (x,y,z):          " << camPos[0] << ", " << camPos[1] << ", " << camPos[2] << endl;
	double *camFoc = cam->GetFocalPoint();
	os << "Camera Focus (x,y,z):             " << camFoc[0] << ", " << camFoc[1] << ", " << camFoc[2] << endl;
    double focDist = cam->GetDistance();  // Distance of focal point to camera position
    os << "Focal point distance (from cam):  " << focDist << endl;
    double *normal = cam->GetViewPlaneNormal();
    os << "Camera View Plane Normal (x,y,z): " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
    double *projDir = cam->GetDirectionOfProjection();
    os << "Direction of projection:          " << projDir[0] << ", " << projDir[1] << ", " << projDir[2] << endl;
    double *camUp = cam->GetViewUp();
    os << "Camera View Up Vector (x,y,z):    " << camUp[0] << ", " << camUp[1] << ", " << camUp[2] << endl;
    double *camOr = cam->GetOrientationWXYZ();
    os << "Camera Orientation (w,x,y,z):     " << camOr[0] << ", " << camOr[1] << ", " << camOr[2] << ", " << camOr[3] << endl;

	if ( cam->GetParallelProjection())
	{
		os << "Cam Projection:                   ORTHOGONAL" << endl;
		double ps = cam->GetParallelScale();
		os << "Cam Parallel Scale:           " << ps << " metres (viewport height)" << endl;
	}	// end if
	else
	{
		os << "Cam Projection:                   PERSPECTIVE" << endl;
		double va = cam->GetViewAngle();
		os << "Cam View Angle:                   " << va << " degrees" << endl;
	}	// end else
    os << "*********************************************" << endl;
}	// end printCameraDetails



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
    /*
	vtkSmartPointer<vtkCamera> cam = _ren->GetActiveCamera();
    double nearZ = cam->GetClippingRange()[0];
    double farZ = cam->GetClippingRange()[1];
    std::cerr << "Camera clipping range (near, far): " << nearZ << ", " << farZ << std::endl;
    */

    return RVTK::extractZBuffer( _renWin);
}   // end extractZBuffer



typedef std::pair< const vtkActor*, boost::unordered_set<int> > ActorCellsPair;

int Viewer::pickActorCells( const vector<cv::Point>& points2d, vector<ActorSubset>& picked) const
{
    vtkSmartPointer<vtkCellPicker> cellPicker = vtkSmartPointer<vtkCellPicker>::New();
    cellPicker->SetTolerance( 0.0005);
    vtkSmartPointer<vtkPropPicker> propPicker = vtkSmartPointer<vtkPropPicker>::New();

    boost::unordered_map< vtkActor*, boost::unordered_set<int> > actorCells;
    BOOST_FOREACH ( const cv::Point& p, points2d)
    {
        propPicker->Pick( p.x, p.y, 0, _ren);
        vtkActor* actor = propPicker->GetActor();
        if ( actor == NULL)
            continue;

        cellPicker->Pick( p.x, p.y, 0, _ren);
        const int cellId = cellPicker->GetCellId();
        assert( cellId != -1);  // We have the actor so we must have one of its cells
        actorCells[actor].insert(cellId);
    }   // end foreach

    int numActorsPicked = 0;
    // Copy the picked actors and their cells to the output parameter
    BOOST_FOREACH ( const ActorCellsPair& pickedActor, actorCells)
    {
        numActorsPicked++;
        picked.resize( picked.size() + 1);
        ActorSubset& actorSubset = *picked.rbegin();
        actorSubset.actor = const_cast<vtkActor*>(pickedActor.first);
        const boost::unordered_set<int>& actorCellIds = pickedActor.second;
        actorSubset.cellIds.insert( actorSubset.cellIds.end(), actorCellIds.begin(), actorCellIds.end());
    }   // end foreach

    return numActorsPicked;
}   // end pickActorCells



int Viewer::pickActorCells( const vector<cv::Point>& points2d, const vtkActor* actor, vector<int>& cellIds) const
{
    if ( actor == NULL)
        return 0;

    boost::unordered_set<int> setCellIds;   // To avoid duplicates

    vtkSmartPointer<vtkCellPicker> cellPicker = vtkSmartPointer<vtkCellPicker>::New();
    cellPicker->SetTolerance( 0.0005);
    vtkSmartPointer<vtkPropPicker> propPicker = vtkSmartPointer<vtkPropPicker>::New();

    BOOST_FOREACH ( const cv::Point& p, points2d)
    {
        propPicker->Pick( p.x, p.y, 0, _ren);
        vtkActor* gotActor = propPicker->GetActor();
        if ( gotActor != actor)
            continue;

        cellPicker->Pick( p.x, p.y, 0, _ren);
        const int cellId = cellPicker->GetCellId();
        assert( cellId != -1);  // We have the actor so we must have one of its cells
        setCellIds.insert(cellId);
    }   // end foreach

    cellIds.insert( cellIds.end(), setCellIds.begin(), setCellIds.end());
    return (int)setCellIds.size();
}   // end pickActorCells



vtkSmartPointer<vtkActor> Viewer::pickActor( const cv::Point& p) const
{
    vtkSmartPointer<vtkPropPicker> propPicker = vtkSmartPointer<vtkPropPicker>::New();
    propPicker->Pick( p.x, p.y, 0, _ren);
    return propPicker->GetActor();
}   // end pickActor



vtkSmartPointer<vtkActor> Viewer::pickActor( const cv::Point& p, const vector<vtkSmartPointer<vtkActor> >& possActors) const
{
    vtkSmartPointer<vtkPropPicker> propPicker = vtkSmartPointer<vtkPropPicker>::New();
    vtkSmartPointer<vtkPropCollection> pickFrom = vtkSmartPointer<vtkPropCollection>::New();
    BOOST_FOREACH ( vtkSmartPointer<vtkActor> actor, possActors)
        pickFrom->AddItem(actor);
    propPicker->PickProp( p.x, p.y, _ren, pickFrom);
    return propPicker->GetActor();
}   // end pickActor


int Viewer::pickCell( const cv::Point& p) const
{
    vtkSmartPointer<vtkCellPicker> picker = vtkSmartPointer<vtkCellPicker>::New();
    picker->SetTolerance( 0.0005);
    picker->Pick( p.x, p.y, 0, _ren);
    return picker->GetCellId();

    /*
    if ( picker->GetCellId() == -1)
        return -1;

    vtkSmartPointer<vtkIdTypeArray> ids = vtkSmartPointer<vtkIdTypeArray>::New();
    ids->SetNumberOfComponents( 1);
    ids->InsertNextValue( picker->GetCellId());
                             
    vtkSmartPointer<vtkSelectionNode> selectionNode = vtkSmartPointer<vtkSelectionNode>::New();
    selectionNode->SetFieldType( vtkSelectionNode::CELL);
    selectionNode->SetContentType( vtkSelectionNode::INDICES);
    selectionNode->SetSelectionList( ids);
                                                             
    vtkSmartPointer<vtkSelection> selection = vtkSmartPointer<vtkSelection>::New();
    selection->AddNode( selectionNode);

    vtkSmartPointer<vtkActor> actor = this->pickActor(p);
    vtkSmartPointer<vtkExtractSelection> extractSelection = vtkSmartPointer<vtkExtractSelection>::New();
#if VTK_MAJOR_VERSION <= 5
    extractSelection->SetInput(0, actor->GetMapper()->GetInput());
    extractSelection->SetInput(1, selection);
#else
    extractSelection->SetInputData(0, actor->GetMapper()->GetInputData());
    extractSelection->SetInputData(1, selection);
#endif
    extractSelection->Update();
    */
}   // end pickCell
