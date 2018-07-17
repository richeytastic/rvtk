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

#include <ScalarLegend.h>
using RVTK::ScalarLegend;
#include <vtkTextProperty.h>
#include <vtkScalarBarActor.h>
#include <vtkColor.h>
#include <sstream>


ScalarLegend::ScalarLegend( vtkRenderWindowInteractor* rwint)
{
    _widget = vtkSmartPointer<vtkScalarBarWidget>::New();
    vtkScalarBarActor* legend = _widget->GetScalarBarActor();
    _lut = vtkSmartPointer<vtkLookupTable>::New();
    legend->SetLookupTable(_lut);

    legend->GetLabelTextProperty()->SetFontFamilyToCourier();
    legend->GetLabelTextProperty()->SetItalic(false);
    legend->GetLabelTextProperty()->SetFontSize(14);
    legend->SetNumberOfLabels( 11);
    legend->SetMaximumWidthInPixels( 80);
    legend->SetUnconstrainedFontSize(true);
    legend->DrawTickLabelsOn();
    legend->SetPosition(0.61, 0.3);
    legend->SetHeight(0.65);

    _widget->SetInteractor(rwint);
    _widget->SetRepositionable(false);
}   // end dtor


// public
void ScalarLegend::setTitle( const std::string& title)
{
    vtkScalarBarActor* legend = _widget->GetScalarBarActor();
    legend->GetTitleTextProperty()->SetFontFamilyToCourier();
    legend->GetTitleTextProperty()->SetFontSize(16);
    legend->GetTitleTextProperty()->SetBold(false);
    legend->GetTitleTextProperty()->SetItalic(false);
    legend->SetTitle( title.c_str());
}   // end setTitle


// public
void ScalarLegend::setColours( const vtkColor3ub& scol, const vtkColor3ub& fcol, size_t ncols)
{
    assert( ncols > 0);
    if ( ncols < 2)
        ncols = 2;

    // Map lookup colours to the lookup table
    _lut->SetNumberOfTableValues( (int)ncols);
    _lut->Build();

    float cstep[3] = {0,0,0};
    if ( ncols > 1)
    {
        cstep[0] = (float(fcol[0]) - float(scol[0]))/(ncols-1);
        cstep[1] = (float(fcol[1]) - float(scol[1]))/(ncols-1);
        cstep[2] = (float(fcol[2]) - float(scol[2]))/(ncols-1);
    }   // end if

    float rgb[3] = {0,0,0};
    for ( size_t i = 0; i < ncols; ++i)
    {
        rgb[0] = (scol[0] + i*cstep[0])/255.0f;
        rgb[1] = (scol[1] + i*cstep[1])/255.0f;
        rgb[2] = (scol[2] + i*cstep[2])/255.0f;
        _lut->SetTableValue( i, rgb[0], rgb[1], rgb[2], 1);
    }   // end for
}   // end setColours


// public
void ScalarLegend::setColours( const vtkColor3ub& scol, const vtkColor3ub& mcol, const vtkColor3ub& fcol, size_t ncols0, size_t ncols1)
{
    assert( ncols0 > 0);
    if ( ncols1 == 0)
    {
        ncols1 = ncols0/2;
        ncols0 = ncols0 - ncols1;
    }   // end if

    const size_t totCols = ncols0 + ncols1;
    _lut->SetNumberOfTableValues( (int)totCols);
    _lut->Build();

    float cstep[3] = {0,0,0};
    if ( ncols0 > 1)
    {
        cstep[0] = (float(mcol[0]) - float(scol[0]))/(ncols0-1);
        cstep[1] = (float(mcol[1]) - float(scol[1]))/(ncols0-1);
        cstep[2] = (float(mcol[2]) - float(scol[2]))/(ncols0-1);
    }   // end if

    float rgb[3] {0,0,0};

    // First half
    for ( size_t i = 0; i < ncols0; ++i)
    {
        rgb[0] = (scol[0] + i*cstep[0])/255.0f;
        rgb[1] = (scol[1] + i*cstep[1])/255.0f;
        rgb[2] = (scol[2] + i*cstep[2])/255.0f;
        _lut->SetTableValue( i, rgb[0], rgb[1], rgb[2], 1);
    }   // end for

    cstep[0] = 0;
    cstep[1] = 0;
    cstep[2] = 0;
    if ( ncols1 > 1)
    {
        cstep[0] = (float(fcol[0]) - float(mcol[0]))/(ncols1-1);
        cstep[1] = (float(fcol[1]) - float(mcol[1]))/(ncols1-1);
        cstep[2] = (float(fcol[2]) - float(mcol[2]))/(ncols1-1);
    }   // end if

    for ( size_t i = ncols0, j = 0; i < totCols; ++i, ++j)
    {
        rgb[0] = (mcol[0] + j*cstep[0])/255.0f;
        rgb[1] = (mcol[1] + j*cstep[1])/255.0f;
        rgb[2] = (mcol[2] + j*cstep[2])/255.0f;
        _lut->SetTableValue( i, rgb[0], rgb[1], rgb[2], 1);
    }   // end for
}   // end setColours


// public
void ScalarLegend::setLookupTable( vtkMapper* mapper, float minv, float maxv)
{
    const int maxWidth = 4;
    int ndecimals = std::max<int>(0, maxWidth - (int)(logf(maxv - minv) + 1));
    ndecimals = std::min<int>(ndecimals, maxWidth-1);

    vtkScalarBarActor* legend = _widget->GetScalarBarActor();
    std::ostringstream oss;
    oss << "% " << maxWidth << "." << ndecimals << "f";
    legend->SetLabelFormat(oss.str().c_str());
    mapper->SetScalarRange( minv, maxv);
    mapper->SetLookupTable( _lut);
}   // end setLookupTable


// public
size_t ScalarLegend::getNumColours() const { return (size_t)_lut->GetNumberOfTableValues();}
bool ScalarLegend::isVisible() const { return _widget->GetEnabled() > 0;}
void ScalarLegend::setVisible( bool visible) { _widget->SetEnabled(visible);}
