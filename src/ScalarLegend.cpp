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
#include <vtkColor.h>
#include <sstream>


ScalarLegend::ScalarLegend( vtkRenderer* r) : _ren(r)
{
    _legend = vtkScalarBarActor::New();
    _lut = vtkLookupTable::New();
    _legend->SetLookupTable(_lut);

    _legend->GetLabelTextProperty()->SetFontFamilyToCourier();
    _legend->GetLabelTextProperty()->SetItalic(false);
    _legend->GetLabelTextProperty()->SetFontSize(14);
    _legend->SetNumberOfLabels( 11);
    _legend->SetMaximumWidthInPixels( 80);
    _legend->SetUnconstrainedFontSize(true);
    _legend->DrawTickLabelsOn();
    _legend->SetPosition(0.81, 0.3);
    _legend->SetHeight(0.65);

    _shown = false;
}   // end dtor


ScalarLegend::~ScalarLegend()
{
    _legend->Delete();
    _lut->Delete();
}   // end dtor


// public
void ScalarLegend::setTitle( const std::string& title)
{
    _legend->GetTitleTextProperty()->SetFontFamilyToCourier();
    _legend->GetTitleTextProperty()->SetFontSize(16);
    _legend->GetTitleTextProperty()->SetBold(false);
    _legend->GetTitleTextProperty()->SetItalic(false);
    _legend->SetTitle( title.c_str());
}   // end setTitle


// public
void ScalarLegend::setColours( const vtkColor3ub& scol, const vtkColor3ub& fcol, int ncols)
{
    assert( ncols > 0);
    if ( ncols < 2)
        ncols = 2;

    // Map lookup colours to the lookup table
    _lut->SetNumberOfTableValues( ncols);
    _lut->Build();

    float cstep[3] = {0,0,0};
    if ( ncols > 1)
    {
        cstep[0] = (float(fcol[0]) - float(scol[0]))/(ncols-1);
        cstep[1] = (float(fcol[1]) - float(scol[1]))/(ncols-1);
        cstep[2] = (float(fcol[2]) - float(scol[2]))/(ncols-1);
    }   // end if

    float rgb[3] = {0,0,0};
    for ( int i = 0; i < ncols; ++i)
    {
        rgb[0] = (scol[0] + i*cstep[0])/255.0f;
        rgb[1] = (scol[1] + i*cstep[1])/255.0f;
        rgb[2] = (scol[2] + i*cstep[2])/255.0f;
        _lut->SetTableValue( i, rgb[0], rgb[1], rgb[2], 1);
    }   // end for
}   // end setColours


// public
void ScalarLegend::setColours( const vtkColor3ub& scol, const vtkColor3ub& mcol, const vtkColor3ub& fcol, int ncols0, int ncols1)
{
    assert( ncols0 > 0);
    if ( ncols1 == 0)
    {
        ncols1 = ncols0/2;
        ncols0 = ncols0 - ncols1;
    }   // end if

    const int totCols = ncols0 + ncols1;
    _lut->SetNumberOfTableValues( totCols);
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
    for ( int i = 0; i < ncols0; ++i)
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

    for ( int i = ncols0, j = 0; i < totCols; ++i, ++j)
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
    int ndecimals = maxWidth - (int)(logf(maxv - minv) + 1);
    ndecimals = std::min<int>(ndecimals, maxWidth-1);

    std::ostringstream oss;
    oss << "% " << maxWidth << "." << ndecimals << "f";
    _legend->SetLabelFormat(oss.str().c_str());
    mapper->SetScalarRange( minv, maxv);
    mapper->SetLookupTable( _lut);
}   // end setLookupTable


// public
int ScalarLegend::getNumColours() const
{
    return _lut->GetNumberOfTableValues();
}   // end getNumColours


// public
void ScalarLegend::show()
{
    _ren->AddActor2D( _legend);
    _shown = true;
}   // end show


// public
void ScalarLegend::hide()
{
    _ren->RemoveActor2D( _legend);
    _shown = false;
}   // end hide


// public
bool ScalarLegend::isShown() const
{
    return _shown;
}   // end isShown

