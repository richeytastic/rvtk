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

#include <VtkActorCreator.h>
#include <VtkTools.h>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <vtkProperty.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <ObjModelTriangleMeshParser.h> // RFeatures
#include <VectorFloatKeyHashing.h>      // RFeatures
using RFeatures::ObjModel;
using RVTK::VtkActorCreator;


namespace {

vtkActor* makeActor( vtkSmartPointer<vtkPolyData> pd)
{
    /*
    pd = RVTK::generateNormals( pd);
    pd->BuildLinks();   // Required to use vtkPolyData::GetPointCells
    pd->BuildCells();
    */
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData( pd);
    vtkActor* actor = vtkActor::New();
    actor->SetMapper( mapper);
    return actor;
}   // end makeActor


void init()
{
    static bool initCalled = false;
    if (initCalled)
        return;
    initCalled = true;
    // Add static initialisation here...
    vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
}   // end init

}   // end namespace


// public
VtkActorCreator::VtkActorCreator()
    : _uvidxs(nullptr), _ufidxs(nullptr),
      _uvmappings(nullptr), _vmappings(nullptr), _ufmappings(nullptr), _fmappings(nullptr),
      _ruvmappings(nullptr), _rvmappings(nullptr), _rufmappings(nullptr), _rfmappings(nullptr)
{
    init();
}   // end ctor


// private
vtkSmartPointer<vtkPoints> VtkActorCreator::createVertices( const ObjModel* model, IntIntMap* uvmappings, vtkCellArray* vertices)
{
    uvmappings->clear();
    if ( _ruvmappings)
        _ruvmappings->clear();
    if ( _uvidxs)
        _uvidxs->clear();

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints( model->getNumVertices());

    int i = 0;
    const IntSet& vidxs = model->getVertexIds();
    for ( int vi : vidxs)
    {
        const cv::Vec3f& v = model->vtx(vi);
        points->SetPoint( i, &v[0]);
        vertices->InsertNextCell( 1);
        vertices->InsertCellPoint( i);

        (*uvmappings)[vi] = i; // Set the mapping of ObjModel vertices to VTK vertices
        if ( _ruvmappings)
            (*_ruvmappings)[i] = vi;
        if ( _uvidxs)
            _uvidxs->insert(i);
        i++;
    }   // end foreach
    return points;
}   // end createVertices


// private
vtkSmartPointer<vtkCellArray> VtkActorCreator::createPolygons( const ObjModel* model, const IntIntMap* uvmappings)
{
    int vtkid = 0;
    vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
    for ( int fid : model->getFaceIds())
    {
        const int* vidxs = model->getFaceVertices(fid);
        polys->InsertNextCell( 3);
        polys->InsertCellPoint( uvmappings->at( vidxs[0]));
        polys->InsertCellPoint( uvmappings->at( vidxs[1]));
        polys->InsertCellPoint( uvmappings->at( vidxs[2]));

        if ( _rufmappings)
            (*_rufmappings)[vtkid] = fid;
        if ( _ufmappings)
            (*_ufmappings)[fid] = vtkid;
        if ( _ufidxs)
            _ufidxs->insert(vtkid);

        vtkid++;
    }   // end for

    return polys;
}   // end createPolygons


// public
vtkActor* VtkActorCreator::generateSurfaceActor( const ObjModel* model)
{
    IntIntMap* uvmappings = _uvmappings;    // Provided memory
    bool dodelete = false;
    if ( !uvmappings)
    {
        dodelete = true;
        uvmappings = new IntIntMap;
    }   // end if

    vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkPoints> points = createVertices( model, uvmappings, vertices);
    vtkSmartPointer<vtkCellArray> faces = createPolygons( model, uvmappings);

    vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
    //pd->SetVerts( vertices);  // NOPE! Can't seem to get rid of the vertices when visualising!
    pd->SetPoints( points);
    pd->SetPolys( faces);
    if ( dodelete)
        delete uvmappings;

    return makeActor(pd);
}   // end generateSurfaceActor


