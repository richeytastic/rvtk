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

#include "DataReader.h"
using namespace RVTK;

#include <vtkBYUReader.h>
#include <vtkPolyDataReader.h>
#include <sstream>
using std::ostringstream;


DataReaderException::DataReaderException( const string& invalidext) : ext(invalidext){}


const char* DataReaderException::what() const throw()
{
   ostringstream oss;
   oss << "Files with extension " << ext << " are not supported by DataReaders!";
   return oss.str().c_str();
}  // end what



DataReader::DataReader( const string &fname) throw(DataReaderException)
{
   string ext = fname.substr(fname.rfind('.'));

   if (ext.compare( ".g") == 0)
   {
      vtkBYUReader* r = vtkBYUReader::New();
      r->SetFileName( fname.c_str());
      m_reader = r;
   }  // end if
   else if (ext.compare( ".vtk") == 0)
   {
      vtkPolyDataReader* r = vtkPolyDataReader::New();
      r->SetFileName( fname.c_str());
      m_reader = r;
   }  // end else
   else
   {
      throw DataReaderException(ext);
   }  // end else
}  // end ctor

