#pragma once
#ifndef RVTK_MODEL_PATH_DRAWER_H
#define RVTK_MODEL_PATH_DRAWER_H

#include "VtkModel.h"   // RVTK
#include <vtkRenderer.h>
#include <vtkContourWidget.h>

namespace RVTK
{

// Public callback observer for ModelPathDrawer
class InteractionObserver
{
public:
    virtual void callback() = 0;
};  // end clas


class MBInteractionObserver;    // Used internally only

class rVTK_EXPORT ModelPathDrawer
{
public:
    typedef boost::shared_ptr<ModelPathDrawer> Ptr;
    static Ptr create( const vtkSmartPointer<vtkRenderer>);

    ~ModelPathDrawer();

    void setInteractor( const vtkSmartPointer<vtkRenderWindowInteractor>);
    void setModel( const RVTK::VtkModel::Ptr);

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

    // Set an optional callback for when user has ended interaction with the model bounder.
    void setEndInteractionObserver( InteractionObserver* io);

private:
    const vtkSmartPointer<vtkRenderer> _renderer;
    vtkSmartPointer<vtkContourWidget> _contourWidget;
    bool _closedLoop;

    RVTK::VtkModel::Ptr _model;
    friend class MBInteractionObserver;
    vtkSmartPointer<MBInteractionObserver> _iobserver;  // Internal VTK observer
    InteractionObserver* _endInteractionObserver;

    explicit ModelPathDrawer( const vtkSmartPointer<vtkRenderer>);
    ModelPathDrawer( const ModelPathDrawer&); // Not implemented
    void operator=( const ModelPathDrawer&);   // Not implemented
};  // end class


// Define an object to listen to events on ModelPathDrawer objects
class MBInteractionObserver : public vtkObject
{
public:
    static MBInteractionObserver* New();
    vtkTypeMacro( MBInteractionObserver, vtkObject);

    void setModelPathDrawer( const ModelPathDrawer*);

    void endInteractionCallback();

protected:
    MBInteractionObserver();
    virtual ~MBInteractionObserver();

private:
    const ModelPathDrawer *_mb;

    // Not implemented
    MBInteractionObserver( const MBInteractionObserver&);
    void operator=( const MBInteractionObserver&);
};  // end class

}   // end namespace

#endif