namespace {

vtkSmartPointer<vtkPoints> createVerts( const std::vector<cv::Vec3f>& vtxs, vtkCellArray* vertices)
{
    const int n = (int)vtxs.size();
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints( n);
    for ( int i = 0; i < n; ++i)
    {
        points->SetPoint( i, &vtxs[i][0]);
        vertices->InsertNextCell(1);
        vertices->InsertCellPoint(i);
    }   // end for
    return points;
}   // end createVerts


vtkSmartPointer<vtkPoints> createLines( const std::vector<cv::Vec3f>& vtxs,
                                        vtkCellArray* vertices, vtkCellArray* lines,
                                        bool joinLoop)
{
    const int n = (int)vtxs.size();
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints( n);
    for ( int i = 0; i < n; ++i)
    {
        points->SetPoint( i, &vtxs[i][0]);
        vertices->InsertNextCell(1);
        vertices->InsertCellPoint(i);
    }   // end for

    if ( joinLoop)
    {
        lines->InsertNextCell(2);
        lines->InsertCellPoint(n-1);
        lines->InsertCellPoint(0);
    }   // end if

    for ( int i = 1; i < n; ++i)
    {
        lines->InsertNextCell(2);
        lines->InsertCellPoint(i-1);
        lines->InsertCellPoint(i);
    }   // end for

    return points;
}   // end createLines


vtkSmartPointer<vtkPoints> createLinePairs( const std::vector<cv::Vec3f>& lps,
                                        vtkCellArray* vertices, vtkCellArray* lines)
{
    const int n = (int)lps.size();
    assert( n % 2 == 0);
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints( n);
    for ( int i = 0; i < n; ++i)
    {
        points->SetPoint( i, &lps[i][0]);
        vertices->InsertNextCell(1);
        vertices->InsertCellPoint(i);
    }   // end for

    const int m = n/2;
    for ( int i = 0; i < m; ++i)
    {
        lines->InsertNextCell(2);
        lines->InsertCellPoint(2*i+0);
        lines->InsertCellPoint(2*i+1);
    }   // end for

    return points;
}   // end createLinePairs


}   // end namespace


// public static
vtkActor* VtkActorCreator::generatePointsActor( const std::vector<cv::Vec3f>& vtxs)
{
    vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkPoints> points = createVerts( vtxs, vertices);
    vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
    pd->SetVerts( vertices);
    pd->SetPoints( points);
    return makeActor(pd);
}   // end generatePointsActor


// public static
vtkActor* VtkActorCreator::generatePointsActor( const ObjModel* model)
{
    const IntSet& vidxs = model->getVertexIds();
    std::vector<cv::Vec3f> vtxs(vidxs.size());
    int i = 0;
    std::for_each( std::begin(vidxs), std::end(vidxs), [&](int vidx){ vtxs[i++] = model->vtx(vidx);});
    return generatePointsActor(vtxs);
}   // end generatePointsActor


// public static
vtkActor* VtkActorCreator::generateLineActor( const std::vector<cv::Vec3f>& vtxs, bool joinLoop)
{
    vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkPoints> points = createLines( vtxs, vertices, lines, joinLoop);
    vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
    pd->SetVerts( vertices);
    pd->SetPoints( points);
    pd->SetPolys( lines);
    return makeActor(pd);
}   // end generateLineActor


// public static
vtkActor* VtkActorCreator::generateLineActor( const std::list<cv::Vec3f>& vtxs, bool joinLoop)
{
    std::vector<cv::Vec3f> vpts(vtxs.begin(), vtxs.end());
    return generateLineActor( vpts, joinLoop);
}   // end generateLineActor


// public static
vtkActor* VtkActorCreator::generateLinePairsActor( const std::vector<cv::Vec3f>& lps)
{
    vtkSmartPointer<vtkCellArray> vertices = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkPoints> points = createLinePairs( lps, vertices, lines);
    vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
    pd->SetVerts( vertices);
    pd->SetPoints( points);
    pd->SetPolys( lines);
    return makeActor(pd);
}   // end generateLinePairsActor



namespace {

struct Mappings
{
private:
    bool _dodelete;

public:
    IntIntMap *vmappings;   // Obj to VTK vertex ID mappings
    IntIntMap *rvmappings;  // VTK to Obj vertex ID mappings
    IntIntMap *fmappings;   // Obj to VTK face ID mappings
    IntIntMap *rfmappings;  // VTK to Obj face ID mappings

    Mappings( IntIntMap* vm, IntIntMap* rvm, IntIntMap* fm, IntIntMap* rfm)
        : _dodelete(false), vmappings(vm), rvmappings(rvm), fmappings(fm), rfmappings(rfm)
    {
        if (!vmappings)
        {
            _dodelete = true;
            vmappings = new IntIntMap;
        }   // end if

        vmappings->clear();
        if ( rvmappings)
            rvmappings->clear();
        if ( fmappings)
            fmappings->clear();
        if ( rfmappings)
            rfmappings->clear();
    }   // end ctor

    ~Mappings()
    {
        if (_dodelete)  // Don't delete if vmappings set externally
            delete vmappings;
    }   // end dtor

