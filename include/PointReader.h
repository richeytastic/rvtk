/**
 * Simple source object for reading points into VTK from a file with
 * a single point per line. Each line should be in the format:
 * X Y Z R G B
 * Where X,Y,Z are the coordinates of the point along those axes and
 * R,G,B are the pixel intensity values for those colours.
 *
 * Richard Palmer
 * June 2011
 */

#pragma once
#ifndef RVTK_POINT_READER_H
#define RVTK_POINT_READER_H

#include "VTKTypes.h"

namespace RVTK
{

class VTK_IO_EXPORT PointReader : public vtkPolyDataAlgorithm
{
public:
   static PointReader* New();
   vtkTypeRevisionMacro( PointReader, vtkPolyDataAlgorithm);
   void PrintSelf( ostream& os, vtkIndent indent);

   /**
    * Set/Get the name of the file from which to read points.
    */
   vtkSetStringMacro(FileName);
   vtkGetStringMacro(FileName);

protected:
   PointReader();
   ~PointReader();

   string filename;

   int RequestData( vtkInformation*, vtkInformationVector**, vtkInformationVector*);

private:
   // Copy construction not implemented
   PointReader( const PointReader&);
   void operator=( const PointReader&)
}; // end class

}   // end namespace

#endif
