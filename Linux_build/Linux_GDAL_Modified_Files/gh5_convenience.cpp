/******************************************************************************
 * $Id: gh5_convenience.cpp 26010 2013-05-17 23:31:00Z warmerdam $
 *
 * Project:  Hierarchical Data Format Release 5 (HDF5)
 * Purpose:  HDF5 convenience functions.
 * Author:   Frank Warmerdam <warmerdam@pobox.com>
 *
 ******************************************************************************
 * Copyright (c) 2009, Frank Warmerdam <warmerdam@pobox.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

#if defined multispec_lin
	#if defined NetBeansProject
		/* The size of a 'unsigned long', as computed by sizeof. */
		#define SIZEOF_UNSIGNED_LONG 8
		/* The size of a 'void p', as computed by sizeof. */
		#define SIZEOF_VOIDP 8
	#endif
#endif

#include "gh5_convenience.h"
#include <hdf5imagedataset.h>		// Larry Biehl 5/5/2012
//#include "bagdataset.h"				// Larry Biehl 5/5/2012

CPL_CVSID("$Id: gh5_convenience.cpp 26010 2013-05-17 23:31:00Z warmerdam $");

/************************************************************************/
/*                    GH5_FetchAttribute(CPLString)                     */
/************************************************************************/

bool GH5_FetchAttribute( hid_t loc_id, const char *pszAttrName, 
                        CPLString &osResult, bool bReportError )

{
    bool retVal = false;

    hid_t hAttr = H5Aopen_name( loc_id, pszAttrName );

    osResult.clear();

    if( hAttr < 0 )
    {
        if( bReportError )
            CPLError( CE_Failure, CPLE_AppDefined,
                      "Attempt to read attribute %s failed, not found.",
                      pszAttrName );
        return false;
    }

    hid_t hAttrTypeID      = H5Aget_type( hAttr );
    hid_t hAttrNativeType  = H5Tget_native_type( hAttrTypeID, H5T_DIR_DEFAULT );

    if( H5Tget_class( hAttrNativeType ) == H5T_STRING ) 
    {
	int nAttrSize = H5Tget_size( hAttrTypeID );
        char *pachBuffer = (char *) CPLCalloc(nAttrSize+1,1);
	H5Aread( hAttr, hAttrNativeType, pachBuffer );
        
        osResult = pachBuffer;
        CPLFree( pachBuffer );

        retVal = true;
    }

    else
    {
        if( bReportError )
            CPLError( CE_Failure, CPLE_AppDefined,
                      "Attribute %s of unsupported type for conversion to string.",
                      pszAttrName );

        retVal = false;
    }

    H5Tclose( hAttrNativeType );
    H5Tclose( hAttrTypeID );
    H5Aclose( hAttr );
    return retVal;
}

/************************************************************************/
/*                      GH5_FetchAttribute(double)                      */
/************************************************************************/

bool GH5_FetchAttribute( hid_t loc_id, const char *pszAttrName, 
                         double &dfResult, bool bReportError )

{
    hid_t hAttr = H5Aopen_name( loc_id, pszAttrName );

    dfResult = 0.0;
    if( hAttr < 0 )
    {
        if( bReportError )
            CPLError( CE_Failure, CPLE_AppDefined,
                      "Attempt to read attribute %s failed, not found.",
                      pszAttrName );
        return false;
    }

    hid_t hAttrTypeID      = H5Aget_type( hAttr );
    hid_t hAttrNativeType  = H5Tget_native_type( hAttrTypeID, H5T_DIR_DEFAULT );

/* -------------------------------------------------------------------- */
/*      Confirm that we have a single element value.                    */
/* -------------------------------------------------------------------- */

    hid_t hAttrSpace       = H5Aget_space( hAttr );
    hsize_t anSize[64];
    int nAttrDims       = H5Sget_simple_extent_dims( hAttrSpace, anSize, NULL );
    
    int i, nAttrElements = 1;

    for( i=0; i < nAttrDims; i++ ) {
        nAttrElements *= (int) anSize[i];
    }

    if( nAttrElements != 1 )
    {
        if( bReportError )
            CPLError( CE_Failure, CPLE_AppDefined,
                      "Attempt to read attribute %s failed, count=%d, not 1.",
                      pszAttrName, nAttrElements );

        H5Sclose( hAttrSpace );
        H5Tclose( hAttrNativeType );
        H5Tclose( hAttrTypeID );
        H5Aclose( hAttr );
        return false;
    }
    
/* -------------------------------------------------------------------- */
/*      Read the value.                                                 */
/* -------------------------------------------------------------------- */
    void *buf = (void *)CPLMalloc( H5Tget_size( hAttrNativeType ));
    H5Aread( hAttr, hAttrNativeType, buf );

/* -------------------------------------------------------------------- */
/*      Translate to double.                                            */
/* -------------------------------------------------------------------- */
    if( H5Tequal( H5T_NATIVE_INT, hAttrNativeType ) )
        dfResult = *((int *) buf);
    else if( H5Tequal( H5T_NATIVE_FLOAT,    hAttrNativeType ) )
        dfResult = *((float *) buf);
    else if( H5Tequal( H5T_NATIVE_DOUBLE,    hAttrNativeType ) )
        dfResult = *((double *) buf);
    else
    {
        if( bReportError )
            CPLError( CE_Failure, CPLE_AppDefined,
                      "Attribute %s of unsupported type for conversion to double.",
                      pszAttrName );
        CPLFree( buf );

        H5Sclose( hAttrSpace );
        H5Tclose( hAttrNativeType );
        H5Tclose( hAttrTypeID );
        H5Aclose( hAttr );

        return false;
    }

    CPLFree( buf );

    H5Sclose( hAttrSpace );
    H5Tclose( hAttrNativeType );
    H5Tclose( hAttrTypeID );
    H5Aclose( hAttr );
    return true;
}