    void mapVertexIndices( int objVtxId, int vtkPointId)
    {
        (*vmappings)[objVtxId] = vtkPointId;
        if ( rvmappings)
            (*rvmappings)[vtkPointId] = objVtxId;
    }   // end mapVertexIndices

    void mapFaceIndices( int objFaceId, int vtkFaceId)
    {
        if ( rfmappings)
            (*rfmappings)[vtkFaceId] = objFaceId;
        if ( fmappings)
            (*fmappings)[objFaceId] = vtkFaceId;
    }   // end mapFaceIndices
};  // end struct


vtkSmartPointer<vtkTexture> createMaterialTexture( const ObjModel* model, int mid)
{
    assert( model->getMaterialIds().count(mid) > 0);
    vtkSmartPointer<vtkTexture> tx;
    // Only take one of the texture maps
    const std::vector<cv::Mat>& ambient = model->getMaterialAmbient(mid);
    const std::vector<cv::Mat>& diffuse = model->getMaterialDiffuse(mid);
    const std::vector<cv::Mat>& specular = model->getMaterialSpecular(mid);
    if ( !diffuse.empty())  // Diffuse texture
        tx = RVTK::convertToTexture( diffuse[0]);
    else if ( !ambient.empty())  // Ambient texture
        tx = RVTK::convertToTexture( ambient[0]);
    else if ( !specular.empty()) // Specular texture
        tx = RVTK::convertToTexture( specular[0]);
    return tx;
}   // end createMaterialTexture


// VTK requires a one-to-one correspondence between geometric points and texture coordinates.
// SingleMaterialData encapsulates all of the one-to-one geometry with a texture map as
// defined by a ObjModel::Material struct.
struct SingleMaterialData
{
    vtkSmartPointer<vtkTexture> texture; // The material's texture (only 1 per actor currently)
    vtkSmartPointer<vtkFloatArray> uvs;  // Texture coords (|uvs| == |points|)
    vtkSmartPointer<vtkPoints> points;   // 3D vertices (may contain duplicates due to element pairing with uvs)
    vtkSmartPointer<vtkCellArray> faces; // Each cell element holds three integers which are the indices into points

    // Create the required VTK data from
    SingleMaterialData( const ObjModel* model, int mid, Mappings& mappings)
    {
        texture = createMaterialTexture( model, mid);
        faces = vtkSmartPointer<vtkCellArray>::New();
        points = vtkSmartPointer<vtkPoints>::New();
        uvs = vtkSmartPointer<vtkFloatArray>::New();
        uvs->SetNumberOfComponents(2);
 
        std::ostringstream oss;
        oss << "TCoords_" << mid;
        uvs->SetName( oss.str().c_str());

        int vtkPointId = 0;
        const IntSet& mfaceIds = model->getMaterialFaceIds(mid);
        for ( int fid : mfaceIds)
        {
            vtkIdType vtkFaceId = faces->InsertNextCell( 3);
            mappings.mapFaceIndices( fid, (int)vtkFaceId);

            const int* uvids = model->getFaceUVs(fid);
            const int* vtxs = model->getFaceVertices(fid);

            for ( int i = 0; i < 3; ++i)
            {
                const int vid = vtxs[i];
                const cv::Vec2f& uv = model->uv( mid, uvids[i]);
                mappings.mapVertexIndices( vid, vtkPointId);
                faces->InsertCellPoint( vtkPointId);
                points->InsertPoint( vtkPointId, &(model->vtx( vid)[0]));
                uvs->InsertTuple2( vtkPointId, uv[0], uv[1]);
                vtkPointId++;
            }   // end for
        }   // end foreach
    }   // end ctor
};  // end struct

}   // end namespace


