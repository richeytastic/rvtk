#ifndef RVTK_LINE_PLOTTER_H
#define RVTK_LINE_PLOTTER_H
#include "VTKTypes.h"

namespace RVTK
{

class rVTK_EXPORT LinePlotter
{
public:
	LinePlotter( double min = 0.0, double max = 1.0, int lineWidth = 1, vtkLookupTable* lookupTable = NULL);
   ~LinePlotter();

	void plotLine( double m[3], double n[3], double scalar);
	void plotLine( double x, double y, double z, double x2, double y2, double z2, double scalar);
	
	vtkActor* createActor() const;
	vtkPolyData* createPolyData() const;

private:
	double m_scalarMin, m_scalarMax ;
	int m_curPointID ;
	int m_allLineWidth ;
    bool m_newtable;

	vtkLookupTable* m_lookupTable ;
	vtkPoints* m_points;
	vtkCellArray* m_lines;
	vtkFloatArray* m_lineScalars ;
}; // end class LinePlotter

}   // end namespace

#endif
