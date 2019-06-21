/************************************************************************
 * Copyright (C) 2019 Richard Palmer
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

#include <OffscreenModelViewer.h>
#include <VtkActorCreator.h>
#include <ImageGrabber.h>
using RVTK::OffscreenModelViewer;
using RFeatures::CameraParams;


OffscreenModelViewer::OffscreenModelViewer( const cv::Size& dims, float rng)
    : _actor(nullptr), _picker(nullptr)
{
    _viewer = Viewer::create(true/*offscreen*/);
    _viewer->renderer()->UseFXAAOn();
    setSize(dims);
    setCamera( CameraParams( cv::Vec3f( 0, 0, rng)));
}   // end ctor


OffscreenModelViewer::~OffscreenModelViewer()
{
    clear();
    if ( _picker)
        delete _picker;
}   // end dtor


void OffscreenModelViewer::setSize( const cv::Size& dims)
{
    _viewer->setSize( static_cast<size_t>(dims.width), static_cast<size_t>(dims.height));
    setCamera( _viewer->camera());  // Refresh
}   // end setSize


void OffscreenModelViewer::clear()
{
    if ( _actor)
    {
        _viewer->removeActor(_actor);
        _actor->Delete();
        _actor = nullptr;
        setCamera( _viewer->camera());  // Refresh
    }   // end if
}   // end clear


void OffscreenModelViewer::setModel( const RFeatures::ObjModel& model)
{
    clear();
    // Create the actor
    vtkSmartPointer<vtkTexture> texture;
    _actor = VtkActorCreator::generateActor( model, texture);
    _viewer->addActor( _actor);
    setCamera( _viewer->camera());  // Refresh
}   // end setModel


void OffscreenModelViewer::setCamera( const CameraParams& cp)
{
    _viewer->setCamera( cp);
    _viewer->resetClippingRange();
    _viewer->updateRender();
}   // end setCamera


cv::Mat_<cv::Vec3b> OffscreenModelViewer::snapshot() const
{
    ImageGrabber ig(*_viewer);
    return ig.colour();
}   // end snapshot


cv::Mat_<byte> OffscreenModelViewer::lightnessSnapshot() const
{
    ImageGrabber ig(*_viewer);
    return ig.light();
}   // end lightnessSnapshot


bool OffscreenModelViewer::pick( const cv::Point2f& p) const
{
    return picker()->pickActor(p) != nullptr;
}   // end pick


cv::Vec3f OffscreenModelViewer::worldPosition( const cv::Point2f& p) const
{
    return picker()->pickWorldPosition(p);
}   // end worldPosition


cv::Point2f OffscreenModelViewer::imagePlane( const cv::Vec3f& v) const
{
    const cv::Size sz = _viewer->size();
    cv::Point p = picker()->projectToImagePlane(v);
    return cv::Point2f( float(p.x)/sz.width, float(p.y)/sz.height);
}   // end imagePlane


RVTK::RendererPicker* OffscreenModelViewer::picker() const
{
    if ( !_picker)
        _picker = new RendererPicker( _viewer->renderer(), RendererPicker::TOP_LEFT);
    return _picker;
}   // end picker
