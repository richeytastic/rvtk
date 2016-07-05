#include "VtkModel.h"
using RVTK::VtkModel;
using RFeatures::ObjModel;
#include <vtkSelectPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkImplicitSelectionLoop.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <iostream>
#include <boost/foreach.hpp>


// public static
VtkModel::Ptr VtkModel::create(const ObjModel::Ptr model)
{
    return VtkModel::Ptr( new VtkModel(model));
}   // end create


// private
VtkModel::VtkModel( const ObjModel::Ptr model)
    : _actorFactory( new VtkActorFactory(model))
{
    _basicActor = _actorFactory->generateBasicActor();
    _basicActor->GetProperty()->SetLineWidth(0.7);  // OpenGL only
    _basicActor->GetProperty()->SetPointSize(1.3);
    //_basicActor->GetProperty()->SetInterpolationToPhong();

    _texturedActor = _actorFactory->generateTexturedActor();

    /*
    const int ncols = 500;
    const vtkColor3ub scol( 255, 255,   0);
    const vtkColor3ub fcol( 0,     0, 255);
    vtkSmartPointer<vtkLookupTable> coloursLut = RVTK::createColoursLookupTable( ncols, scol, fcol);
    RVTK::setLookupTable( _basicActor, coloursLut);

    // Set the curvature information on the actor
    CurvFaceValuer cfv( model);
    vtkSmartPointer<vtkFloatArray> cvals = _actorFactory->createFaceLookupTableIndices( &cfv);
    cvals->SetName( "Curvature");
    RVTK::getPolyData(_basicActor)->GetCellData()->AddArray( cvals);

    // Set the colours (white) for the basic actor
    const int ncells = RVTK::getPolyData( _basicActor)->GetPolys()->GetNumberOfCells();
                    
    _basicColours = vtkSmartPointer<vtkUnsignedCharArray>::New();
    _transColours = vtkSmartPointer<vtkUnsignedCharArray>::New();
    _basicColours->SetNumberOfComponents(4);
    _transColours->SetNumberOfComponents(4);
    _basicColours->SetName("BaseColours");
    _transColours->SetName("Transparency");
    const unsigned char ucwhite[4] = {255,255,255,255}; // R,G,B,Opacity
    const unsigned char uctrans[4] = {255,255,255,45}; // R,G,B,Opacity
    for ( int i = 0; i < ncells; ++i)
    {
        _basicColours->InsertNextTupleValue( ucwhite);
        _transColours->InsertNextTupleValue( uctrans);
    }   // end for

    RVTK::getPolyData( _basicActor)->GetCellData()->AddArray( _basicColours);
    RVTK::getPolyData( _basicActor)->GetCellData()->AddArray( _transColours);
    */

    _unqcpf = new RVTK::ClosestPointFinder( RVTK::getPolyData( _basicActor));
    _tcpf = new RVTK::ClosestPointFinder( RVTK::getPolyData( _texturedActor));
}   // end ctor


// public
VtkModel::~VtkModel()
{
    delete _tcpf;
    delete _unqcpf;
    delete _actorFactory;
}   // end dtor


// public
int VtkModel::getVtkBasicIdxFromObjFaceIdx( int objFaceIdx) const
{
    return _actorFactory->getVtkBasicIdxFromObjFaceIdx( objFaceIdx);
}   // end getVtkBasicIdxFromObjFaceIdx


// public
int VtkModel::getVtkIdxFromObjUniqueVtx( int objUvidx) const
{
    return _actorFactory->getVtkIdxFromObjUniqueVtx( objUvidx);
}   // end getVtkIdxFromObjUniqueVtx


// public
int VtkModel::getObjIdxFromVtkUniqueVtx( int vtkUvidx) const
{
    return _actorFactory->getObjIdxFromVtkUniqueVtx( vtkUvidx);
}   // end getObjIdxFromVtkUniqueVtx


// public
int VtkModel::getObjIdxFromVtkUniqueFace( int vtkFaceIdx) const
{
    return _actorFactory->getObjIdxFromVtkUniqueFace( vtkFaceIdx);
}   // end getObjIdxFromVtkUniqueFace


// public
int VtkModel::getClosestObjUniqueVertexIdx( const double v[3]) const
{
    const int uvtkid = _unqcpf->getClosestVertex(v);
    return _actorFactory->getObjIdxFromVtkUniqueVtx( uvtkid);
}   // end getClosestObjUniqueVertexIdx

// public
int VtkModel::getClosestObjUniqueVertexIdx( const cv::Vec3f& v) const
{
    const int uvtkid = _unqcpf->getClosestVertex(v);
    return _actorFactory->getObjIdxFromVtkUniqueVtx( uvtkid);
}   // end getClosestObjUniqueVertexIdx

