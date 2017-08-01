#include "RendererPicker.h"
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/foreach.hpp>
#include <map>
#include <vtkCellPicker.h>
#include <vtkPropPicker.h>
#include <vtkWorldPointPicker.h>
#include <vtkCoordinate.h>

using RVTK::RendererPicker;
using RVTK::ActorSubset;


RendererPicker::RendererPicker( vtkRenderer* ren, PointOrigin po)
    : _ren(ren), _pointOrigin(po), _tolerance(0.0005)
{
}   // end ctor


cv::Point changeOriginOfPoint( vtkRenderer* ren, const cv::Point& p, RendererPicker::PointOrigin po)
{
    if ( po == RendererPicker::BOTTOM_LEFT)
        return p;

    // p is given using a top left origin, so need to create and return a new point
    // that treats the bottom left as the origin so the algorithms work as per VTK's expectations.
    cv::Point np = p;
    const int nrows = ren->GetSize()[1];
    np.y = nrows - p.y - 1;
    return np;
}   // end changeOriginOfPoint


// public
int RendererPicker::pickActorCells( const std::vector<cv::Point>& points2d, std::vector<ActorSubset>& picked) const
{
    vtkCellPicker* cellPicker = vtkCellPicker::New();
    vtkPropPicker* propPicker = vtkPropPicker::New();
    cellPicker->SetTolerance( _tolerance);

    boost::unordered_map<vtkActor*, boost::unordered_set<int> > actorCells;
    BOOST_FOREACH ( const cv::Point& p, points2d)
    {
        const cv::Point np = changeOriginOfPoint( _ren, p, _pointOrigin);
        propPicker->Pick( np.x, np.y, 0, _ren);
        vtkActor* actor = propPicker->GetActor();
        if ( !actor)
            continue;

        cellPicker->Pick( np.x, np.y, 0, _ren);
        const int cellId = cellPicker->GetCellId();
        actorCells[actor].insert(cellId);
    }   // end foreach

    propPicker->Delete();
    cellPicker->Delete();

    int numActorsPicked = 0;
    // Copy the picked actors and their cells to the output parameter
    typedef std::pair<vtkActor*, boost::unordered_set<int> > ActorCellsPair;
    BOOST_FOREACH ( const ActorCellsPair& pickedActor, actorCells)
    {
        numActorsPicked++;
        picked.resize( picked.size() + 1);
        ActorSubset& actorSubset = *picked.rbegin();
        actorSubset.actor = pickedActor.first;
        const boost::unordered_set<int>& actorCellIds = pickedActor.second;
        actorSubset.cellIds.insert( actorSubset.cellIds.end(), actorCellIds.begin(), actorCellIds.end());
    }   // end foreach

    return numActorsPicked;
}   // end pickActorCells


// public
int RendererPicker::pickActorCells( const std::vector<cv::Point>& points2d,
                                    vtkActor* actor, std::vector<int>& cellIds) const
{
    if ( !actor)
        return 0;

    vtkCellPicker* cellPicker = vtkCellPicker::New();
    vtkPropPicker* propPicker = vtkPropPicker::New();
    vtkPropCollection* pickFrom = vtkPropCollection::New();
    pickFrom->AddItem(actor);
    cellPicker->SetTolerance( _tolerance);

    boost::unordered_set<int> setCellIds;   // Avoid duplicate cell IDs being picked
    BOOST_FOREACH ( const cv::Point& p, points2d)
    {
        const cv::Point np = changeOriginOfPoint( _ren, p, _pointOrigin);
        propPicker->PickProp( np.x, np.y, _ren, pickFrom);
        if ( propPicker->GetActor() == actor)
        {
            cellPicker->Pick( np.x, np.y, 0, _ren);
            setCellIds.insert( cellPicker->GetCellId());
        }   // end if
    }   // end foreach

    pickFrom->Delete();
    propPicker->Delete();
    cellPicker->Delete();

    cellIds.insert( cellIds.end(), setCellIds.begin(), setCellIds.end());
    return (int)setCellIds.size();
}   // end pickActorCells


