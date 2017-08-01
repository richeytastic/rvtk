#ifndef RVTK_RENDERER_PICKER_H
#define RVTK_RENDERER_PICKER_H

#include <vector>
#include <opencv2/opencv.hpp>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include "rVTK_Export.h"

namespace RVTK
{

// An actor and a subset of its cells.
struct rVTK_EXPORT ActorSubset
{
    vtkActor* actor;
    std::vector<int> cellIds;
};  // end struct



class rVTK_EXPORT RendererPicker
{
public:
    enum PointOrigin
    {
        BOTTOM_LEFT,    // VTK render window - default
        TOP_LEFT        // OpenCV, Qt, and others
    };  // end enum

    // Use the PointOrigin parameter to let the picking algorithms know if the provided
    // points are given with a bottom left origin (VTK default) or a top left origin.
    RendererPicker( vtkRenderer*, PointOrigin po=BOTTOM_LEFT);

    // Given array of 2D pixel coordinates, find the actors and their cell IDs
    // pointed to by these locations. Returns number of actors found at the given 2D
    // coordinates (the number of entries appended to // output parameter picked).
    int pickActorCells( const std::vector<cv::Point>& points2d,
                        std::vector<ActorSubset>& picked) const;

    // Given array of 2D pixel coords and a specified actor, find the cell IDs
    // being addressed, returning the number appended to cellIds.
    int pickActorCells( const std::vector<cv::Point>& points2d,
                        vtkActor* actor,
                        std::vector<int>& cellIds) const;

    // Given a 2D point, find the actor being pointed to. Returns NULL if no actor found.
    vtkActor* pickActor( const cv::Point& point) const;

    // As above, but only selects the actor from the given vector of actors.
    vtkActor* pickActor( const cv::Point& point,
                         const std::vector<vtkActor*>& possActors) const;
    vtkSmartPointer<vtkActor> pickActor( const cv::Point& point,
                         const std::vector<vtkSmartPointer<vtkActor> >& possActors) const;

    // Pick an actor's cell returning its ID. Returns -1 if no cell found.
    int pickCell( const cv::Point& point) const;

    // Given a 2D point, find the intersecting world position. Note that a position
    // in space is always returned even if the point does not intersect with a prop/actor!
    cv::Vec3f pickWorldPosition( const cv::Point& point) const;

    // Pick the normal vector to the surface at the given point.
    // If no 3D point intersects, return the zero vector.
    cv::Vec3f pickNormal( const cv::Point& point) const;

    // Project v to the rendering image plane. Point is returned using
    // the coordinates origin set in the constructor.
    cv::Point projectToImagePlane( const cv::Vec3f& v) const;

private:
    vtkRenderer* _ren;
    PointOrigin _pointOrigin;
    const double _tolerance;
};  // end class

}   // end namespace

#endif