// Following section added by Larry Biehl
//------------------------------------------------------------------------------------
//								 Copyright (1988-2020)
//								© Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		int GH5_GetCompressionInformation
//
//	Software purpose:	This routine reads determines the type of compression used for
//							the input data set.
//
//	Parameters in:					
//
//	Parameters out:	
//
//	Value Returned:	
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/30/2012
//	Revised By:			Larry L. Biehl			Date: 05/01/2012

int GH5_GetCompressionInformation (
				hid_t									dataset_id)

{
	hid_t									propertyList_id;
											
	int									compressionCode = 0,
											numberFilters;
											
//	unsigned int						cd_values;
 	

	if (dataset_id > 0)
		{
		propertyList_id = H5Dget_create_plist (dataset_id);
		
		if (propertyList_id > 0)
			{
			numberFilters = H5Pget_nfilters (propertyList_id);
			
			if (numberFilters > 0)
				{
						// Note that it is possible for there to be more that one
						// compression scheme used on a given data set. This code will
						// only return the information for the first filter in the list
						// in those cases.
				
//				cd_values = 0;
				compressionCode = H5Pget_filter2 (propertyList_id, 0, NULL, NULL, 0, 0, NULL, NULL);
				
				}		// end "if (numberFilters > 0)"
		
			H5Pclose (propertyList_id);
			
			}		// end "if (propertyList_id > 0)"
		
		}		// end "if (dataset_id > 0)"
		
	return (compressionCode);
    
}		// end "GH5_GetCompressionInformation"



int GH5_GetDataSetCompressionCode (
				GDALDatasetH					hDS)
{
	char									*fileFormatDescriptionPtr;
	
	
	GDALDriverH dataSetDriver = GDALGetDatasetDriver (hDS);
	fileFormatDescriptionPtr = (char*)GDALGetDescription ((GDALMajorObjectH)dataSetDriver);
		
	//if (strcmp (fileFormatDescriptionPtr, "BAG\0") == 0)
	//	return ((BAGDataset*)hDS)->GetCompressionCode();
		
	//else		// must be hdf5 image data set
		return ((HDF5ImageDataset*)hDS)->GetCompressionCode();
	
}		// end "GH5_GetDataSetCompressionCode"
// end section added by Larry Biehl

/************************************************************************/
/*                          GH5_GetDataType()                           */
/*                                                                      */
/*      Transform HDF5 datatype to GDAL datatype                        */
/************************************************************************/
GDALDataType GH5_GetDataType(hid_t TypeID) 
{
    if( H5Tequal( H5T_NATIVE_CHAR,        TypeID ) )
	return GDT_Byte;
    else if( H5Tequal( H5T_NATIVE_SCHAR,  TypeID ) ) 
	return GDT_Byte;
    else if( H5Tequal( H5T_NATIVE_UCHAR,  TypeID ) ) 
	return GDT_Byte;
    else if( H5Tequal( H5T_NATIVE_SHORT,  TypeID ) )
	return GDT_Int16;
    else if( H5Tequal( H5T_NATIVE_USHORT, TypeID ) ) 
	return GDT_UInt16;
    else if( H5Tequal( H5T_NATIVE_INT,    TypeID ) ) 
	return GDT_Int32;      
    else if( H5Tequal( H5T_NATIVE_UINT,   TypeID ) ) 
	return GDT_UInt32;
    else if( H5Tequal( H5T_NATIVE_LONG,   TypeID ) ) 
    {
        if( sizeof(long) == 4 )
            return GDT_Int32;      
        else
            return GDT_Unknown;
    }
    else if( H5Tequal( H5T_NATIVE_ULONG,  TypeID ) ) 
    {
        if( sizeof(unsigned long) == 4 )
            return GDT_UInt32;      
        else
            return GDT_Unknown;
    }
    else if( H5Tequal( H5T_NATIVE_FLOAT,  TypeID ) ) 
	return GDT_Float32;
    else if( H5Tequal( H5T_NATIVE_DOUBLE, TypeID ) ) 
	return GDT_Float64;
    else if( H5Tequal( H5T_NATIVE_LLONG,  TypeID ) ) 
	return GDT_Unknown;
    else if( H5Tequal( H5T_NATIVE_ULLONG, TypeID ) ) 
	return GDT_Unknown;
    else if( H5Tequal( H5T_NATIVE_DOUBLE, TypeID ) ) 
	return GDT_Unknown;

    return GDT_Unknown;
}