// public
vtkActor* RendererPicker::pickActor( const cv::Point& p) const
{
    vtkPropPicker* propPicker = vtkPropPicker::New();
    const cv::Point np = changeOriginOfPoint( _ren, p, _pointOrigin);
    propPicker->Pick( np.x, np.y, 0, _ren);
    vtkActor* act = propPicker->GetActor();
    propPicker->Delete();
    return act;
}   // end pickActor


vtkPropCollection* createPropCollection( const std::vector<vtkActor*>& possActors)
{
    vtkPropCollection* pickFrom = vtkPropCollection::New();
    BOOST_FOREACH ( vtkActor* actor, possActors)
        pickFrom->AddItem(actor);
    return pickFrom;
}   // end createPropCollection


vtkPropCollection* createPropCollection( const std::vector<vtkSmartPointer<vtkActor> >& possActors)
{
    vtkPropCollection* pickFrom = vtkPropCollection::New();
    BOOST_FOREACH ( vtkSmartPointer<vtkActor> actor, possActors)
        pickFrom->AddItem(actor.GetPointer());
    return pickFrom;
}   // end createPropCollection


vtkActor* pick( const cv::Point& p, vtkPropCollection* pickFrom, vtkRenderer* ren, RendererPicker::PointOrigin po)
{
    const cv::Point np = changeOriginOfPoint( ren, p, po);
    vtkPropPicker* propPicker = vtkPropPicker::New();
    propPicker->PickProp( np.x, np.y, ren, pickFrom);
    vtkActor* act = propPicker->GetActor();
    pickFrom->Delete();
    propPicker->Delete();
    return act;
}   // end pick


// public
vtkActor* RendererPicker::pickActor( const cv::Point& p, const std::vector<vtkActor*>& possActors) const
{
    return pick( p, createPropCollection( possActors), _ren, _pointOrigin);
}   // end pickActor


// public
vtkSmartPointer<vtkActor> RendererPicker::pickActor( const cv::Point& p,
                                                     const std::vector<vtkSmartPointer<vtkActor> >& possActors) const
{
    return pick( p, createPropCollection( possActors), _ren, _pointOrigin);
}   // end pickActor


// public
int RendererPicker::pickCell( const cv::Point& p) const
{
    const cv::Point np = changeOriginOfPoint( _ren, p, _pointOrigin);
    vtkCellPicker* picker = vtkCellPicker::New();
    picker->SetTolerance( _tolerance);
    picker->Pick( np.x, np.y, 0, _ren);
    const int cid = picker->GetCellId();
    picker->Delete();
    return cid;

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


// public
cv::Vec3f RendererPicker::pickWorldPosition( const cv::Point& p) const
{
    const cv::Point np = changeOriginOfPoint( _ren, p, _pointOrigin);
    vtkWorldPointPicker* picker = vtkWorldPointPicker::New();   // Hardware accelerated
    picker->Pick( np.x, np.y, 0, _ren);
    const double* wpos = picker->GetPickPosition();
    const cv::Vec3f v( (float)wpos[0], (float)wpos[1], (float)wpos[2]);
    picker->Delete();
    return v;
}   // end pickWorldPosition



// public
cv::Vec3f RendererPicker::pickNormal( const cv::Point& p) const
{
    const cv::Point np = changeOriginOfPoint( _ren, p, _pointOrigin);
    vtkCellPicker* picker = vtkCellPicker::New();
    cv::Vec3f v(0,0,0);
    if ( picker->Pick( np.x, np.y, 0, _ren))
    {
        const double* normal = picker->GetPickNormal();
        v = cv::Vec3f( (float)normal[0], (float)normal[1], (float)normal[2]);
    }   // end if
    picker->Delete();
    return v;
}   // end pickNormal


// public
cv::Point RendererPicker::projectToImagePlane( const cv::Vec3f& v) const
{
    vtkCoordinate* coordConverter = vtkCoordinate::New();
    coordConverter->SetCoordinateSystemToWorld();
    coordConverter->SetValue( v[0], v[1], v[2]);
    const int* dpos = coordConverter->GetComputedDisplayValue( _ren);
    const cv::Point p( dpos[0], dpos[1]); // Bottom left origin
    coordConverter->Delete();
    return changeOriginOfPoint( _ren, p, _pointOrigin);
}   // end projectToImagePlane

