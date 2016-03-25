#pragma once
#ifndef RVTK_VTK_MODEL_H
#define RVTK_VTK_MODEL_H

#include "VtkTools.h"   // RVTK
#include "ClosestPointFinder.h" // RVTK
#include "VtkActorFactory.h"    // RVTK

namespace RVTK
{

// Available model types
enum rVTK_EXPORT ModelType
{
    NULL_MODEL, // No model selected or none available
    SURFACE_MODEL,
    GRADIENTS_MODEL,
    POINTS_MODEL,
    EDGES_MODEL,
    TEXTURED_MODEL
};  // end enum


class rVTK_EXPORT VtkModel
{
public:
    typedef boost::shared_ptr<VtkModel> Ptr;
    static Ptr create( const RFeatures::ObjModel::Ptr model);
    explicit VtkModel( const RFeatures::ObjModel::Ptr model);
    ~VtkModel();

    const RFeatures::ObjModel::Ptr getObjectModel() const { return _objModel;}

    const vtkSmartPointer<vtkActor> getSurfaceActor() const { return _basicActor;}
    const vtkSmartPointer<vtkActor> getTexturedActor() const { return _texturedActor;}
    const vtkSmartPointer<vtkActor> getCurrentActor() const { return _currentActor;}

    ModelType getCurrentModelType() const { return _currentModelType;}
    void setCurrentModel( ModelType);

    // Given a face index from the object model, colour the respective surface triangle.
    void colourObjectFace( int faceId, const cv::Vec4b& rgba);

    // Use this model's internal ClosestPointFinder to return the ObjModel indices
    // of the respective unique and non-unique vertices and faces.
    int getClosestObjUniqueVertexIdx( const cv::Vec3f&) const;
    int getClosestObjUniqueVertexIdx( const double p[3]) const;
    int getClosestObjVertexIdx( const cv::Vec3f&) const;
    int getClosestObjVertexIdx( const double p[3]) const;
    int getClosestObjFaceIdx( const cv::Vec3f&) const;
    int getClosestObjFaceIdx( const double p[3]) const;

    // Segment this VtkModel based on the given set of points that form a loop without
    // specifying every vertex in the loop and return a new model.
    // This function segments by creating a frustum between the front and back view planes.
    // If segmentation could not be performed, a null pointer is returned.
    //Ptr segmentOnContour( const vtkSmartPointer<vtkPoints>& boundaryPoints) const;

private:
    VtkActorFactory* _actorFactory;
    RFeatures::ObjModel::Ptr _objModel;
    vtkSmartPointer<vtkActor> _basicActor;
    vtkSmartPointer<vtkActor> _texturedActor;

    vtkSmartPointer<vtkActor> _currentActor;
    ModelType _currentModelType;

    // The basic colours for the surface (normally all white) but can
    // use colourObjectFace to colour a given face to a particular colour.
    vtkSmartPointer<vtkUnsignedCharArray> _basicColours;
    vtkSmartPointer<vtkUnsignedCharArray> _transColours;

    ClosestPointFinder* _unqcpf;    // Closest point finder for unique vertices
    ClosestPointFinder* _tcpf;      // Closest point finder for texture vertices
};  // end class

}   // end namespace

#endif

