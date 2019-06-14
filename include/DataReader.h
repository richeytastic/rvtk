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

/**
 * Generic data reader class for VTK.
 *
 * Richard Palmer
 * 24 May 2011
 */

#ifndef RVTK_DATA_READER_H
#define RVTK_DATA_READER_H
// Disable warnings about MSVC compiler not implementing exception specifications
#ifdef _WIN32
#pragma warning( disable : 4275)
#pragma warning( disable : 4290)
#endif

#include "rVTK_Export.h"
#include <string>
using std::string;
#include <exception>
#include "VTKTypes.h"


namespace RVTK {

class rVTK_EXPORT DataReaderException: public std::exception
{
public:
   DataReaderException( const string& invalidext);
   ~DataReaderException() throw(){}
   virtual const char* what() const throw();
private:
   string ext;
}; // end DataReaderException


class rVTK_EXPORT DataReader
{
public:
   DataReader( const string &fname) throw (DataReaderException);
   inline vtkAlgorithmOutput* GetOutputPort(){ return m_reader->GetOutputPort();}
private:
   vtkAlgorithm* m_reader;
}; // end DataReader

}   // end namespace


#endif
