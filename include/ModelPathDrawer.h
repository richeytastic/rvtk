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

#ifndef RVTK_MODEL_PATH_DRAWER_H
#define RVTK_MODEL_PATH_DRAWER_H

#include <vector>
#include <vtkObject.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkContourWidget.h>
#include <vtkObjectFactory.h>
#include <boost/shared_ptr.hpp>
#include "ClosestPointFinder.h"

namespace RVTK
{

// Public callback observer for ModelPathDrawer
class rVTK_EXPORT ModelPathEventObserver
{
public:
    virtual void startInteraction() {}
    virtual void endInteraction() {}
    virtual void interactionEvent() {}  // Moving after selecting something
};  // end class


class rVTK_EXPORT ModelPathDrawer
{
public:
    typedef boost::shared_ptr<ModelPathDrawer> Ptr;
    static Ptr create( const ClosestPointFinder*);
    ~ModelPathDrawer();

    void setInteractor( vtkRenderWindowInteractor*);
    void setEventObserver( ModelPathEventObserver*);

    // Set/get whether the path is closed (end points are joined to form a loop).
    void setClosed( bool closed) { _closedLoop = closed;}
    bool isClosed() const { return _closedLoop;}

    // Set/get the boundary handles.
    void setPathHandles( const std::vector<cv::Vec3f>&);
    int getPathHandles( std::vector<cv::Vec3f>&) const; // Returns number of handles.
    int getNumHandles() const;  // Returns the number of handles

    // Get all of the boundary vertices (nodes plus intermediate points).
    int getAllPathVertices( std::vector<cv::Vec3f>&) const;

    void setVisibility( bool visible);
    bool getVisibility() const;

private:
    vtkSmartPointer<vtkContourWidget> _cWidget;
    bool _closedLoop;

    ModelPathDrawer();
    ModelPathDrawer( const ModelPathDrawer&);   // No copy
    void operator=( const ModelPathDrawer&);    // No copy

    void setModel( const ClosestPointFinder*);
};  // end class

}   // end namespace

#endif
