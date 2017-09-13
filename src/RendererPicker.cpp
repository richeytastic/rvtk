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


// private
cv::Point RendererPicker::toPxls( const cv::Point2f& p) const { return cv::Point( p.x * _ren->GetSize()[0], p.y * _ren->GetSize()[1]);}


// public
RendererPicker::RendererPicker( vtkRenderer* ren, PointOrigin po, double t) : _ren(ren), _pointOrigin(po), _tolerance(t){}


namespace {
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
}   // end namespace


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


// public
vtkActor* RendererPicker::pickActor( const cv::Point2f& p) const { return pickActor( toPxls(p));}


namespace {
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
}   // end namespace



// public
vtkActor* RendererPicker::pickActor( const cv::Point& p, const std::vector<vtkActor*>& possActors) const
{
    return pick( p, createPropCollection( possActors), _ren, _pointOrigin);
}   // end pickActor


// public
vtkActor* RendererPicker::pickActor( const cv::Point2f& p, const std::vector<vtkActor*>& possActors) const { return pickActor( toPxls(p), possActors);} 


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
}   // end pickCell


// public
int RendererPicker::pickCell( const cv::Point2f& p) const { return pickCell( toPxls(p));}


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
cv::Vec3f RendererPicker::pickWorldPosition( const cv::Point2f& p) const { return pickWorldPosition( toPxls(p));}



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
cv::Vec3f RendererPicker::pickNormal( const cv::Point2f& p) const { return pickNormal( toPxls(p));}


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

