#ifndef RVTK_VTK_MODEL_H__
#define RVTK_VTK_MODEL_H__

#include "VtkTools.h"   // RVTK
#include "ClosestPointFinder.h" // RVTK
#include "VtkActorFactory.h"    // RVTK

namespace RVTK
{

class rVTK_EXPORT VtkModel
{
public:
    typedef boost::shared_ptr<VtkModel> Ptr;
    static Ptr create( const RFeatures::ObjModel::Ptr model);
    explicit VtkModel( const RFeatures::ObjModel::Ptr model);
    ~VtkModel();

    const RFeatures::ObjModel::Ptr getObjectModel() const { return _actorFactory->getObjectModel();}
    const vtkSmartPointer<vtkActor> getSurfaceActor() const { return _basicActor;}
    const vtkSmartPointer<vtkActor> getTexturedActor() const { return _texturedActor;}

    // Use this model's internal ClosestPointFinder to return the ObjModel indices
    // of the respective unique and non-unique vertices and faces.
    int getClosestObjUniqueVertexIdx( const cv::Vec3f&) const;
    int getClosestObjUniqueVertexIdx( const double p[3]) const;
    int getClosestObjVertexIdx( const cv::Vec3f&) const;
    int getClosestObjVertexIdx( const double p[3]) const;
    int getClosestObjFaceIdx( const cv::Vec3f&) const;
    int getClosestObjFaceIdx( const double p[3]) const;

    // Get the closest point in space on the model (not necessarily a vertex).
    cv::Vec3f getClosestPoint( const cv::Vec3f&) const;

    // Straight up mapping to get the VTK basic surface face index
    // (i.e. not a texture polygon) from an object face index.
    int getVtkBasicIdxFromObjFaceIdx( int objFaceIdx) const;
    int getVtkIdxFromObjUniqueVtx( int objUvidx) const;

    int getObjIdxFromVtkUniqueVtx( int vtkUvidx) const;
    int getObjIdxFromVtkUniqueFace( int vtkFaceId) const;

    const boost::unordered_set<int>& getVtkUniqueVertexIds() const;
    const boost::unordered_set<int>& getVtkUniqueFaceIds() const;

private:
    VtkActorFactory* _actorFactory;
    vtkSmartPointer<vtkActor> _basicActor;
    vtkSmartPointer<vtkActor> _texturedActor;

    ClosestPointFinder* _unqcpf;    // Closest point finder for unique vertices
    ClosestPointFinder* _tcpf;      // Closest point finder for texture vertices
};  // end class

}   // end namespace

#endif

