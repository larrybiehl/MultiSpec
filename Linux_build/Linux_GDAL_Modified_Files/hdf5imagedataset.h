/******************************************************************************
 * $Id: hdf5imagedataset.h 23035 2011-09-04 10:28:41Z antonio $ Larry Biehl
 * This file was created to be able to use with the get compression information
 * routine in g5_convenience.cpp
 *
 * Project:  Hierarchical Data Format Release 5 (HDF5)
 * Purpose:  Header file for HDF5 datasets reader.
 * Author:   Denis Nadeau (denis.nadeau@gmail.com)
 *
 ******************************************************************************
 * Copyright (c) 2005, Frank Warmerdam <warmerdam@pobox.com>
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

#ifndef _HDF5IMAGEDATASET_H_INCLUDED_
#define _HDF5IMAGEDATASET_H_INCLUDED_

#include "ogr_spatialref.h"
#include "hdf5dataset.h"

class HDF5ImageDataset : public HDF5Dataset
{
    typedef enum
    {
        UNKNOWN_PRODUCT=0,
        CSK_PRODUCT
    } Hdf5ProductType;

    typedef enum
    {
        PROD_UNKNOWN=0,
        PROD_CSK_L0,
        PROD_CSK_L1A,
        PROD_CSK_L1B,
        PROD_CSK_L1C,
        PROD_CSK_L1D
    } HDF5CSKProductEnum;

    friend class HDF5ImageRasterBand;

    char        *pszProjection;
    char        *pszGCPProjection;
    GDAL_GCP    *pasGCPList;
    int         nGCPCount;
    OGRSpatialReference oSRS;

    hsize_t      *dims,*maxdims;
    HDF5GroupObjects *poH5Objects;
    int          ndims,dimensions;
	 int			  nCompressionCode;	// Added by Larry Biehl 4/27/2012
    hid_t        dataset_id;
    hid_t        dataspace_id;
    hsize_t      size;
    haddr_t      address;
    hid_t        datatype;
    hid_t        native;
    H5T_class_t  clas;
    Hdf5ProductType    iSubdatasetType;
    HDF5CSKProductEnum iCSKProductType;
    double       adfGeoTransform[6];
    bool         bHasGeoTransform;
    
    CPLErr CreateODIMH5Projection();

public:
    HDF5ImageDataset();
    ~HDF5ImageDataset();

    CPLErr CreateProjections( );
    static GDALDataset  *Open( GDALOpenInfo * );
    static int           Identify( GDALOpenInfo * );

    const char          *GetProjectionRef();
    virtual int         GetGCPCount( );
    virtual const char  *GetGCPProjection();
    virtual const GDAL_GCP *GetGCPs( );  
	 virtual const int GetCompressionCode (); 	// Added by Larry Biehl
    virtual CPLErr GetGeoTransform( double * padfTransform );

    Hdf5ProductType GetSubdatasetType() const {return iSubdatasetType;}
    HDF5CSKProductEnum GetCSKProductType() const {return iCSKProductType;}

    int     IsComplexCSKL1A() const
    {
        return (GetSubdatasetType() == CSK_PRODUCT) &&
               (GetCSKProductType() == PROD_CSK_L1A) &&
               (ndims == 3);
    }
    int     GetYIndex() const { return IsComplexCSKL1A() ? 0 : ndims - 2; }
    int     GetXIndex() const { return IsComplexCSKL1A() ? 1 : ndims - 1; }

    /**
     * Identify if the subdataset has a known product format
     * It stores a product identifier in iSubdatasetType,
     * UNKNOWN_PRODUCT, if it isn't a recognizable format.
     */
    void IdentifyProductType();

    /**
     * Captures Geolocation information from a COSMO-SKYMED
     * file.
     * The geoid will allways be WGS84
     * The projection type may be UTM or UPS, depending on the
     * latitude from the center of the image.
     * @param iProductType type of HDF5 subproduct, see HDF5CSKProduct
     */
    void CaptureCSKGeolocation(int iProductType);

    /**
    * Get Geotransform information for COSMO-SKYMED files
    * In case of sucess it stores the transformation
    * in adfGeoTransform. In case of failure it doesn't
    * modify adfGeoTransform
    * @param iProductType type of HDF5 subproduct, see HDF5CSKProduct
    */
    void CaptureCSKGeoTransform(int iProductType);
    
    /**
     * @param iProductType type of HDF5 subproduct, see HDF5CSKProduct
     */
    void CaptureCSKGCPs(int iProductType);  
};

#endif /* _HDF5IMAGEDATASET_H_INCLUDED_ */