// public
vtkActor* VtkActorCreator::generateActor( const ObjModel* model)
{
    if ( model->getNumMaterials() > 1)  // Can't create if more than one material!
    {
        std::cerr << "[ERROR] RVTK::VtkActorCreator::generateActor: Model has more than one material! Merge first." << std::endl;
        return nullptr;
    }   // end if

    if ( model->getNumMaterials() == 0)
    {
        std::cerr << "[INFO] RVTK::VtkActorCreator::generateActor: Model has no materials; generating surface actor." << std::endl;
        return generateSurfaceActor( model);
    }   // end if

    const int NP = 3*model->getNumFaces();
    Mappings mappings( _uvmappings, _ruvmappings, _ufmappings, _rufmappings);

    const int MID = *model->getMaterialIds().begin();   // The one and only material ID
    vtkSmartPointer<vtkCellArray> faces = vtkSmartPointer<vtkCellArray>::New();
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints( NP);
    vtkSmartPointer<vtkFloatArray> uvs = vtkSmartPointer<vtkFloatArray>::New();
    uvs->SetNumberOfComponents(2);
    uvs->SetNumberOfTuples( NP);
    uvs->SetName( "TCoords_0");

    int vtkPointId = 0;
    for ( int fid : model->getFaceIds())    // Over all polygons
    {
        mappings.mapFaceIndices( fid, (int)faces->InsertNextCell( 3));

        const int* vtxs = model->getFaceVertices(fid);
        const int* uvids = model->getFaceUVs(fid);
        if ( uvids)
        {
            for ( int i = 0; i < 3; ++i)
            {
                const int vid = vtxs[i];
                mappings.mapVertexIndices( vid, vtkPointId);
                faces->InsertCellPoint( vtkPointId);
                points->SetPoint( vtkPointId, &(model->vtx( vid)[0]));
                const cv::Vec2f& uv = model->uv( MID, uvids[i]);
                uvs->SetTuple2( vtkPointId, uv[0], uv[1]);
                vtkPointId++;
            }   // end for
        }   // end if
        else
        {
            for ( int i = 0; i < 3; ++i)
            {
                const int vid = vtxs[i];
                mappings.mapVertexIndices( vid, vtkPointId);
                faces->InsertCellPoint( vtkPointId);
                points->SetPoint( vtkPointId, &(model->vtx( vid)[0]));
                uvs->SetTuple2( vtkPointId, 0.0, 0.0);
                vtkPointId++;
            }   // end for
        }   // end else
    }   // end for

    vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
    pd->SetPoints( points);
    pd->SetPolys( faces);
    pd->GetPointData()->SetTCoords( uvs);  // Add texture coords to poly data

    vtkActor* actor = makeActor(pd);
    actor->SetTexture( createMaterialTexture( model, MID));

    // Set ambient lighting for proper texture lighting
    actor->GetProperty()->SetAmbient(1.0);
    actor->GetProperty()->SetDiffuse(0.0);
    actor->GetProperty()->SetSpecular(0.0);

    return actor;
}   // end generateActor



// public
size_t VtkActorCreator::generateTexturedActors( const ObjModel* model, std::vector<vtkActor*>& actors)
{
    actors.clear();

    if ( model->getNumMaterials() == 0) // Can't generate if no materials!
        return 0;

    Mappings mappings( _vmappings, _rvmappings, _fmappings, _rfmappings);

    const IntSet& materialIds = model->getMaterialIds();
    for ( int mid : materialIds)
    {
        SingleMaterialData smd( model, mid, mappings); // Read in the material data
        vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
        pd->SetPoints( smd.points);
        pd->SetPolys( smd.faces);
        // For multitexturing (useMultiTexturing true), may need to use AddArray instead of SetTCoords.
        // This would allow multitexturing over a single actor. However, in testing in VTK-7.1,
        // multi-texturing is not properly supported, and SingleMaterialData gets the geometric data
        // corresponding to a single texture mapped actor.
        //pd->GetPointData()->AddArray( smd.uvs);  // Add texture coords to poly data
        pd->GetPointData()->SetTCoords( smd.uvs);  // Add texture coords to poly data

        vtkActor* actor = makeActor(pd);
        /*************** Multitexturing code ***************************
        tmaps[0].texture->SetBlendingMode( vtkTexture::VTK_TEXTURE_BLENDING_MODE_REPLACE);
        for ( int i = 0; i < numMaterials; ++i)
        {
            vtkPolyDataMapper* mapper = vtkPolyDataMapper::SafeDownCast( actor->GetMapper());
            if ( i > 0)
                tmaps[i].texture->SetBlendingMode( vtkTexture::VTK_TEXTURE_BLENDING_MODE_ADD);
            mapper->MapDataArrayToMultiTextureAttribute( i, tmaps[i].uvs->GetName(), vtkDataObject::FIELD_ASSOCIATION_POINTS);
            actor->GetProperty()->SetTexture( i, tmaps[i].texture);
        }   // end for
        ****************************************************************/
        actor->SetTexture( smd.texture);

        // Set ambient lighting for proper texture lighting
        actor->GetProperty()->SetAmbient(1.0);
        actor->GetProperty()->SetDiffuse(0.0);
        actor->GetProperty()->SetSpecular(0.0);

        actors.push_back(actor);
    }   // end foreach

    // With all of the material actors made, we find the remaining geometry not accounted for
    // by the collection of SingleMaterialData instantiations, and create the final actor.
    //actors.insert( actors.end(), mactors.begin(), mactors.end());
    return actors.size();
}   // end generateTexturedActor
