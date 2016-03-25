#include "VtkActorFactory.h"
using RVTK::VtkActorFactory;
using RVTK::ObjectFaceValuer;
#include <cassert>
#include <algorithm>
#include <cmath>


vtkSmartPointer<vtkActor> makeActor( vtkSmartPointer<vtkPolyData> pd)
{
    pd->BuildLinks();   // Required to use vtkPolyData::GetPointCells
    pd = RVTK::generateNormals( pd);
    vtkSmartPointer<vtkPolyDataMapper> mapper = RVTK::createMapper( pd);
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper( mapper);
    return actor;
}   // end makeActor


// public static
void VtkActorFactory::init()
{
    vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
}   // end init


// public
VtkActorFactory::VtkActorFactory( const RFeatures::ObjModel::Ptr& model) : _omodel(model)
{}   // end ctor


// public
vtkSmartPointer<vtkActor> VtkActorFactory::generateBasicActor()
{
    vtkSmartPointer<vtkPoints> upoints = generateUniqueVertices();
    vtkSmartPointer<vtkCellArray> ufaces = generateUniqueVertexFaces();
    vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
    pd->SetPoints( upoints);
    pd->SetPolys( ufaces);
    return makeActor(pd);
}   // end generateBasicActor


// public
vtkSmartPointer<vtkActor> VtkActorFactory::generateTexturedActor()
{
    vtkSmartPointer<vtkFloatArray> uvs;
    vtkSmartPointer<vtkPoints> tpoints = generateNonUniqueVertices( uvs);
    vtkSmartPointer<vtkCellArray> tfaces = generateNonUniqueVertexFaces();
    vtkSmartPointer<vtkTexture> texture = RVTK::convertToTexture( _omodel->getTexture());
    // Since VTK requires one-to-one correspondence of geometric vertices and texture
    // coordinates, we must generate a new set of data that includes duplicate vertices.
    // A BETTER way for VTK to map textures, would be to associate texture coordinates
    // with polygon (cell) data (which would reduce the number of polygons required
    // for the textured object). Unfortunately, this is not possible.
    vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
    pd->SetPoints( tpoints);
    pd->SetPolys( tfaces);
    pd->GetPointData()->SetTCoords( uvs);
    vtkSmartPointer<vtkActor> actor = makeActor( pd);
    actor->SetTexture( texture);
    return actor;
}   // end generateTexturedActor


// public
vtkSmartPointer<vtkFloatArray> VtkActorFactory::createFaceLookupTableIndices( const ObjectFaceValuer& ofv,
                                                    int numVals, double minVal, double maxVal)
{
    const IntSet& faceIds = _omodel->getFaceIds();
    const double drng = maxVal - minVal;

    const float maxIdx = numVals - 1;

    vtkSmartPointer<vtkFloatArray> faceLookupTableIdxs = vtkSmartPointer<vtkFloatArray>::New();
    faceLookupTableIdxs->SetNumberOfValues( _ufmappings.size());

    typedef std::pair<int, int> IntsPair;
    BOOST_FOREACH ( const IntsPair& fmap, _ufmappings)
    {
        const double d = ofv.getFaceValue(fmap.first);

        float v = (d - minVal)/drng;
        if ( v < 0.0)
            v = 0.0;
        if ( v > 1.0)
            v = 1.0;
        float cidx = cvRound(v * maxIdx);
        faceLookupTableIdxs->SetValue( fmap.second, cidx);
    }   // end for

    return faceLookupTableIdxs;
}   // end createFaceLookupTableIndices

    /*
    vtkSmartPointer<vtkCurvatures> curveFilter = vtkSmartPointer<vtkCurvatures>::New();
#if VTK_MAJOR_VERSION == 6
    curveFilter->SetInputData( trianglesPolyData);
#else
    curveFilter->SetInput( trianglesPolyData);
#endif
    curveFilter->SetCurvatureTypeToMean();
    curveFilter->Update();
    mapper->SetInputConnection( curveFilter->GetOutputPort());
    */