// public
int VtkModel::getClosestObjVertexIdx( const double v[3]) const
{
    const int vtkid = _tcpf->getClosestVertex(v);
    return _actorFactory->getObjIdxFromVtkVtx( vtkid);
}   // end getClosestObjVertexIdx

// public
int VtkModel::getClosestObjVertexIdx( const cv::Vec3f& v) const
{
    double v0[3] = {v[0], v[1], v[2]};
    return getClosestObjVertexIdx(v0);
}   // end getClosestObjVertexIdx

// public
int VtkModel::getClosestObjFaceIdx( const double v[3]) const
{
    const int cid = _unqcpf->getClosestCell(v);
    return _actorFactory->getObjIdxFromVtkUniqueFace( cid);
}   // end getClosestObjFaceIdx

// public
int VtkModel::getClosestObjFaceIdx( const cv::Vec3f& v) const
{
    double v0[3] = {v[0], v[1], v[2]};
    return getClosestObjFaceIdx(v0);
}   // end getClosestObjFaceIdx


// public
cv::Vec3f VtkModel::getClosestPoint( const cv::Vec3f& v) const
{
    cv::Vec3f outv;
    _unqcpf->getClosestCell( v, &outv);
    return outv;
}   // end getClosestPoint


// public
const boost::unordered_set<int>& VtkModel::getVtkUniqueVertexIds() const
{
    return _actorFactory->getVtkUniqueVertexIds();
}   // end getVtkUniqueVertexIds


// public
const boost::unordered_set<int>& VtkModel::getVtkUniqueFaceIds() const
{
    return _actorFactory->getVtkUniqueFaceIds();
}   // end getVtkUniqueFaceIds



/*
// public
void VtkModel::colourObjectFace( int fid, const cv::Vec4b& rgba)
{
    // Get the VTK poly ID for the correct face on the _basicActor
    const int cid = _actorFactory->getVtkBasicIdxFromObjFaceIdx(fid);
    const unsigned char ucol[4] = {rgba[0],rgba[1],rgba[2],rgba[3]}; // R,G,B,Opacity
    _transColours->SetTupleValue( cid, ucol);
}   // end colourObjectFace
*/


/*
// public
VtkModel::Ptr VtkModel::segmentOnContour( const vtkSmartPointer<vtkPoints>& boundaryPts) const
{
    vtkPolyData* pdata = RVTK::getPolyData( _basicActor);

    vtkSmartPointer<vtkImplicitSelectionLoop> isloop = vtkSmartPointer<vtkImplicitSelectionLoop>::New();
    isloop->SetLoop( boundaryPts);

    vtkSmartPointer<vtkExtractPolyDataGeometry> gclipper = vtkSmartPointer<vtkExtractPolyDataGeometry>::New();
#if VTK_MAJOR_VERSION == 6
    gclipper->SetInputData(pdata);
#else
    gclipper->SetInput(pdata);
#endif
    gclipper->SetImplicitFunction( isloop);
    gclipper->Update();
    pdata = gclipper->GetOutput();

    const int numCells = pdata->GetNumberOfCells();
    if ( numCells <= 0)
        return VtkModel::Ptr(); // Unable to perform segmentation

    // Get the unique vertex indices from the extracted cells
    std::vector<int> uniqVtxIds;
    const int numUniqVtxs = (int)_objModel->getNumUniqueVertices();
    for ( int i = 0; i < numCells; ++i)
    {
        vtkCell* cell = pdata->GetCell(i);
        assert( cell->GetNumberOfPoints() == 3);

        const int u0 = cell->GetPointId(0);
        const int u1 = cell->GetPointId(1);
        const int u2 = cell->GetPointId(2);
        assert( u0 >= 0 && u0 < numUniqVtxs);
        assert( u1 >= 0 && u1 < numUniqVtxs);
        assert( u2 >= 0 && u2 < numUniqVtxs);

        uniqVtxIds.push_back(u0);
        uniqVtxIds.push_back(u1);
        uniqVtxIds.push_back(u2);
    }   // end for

    ObjModel::Ptr obj = _objModel->createFromUniqueVertices( uniqVtxIds);
    if ( !obj)
        return VtkModel::Ptr();

    // Use only the largest component of obj
    RFeatures::ObjComponentFinder ocf( obj);
    const std::list<ObjModel::Ptr>& components = ocf.find();
    std::cout << "Object components: " << components.size() << std::endl;
    const ObjModel::Ptr cmodel = *components.begin();
    std::vector<double> faceCurvature;
    cmodel->calculateCurvature(faceCurvature);
    return VtkModel::create( cmodel, faceCurvature);
}   // end segmentOnContour
*/

