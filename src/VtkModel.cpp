#include "VtkModel.h"
using RVTK::VtkModel;
using RFeatures::ObjModel;
#include <vtkSelectPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkExtractPolyDataGeometry.h>
#include <vtkImplicitSelectionLoop.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <iostream>


// public static
VtkModel::Ptr VtkModel::create(const ObjModel::Ptr model)
{
    return VtkModel::Ptr( new VtkModel(model));
}   // end create


class CurvFaceValuer : public RVTK::ObjectFaceValuer
{
public:
    explicit CurvFaceValuer( const ObjModel::Ptr om) : _omodel(om) {}

    virtual double getFaceValue( int faceId) const
    {
        return _omodel->getFaceCurvature( faceId);
    }   // end getFaceValue

private:
    const ObjModel::Ptr _omodel;
};  // end class


// private
VtkModel::VtkModel( const ObjModel::Ptr model)
    : _actorFactory( new VtkActorFactory(model)), _objModel(model)
{
    _basicActor = _actorFactory->generateBasicActor();
    _basicActor->GetProperty()->SetLineWidth(0.7);  // OpenGL only
    _basicActor->GetProperty()->SetPointSize(1.3);
    //_basicActor->GetProperty()->SetInterpolationToPhong();

    _texturedActor = _actorFactory->generateTexturedActor();

    const int ncols = 500;
    const vtkColor3ub scol( 255, 255,   0);
    const vtkColor3ub fcol( 0,     0, 255);
    vtkSmartPointer<vtkLookupTable> coloursLut = RVTK::createColoursLookupTable( ncols, scol, fcol);
    RVTK::setLookupTable( _basicActor, coloursLut);

    // Set the curvature information on the actor
    CurvFaceValuer cfv( model);
    vtkSmartPointer<vtkFloatArray> cvals
        = _actorFactory->createFaceLookupTableIndices( cfv, ncols, -0.2, 0.2);
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

    // Default actor is the textured actor
    _currentModelType = RVTK::TEXTURED_MODEL;
    _currentActor = _texturedActor;

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
int VtkModel::getClosestObjUniqueVertexIdx( const double v[3]) const
{
    const int uvtkid = _unqcpf->getClosestVertex(v);
    // Translate to object index using the actor factory
    return _actorFactory->getObjIdxFromVtkUniqueVtx( uvtkid);
}   // end getClosestObjUniqueVertexIdx

// public
int VtkModel::getClosestObjUniqueVertexIdx( const cv::Vec3f& v) const
{
    double v0[3] = {v[0], v[1], v[2]};
    return getClosestObjUniqueVertexIdx(v0);
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
void VtkModel::setCurrentModel( RVTK::ModelType mt)
{
    switch ( mt)
    {
        case RVTK::POINTS_MODEL:
            RVTK::getPolyData( _basicActor)->GetCellData()->SetActiveScalars( "BaseColours");
            _basicActor->GetProperty()->SetRepresentationToPoints();
            _basicActor->GetProperty()->SetBackfaceCulling(0);
            break;
        case RVTK::EDGES_MODEL:
            RVTK::getPolyData( _basicActor)->GetCellData()->SetActiveScalars( "Transparency");
            _basicActor->GetProperty()->SetRepresentationToSurface();
            _basicActor->GetProperty()->SetEdgeVisibility(1);
            _basicActor->GetProperty()->SetEdgeColor(1.0,1.0,1);
            _basicActor->GetProperty()->SetBackfaceCulling(0);
            break;
        case RVTK::SURFACE_MODEL:
            RVTK::getPolyData( _basicActor)->GetCellData()->SetActiveScalars( "BaseColours");
            _basicActor->GetProperty()->SetRepresentationToSurface();
            _basicActor->GetProperty()->SetEdgeVisibility(0);
            _basicActor->GetProperty()->SetBackfaceCulling(1);
            break;
        case RVTK::GRADIENTS_MODEL:
            RVTK::getPolyData( _basicActor)->GetCellData()->SetActiveScalars( "Curvature");
            _basicActor->GetProperty()->SetBackfaceCulling(1);
            break;
        case RVTK::TEXTURED_MODEL:
            _texturedActor->GetProperty()->SetBackfaceCulling(1);
            break;
    }   // end switch

    if ( mt == RVTK::NULL_MODEL)
        _currentActor = NULL;
    else if ( mt == RVTK::TEXTURED_MODEL)
        _currentActor = _texturedActor;
    else
        _currentActor = _basicActor;

    _currentModelType = mt;
}   // end setCurrentModel


// public
void VtkModel::colourObjectFace( int fid, const cv::Vec4b& rgba)
{
    // Get the VTK poly ID for the correct face on the _basicActor
    const int cid = _actorFactory->getVtkBasicIdxFromObjFaceIdx(fid);
    const unsigned char ucol[4] = {rgba[0],rgba[1],rgba[2],rgba[3]}; // R,G,B,Opacity
    _transColours->SetTupleValue( cid, ucol);
    setCurrentModel( _currentModelType);
}   // end colourObjectFace


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