// private
vtkSmartPointer<vtkPoints> VtkActorFactory::generateUniqueVertices()
{
    vtkSmartPointer<vtkPoints> upoints = vtkSmartPointer<vtkPoints>::New();
    upoints->SetNumberOfPoints( _omodel->getNumUniqueVertices());
    // Set the mapping of ObjModel unique vertices to VTK vertices
    const IntSet& uvidxs = _omodel->getUniqueVertexIds();
    _uvmappings.clear();
    _ruvmappings.clear();
    int i = 0;
    BOOST_FOREACH ( const int& ui, uvidxs)
    {
        const cv::Vec3f& uv = _omodel->getUniqueVertex(ui);
        upoints->SetPoint( i, &uv[0]);
        _ruvmappings[i] = ui;
        _uvmappings[ui] = i++;
    }   // end foreach
    return upoints;
}   // end generateUniqueVertices


// private
vtkSmartPointer<vtkCellArray> VtkActorFactory::generateUniqueVertexFaces()
{
    vtkSmartPointer<vtkCellArray> ufaces = vtkSmartPointer<vtkCellArray>::New();
    const IntSet& fids = _omodel->getFaceIds();
    _ufmappings.clear();
    _rufmappings.clear();
    int i = 0;
    BOOST_FOREACH ( const int& fid, fids)
    {
        const RFeatures::ObjPoly& uface = _omodel->getUniqueVertexFace(fid);
        ufaces->InsertNextCell( 3);
        ufaces->InsertCellPoint( _uvmappings[uface.vindices[0]]);
        ufaces->InsertCellPoint( _uvmappings[uface.vindices[1]]);
        ufaces->InsertCellPoint( _uvmappings[uface.vindices[2]]);
        _rufmappings[i] = fid;
        _ufmappings[fid] = i++;
    }   // end for
    return ufaces;
}   // end generateUniqueVertexFaces


// private
vtkSmartPointer<vtkPoints> VtkActorFactory::generateNonUniqueVertices( vtkSmartPointer<vtkFloatArray>& uvs)
{
    const int nvs = _omodel->getNumVertices();
    vtkSmartPointer<vtkPoints> tpoints = vtkSmartPointer<vtkPoints>::New();
    tpoints->SetNumberOfPoints( nvs);
    uvs = vtkSmartPointer<vtkFloatArray>::New();
    uvs->SetNumberOfComponents(2);
    uvs->SetNumberOfTuples( nvs);
    uvs->SetName("TextureCoordinates");

    _vmappings.clear();
    _rvmappings.clear();
    const IntSet& vidxs = _omodel->getVertexIds();
    int i = 0;
    BOOST_FOREACH ( const int& vi, vidxs)
    {
        const cv::Vec3f& v = _omodel->getVertex(vi);
        const cv::Vec2f& uv = _omodel->getTextureOffset(vi);
        tpoints->SetPoint( i, &v[0]);
        uvs->SetTuple2( i, uv[0], uv[1]);
        _rvmappings[i] = vi;
        _vmappings[vi] = i++;
    }   // end foreach
    return tpoints;
}   // end generateNonUniqueVertices


// private
vtkSmartPointer<vtkCellArray> VtkActorFactory::generateNonUniqueVertexFaces()
{
    vtkSmartPointer<vtkCellArray> tfaces = vtkSmartPointer<vtkCellArray>::New();
    const IntSet& fids = _omodel->getFaceIds();
    _fmappings.clear();
    _rfmappings.clear();
    int i = 0;
    BOOST_FOREACH ( const int& fid, fids)
    {
        const RFeatures::ObjPoly& face = _omodel->getFace(fid);
        tfaces->InsertNextCell( 3);
        tfaces->InsertCellPoint( _vmappings[face.vindices[0]]);
        tfaces->InsertCellPoint( _vmappings[face.vindices[1]]);
        tfaces->InsertCellPoint( _vmappings[face.vindices[2]]);
        _rfmappings[i] = fid;
        _fmappings[fid] = i++;
    }   // end for
    return tfaces;
}   // end generateNonUniqueVertexFaces

