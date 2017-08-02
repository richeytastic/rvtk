#include "ScalarLegend.h"
using RVTK::ScalarLegend;
#include <vtkTextProperty.h>
#include <vtkColor.h>
#include <sstream>


ScalarLegend::ScalarLegend( vtkRenderer* r) : _ren(r)
{
    _legend = vtkScalarBarActor::New();
    _lut = vtkLookupTable::New();
    _legend->SetLookupTable(_lut);

    _legend->GetLabelTextProperty()->SetFontFamilyToArial();
    _legend->GetLabelTextProperty()->SetItalic(false);
    _legend->GetLabelTextProperty()->SetFontSize(10);
    _legend->SetNumberOfLabels( 10);
    _legend->SetMaximumWidthInPixels( 60);
    _legend->SetUnconstrainedFontSize(true);
    _legend->DrawTickLabelsOn();
    _legend->SetPosition(0.87, 0.3);
    _legend->SetHeight(0.6);
}   // end dtor


ScalarLegend::~ScalarLegend()
{
    _legend->Delete();
    _lut->Delete();
}   // end dtor


// public
void ScalarLegend::setTitle( const std::string& title)
{
    _legend->GetTitleTextProperty()->SetFontFamilyToTimes();
    _legend->GetTitleTextProperty()->SetFontSize(10);
    _legend->GetTitleTextProperty()->SetBold(false);
    _legend->GetTitleTextProperty()->SetItalic(false);
    _legend->SetTitle( title.c_str());
}   // end setTitle


// public
void ScalarLegend::setColours( int ncols, const vtkColor3ub& scol, const vtkColor3ub& fcol)
{
    // Map lookup colours to the lookup table
    _lut->SetNumberOfTableValues( ncols);
    _lut->Build();

    float cstep[3];
    cstep[0] = (float(fcol[0]) - float(scol[0]))/(ncols-1);
    cstep[1] = (float(fcol[1]) - float(scol[1]))/(ncols-1);
    cstep[2] = (float(fcol[2]) - float(scol[2]))/(ncols-1);
    for ( int i = 0; i < ncols; ++i)
    {
        float rgb[3];
        rgb[0] = (scol[0] + i*cstep[0])/255.0;
        rgb[1] = (scol[1] + i*cstep[1])/255.0;
        rgb[2] = (scol[2] + i*cstep[2])/255.0;
        _lut->SetTableValue( i, rgb[0], rgb[1], rgb[2], 1);
    }   // end for
}   // end setColours


// public
void ScalarLegend::setLookupTable( vtkMapper* m, float minv, float maxv)
{
    const int maxWidth = 6;
    int ndecimals = maxWidth - (int)(logf(maxv - minv) + 1);
    ndecimals = std::min<int>(ndecimals, maxWidth-1);

    std::ostringstream oss;
    oss << "% " << maxWidth << "." << ndecimals << "f";
    _legend->SetLabelFormat(oss.str().c_str());
    m->SetScalarRange( minv, maxv);
    m->SetLookupTable( _lut);
}   // end setLookupTable


// public
void ScalarLegend::show()
{
    _ren->AddActor2D( _legend);
}   // end show


// public
void ScalarLegend::hide()
{
    _ren->RemoveActor2D( _legend);
}   // end hide

