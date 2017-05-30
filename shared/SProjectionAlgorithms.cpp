//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//
//	File:						SProjectionAlgorithms.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		3.0
//
//	Revision date:			12/22/2015
//
//	Language:				C
//
//	System:					Macintosh and Windows Operating Systems
//
//	Brief description:	This file contains functions that handle conversions
//								between line/columns and map coordinates.
//
//	Diagram of MultiSpec routine calls for the routines in the file.
//
//	Functions in file:	Boolean				ConvertLambertAzimuthalEqualAreaToLatLong
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
  
#include "SMulSpec.h"

#if defined multispec_mac
	
#endif	// defined multispec_mac   
                             
#if defined multispec_win
	 
#endif	// defined multispec_win 

#include	"SExtGlob.h"	

extern double 			msfnz (
								double								eccent,
								double								sinphi,
								double								cosphi);
								
extern double			tsfnz(
								double								eccent,
								double								phi,
								double								sinphi);			

extern void				ConvertMapRectByGivenFactor (
								double								factor,
								DoubleRect*							coordinateRectPtr);



			// Prototypes for routines in this file that are only called by		
			// other routines in this file.													
			
			// Function to adjust longitude to -180 to 180 
double					adjust_lon(
								double								x); 

double 					asinz (
								double								con);				

Boolean 					ConvertAlbersEqualAreaToLatLong (
								MapProjectionInfoPtr				mapProjectionInfoPtr, 
								double* 								xCoordinateValuePtr, 
								double* 								yCoordinateValuePtr);

Boolean 					ConvertCylindricalEqualAreaToLatLong (
								MapProjectionInfoPtr				mapProjectionInfoPtr, 
								double* 								xCoordinateValuePtr, 
								double* 								yCoordinateValuePtr);

SInt16 					ConvertDecimalDegreesToDMS (
								double								decimalDegrees,
								SInt16*								degreesPtr,
								SInt16*								minutesPtr,
								double*								secondsPtr,
								Boolean*								outOfRangeFlagPtr);

Boolean 					ConvertEquirectangularToLatLong (
								MapProjectionInfoPtr				mapProjectionInfoPtr,
								double* 								xCoordinateValuePtr, 
								double* 								yCoordinateValuePtr);			

Boolean 					ConvertKrovakToLatLong (
								MapProjectionInfoPtr				mapProjectionInfoPtr, 
								double* 								xCoordinateValuePtr, 
								double* 								yCoordinateValuePtr);

Boolean 					ConvertLambertAzimuthalEqualAreaToLatLong  (
								MapProjectionInfoPtr				mapProjectionInfoPtr, 
								double* 								xCoordinateValuePtr, 
								double* 								yCoordinateValuePtr);		

Boolean					ConvertLambertConformalConicToLatLong (
								MapProjectionInfoPtr				mapProjectionInfoPtr,
								double* 								xCoordinateValuePtr, 
								double* 								yCoordinateValuePtr);			

Boolean 					ConvertOrthographicToLatLong (
								MapProjectionInfoPtr				mapProjectionInfoPtr, 
								double* 								xCoordinateValuePtr, 
								double* 								yCoordinateValuePtr);		

Boolean					ConvertPolarStereographicToLatLong (
								MapProjectionInfoPtr				mapProjectionInfoPtr, 
								double* 								xCoordinateValuePtr, 
								double* 								yCoordinateValuePtr);		

Boolean 					ConvertSinusoidalToLatLong (
								MapProjectionInfoPtr				mapProjectionInfoPtr, 
								double* 								xCoordinateValuePtr, 
								double* 								yCoordinateValuePtr);

Boolean 					ConvertTransverseMercatorToLatLong (
								MapProjectionInfoPtr				mapProjectionInfoPtr, 
								double* 								xCoordinateValuePtr, 
								double* 								yCoordinateValuePtr);	

Boolean 					ConvertLatLongToLambertAzimuthalEqualArea (
								MapProjectionInfoPtr				mapProjectionInfoPtr,
								double* 								xCoordinateValuePtr, 
								double* 								yCoordinateValuePtr);			

Boolean					ConvertLatLongToLambertConformalConic (
								MapProjectionInfoPtr				mapProjectionInfoPtr,
								double* 								xCoordinateValuePtr, 
								double* 								yCoordinateValuePtr);

Boolean 					ConvertLatLongToSinusoidal (
								MapProjectionInfoPtr				mapProjectionInfoPtr,
								double* 								xCoordinateValuePtr, 
								double* 								yCoordinateValuePtr);

Boolean 					ConvertLatLongToTransverseMercator (
								double* 								xCoordinateValuePtr, 
								double* 								yCoordinateValuePtr,
								double								semiMajorAxis,
								double								semiMinorAxis,
								double								scaleFactorOfCentralMeridian,
								double								eSquared,
								double								longitudeCentralMeridian,
								double								falseEasting,
								double								falseNorthing,
								double								M0);

double 					phi1z (
								double								eccent,
								double								qs,
								long*									flag);
								
double					phi2z (
								double								eccent,
								double								ts,
								SInt32*								flagPtr);

double 					qsfnz (
								double								eccent,
								double								sinphi,
								double								cosphi);
											
int 						sign(
								double 								x);	
								
								

// Function to adjust longitude to -180 to 180
// -------------------------------------------
double adjust_lon(
				double 			x)
								
{
	x = ( fabs(x)<kPI)?x:(x-(sign(x)*kTWO_PI) );
	return (x);
	
}		// end "adjust_lon"



// Function to eliminate roundoff errors in asin

double asinz (
				double								con)
				
{
	if (fabs(con) > 1.0)
		{
		if (con > 1.0)
			con = 1.0;
		else
			con = -1.0;
		
		}
		
 return (asin(con));
 
}		// end "asinz"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									Allrights reserved.
//
//	Function name:				Boolean ConvertAlbersEqualAreaToLatLong
//
//	Software purpose:			This routine handles a conversions from Albers Equal
//									Area (AEA) meter values to latitude and longitude.  This routine 
//									is based on code from John Snyder's Map Projections - A 
//									Working Manual.
//
//	Parameters in:		xCoordinateValuePtr - input AEA horizontal meter coordinate	
//							yCoordinateValuePtr - input AEA vertical meter coordinate	
//
//	Parameters out:	xCoordinateValuePtr - output Longitude	
//							yCoordinateValuePtr - output Latitude	
//
// Value Returned:	None	
//												
// Called By:			UpdateCursorCoordinates in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 04/09/2007
//	Revised By:			Larry L. Biehl			Date: 04/26/2007			

Boolean ConvertAlbersEqualAreaToLatLong (
				MapProjectionInfoPtr				mapProjectionInfoPtr, 
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)
// long alberinvint(r_maj,r_min,lat1,lat2,lon0,lat0,false_east,false_north)

{
	double								centerLongitude;

	double r_major;                   // major axis 
	double r_minor;                   // minor axis
	double standardParallel1;                    // first standard parallel
	double standardParallel2;                    // second standard parallel
	double centerLatitude;
	double falseEasting;              // x offset in meters
	double falseNorthing;             // y offset in meters
	
	double c;
	double sin_po,cos_po;		// sine and cos values
	double con;			// temporary variable
	double e;
	double e2;
	double ms1;   		        // small m 1  
	double ms2;            		// small m 2
	double ns0;
	double qs0;            		// small q 0 
	double qs1;            		// small q 1 
	double qs2;            		// small q 2
	double rh;
	
	double x;			// (O) X projection coordinate
	double y;			// (O) Y projection coordinate

	double rh1;			// height above ellipsoid
	double qs;			// function q
	double theta;			// angle
	long   flag;			// error flag;

	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
	
	x = *xCoordinateValuePtr;
	y = *yCoordinateValuePtr;
		
	standardParallel1 = 
					mapProjectionInfoPtr->gridCoordinate.standardParallel1 * kDegreesToRadians;
		
	standardParallel2 = 
					mapProjectionInfoPtr->gridCoordinate.standardParallel2 * kDegreesToRadians;
					
	centerLongitude = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian * 
																						kDegreesToRadians;
																						
	centerLatitude = mapProjectionInfoPtr->gridCoordinate.latitudeOrigin * 
																						kDegreesToRadians;

	falseEasting = mapProjectionInfoPtr->gridCoordinate.falseEasting;
	falseNorthing = mapProjectionInfoPtr->gridCoordinate.falseNorthing;

	if ((standardParallel1 < 0 && standardParallel2 > 0) ||
				(standardParallel1 > 0 && standardParallel2 < 0))
 	  {
//	  p_error("Equal latitudes for Standard Parallels on opposite sides of equator"
//		  ,"alber-invinit");
 	  return																							(FALSE);
 	  }
 	  
	r_major = mapProjectionInfoPtr->geodetic.semiMajorAxis;
	r_minor = mapProjectionInfoPtr->geodetic.semiMinorAxis;
	e2 = mapProjectionInfoPtr->geodetic.eSquared;
	e = sqrt(e2);
	
	sin_po = sin(standardParallel1);
	cos_po = cos(standardParallel1);
	con = sin_po;

	ms1 = msfnz (e,sin_po,cos_po);
	qs1 = qsfnz (e,sin_po,cos_po);

	sin_po = sin(standardParallel2);
	cos_po = cos(standardParallel2);

	ms2 = msfnz(e,sin_po,cos_po);
	qs2 = qsfnz(e,sin_po,cos_po);

	sin_po = sin(centerLatitude);
	cos_po = cos(centerLatitude);

	qs0 = qsfnz(e,sin_po,cos_po);

	if (fabs(standardParallel1 - standardParallel2) > kEPSLN)
		ns0 = (ms1 * ms1 - ms2 *ms2)/ (qs2 - qs1);
		
	else
		ns0 = con;
		
	c = ms1 * ms1 + ns0 * qs1;
	
	if (ns0 != 0)
		rh = r_major * sqrt(c - ns0 * qs0)/ns0;
		
	else		// ns0 == 0
		rh = r_major;

			// Albers Conical Equal Area inverse equations--mapping x,y to lat/long
			// long alberinv(x, y, lon, lat)

	flag = 0;
	x -= falseEasting;
	y = rh - y + falseNorthing;
	if (ns0 >= 0)
		{
		rh1 = sqrt(x * x + y * y);
		con = 1.0;
		
		}
		
	else
		{
		rh1 = -sqrt(x * x + y * y);
		con = -1.0;
		
		}
		
	theta = 0.0;
	if (rh1 != 0.0)
 	  theta = atan2(con * x, con * y);
 	  
	con = rh1 * ns0 / r_major;
	
	if (ns0 != 0)
		qs = (c - con * con)/ns0;
		
	else		// ns0 == 0
		qs = 0;		// ??? What should it be equal to.
	
	if (e >= 1e-10)
		{
		con = 1 - .5 * (1.0 - e2) * log((1.0 - e) / (1.0 + e))/e;
		if (fabs(fabs(con) - fabs(qs)) > .0000000001)
			{
			*yCoordinateValuePtr = phi1z(e,qs,&flag);
			if (flag != 0)
         	return(FALSE);
         
			}
			
		else
			{
			if (qs >= 0)
 				*yCoordinateValuePtr = .5 * kPI;
 				
			else
				*yCoordinateValuePtr = -.5 * kPI;
				
 			}
 			
		}		// end "if (e >= 1e-10)"
		
	else		// e < 1e-10
		{
		*yCoordinateValuePtr = phi1z(e,qs,&flag);
		if (flag != 0)
			return(FALSE);
		
		}		// end "else e < 1e-10"

	if (ns0 != 0)
		*xCoordinateValuePtr = theta/ns0 + centerLongitude;
		
	else		// ns0 == 0
		*xCoordinateValuePtr = centerLongitude;		// What should it be equal to??
		
	*xCoordinateValuePtr = adjust_lon (*xCoordinateValuePtr);
   
   *yCoordinateValuePtr *= kRadiansToDegrees;
   *xCoordinateValuePtr *= kRadiansToDegrees;
	   
//   if (*xCoordinateValuePtr < -180)
//   	*xCoordinateValuePtr += 360;
//   else if (*xCoordinateValuePtr > 180)
//   	*xCoordinateValuePtr -= 360;
   
   return (TRUE);
			
}		// end "ConvertAlbersEqualAreaToLatLong" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertCylindricalEqualAreaToLatLong
//
//	Software purpose:			This routine handles a conversions from Cylindrical Equal
//									Area (CEA) meter values to latitude and longitude.  This routine 
//									is based on code from John Snyder's Map Projections - A 
//									Working Manual.
//
//	Parameters in:		xCoordinateValuePtr - input CEA horizontal meter coordinate	
//							yCoordinateValuePtr - input CEA vertical meter coordinate	
//
//	Parameters out:	xCoordinateValuePtr - output Longitude	
//							yCoordinateValuePtr - output Latitude	
//
// Value Returned:	None	
//												
// Called By:			UpdateCursorCoordinates in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 07/11/2006
//	Revised By:			Larry L. Biehl			Date: 02/22/2007			

Boolean ConvertCylindricalEqualAreaToLatLong (
				MapProjectionInfoPtr				mapProjectionInfoPtr, 
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								centerLongitude,
											cosStandardLatitude,
											standardLatitude,
											xDistanceCoordinate,
											yDistanceCoordinate;
											

	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode != kSphereEllipsoidCode)
																							return (FALSE);
		
	standardLatitude = 
					mapProjectionInfoPtr->gridCoordinate.standardParallel1 * kDegreesToRadians;
	centerLongitude = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian * 
																						kDegreesToRadians;
	
	if (mapProjectionInfoPtr->geodetic.radiusSpheroid > 0)
		{
		xDistanceCoordinate = *xCoordinateValuePtr;
		yDistanceCoordinate = *yCoordinateValuePtr;
			
		cosStandardLatitude = cos(standardLatitude);
		
		*xCoordinateValuePtr = xDistanceCoordinate/
				(mapProjectionInfoPtr->geodetic.radiusSpheroid * cosStandardLatitude) + 
																								centerLongitude;
											
		*yCoordinateValuePtr = asin (yDistanceCoordinate/
					mapProjectionInfoPtr->geodetic.radiusSpheroid * cosStandardLatitude);
			
		}		// end "if (mapProjectionInfoPtr->geodetic.radiusSpheroid > 0)"
	     								
	else		// mapProjectionInfoPtr->geodetic.radiusSpheroid < 0
		{
		*xCoordinateValuePtr = centerLongitude;
		*yCoordinateValuePtr = 0;
		
		}		// end "else mapProjectionInfoPtr->geodetic.radiusSpheroid < 0"
   
   *yCoordinateValuePtr *= kRadiansToDegrees;
   *xCoordinateValuePtr *= kRadiansToDegrees;
	   
   if (*xCoordinateValuePtr < -180)
   	*xCoordinateValuePtr += 360;
   else if (*xCoordinateValuePtr > 180)
   	*xCoordinateValuePtr -= 360;
   
   return (TRUE);
			
}		// end "ConvertCylindricalEqualAreaToLatLong" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertEquirectangularToLatLong
//
//	Software purpose:			This routine handles conversions from Equirectagular meter values
//									to latitude and longitude.  This routine is based
//									on code from John Snyder's Map Projections - A Working
//									Manual.	
//
//	Parameters out:	xCoordinateValuePtr - input Orthographic horizontal meter coordinate	
//							yCoordinateValuePtr - input Orthographic vertical meter coordinate
//
//	Parameters in:		xCoordinateValuePtr - output Longitude	
//							yCoordinateValuePtr - output Latitude	
//
// Value Returned:	None	
//												
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/18/2007
//	Revised By:			Larry L. Biehl			Date: 10/18/2007			

Boolean ConvertEquirectangularToLatLong (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								cosStandardParallel,
											standardParallel;
											
	Boolean								returnFlag = TRUE;
																								

	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
			
			// Get input lat-long
	
	standardParallel = 
				mapProjectionInfoPtr->gridCoordinate.standardParallel1 * kDegreesToRadians;
	cosStandardParallel = cos(standardParallel);
	
	*xCoordinateValuePtr /= 
						mapProjectionInfoPtr->geodetic.radiusSpheroid * cosStandardParallel;
	*xCoordinateValuePtr *= kRadiansToDegrees;
	*xCoordinateValuePtr += mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
																		
	*yCoordinateValuePtr /= mapProjectionInfoPtr->geodetic.radiusSpheroid;
	*yCoordinateValuePtr *= kRadiansToDegrees;
   
   return (returnFlag);
			
}		// end "ConvertEquirectangularToLatLong" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertKrovakToLatLong
//
//	Software purpose:			This routine handles a conversions from Krovak meter values 
//									to latitude and longitude.  This routine is based
//									on code from PJ_krovak.c file from GRASS-CVS
//
//	Parameters in:		xCoordinateValuePtr - input Krovak horizontal meter coordinate	
//							yCoordinateValuePtr - input Krovak vertical meter coordinate	
//
//	Parameters out:	xCoordinateValuePtr - output Longitude	
//							yCoordinateValuePtr - output Latitude	
//
// Value Returned:	None	
//												
// Called By:			UpdateCursorCoordinates in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 12/29/2006
//	Revised By:			Larry L. Biehl			Date: 02/22/2007			

Boolean ConvertKrovakToLatLong (
				MapProjectionInfoPtr				mapProjectionInfoPtr, 
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double 								s45, 
											s90, 
											fi0, 
											e2, 
											e, 
											alfa, 
											uq, 
											u0, 
											g, 
											k, 
											k1, 
											n0, 
											ro0, 
											ad, 
											a, 
											s0, 
											n;
											
	double 								u, 
											l24, 
											deltav, 
											s, 
											d, 
											eps, 
											ro, 
											fi1, 
											latitudeCoordinate,
											longitudeCoordinate,
											xDistanceCoordinate,
											yDistanceCoordinate;
											
	SInt16								ok;
											

	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode != kBesselEllipsoidCode)
																							return (FALSE);
		
	
	s45 = 0.785398163397448;    // 45¡
	s90 = 2 * s45;
	fi0 = 0.863937979737193;    // Latitude of projection centre 49¡ 30'

			// Ellipsoid is used as Parameter in for.c and inv.c, therefore a must 
			// be set to 1 here.
			// Ellipsoid Bessel 1841 a = 6377397.155m 1/f = 299.1528128,
			// e2=0.006674372230614;

//	a = 1; // 6377397.155;
	a = 6377397.155;
	
			// e2 = P->es;      0.006674372230614;
			
	e2 = 0.006674372230614;
	e = sqrt(e2);

	alfa = sqrt(1. + (e2 * pow(cos(fi0), 4)) / (1. - e2));
	uq = 1.04216856380474;      // DU(2, 59, 42, 42.69689)
	u0 = asin(sin(fi0) / alfa);
	g = pow(   (1. + e * sin(fi0)) / (1. - e * sin(fi0)) , alfa * e / 2.  );

	k = tan( u0 / 2. + s45) / pow  (tan(fi0 / 2. + s45) , alfa) * g;

	k1 = 0.9999;
	n0 = a * sqrt(1. - e2) / (1. - e2 * pow(sin(fi0), 2));
	s0 = 1.37008346281555;       // Latitude of pseudo standard parallel 78¡ 30'00" N
	n = sin(s0);
	ro0 = k1 * n0 / tan(s0);
	ad = s90 - uq;
	
			// Transformation
			// revert y, x
				
	xDistanceCoordinate = -*yCoordinateValuePtr;		// Krovak Easting
	yDistanceCoordinate = -*xCoordinateValuePtr;		// Krovak Northing

	ro = sqrt(xDistanceCoordinate * xDistanceCoordinate + 
													yDistanceCoordinate * yDistanceCoordinate);
	eps = atan2(yDistanceCoordinate, xDistanceCoordinate);
	d = eps / sin(s0);
	s = 2. * (atan(  pow(ro0 / ro, 1. / n) * tan(s0 / 2. + s45)) - s45);

	u = asin(cos(ad) * sin(s) - sin(ad) * cos(s) * cos(d));
	deltav = asin(cos(s) * sin(d) / cos(u));

	l24 = 0.433423430911925;   // DU(2, 24, 50, 0.) diff of 42d 30m and 17d 40m
//	lon17 = 0.308341501185665; // Longitude of Ferro is 17¡ 40'00" West of Greenwich
	longitudeCoordinate = l24 - deltav / alfa;

			// ITERATION FOR latitude
			
   fi1 = u;

	ok = 0;
	do
		{
		latitudeCoordinate = 2. * ( atan( pow( k, -1. / alfa)  *
                            pow( tan(u / 2. + s45) , 1. / alfa)  *
                            pow( (1. + e * sin(fi1)) / (1. - e * sin(fi1)) , e / 2.)
                           )  - s45);

		if (fabs(fi1 - latitudeCoordinate) < 0.000000000000001) 
			ok = 1;
		
		fi1 = latitudeCoordinate;

		} while (ok==0);
   
   *xCoordinateValuePtr = longitudeCoordinate * kRadiansToDegrees;
   *yCoordinateValuePtr = latitudeCoordinate * kRadiansToDegrees;

   return (TRUE);
			
}		// end "ConvertKrovakToLatLong" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertLambertAzimuthalEqualAreaToLatLong
//
//	Software purpose:			This routine handles a conversions from line and column
//									values to latitude and longitude.  This routine is based
//									on code from Dan Steinwand on 7/10/1992.
//
//   								by DRS/TGS/ISS/EROS Data Center, May 1991
//
//   								Algorithm references:
//
//   								1.  "New Equal-Area Map Projections for Noncircular Regions", 
//   								John P. Snyder, The American Cartographer, Vol 15, No. 4, 
//   								October 1988, pp. 341-355.
//   								2.  Snyder, John P., "Map Projections--A Working Manual", 
//   								U.S. Geological Survey Professional Paper 1395 (Supersedes 
//   								USGS Bulletin 1532), United State Government Printing 
//   								Office, Washington D.C., 1987.
//   								3.  "Software Documentation for GCTP General Cartographic 
//   								Transformation Package", U.S. Geological Survey National 
//   								Mapping Division, May 1982.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	None	
//												
// Called By:			UpdateCursorCoordinates in multiSpec.c
//
//	Coded By:			Dan Steinwand			Date: 05/??/1991
//	Revised By:			Larry L. Biehl			Date: 02/22/2007			

Boolean ConvertLambertAzimuthalEqualAreaToLatLong  (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								cosLatitudeCenter,
											cos_z,		// Cosine of z 
											latitude,
											latitudeCenter,
											longitude,
											longitudeCenter,
											projectionXMeters,
											projectionYMeters,
											Rh,
											sinLatitudeCenter,
											sin_z,		// Sine of z 
											temp;		// Re-used temporary variable 
											

	if (mapProjectionInfoPtr == NULL || 
									mapProjectionInfoPtr->geodetic.radiusSpheroid == 0)
																							return (FALSE);

			// Initialize the Lambert Azimuthal Equal Area projection			

	longitudeCenter = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian * 
																							kDegreesToRadians;
	latitudeCenter = 
			mapProjectionInfoPtr->gridCoordinate.latitudeOrigin * kDegreesToRadians;
	sinLatitudeCenter = sin(latitudeCenter); 
	cosLatitudeCenter = cos(latitudeCenter);

			// Convert Line/Sample to projection space meters and convert to 	
			// lat/long.  The center of the pixel is assumed.	
			
   projectionXMeters =  *xCoordinateValuePtr;
   projectionYMeters = *yCoordinateValuePtr;
   
//   if (mapProjectionInfoPtr->geodetic.semiMajorAxis == 
//   												mapProjectionInfoPtr->geodetic.semiMinorAxis)
//	   {

				// Lambert Azimuthal Equal Area inverse equations--mapping x,y to 
				// lat,long  Inverse equations.												
				
		Rh = sqrt(projectionXMeters * projectionXMeters + 
													projectionYMeters * projectionYMeters);
		temp = Rh / (2 * mapProjectionInfoPtr->geodetic.radiusSpheroid);
		if (temp > 1)
																								return (FALSE);
																					
		temp = 2 * asin(temp);
		sin_z = sin(temp); 
		cos_z = cos(temp);
		longitude = longitudeCenter;
		
		if (fabs(Rh) > kEPSLN)
		   {
		   latitude = asin(sinLatitudeCenter * cos_z + 
		   					cosLatitudeCenter * sin_z * projectionYMeters / Rh);
		   temp = fabs(latitudeCenter) - kHALF_PI;
		   if (fabs(temp) > kEPSLN)
		      {
		      temp = cos_z - sinLatitudeCenter * sin(latitude);
		      if(temp != 0.0)
		      	longitude = adjust_lon (longitudeCenter +
		      			atan2(projectionXMeters*sin_z*cosLatitudeCenter,temp*Rh));
		      						
		      }		// end "if (fabs(temp) > kEPSLN)" 
		      
		   else		// fabs(temp) <= kEPSLN 
		   	{
		   	if (latitudeCenter < 0.0) 
		   		longitude = adjust_lon(longitudeCenter - 
		   							atan2(-projectionXMeters, projectionYMeters));
		   	else 
		   		longitude = adjust_lon(longitudeCenter + 
		   							atan2(projectionXMeters, -projectionYMeters));
		   		
		   	}		// end "else fabs(temp) <= kEPSLN" 
		   	
		   }		// end "if (fabs(Rh) > kEPSLN)" 
		   
		else		// end "fabs(Rh) <= kEPSLN" 
			latitude = latitudeCenter;
		
//		}		// end "if (mapProjectionInfoPtr->geodetic.semiMajorAxis == ..."
/*		This needs a lot more work.
	else		// this is an ellipsoid
		{
		double cCe, sCe, q, rho, ab=0.0;
		double t;
		
				// Determine whether this is oblique, equitorial or polar

		t = fabs (latitudeCenter);
		if (fabs(t-HALFPI) < kEPSLN)
			mode = latitudeCenter < 0. ? S_POLE : N_POLE;
		else if (t < EPS10)
			mode = EQUIT;
		else
			mode = OBLIQ;

		P->e = sqrt(P->es);
		P->qp = pj_qsfn(1., P->e, P->one_es);
		P->mmf = .5 / (1. - P->es);
		P->apa = pj_authset(P->es);
		switch (P->mode) 
			{
			case N_POLE:
			case S_POLE:
				P->dd = 1.;
				break;
				
			case EQUIT:
				P->dd = 1. / (P->rq = sqrt(.5 * P->qp));
				P->xmf = 1.;
				P->ymf = .5 * P->qp;
				break;
				
			case OBLIQ:
				P->rq = sqrt(.5 * P->qp);
				sinphi = sin(P->phi0);
				P->sinb1 = pj_qsfn(sinphi, P->e, P->one_es) / P->qp;
				P->cosb1 = sqrt(1. - P->sinb1 * P->sinb1);
				P->dd = cos(P->phi0) / (sqrt(1. - P->es * sinphi * sinphi) *
				   P->rq * P->cosb1);
				P->ymf = (P->xmf = P->rq) / P->dd;
				P->xmf *= P->dd;
				break;
				
			}		// end "switch (P->mode)"

		switch (P->mode) 
			{
			case EQUIT:
			case OBLIQ:
				if ((rho = hypot(xy.x /= P->dd, xy.y *=  P->dd)) < EPS10) 
					{
					lp.lam = 0.;
					lp.phi = P->phi0;
					return (lp);
					
					}		// end "if ((rho = hypot(xy.x ..."
					
				cCe = cos(sCe = 2. * asin(.5 * rho / P->rq));
				xy.x *= (sCe = sin(sCe));
				if (P->mode == OBLIQ) 
					{
					q = P->qp * (ab = cCe * P->sinb1 + xy.y * sCe * P->cosb1 / rho);
					xy.y = rho * P->cosb1 * cCe - xy.y * P->sinb1 * sCe;
					
					}		// end "if (P->mode == OBLIQ)"
					
				else		// P->mode != OBLIQ
					{
					q = P->qp * (ab = xy.y * sCe / rho);
					xy.y = rho * cCe;
					
					}		// end "else P->mode != OBLIQ"
				break;
				
			case N_POLE:
				xy.y = -xy.y;
			case S_POLE:
				if (!(q = (xy.x * xy.x + xy.y * xy.y)) ) 
					{
					lp.lam = 0.;
					lp.phi = P->phi0;
					return (lp);
					
					}		// end "if (!(q = (xy.x * xy.x + xy.y * xy.y)) )"
				
	//			q = P->qp - q;
				
				ab = 1. - q / P->qp;
				if (P->mode == S_POLE)
					ab = - ab;
				break;
				
			}		// end "switch (P->mode)"
			
		longitude = atan2(xy.x, xy.y);
		latitude = pj_authlat(asin(ab), P->apa);
		
		}		// end "else this is an ellipsoid"
*/   
   *xCoordinateValuePtr = longitude * kRadiansToDegrees;	
   *yCoordinateValuePtr = latitude * kRadiansToDegrees;
	   
   if (*xCoordinateValuePtr < -180)
   	*xCoordinateValuePtr += 360;
   else if (*xCoordinateValuePtr > 180)
   	*xCoordinateValuePtr -= 360;
   
   return (TRUE);
			
}		// end "ConvertLambertAzimuthalEqualAreaToLatLong" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertLambertConformalConicToLatLong
//
//	Software purpose:			This routine handles conversions from latitude and longitude
//									to Lambert Conformal Conic meter values.  This routine is based
//									on code from John Snyder's Map Projections - A Working
//									Manual.	
//
//	Parameters in:		xCoordinateValuePtr - input Lambert Conformal Conic horizontal meter coordinate	
//							yCoordinateValuePtr - input Lambert Conformal Conic vertical meter coordinate	
//
//	Parameters out:	xCoordinateValuePtr - output Longitude	
//							yCoordinateValuePtr - output Latitude	
//
// Value Returned:	True if lat/long calculated
//							False if lat/long not calculated
//												
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/07/2012
//	Revised By:			Larry L. Biehl			Date: 01/07/2012			

Boolean ConvertLambertConformalConicToLatLong (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								centerLongitude,
											con,						// temporary angle variable
											f0,
											ns,
											rh,
											rh1,						// height above ellipsoid
											theta,
											ts,
											x,
											y;
											
	SInt32								flag;
																								

	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
				
	centerLongitude = 
		mapProjectionInfoPtr->gridCoordinate.falseOriginLongitude * kDegreesToRadians;					
	flag = 0;
	f0 = mapProjectionInfoPtr->geodetic.f0;
	ns = mapProjectionInfoPtr->geodetic.ns;
	rh = mapProjectionInfoPtr->geodetic.rh;
									
	x = *xCoordinateValuePtr - mapProjectionInfoPtr->gridCoordinate.falseEasting;
	y = rh - *yCoordinateValuePtr + mapProjectionInfoPtr->gridCoordinate.falseNorthing;
	
	if (ns > 0)
		{
		rh1 = sqrt (x * x + y * y);
		con = 1.0;
		
		}		// end "if (ns > 0)"
		
	else		// ns <= 0
		{
		rh1 = -sqrt (x * x + y * y);
		con = -1.0;
		
		}		// end "else ns <= 0"
		
	theta = 0.0;
	if (rh1 != 0)
		theta = atan2((con * x),(con * y));
		
	if ((rh1 != 0) || (ns > 0.0))
		{
		con = 1.0/ns;
		ts = pow((rh1/(mapProjectionInfoPtr->geodetic.semiMajorAxis * f0)), con);
		*yCoordinateValuePtr = phi2z (mapProjectionInfoPtr->geodetic.e, ts, &flag);
		if (flag != 0)
			return (FALSE);
			
		}		// end "if ((rh1 != 0) || (ns > 0.0))"
		
	else		// (rh1 == 0) && (...ns <= 0.0)
		*yCoordinateValuePtr = -kHALF_PI;
	
	*xCoordinateValuePtr = adjust_lon(theta/ns + centerLongitude);
	
	*xCoordinateValuePtr *= kRadiansToDegrees;
	*yCoordinateValuePtr *= kRadiansToDegrees;
	
	return (TRUE);			

/* Lambert Conformal Conic inverse equations--mapping x,y to lat/long
  -----------------------------------------------------------------
long lamccinv(x , y, lon, lat)
double x;			// (O) X projection coordinate
double y;			// (O) Y projection coordinate
double *lon;			// (I) Longitude
double *lat;			// (I) Latitude

{
double rh1;			// height above ellipsoid
double con;			// sign variable
double ts;			// small t
double theta;			// angle
long   flag;			// error flag

flag = 0;
x -= false_easting;
y = rh - y + false_northing;
 if (ns > 0)
    {
    rh1 = sqrt (x * x + y * y);
    con = 1.0;
    }
 else
    {
    rh1 = -sqrt (x * x + y * y);
    con = -1.0;
    }
 theta = 0.0;
 if (rh1 != 0)
    theta = atan2((con * x),(con * y));
 if ((rh1 != 0) || (ns > 0.0))
    {
    con = 1.0/ns;
    ts = pow((rh1/(r_major * f0)),con);
    *lat = phi2z(e,ts,&flag);
    if (flag != 0)
       return(flag);
    }
 else
    *lat = -HALF_PI;
 *lon = adjust_lon(theta/ns + center_lon);
 return(OK);
 }
*/			
}		// end "ConvertLambertConformalConicToLatLong" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ConvertLatLongPointToMapPoint
//
//	Software purpose:	This routine converts the input lat-long coordinates to 
//							map coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/26/2012
//	Revised By:			Larry L. Biehl			Date: 04/26/2012		

Boolean ConvertLatLongPointToMapPoint (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				DoublePoint*						coordinatePointPtr)
				
{
	return (ConvertLatLongPointToMapPoint (mapProjectionInfoPtr,
														&coordinatePointPtr->h,
														&coordinatePointPtr->v));

}		// end "ConvertLatLongPointToMapPoint"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ConvertLatLongPointToMapPoint
//
//	Software purpose:	This routine converts the input lat-long coordinates to 
//							map coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/23/2005
//	Revised By:			Larry L. Biehl			Date: 04/26/2012		

Boolean ConvertLatLongPointToMapPoint (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{		
	SInt16								projectionCode;
	
	Boolean								returnFlag = TRUE;
	
											
	if (mapProjectionInfoPtr != NULL)
		{								
		projectionCode = mapProjectionInfoPtr->gridCoordinate.projectionCode;
	
		if (projectionCode == kTransverseMercatorCode)
			ConvertLatLongToTransverseMercator (mapProjectionInfoPtr,
															xCoordinateValuePtr, 
															yCoordinateValuePtr);
		
		else if (projectionCode == kAlbersConicalEqualAreaCode)
			ConvertLatLongToAlbersEqualArea (mapProjectionInfoPtr,
														xCoordinateValuePtr, 
														yCoordinateValuePtr);
	
		else if (projectionCode == kEquirectangularCode)
			{
			ConvertLatLongToEquirectangular (mapProjectionInfoPtr,
															xCoordinateValuePtr, 
															yCoordinateValuePtr);
			
			}		// end "else if (projectionCode == kEquirectangularCode)"
	
		else if (projectionCode == kOrthographicCode)
			{
			ConvertLatLongToOrthographic (mapProjectionInfoPtr,
															xCoordinateValuePtr, 
															yCoordinateValuePtr);
			
			}		// end "else if (projectionCode == kOrthographicCode)"
	
		else if (projectionCode == kCylindricalEqualAreaCode)
			{
			ConvertLatLongToCylindricalEqualArea (mapProjectionInfoPtr,
																xCoordinateValuePtr, 
																yCoordinateValuePtr);
			
			}		// end "else if (projectionCode == kCylindricalEqualAreaCode)"
	
		else if (projectionCode == kLambertAzimuthalEqualAreaCode)
			{
			ConvertLatLongToLambertAzimuthalEqualArea (mapProjectionInfoPtr,
																xCoordinateValuePtr, 
																yCoordinateValuePtr);
			
			}		// end "else if (projectionCode == kLambertAzimuthalEqualAreaCode)"
	
		else if (projectionCode == kLambertConformalConicCode)
			{
			ConvertLatLongToLambertConformalConic (mapProjectionInfoPtr,
																xCoordinateValuePtr, 
																yCoordinateValuePtr);
			
			}		// end "else if (projectionCode == kLambertConformalConicCode)"
	
		else if (projectionCode == kMercatorCode)
			{
			ConvertLatLongToMercator (mapProjectionInfoPtr,
																xCoordinateValuePtr, 
																yCoordinateValuePtr);
			
			}		// end "else if (projectionCode == kMercatorCode)"
	
		else if (projectionCode == kSinusoidalCode)
			{
			ConvertLatLongToSinusoidal (mapProjectionInfoPtr,
															xCoordinateValuePtr, 
															yCoordinateValuePtr);
			
			}		// end "else if (projectionCode == kSinusoidalCode)"
	
		else if (projectionCode == kKrovakCode)
			{
			ConvertLatLongToKrovak (mapProjectionInfoPtr,
															xCoordinateValuePtr, 
															yCoordinateValuePtr);
			
			}		// end "else if (projectionCode == kKrovakCode)"
	
		else if (projectionCode == kPolarStereographicCode)
			{
			ConvertLatLongToPolarStereographic (mapProjectionInfoPtr,
																xCoordinateValuePtr, 
																yCoordinateValuePtr);
			
			}		// end "else if (projectionCode == kPolarStereographicCode)"
			
		else if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																						kGeographicRSCode)
			{
					// Map point is latitude/longitude for geographic projection
					
			returnFlag = TRUE;
					
			}		// end "else if (...referenceSystemCode == kGeographicRSCode"
			
		else		// Conversion not done.
			returnFlag = FALSE;
			
		}		// end "if (mapProjectionInfoPtr != NULL)"
		
	return (returnFlag);

}		// end "ConvertLatLongPointToMapPoint" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertLatLongRectToMapRect
//
//	Software purpose:			This routine converts the input lat-long coordinates to 
//									map coordinates.
//
//	Parameters in:		xCoordinateValuePtr - longitude	
//							yCoordinateValuePtr - latitude	
//
//	Parameters out:	xCoordinateValuePtr - map horizontal coordinate	
//							yCoordinateValuePtr - map vertical coordinate
//
// Value Returned:	True if conversion was okay	
//												
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/23/2007
//	Revised By:			Larry L. Biehl			Date: 02/23/2007			

Boolean ConvertLatLongRectToMapRect (
				Handle								windowInfoHandle,
				DoubleRect*							coordinateRectanglePtr)

{
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	Handle								mapProjectionHandle;

																								
			// Get pointer to the map projection structure.									
			
	mapProjectionHandle = GetFileMapProjectionHandle2(windowInfoHandle);											
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer (
																	mapProjectionHandle,
																	kNoLock,
																	kNoMoveHi);		

	return (ConvertLatLongRectToMapRect (mapProjectionInfoPtr,
														coordinateRectanglePtr));
			
}		// end "ConvertLatLongRectToMapRect" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean ConvertLatLongRectToMapRect
//
//	Software purpose:	This routine converts the input lat-long coordinates to 
//							map coordinates. The output map coordinates are in the native
//							units for the image file.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/14/2005
//	Revised By:			Larry L. Biehl			Date: 04/29/2012		

Boolean ConvertLatLongRectToMapRect (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				DoubleRect*							coordinateRectanglePtr)

{	
	DoubleRect							savedRectangle;
											
	SInt16								projectionCode;
	
	Boolean								convertedFlag = FALSE;
	
											
	if (mapProjectionInfoPtr != NULL)
		{								
		projectionCode = mapProjectionInfoPtr->gridCoordinate.projectionCode;
	
		if (projectionCode == kTransverseMercatorCode)
			{
			convertedFlag = ConvertLatLongToTransverseMercator (
															mapProjectionInfoPtr,
															&coordinateRectanglePtr->left, 
															&coordinateRectanglePtr->top);
															
			if (convertedFlag)
				convertedFlag = ConvertLatLongToTransverseMercator (
															mapProjectionInfoPtr,
															&coordinateRectanglePtr->right, 
															&coordinateRectanglePtr->bottom);
			
			}		// end "if (projectionCode == kTransverseMercatorCode)"
		
		else if (projectionCode == kAlbersConicalEqualAreaCode)
			{
			convertedFlag = ConvertLatLongToAlbersEqualArea (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->left, 
																&coordinateRectanglePtr->top);
													
			if (convertedFlag)								
				convertedFlag = ConvertLatLongToAlbersEqualArea (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->right, 
																&coordinateRectanglePtr->bottom);
			
			}		// end "else if (projectionCode == kAlbersConicalEqualAreaCode)"
	
		else if (projectionCode == kCylindricalEqualAreaCode)
			{
			convertedFlag = ConvertLatLongToCylindricalEqualArea (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->left, 
																&coordinateRectanglePtr->top);
													
			if (convertedFlag)								
				convertedFlag = ConvertLatLongToCylindricalEqualArea (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->right, 
																&coordinateRectanglePtr->bottom);
			
			}		// end "else if (projectionCode == kCylindricalEqualAreaCode)"
	
		else if (projectionCode == kEquirectangularCode)
			{
			convertedFlag = ConvertLatLongToEquirectangular (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->left, 
																&coordinateRectanglePtr->top);
									
			if (convertedFlag)												
				convertedFlag = ConvertLatLongToEquirectangular (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->right, 
																&coordinateRectanglePtr->bottom);
			
			}		// end "else if (projectionCode == kEquirectangularCode)"
	
		else if (projectionCode == kKrovakCode)
			{
			convertedFlag = ConvertLatLongToKrovak (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->left, 
																&coordinateRectanglePtr->top);
									
			if (convertedFlag)												
				convertedFlag = ConvertLatLongToKrovak (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->right, 
																&coordinateRectanglePtr->bottom);
			
			}		// end "else if (projectionCode == kKrovakCode)"
	
		else if (projectionCode == kLambertAzimuthalEqualAreaCode)
			{
			convertedFlag = ConvertLatLongToLambertAzimuthalEqualArea (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->left, 
																&coordinateRectanglePtr->top);
													
			if (convertedFlag)							
				convertedFlag = ConvertLatLongToLambertAzimuthalEqualArea (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->right, 
																&coordinateRectanglePtr->bottom);
			
			}		// end "else if (projectionCode == kLambertAzimuthalEqualAreaCode)"
	
		else if (projectionCode == kLambertConformalConicCode)
			{
			convertedFlag = ConvertLatLongToLambertConformalConic (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->left, 
																&coordinateRectanglePtr->top);
													
			if (convertedFlag)							
				convertedFlag = ConvertLatLongToLambertConformalConic (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->right, 
																&coordinateRectanglePtr->bottom);
			
			}		// end "else if (projectionCode == kLambertAzimuthalEqualAreaCode)"
	
		else if (projectionCode == kMercatorCode)
			{
			convertedFlag = ConvertLatLongToMercator (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->left, 
																&coordinateRectanglePtr->top);	
																			
			if (convertedFlag)
				convertedFlag = ConvertLatLongToMercator (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->right, 
																&coordinateRectanglePtr->bottom);
			
			}		// end "else if (projectionCode == kMercatorCode)"
	
		else if (projectionCode == kOrthographicCode)
			{
			Boolean								converted2Flag = FALSE,
													converted3Flag = FALSE;
													
					// This is a special case in that the lat-long values could be wrapped
					// around to the "back" of the globe.  We want to catch with the entire
					// rectangle is hidden.  Not just the upper left and lower right 
					// coordinates.
					
			savedRectangle = *coordinateRectanglePtr;
					
			convertedFlag = ConvertLatLongToOrthographic (
															mapProjectionInfoPtr,
															&coordinateRectanglePtr->left, 
															&coordinateRectanglePtr->top);
																		
			converted2Flag = ConvertLatLongToOrthographic (
															mapProjectionInfoPtr,
															&coordinateRectanglePtr->right, 
															&coordinateRectanglePtr->bottom);
															
			convertedFlag = (convertedFlag || converted2Flag);
			
			if (!convertedFlag)
				{
						// Check if upper right point is hidden
					
				converted3Flag = ConvertLatLongToOrthographic (
															mapProjectionInfoPtr,
															&savedRectangle.right, 
															&savedRectangle.top);
				
						// Check if upper right point is hidden
				
				if (!converted3Flag)	
					convertedFlag = ConvertLatLongToOrthographic (
															mapProjectionInfoPtr,
															&savedRectangle.left, 
															&savedRectangle.bottom);
				
				if (!convertedFlag)
					{
							// Check if box might span the viewable area even though
							// the corners may be hidden.
																		
					if (coordinateRectanglePtr->left < 0 && 
												coordinateRectanglePtr->right > 0)
						convertedFlag = TRUE;
												
					}		// end "if (!convertedFlag)"
				
				}		// end "if (!convertedFlag)"
			
			}		// end "else if (gridCode == kOrthographicCode)"
	
		else if (projectionCode == kPolarStereographicCode)
			{
			convertedFlag = ConvertLatLongToPolarStereographic (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->left, 
																&coordinateRectanglePtr->top);
									
			if (convertedFlag)												
				convertedFlag = ConvertLatLongToPolarStereographic (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->right, 
																&coordinateRectanglePtr->bottom);
			
			}		// end "else if (projectionCode == kPolarStereographicCode)"
	
		else if (projectionCode == kSinusoidalCode)
			{
			convertedFlag = ConvertLatLongToSinusoidal (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->left, 
																&coordinateRectanglePtr->top);
									
			if (convertedFlag)												
				convertedFlag = ConvertLatLongToSinusoidal (
																mapProjectionInfoPtr,
																&coordinateRectanglePtr->right, 
																&coordinateRectanglePtr->bottom);
			
			}		// end "else if (projectionCode == kSinusoidalCode)"
													
		else if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																		kGeographicRSCode)
				// Map point is latitude/longitude for geographic projection
			convertedFlag = TRUE;
	
		if (convertedFlag)
			ConvertMapRectByGivenFactor (
						mapProjectionInfoPtr->planarCoordinate.metersToNativeFactor,
						coordinateRectanglePtr);
			
		}		// end "if (mapProjectionInfoPtr != NULL)"
		
	return (convertedFlag);

}		// end "ConvertLatLongRectToMapRect" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertLatLongToAlbersEqualArea
//
//	Software purpose:			This routine handles a conversions from latitude/longitude
//									to Albers Conical Equal Area meter values.  This routine 
//									is based on code from John Snyder's Map Projections - A 
//									Working Manual.
//
//	Parameters in:		xCoordinateValuePtr - input AEA horizontal meter coordinate	
//							yCoordinateValuePtr - input AEA vertical meter coordinate	
//
//	Parameters out:	xCoordinateValuePtr - output Longitude	
//							yCoordinateValuePtr - output Latitude	
//
// Value Returned:	None	
//												
// Called By:			UpdateCursorCoordinates in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 04/09/2007
//	Revised By:			Larry L. Biehl			Date: 04/27/2007			

Boolean ConvertLatLongToAlbersEqualArea (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								centerLatitude,
											centerLongitude,
											latitude,
											longitude,
											standardParallel1,
											standardParallel2;

	double r_major;	// major axis
	double r_minor;	// minor axis	
	double c;		// constant c	
	double e;		// eccentricity 	
	double rh;		// heigth above elipsoid
	double ns0;		// ratio between meridians
	double falseEasting;	// x offset in meters
	double falseNorthing;	// y offset in meters

	double sin_po,cos_po;		// sin and cos values
	double con;			// temporary variable
	double e2;			// eccentricity squared and temp var
	double ms1;			// small m 1
	double ms2;			// small m 2
	double qs0;			// small q 0
	double qs1;			// small q 1
	double qs2;			// small q 2
	
	double sin_phi,cos_phi;		// sine and cos values	
	double qs;			// small q	
	double theta;			// angle	
	double rh1;			// height above ellipsoid

																								
	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
	
			// Initialize the Albers projection
			
			// Get input lat-long
			
	longitude = *xCoordinateValuePtr * kDegreesToRadians;
	latitude = *yCoordinateValuePtr * kDegreesToRadians;
		
	standardParallel1 = 
					mapProjectionInfoPtr->gridCoordinate.standardParallel1 * kDegreesToRadians;
		
	standardParallel2 = 
					mapProjectionInfoPtr->gridCoordinate.standardParallel2 * kDegreesToRadians;
					
	centerLongitude = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian * 
																						kDegreesToRadians;
																						
	centerLatitude = mapProjectionInfoPtr->gridCoordinate.latitudeOrigin * 
																						kDegreesToRadians;

	falseEasting = mapProjectionInfoPtr->gridCoordinate.falseEasting;
	falseNorthing = mapProjectionInfoPtr->gridCoordinate.falseNorthing;

	if ((standardParallel1 < 0 && standardParallel2 > 0) ||
				(standardParallel1 > 0 && standardParallel2 < 0))
	   {
//	   p_error("Equal latitudes for St. Parallels on opposite sides of equator",
//		  "alber-forinit");
 		return																						(FALSE);

	   }
	   
	r_major = mapProjectionInfoPtr->geodetic.semiMajorAxis;
	r_minor = mapProjectionInfoPtr->geodetic.semiMinorAxis;
	e2 = mapProjectionInfoPtr->geodetic.eSquared;
	e = sqrt(e2);
	
	sin_po = sin(standardParallel1);
	cos_po = cos(standardParallel1);
	con = sin_po;

	ms1 = msfnz (e,sin_po,cos_po);
	qs1 = qsfnz (e,sin_po,cos_po);

	sin_po = sin(standardParallel2);
	cos_po = cos(standardParallel2);

	ms2 = msfnz(e,sin_po,cos_po);
	qs2 = qsfnz(e,sin_po,cos_po);

	sin_po = sin(centerLatitude);
	cos_po = cos(centerLatitude);

	qs0 = qsfnz(e,sin_po,cos_po);

	if (fabs(standardParallel1 - standardParallel2) > kEPSLN)
		ns0 = (ms1 * ms1 - ms2 *ms2)/ (qs2 - qs1);
		
	else
		ns0 = con;
	
	c = ms1 * ms1 + ns0 * qs1;
	
	if (ns0 != 0)
		rh = r_major * sqrt(c - ns0 * qs0)/ns0;
		
	else		// ns0 == 0
		rh = r_major;		// Not sure what it should be equal to.

			// Albers Conical Equal Area forward equations--mapping lat,long to x,y
			// long alberfor(lon, lat, x, y)

	sin_phi = sin(latitude);
	cos_phi = cos(latitude);
	
	qs = qsfnz(e,sin_phi,cos_phi);
	
	if (ns0 != 0)
		rh1 = r_major * sqrt(c - ns0 * qs)/ns0;
		
	else		// ns0 == 0
		rh1 = r_major;		// Not sure what it should be equal to.
	
	theta = ns0 * adjust_lon(longitude - centerLongitude); 
	
	*xCoordinateValuePtr = rh1 * sin(theta) + falseEasting;
	*yCoordinateValuePtr = rh - rh1 * cos(theta) + falseNorthing;
   
   return (TRUE);
			
}		// end "ConvertLatLongToAlbersEqualArea"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertLatLongToCylindricalEqualArea
//
//	Software purpose:			This routine handles a conversions from latitude/longitude
//									to Cylindrical Equal Area (CEA) meter values.  This routine 
//									is based on code from John Snyder's Map Projections - A 
//									Working Manual.
//
//	Parameters in:		xCoordinateValuePtr - input CEA horizontal meter coordinate	
//							yCoordinateValuePtr - input CEA vertical meter coordinate	
//
//	Parameters out:	xCoordinateValuePtr - output Longitude	
//							yCoordinateValuePtr - output Latitude	
//
// Value Returned:	None	
//												
// Called By:			UpdateCursorCoordinates in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 07/13/2006
//	Revised By:			Larry L. Biehl			Date: 02/22/2007			

Boolean ConvertLatLongToCylindricalEqualArea (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								centerLongitude,
											cosStandardLatitude,
											latitude,
											longitude,
											standardLatitude;

																								
	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode != kSphereEllipsoidCode)
																							return (FALSE);
			
			// Get input lat-long
			
	longitude = *xCoordinateValuePtr * kDegreesToRadians;
	latitude = *yCoordinateValuePtr * kDegreesToRadians;
	
	standardLatitude = 
					mapProjectionInfoPtr->gridCoordinate.standardParallel1 * kDegreesToRadians;
	centerLongitude = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian * 
																						kDegreesToRadians;
		
	cosStandardLatitude = cos(standardLatitude);
	
	*xCoordinateValuePtr = mapProjectionInfoPtr->geodetic.radiusSpheroid *
										(longitude - centerLongitude) * cosStandardLatitude;
										
	if (cosStandardLatitude != 0)
		*yCoordinateValuePtr = mapProjectionInfoPtr->geodetic.radiusSpheroid * 
														sin (latitude) / cosStandardLatitude;
														
	else		// cosStandardLatitude == 0
		*yCoordinateValuePtr = 0;
   
   return (TRUE);
			
}		// end "ConvertLatLongToCylindricalEqualArea"  



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertLatLongToEquirectangular
//
//	Software purpose:			This routine handles conversions from latitude and longitude
//									to Equirectagular meter values.  This routine is based
//									on code from John Snyder's Map Projections - A Working
//									Manual.	
//
//	Parameters in:		xCoordinateValuePtr - input Longitude	
//							yCoordinateValuePtr - input Latitude	
//
//	Parameters out:	xCoordinateValuePtr - output Orthographic horizontal meter coordinate	
//							yCoordinateValuePtr - output Orthographic vertical meter coordinate
//
// Value Returned:	None	
//												
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 10/18/2007
//	Revised By:			Larry L. Biehl			Date: 10/18/2007			

Boolean ConvertLatLongToEquirectangular (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								centerLongitude,
											cosStandardParallel,
											latitude,
											longitude,
											standardParallel;
											
	Boolean								returnFlag = TRUE;
																								

	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
			
			// Get input lat-long
	
	standardParallel = 
				mapProjectionInfoPtr->gridCoordinate.standardParallel1 * kDegreesToRadians;
	cosStandardParallel = cos(standardParallel);
	
	longitude = *xCoordinateValuePtr * kDegreesToRadians;
	latitude = *yCoordinateValuePtr * kDegreesToRadians;
	
	centerLongitude = 
			mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian * kDegreesToRadians;
	
	*xCoordinateValuePtr = mapProjectionInfoPtr->geodetic.radiusSpheroid * 
												(longitude - centerLongitude) * cosStandardParallel;
																		
	*yCoordinateValuePtr = mapProjectionInfoPtr->geodetic.radiusSpheroid * latitude;
   
   return (returnFlag);
			
}		// end "ConvertLatLongToEquirectangular" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertLatLongToKrovak
//
//	Software purpose:			This routine handles conversions from latitude and longitude
//									to Krovak meter values.  This routine is based
//									on code from PJ_krovak.c file from GRASS-CVS
//
//	Parameters in:		xCoordinateValuePtr - output Longitude	
//							yCoordinateValuePtr - output Latitude	
//
//	Parameters out:	xCoordinateValuePtr - input Krovak horizontal meter coordinate	
//							yCoordinateValuePtr - input Krovak vertical meter coordinate
//
// Value Returned:	None	
//												
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 12/29/2006
//	Revised By:			Larry L. Biehl			Date: 02/22/2007			

Boolean ConvertLatLongToKrovak (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								s45, 
											s90, 
											fi0, 
											e2, 
											e, 
											alfa, 
											uq, 
											u0, 
											g, 
											k, 
											k1, 
											n0, 
											ro0, 
											ad, 
											a, 
											s0, 
											n;
											
	double								gfi, 
											u, 
											latitude,
											lon17, 
											lon42, 
											longitude,
											lamdd, 
											deltav, 
											s, 
											d, 
											eps, 
											ro;

																								
	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
	
	latitude = *yCoordinateValuePtr * kDegreesToRadians;
	longitude = *xCoordinateValuePtr * kDegreesToRadians;

	s45 = 0.785398163397448;    // 45¡
	s90 = 2 * s45;
	fi0 = 0.863937979737193;    // Latitude of projection centre 49¡30'

			// Ellipsoid is used as Parameter in for.c and inv.c, therefore a must 
			// be set to 1 here.
			// Ellipsoid Bessel 1841 a = 6377397.155m 1/f = 299.1528128,
			// e2=0.006674372230614;

//	a =  1; // 6377397.155;
	a =  6377397.155;
	
			// e2 = P->es;     0.006674372230614;
			
	e2 = 0.006674372230614;
	e = sqrt(e2);

	alfa = sqrt(1. + (e2 * pow(cos(fi0), 4)) / (1. - e2));

	uq = 1.04216856380474;      // DU(2, 59, 42, 42.69689)
	u0 = asin(sin(fi0) / alfa);
	g = pow ((1. + e * sin(fi0)) / (1. - e * sin(fi0)), alfa * e / 2.);

	k = tan( u0 / 2. + s45) / pow  (tan(fi0 / 2. + s45), alfa) * g;

	k1 = 0.9999;
	n0 = a * sqrt(1. - e2) / (1. - e2 * pow(sin(fi0), 2));
	s0 = 1.37008346281555;       // Latitude of pseudo standard parallel 78¡ 30'00" N
	n = sin(s0);
	ro0 = k1 * n0 / tan(s0);
	ad = s90 - uq;

			// Transformation

	gfi =pow ( ((1. + e * sin(latitude)) /
               (1. - e * sin(latitude))) , (alfa * e / 2.));

	u= 2. * (atan(k * pow( tan(latitude / 2. + s45), alfa) / gfi)-s45);

	lon17 = 0.308341501185665;  // Longitude of Ferro is 17¡ 40'00" West of Greenwich
	lon42 = 0.74176493209759;   // Longitude of Origin 42¡ 30'00" East of Ferro
	lamdd = longitude + lon17;
	deltav = alfa * (lon42 - lamdd);

	s = asin(cos(ad) * sin(u) + sin(ad) * cos(u) * cos(deltav));
	d = asin(cos(u) * sin(deltav) / cos(s));
	eps = n * d;
	ro = ro0 * pow(tan(s0 / 2. + s45), n) / pow(tan(s / 2. + s45), n);

			// x and y are reverted!
			
	*yCoordinateValuePtr = -ro * cos(eps) / a;
	*xCoordinateValuePtr = -ro * sin(eps) / a;
	
	return (TRUE);
			
}		// end "ConvertLatLongToKrovak" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertLatLongToLambertAzimuthalEqualArea
//
//	Software purpose:			This routine handles conversions from latitude and longitude
//									to Lambert Azimuthal Equal Area meter values.  This routine is based
//									on code from John Snyder's Map Projections - A Working
//									Manual.	
//
//	Parameters in:		xCoordinateValuePtr - input Longitude	
//							yCoordinateValuePtr - input Latitude	
//
//	Parameters out:	xCoordinateValuePtr - output Orthographic horizontal meter coordinate	
//							yCoordinateValuePtr - output Orthographic vertical meter coordinate
//
// Value Returned:	None	
//												
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/22/2007
//	Revised By:			Larry L. Biehl			Date: 03/22/2007			

Boolean ConvertLatLongToLambertAzimuthalEqualArea (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								centerLongitude,
											cosDeltaLongitude,
											cosLat,
											cosLatitudeOrigin,
											deltaLongitude,
											g,
											ksp,
											latitude,
											latitudeOrigin,
											longitude,
											sinDeltaLongitude,
											sinLat,
											sinLatitudeOrigin;
											
	Boolean								returnFlag = TRUE;
																								

	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
		
			// Get input lat-long
			
	longitude = *xCoordinateValuePtr * kDegreesToRadians;
	latitude = *yCoordinateValuePtr * kDegreesToRadians;
	
	cosLat = cos(latitude);
	sinLat = sin(latitude);
	
	latitudeOrigin = 
				mapProjectionInfoPtr->gridCoordinate.latitudeOrigin * kDegreesToRadians;
	cosLatitudeOrigin = cos(latitudeOrigin);
	sinLatitudeOrigin = sin(latitudeOrigin);
	
	centerLongitude = 
			mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian * kDegreesToRadians;
	
	deltaLongitude = adjust_lon (longitude - centerLongitude);
	cosDeltaLongitude = cos(deltaLongitude);
	sinDeltaLongitude = sin(deltaLongitude);
	
	g = sinLatitudeOrigin * sinLat + cosLatitudeOrigin * cosLat * cosDeltaLongitude;
	if (g != -1.0)
	   { 
		ksp = mapProjectionInfoPtr->geodetic.radiusSpheroid * sqrt(2.0 / (1.0 + g));
		*xCoordinateValuePtr = ksp * cosLat * sinDeltaLongitude;
		*yCoordinateValuePtr = ksp * 
				(cosLatitudeOrigin * sinLat - sinLatitudeOrigin * cosLat * cosDeltaLongitude);
		
	   }		// end "if (g != -1.0)"
	   
	else		// g == -1.0
	   { 
	   		// Point projects to a circle of radius = 
	   		// 2.0 * mapProjectionInfoPtr->geodetic.radiusSpheroid
	   		// For now just give a constant value if this occurs.
	   
	   *xCoordinateValuePtr = 2.0 * mapProjectionInfoPtr->geodetic.radiusSpheroid;
	   *yCoordinateValuePtr = 0;
	   
	   }		// end "else g == -1.0"
	   
/* for ellipsoid

		double t;
		
				// Determine whether this is oblique, equitorial or polar

		t = fabs (latitudeCenter);
		if (fabs(t-HALFPI) < kEPSLN)
			mode = latitudeCenter < 0. ? S_POLE : N_POLE;
		else if (t < EPS10)
			mode = EQUIT;
		else
			mode = OBLIQ;

		P->e = sqrt(P->es);
		P->qp = pj_qsfn(1., P->e, P->one_es);
		P->mmf = .5 / (1. - P->es);
		P->apa = pj_authset(P->es);
		switch (P->mode) 
			{
			case N_POLE:
			case S_POLE:
				P->dd = 1.;
				break;
				
			case EQUIT:
				P->dd = 1. / (P->rq = sqrt(.5 * P->qp));
				P->xmf = 1.;
				P->ymf = .5 * P->qp;
				break;
				
			case OBLIQ:
				P->rq = sqrt(.5 * P->qp);
				sinphi = sin(P->phi0);
				P->sinb1 = pj_qsfn(sinphi, P->e, P->one_es) / P->qp;
				P->cosb1 = sqrt(1. - P->sinb1 * P->sinb1);
				P->dd = cos(P->phi0) / (sqrt(1. - P->es * sinphi * sinphi) *
				   P->rq * P->cosb1);
				P->ymf = (P->xmf = P->rq) / P->dd;
				P->xmf *= P->dd;
				break;
				
			}		// end "switch (P->mode)"

		coslam = cos(lp.lam);
		sinlam = sin(lp.lam);
		sinphi = sin(lp.phi);
		q = pj_qsfn(sinphi, P->e, P->one_es);
		if (P->mode == OBLIQ || P->mode == EQUIT) 
			{
			sinb = q / P->qp;
			cosb = sqrt(1. - sinb * sinb);
			}
			
		switch (P->mode) 
			{
			case OBLIQ:
				b = 1. + P->sinb1 * sinb + P->cosb1 * cosb * coslam;
				break;
			case EQUIT:
				b = 1. + cosb * coslam;
				break;
			case N_POLE:
				b = HALFPI + lp.phi;
				q = P->qp - q;
				break;
			case S_POLE:
				b = lp.phi - HALFPI;
				q = P->qp + q;
				break;
			}
			
		if (fabs(b) < EPS10) 
			F_ERROR;
			
		switch (P->mode) 
			{
			case OBLIQ:
				xy.y = P->ymf * ( b = sqrt(2. / b) )
				   * (P->cosb1 * sinb - P->sinb1 * cosb * coslam);
				xy.x = P->xmf * b * cosb * sinlam;
				break;
				
			case EQUIT:
				xy.y = (b = sqrt(2. / (1. + cosb * coslam))) * sinb * P->ymf; 
				xy.x = P->xmf * b * cosb * sinlam;
				break;
				
			case N_POLE:
			case S_POLE:
				if (q >= 0.) 
					{
					xy.x = (b = sqrt(q)) * sinlam;
					xy.y = coslam * (P->mode == S_POLE ? b : -b);
					
					} 
					
				else
					xy.x = xy.y = 0.;
				break;
				
			}		// end "switch (P->mode)"
*/
   
   return (returnFlag);
			
}		// end "ConvertLatLongToLambertAzimuthalEqualArea" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertLatLongToLambertConformalConic
//
//	Software purpose:			This routine handles conversions from latitude and longitude
//									to Lambert Conformal Conic meter values.  This routine is based
//									on code from John Snyder's Map Projections - A Working
//									Manual.	
//
//	Parameters in:		xCoordinateValuePtr - input Longitude	
//							yCoordinateValuePtr - input Latitude	
//
//	Parameters out:	xCoordinateValuePtr - output Lambert Conformal Conic horizontal meter coordinate	
//							yCoordinateValuePtr - output Lambert Conformal Conic vertical meter coordinate
//
// Value Returned:	None	
//												
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/06/2012
//	Revised By:			Larry L. Biehl			Date: 01/07/2012			

Boolean ConvertLatLongToLambertConformalConic (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								centerLongitude,
											con,						// temporary angle variable
											f0,
											latitude,
											longitude,
											ns,
											rh,
											rh1,						// height above ellipsoid
											sinphi,
											theta,
											ts;
																								

	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
		
			// Get input lat-long
			
	longitude = *xCoordinateValuePtr * kDegreesToRadians;
	latitude = *yCoordinateValuePtr * kDegreesToRadians;
	centerLongitude = 
		mapProjectionInfoPtr->gridCoordinate.falseOriginLongitude * kDegreesToRadians;
	f0 = mapProjectionInfoPtr->geodetic.f0;
	ns = mapProjectionInfoPtr->geodetic.ns;
	rh = mapProjectionInfoPtr->geodetic.rh;
	
	con = fabs (fabs(latitude) - kHALF_PI);
	if (con > kEPSLN)
		{
		sinphi = sin(latitude);
		ts = tsfnz (mapProjectionInfoPtr->geodetic.e, latitude, sinphi);	
		rh1 = mapProjectionInfoPtr->geodetic.semiMajorAxis * f0 * pow (ts, ns);
		
		}		// end "if (con > kEPSLN)"
	
	else		// con <= kEPSLN
		{
		con = latitude * ns;
		if (con <= 0)
																								return(FALSE);
			
		rh1 = 0;
		
		}		// end "else con <= kEPSLN"
		
	theta = ns * adjust_lon (longitude - centerLongitude);
	*xCoordinateValuePtr = rh1 * sin(theta) + 
									mapProjectionInfoPtr->gridCoordinate.falseEasting;
	*yCoordinateValuePtr = rh - rh1 * cos(theta) + 
									mapProjectionInfoPtr->gridCoordinate.falseNorthing;
	
	return (TRUE);
			

/* Lambert Conformal conic forward equations--mapping lat,long to x,y
  -----------------------------------------------------------------
long lamccfor(lon, lat, x, y)
double lon;                     // (I) Longitude
double lat;                     // (I) Latitude
double *x;                      // (O) X projection coordinate
double *y;                      // (O) Y projection coordinate

{
double con;                     // temporary angle variable
double rh1;                     // height above ellipsoid
double sinphi;                  // sin value
double theta;                   // angle
double ts;                      // small value t

con  = fabs( fabs(lat) - HALF_PI);
if (con > EPSLN)
  {
  sinphi = sin(lat);
  ts = tsfnz(e,lat,sinphi);
  rh1 = r_major * f0 * pow(ts,ns);
  }
else
  {
  con = lat * ns;
  if (con <= 0)
    {
    p_error("Point can not be projected","lamcc-for");
    return(44);
    }
  rh1 = 0;
  }
theta = ns * adjust_lon(lon - center_lon);
*x = rh1 * sin(theta) + false_easting;
*y = rh - rh1 * cos(theta) + false_northing;

return(OK);
}
*/
			
}		// end "ConvertLatLongToLambertConformalConic" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertLatLongToMercator
//
//	Software purpose:			This routine handles conversions from latitude and longitude
//									to Mercator meter values.  This routine is based
//									on code from John Snyder's Map Projections - A Working
//									Manual. It is for the
//									sphere.  An approximation is made for the ellipsoid. This
//									needs to be changed in the future.	
//
//	Parameters in:		xCoordinateValuePtr - input Longitude	
//							yCoordinateValuePtr - input Latitude	
//
//	Parameters out:	xCoordinateValuePtr - output Mercator horizontal meter coordinate	
//							yCoordinateValuePtr - output Mercator vertical meter coordinate
//
// Value Returned:	None	
//												
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/26/2012
//	Revised By:			Larry L. Biehl			Date: 04/26/2012			

Boolean ConvertLatLongToMercator (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								centerLongitude,
											latitude,
											longitude;
											
	Boolean								returnFlag = TRUE;
																								

	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
		
			// Get input lat-long
			
	longitude = *xCoordinateValuePtr * kDegreesToRadians;
	latitude = *yCoordinateValuePtr * kDegreesToRadians;
	
	centerLongitude = 
			mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian * kDegreesToRadians;
		
	*xCoordinateValuePtr = mapProjectionInfoPtr->geodetic.radiusSpheroid * 
																(longitude - centerLongitude);
	
	*yCoordinateValuePtr = mapProjectionInfoPtr->geodetic.radiusSpheroid * 
																log(tan(kPI/4+latitude/2));
		   
   return (returnFlag);
			
}		// end "ConvertLatLongToMercator" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertLatLongToOrthographic
//
//	Software purpose:			This routine handles conversions from latitude and longitude
//									to Orthographic meter values.  This routine is based
//									on code from John Snyder's Map Projections - A Working
//									Manual.	
//
//	Parameters in:		xCoordinateValuePtr - output Longitude	
//							yCoordinateValuePtr - output Latitude	
//
//	Parameters out:	xCoordinateValuePtr - input Orthographic horizontal meter coordinate	
//							yCoordinateValuePtr - input Orthographic vertical meter coordinate
//
// Value Returned:	None	
//												
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/10/2005
//	Revised By:			Larry L. Biehl			Date: 03/01/2007			

Boolean ConvertLatLongToOrthographic (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								centerLatitude,
											centerLongitude,
											cosCenterLatitude,
											cosLat,
											latitude,
											longitude,
											sinCenterLatitude,
											sinLat;
											
	Boolean								returnFlag = TRUE;
																								

	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
	
			
			// If the coordinates will be outside of the view (horizontal direction),
			// force to the edge.
			
	if (*xCoordinateValuePtr < 
					mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian - 90)
		{
		*xCoordinateValuePtr =  
					mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian - 90;
		returnFlag = FALSE;
		
		}		// end "if (*xCoordinateValuePtr < ..."
			
	else if (*xCoordinateValuePtr > 
					mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian + 90)
		{
		*xCoordinateValuePtr = 
						mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian + 90;	
		returnFlag = FALSE;
			
		}		// end "else if (*xCoordinateValuePtr >"
		
			// Get input lat-long
			
	longitude = *xCoordinateValuePtr * kDegreesToRadians;
	latitude = *yCoordinateValuePtr * kDegreesToRadians;
	
	cosLat = cos(latitude);
	sinLat = sin(latitude);
	
	centerLatitude = 
				mapProjectionInfoPtr->gridCoordinate.latitudeOrigin * kDegreesToRadians;
	centerLongitude = 
			mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian * kDegreesToRadians;
	
	cosCenterLatitude = cos(centerLatitude);
	sinCenterLatitude = sin(centerLatitude);
	
	*xCoordinateValuePtr = mapProjectionInfoPtr->geodetic.radiusSpheroid * cosLat * 
																		sin(longitude - centerLongitude);
																		
	*yCoordinateValuePtr = mapProjectionInfoPtr->geodetic.radiusSpheroid * 
		(cosCenterLatitude*sinLat - sinCenterLatitude*cosLat*cos(longitude - centerLongitude));
   
   return (returnFlag);
			
}		// end "ConvertLatLongToOrthographic" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertLatLongToPolarStereographic
//
//	Software purpose:			This routine handles a conversions from Latitude/Longitude
//									so Polar Stereographic meter values.
//
//	Parameters in:		xCoordinateValuePtr - input Longitude	
//							yCoordinateValuePtr - input Latitude	
//
//	Parameters out:	xCoordinateValuePtr - output Polar Stereographic horizontal meter coordinate	
//							yCoordinateValuePtr - output Polar Stereographic vertical meter coordinate
//
// Value Returned:	None	
//												
// Called By:			UpdateCursorCoordinates in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 12/06/2011
//	Revised By:			Larry L. Biehl			Date: 02/25/2011			

Boolean ConvertLatLongToPolarStereographic (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								con1,					// adjusted longitude
											con2,					// adjusted latitude
											e,
											e4,
											fac,
											ind,					// flag variable
											latitude,			// (I) Latitude
											longitude,			// (I) Longitude
											centerLatitude,
											centerLongitude,
											mcs,					// small m
											rh,					// height above ellipsoid
											r_major,
											sinphi,				// sine of value
											tcs,					// small t
											ts;					// value of small t
											

	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);

	latitude = *yCoordinateValuePtr * kDegreesToRadians;
	longitude = *xCoordinateValuePtr * kDegreesToRadians;
	r_major = mapProjectionInfoPtr->geodetic.semiMajorAxis;	
	
	centerLatitude = 
				mapProjectionInfoPtr->gridCoordinate.latitudeOrigin * kDegreesToRadians;
	
	centerLongitude = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian *
																							kDegreesToRadians;
		
	e = mapProjectionInfoPtr->geodetic.e;
	e4 = mapProjectionInfoPtr->geodetic.e4;
	fac = mapProjectionInfoPtr->geodetic.fac;
	ind = mapProjectionInfoPtr->geodetic.ind;
	mcs = mapProjectionInfoPtr->geodetic.mcs;
	tcs = mapProjectionInfoPtr->geodetic.tcs;
	
/*	if (fabs(fabs(centerLongitude) - kHALF_PI) > kEPSLN)
		{
		ind = 1;
		con1 = fac * centerLatitude; 
		cosphi = cos (con1);
		sinphi = sin (con1);
		mcs = msfnz (e, sinphi, cosphi);
		tcs = tsfnz (e, con1, sinphi);
		
		}		// end "if (fabs(fabs(longitudeCentralMeridian) - kHALF_PI) > kEPSLN)"
*/	
	con1 = fac * adjust_lon (longitude - centerLongitude);
	con2 = fac * latitude;
	sinphi = sin (con2);
	ts = tsfnz (e, con2, sinphi);
	
	if (ind != 0)
		rh = r_major * mcs * ts / tcs;
	else
		rh = 2.0 * r_major * ts / e4;
		
	*xCoordinateValuePtr = fac * rh * sin(con1) + 
											mapProjectionInfoPtr->gridCoordinate.falseEasting;
	*yCoordinateValuePtr = -fac * rh * cos(con1) + 
											mapProjectionInfoPtr->gridCoordinate.falseNorthing;

	return (TRUE);

}		// end "ConvertLatLongToPolarStereographic"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertLatLongToSinusoidal
//
//	Software purpose:			This routine handles a conversions from Latitude/Longitude
//									so Sinusoidal meter values.
//
//	Parameters in:		xCoordinateValuePtr - input Longitude	
//							yCoordinateValuePtr - input Latitude	
//
//	Parameters out:	xCoordinateValuePtr - output Sinusoidal horizontal meter coordinate	
//							yCoordinateValuePtr - output Sinusoidal vertical meter coordinate
//
// Value Returned:	None	
//												
// Called By:			UpdateCursorCoordinates in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 11/29/2006
//	Revised By:			Larry L. Biehl			Date: 02/22/2007			

Boolean ConvertLatLongToSinusoidal (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
			// Using formula on Page 248 of Snyder's Map Projections - Working Manual

	double								a,
											A,
											cosLat,
											e2,
											ePrimedSquared,
											lat,
											longitudeCentralMeridian,
											M,
											N,
											sinLat;

																								
	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
			
			// Get input lat-long
	lat = *yCoordinateValuePtr * kDegreesToRadians;
	cosLat = cos(lat);
	sinLat = sin(lat);
	
	longitudeCentralMeridian = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
	
			// Get Datum Constants
			
			// Equatorial radius
   a = mapProjectionInfoPtr->geodetic.radiusSpheroid;
	
			// Eccentricity squared
	e2 = mapProjectionInfoPtr->geodetic.eSquared;
	
			// Eccentricity primed squared
	ePrimedSquared = e2/(1-e2);
	
			// Intermediate values.
			
	N = a/sqrt(1 - e2*sinLat*sinLat);
	
	A = (*xCoordinateValuePtr-longitudeCentralMeridian) * kDegreesToRadians * cosLat;
	
	M = a*( (1 - e2*(.25 + e2*((double)3/64 + e2*(double)5/256))) * lat -
					e2*(.375 + e2*((double)3/32 + e2*(double)45/1024)) * sin(2*lat) +
						e2*(e2*((double)15/256 + e2*(double)45/1024)) * sin(4*lat) -
							e2*e2*e2*(double)35/3072 * sin(6*lat));
	
			// Now get the Easting
   	
   *xCoordinateValuePtr = A * N;
	
			// Now get the Northing
			   
   *yCoordinateValuePtr = M;
   
   return (TRUE);
			
}		// end "ConvertLatLongToSinusoidal"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertLatLongToTransverseMercator
//
//	Software purpose:			This routine handles a conversions from UTM meter
//									values to latitude and longitude.  This routine is based
//									on code at a web site by Steven Dutch of the University of
//									Wisconsin.	
//
//	Parameters in:		xCoordinateValuePtr - output Longitude	
//							yCoordinateValuePtr - output Latitude	
//
//	Parameters out:	xCoordinateValuePtr - input UTM horizontal meter coordinate	
//							yCoordinateValuePtr - input UTM vertical meter coordinate
//
// Value Returned:	None	
//												
// Called By:			UpdateCursorCoordinates in multiSpec.c
//
//	Coded By:			Dan Steinwand			Date: 03/18/2005
//	Revised By:			Larry L. Biehl			Date: 03/19/2005			

Boolean ConvertLatLongToTransverseMercator (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{								
	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);	
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);

	return (ConvertLatLongToTransverseMercator (
					xCoordinateValuePtr, 
					yCoordinateValuePtr,
					mapProjectionInfoPtr->geodetic.semiMajorAxis,
					mapProjectionInfoPtr->geodetic.semiMinorAxis,
					mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian,
					mapProjectionInfoPtr->geodetic.eSquared,
					mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian,
					mapProjectionInfoPtr->gridCoordinate.falseEasting,
					mapProjectionInfoPtr->gridCoordinate.falseNorthing,
					mapProjectionInfoPtr->geodetic.M0) );

}		// end "ConvertLatLongToTransverseMercator"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertLatLongToTransverseMercator
//
//	Software purpose:			This routine handles a conversions from UTM meter
//									values to latitude and longitude.  This routine is based
//									on code at a web site by Steven Dutch of the University of
//									Wisconsin.	
//
//	Parameters in:		xCoordinateValuePtr - output Longitude	
//							yCoordinateValuePtr - output Latitude	
//
//	Parameters out:	xCoordinateValuePtr - input UTM horizontal meter coordinate	
//							yCoordinateValuePtr - input UTM vertical meter coordinate
//
// Value Returned:	None	
//												
// Called By:			UpdateCursorCoordinates in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 03/02/2005
//	Revised By:			Larry L. Biehl			Date: 03/19/2005			

Boolean ConvertLatLongToTransverseMercator (
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr,
				double								semiMajorAxis,
				double								semiMinorAxis,
				double								scaleFactorOfCentralMeridian,
				double								eSquared,
				double								longitudeCentralMeridian,
				double								falseEasting,
				double								falseNorthing,
				double								M0)

{
/*
	double								a,
											A0,
											b,
											B0,
											C0,
											cosLat,
											cosLatTo2,
											cosLatTo3,
											D0,
											e,
											e2,
											ePrimedSquared,
											oneLesseSinLatSquared,
											E0,
											f,
											invF,
											k0,
											K1,
											K2,
											K3,
											K4,
											K5,
											lat,
											n,
											nTo2,
											nTo4,
											nu,
											oneMinusN,
											p,
											pTo2,
											pTo4,
											rho,
											rm,
											S,
											sin1,
											sin1to2,
											sin1to4,
											sinLat,
											sqrt1LesseSinLatSquared,
											tanLatTo2;
											
	
			// Get input lat-long
	lat = *yCoordinateValuePtr * kDegreesToRadians;
	cosLat = cos(lat);
	sinLat = sin(lat);
	
			// Get Datum Constants
			
			// Equatorial radius
   a = semiMajorAxis;
   
   		// Polar radius
	b = semiMinorAxis;
	
			// Flattening ratio
	f = (a-b)/a;
	
			// Inverse flattening ratio
	invF = 1/f;
	
			// Mean radius
	rm = sqrt(a*b);
	
			// Scale factor
	k0 = scaleFactorOfCentralMeridian;
	
			// Eccentricity
	e = sqrt(eSquared);
	
			// Eccentricity squared
	e2 = eSquared;
	
			// Eccentricity primed squared
	ePrimedSquared = e2/(1-e2);
	
			//
	n = (a-b)/(a+b);
	
			// Intermediate values for rho and nu.
	oneLesseSinLatSquared = 1 - e2*sinLat*sinLat;
	sqrt1LesseSinLatSquared = sqrt(oneLesseSinLatSquared);
	
			// r curv 2
	nu = a/sqrt1LesseSinLatSquared;
	
			// r curv 1
	rho = nu * (1-e2)/oneLesseSinLatSquared;
	
			// Calculate Meridional Arc Length
	oneMinusN = 1 - n;
	nTo2 = n*n;
	nTo4 = nTo2 * nTo2;
	A0 = a*(oneMinusN * (1 + 5*nTo2/4 + 81*nTo4/64));
	B0 = (3*a*n/2)*(oneMinusN * (1 + 7*nTo2/8 + 55*nTo4/64));
	C0 = (15*a*nTo2/16)*(oneMinusN * (1 + 3*nTo2/4));
	D0 = (35*a*n*nTo2/48)*(oneMinusN * (1 + 11*nTo2/16));
	E0 = (315*a*nTo4/51)*oneMinusN;
	
	S = A0*lat - B0*sin(2*lat) + C0*sin(4*lat) - D0*sin(6*lat) + E0*sin(8*lat);
	
			// Calculation Constants
	p = (*xCoordinateValuePtr-longitudeCentralMeridian)*(double)3600/10000;
//	p = (*xCoordinateValuePtr-longitudeCentralMeridian) * kDegreesToRadians;
	pTo2 = p*p;
	pTo4 = pTo2 * pTo2;
	sin1 = kPI/(180*3600);
	sin1to2 = sin1 * sin1;
	sin1to4 = sin1to2 * sin1to2;
	sinLat = sin(lat);
	cosLat = cos(lat);
	cosLatTo2 = cosLat * cosLat;
	cosLatTo3 = cosLat * cosLatTo2;
	
	tanLatTo2 = tan(lat);
	tanLatTo2 *= tanLatTo2;
	
			// Coefficients for UTM Coordinates
	K1 = S*k0;
	K2 = nu*sinLat*cosLat*sin1to2*k0*((double)(1E+8))/2;
	K3 = ((sin1to4*nu*sinLat*cosLat*cosLatTo2)/24)*(5-tanLatTo2+9*ePrimedSquared*cosLatTo2+4*ePrimedSquared*ePrimedSquared*cosLatTo2*cosLatTo2)*k0*((double)(1E+16));
	K4 = nu*cosLat*sin1*k0*10000;
	K5 = (sin1*sin1to2*cosLat*cosLatTo2)*(nu/6)*(1-tanLatTo2+ePrimedSquared*cosLatTo2)*k0*((double)(1E+12));
	
//	A6 = (pTo2*pTo4*sin1to2*sin1to4*nu*sinLat*cosLatTo2*cosLatTo3/720)*(61-58*tanLatTo2+tanLatTo2*tanLatTo2+270*ePrimedSquared*cosLatTo2-330*ePrimedSquared*sinLat*sinLat)*k0*(1E+24);

	K1 = S*k0;
	K2 = nu*sinLat*cosLat*sin1to2*k0/2;
	K3 = ((sin1to4*nu*sinLat*cosLat*cosLatTo2)/24)*(5-tanLatTo2+9*ePrimedSquared*cosLatTo2+4*ePrimedSquared*ePrimedSquared*cosLatTo2*cosLatTo2)*k0;
	K4 = nu*cosLat*sin1*k0;
	K5 = (sin1*sin1to2*cosLat*cosLatTo2)*(nu/6)*(1-tanLatTo2+ePrimedSquared*cosLatTo2)*k0;
	
			// Now get Northing and Easting
			   
   *yCoordinateValuePtr = K1 + K2*pTo2 + K3*pTo4;
//   *yCoordinateValuePtr = p*(K4+ K5*pTo2);
   if (lat < 0)
   	*yCoordinateValuePtr += (double)10000000;
   	
   *xCoordinateValuePtr = falseEasting + p*(K4+ K5*pTo2);
//   *xCoordinateValuePtr = falseEasting + K1 + K3*pTo2 + K3*pTo4;

*/
			// Using formula on Page 61 of Snyder's Map Projections - Working Manual

	double								a,
											A,
											Ato2,
											Ato4,
											C,
											cosLat,
											e2,
											ePrimedSquared,
											k0,
											lat,
											M,
											N,
											Q1,
											Q2,
											Q3,
											Q4,
											sinLat,
											T,
											tanLat,
											Tto2;
			
			// Get input lat-long
	lat = *yCoordinateValuePtr * kDegreesToRadians;
	cosLat = cos(lat);
	sinLat = sin(lat);
	tanLat = tan(lat);
	
			// Get Datum Constants
			
			// Equatorial radius
   a = semiMajorAxis;
	
			// Scale factor
	k0 = scaleFactorOfCentralMeridian;
	
			// Eccentricity squared
	e2 = eSquared;
	
			// Eccentricity primed squared
	ePrimedSquared = e2/(1-e2);
	
			// Intermediate values for rho and nu.
	N = a/sqrt(1 - e2*sinLat*sinLat);
	
	T = tanLat*tanLat;
	
	C = ePrimedSquared * cosLat * cosLat;
	
	A = (*xCoordinateValuePtr-longitudeCentralMeridian) * kDegreesToRadians * cosLat;
	
	M = a*( (1 - e2*(.25 + e2*((double)3/64 + e2*(double)5/256))) * lat -
					e2*(.375 + e2*((double)3/32 + e2*(double)45/1024)) * sin(2*lat) +
						e2*(e2*((double)15/256 + e2*(double)45/1024)) * sin(4*lat) -
							e2*e2*e2*(double)35/3072 * sin(6*lat));
														
	Ato2 = A * A;	
	Ato4 = Ato2 * Ato2;
	Tto2 = T * T;
							
	Q1 = (1 - T + C)*Ato2*A/6;
	Q2 = (5 - 18*T + Tto2 + 72*C - 58*ePrimedSquared)*Ato4*A/120;
	Q3 = A*A/2 + (5 - T + 9*C + 4*C*C)*Ato4/24;
	Q4 = (61 + 58*T + Tto2 + 600*C - 330*ePrimedSquared)*Ato4*Ato2/720;
	
			// Now get the Easting
   	
   *xCoordinateValuePtr = falseEasting + k0 * N * (A + Q1 + Q2);
	
			// Now get the Northing
			   
   *yCoordinateValuePtr = falseNorthing + k0*(M - M0 + N*tanLat*(Q3 + Q4));
//   if (lat < 0)
//   	*yCoordinateValuePtr += (double)10000000;
   
   return (TRUE);
			
}		// end "ConvertLatLongToTransverseMercator"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ConvertMapPointToLatLongPoint
//
//	Software purpose:	This routine converts the input map coordinates to 
//							lat-long coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			CoordinateDialogInitialize in SMapTran.cpp
//							DrawCursorCoordinates in SMapTran.cpp
//							WriteKMLFile in SSaveWrt.cpp
//							ListFieldData in SLstData.cpp
//							ReprojectNearestNeighborLineColumn in SRectification.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/02/2006
//	Revised By:			Larry L. Biehl			Date: 05/23/2012		

Boolean ConvertMapPointToLatLongPoint (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				DoublePoint*						coordinatePointPtr)

{
	SInt16								projectionCode;
	
	Boolean								validPointFlag = FALSE;
	
	
	projectionCode = mapProjectionInfoPtr->gridCoordinate.projectionCode;			
							
	if (mapProjectionInfoPtr->gridCoordinate.referenceSystemCode == 
																		kGeographicRSCode)
			// Map point is latitude/longitude for geographic projection
		validPointFlag = TRUE;
	
	else if (projectionCode == kTransverseMercatorCode)
		validPointFlag = ConvertTransverseMercatorToLatLong (
														mapProjectionInfoPtr,
														&coordinatePointPtr->h, 
														&coordinatePointPtr->v);
		
	else if (projectionCode == kAlbersConicalEqualAreaCode)
		validPointFlag = ConvertAlbersEqualAreaToLatLong (
														mapProjectionInfoPtr,
														&coordinatePointPtr->h, 
														&coordinatePointPtr->v);
		
	else if (projectionCode == kEquirectangularCode)
		validPointFlag = ConvertEquirectangularToLatLong (
														mapProjectionInfoPtr,
														&coordinatePointPtr->h, 
														&coordinatePointPtr->v);
		
	else if (projectionCode == kLambertAzimuthalEqualAreaCode)
		validPointFlag = ConvertLambertAzimuthalEqualAreaToLatLong (
														mapProjectionInfoPtr,
														&coordinatePointPtr->h, 
														&coordinatePointPtr->v);
		
	else if (projectionCode == kLambertConformalConicCode)
		validPointFlag = ConvertLambertConformalConicToLatLong (
														mapProjectionInfoPtr,
														&coordinatePointPtr->h, 
														&coordinatePointPtr->v);
					
	else if (projectionCode == kOrthographicCode)
		validPointFlag = ConvertOrthographicToLatLong (
													mapProjectionInfoPtr,
													&coordinatePointPtr->h, 
													&coordinatePointPtr->v);
					
	else if (projectionCode == kCylindricalEqualAreaCode)
		validPointFlag = ConvertCylindricalEqualAreaToLatLong (
													mapProjectionInfoPtr,
													&coordinatePointPtr->h, 
													&coordinatePointPtr->v);	
					
	else if (projectionCode == kMercatorCode)		
		validPointFlag = ConvertMercatorToLatLong (
													mapProjectionInfoPtr,
													&coordinatePointPtr->h, 
													&coordinatePointPtr->v);
					
	else if (projectionCode == kSinusoidalCode)
		validPointFlag = ConvertSinusoidalToLatLong (
													mapProjectionInfoPtr,
													&coordinatePointPtr->h, 
													&coordinatePointPtr->v);
					
	else if (projectionCode == kKrovakCode)
		validPointFlag = ConvertKrovakToLatLong (
													mapProjectionInfoPtr,
													&coordinatePointPtr->h, 
													&coordinatePointPtr->v);
					
	else if (projectionCode == kPolarStereographicCode)
		validPointFlag = ConvertPolarStereographicToLatLong (
													mapProjectionInfoPtr,
													&coordinatePointPtr->h, 
													&coordinatePointPtr->v);
													
	return (validPointFlag);
		
}		// end "ConvertMapPointToLatLongPoint" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ConvertMapRectByGivenFactor
//
//	Software purpose:	This routine converts the input map rectangle to a map rect
//							adjusted by the input factor. This is used to convert from
//							native map units to meters or meters to native map units.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/28/2012
//	Revised By:			Larry L. Biehl			Date: 04/29/2012			

void ConvertMapRectByGivenFactor (
				double								factor,
				DoubleRect*							coordinateRectPtr)
				
{
	if (factor != 1)
		{
		coordinateRectPtr->left *= factor;
		coordinateRectPtr->top *= factor;
		coordinateRectPtr->right *= factor;
		coordinateRectPtr->bottom *= factor;
		
		}		// end "if (factor != 1)"
			
}		// end "ConvertMapRectByGivenFactor" 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ConvertMapRectToLatLongRect
//
//	Software purpose:	This routine converts the input rectangle map coordinates to 
//							lat-long coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 02/23/2007
//	Revised By:			Larry L. Biehl			Date: 03/13/2012		

void ConvertMapRectToLatLongRect (
				Handle								windowInfoHandle, 
				SInt16								projectionCode,
				DoubleRect*							coordinateRectanglePtr)

{
	MapProjectionInfoPtr 			mapProjectionInfoPtr;
	
	Handle								mapProjectionHandle;
											
											
	mapProjectionHandle = GetFileMapProjectionHandle2 (windowInfoHandle);

	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer(
												mapProjectionHandle, kNoLock, kNoMoveHi);
	
	ConvertMapRectToLatLongRect (mapProjectionInfoPtr, 
												projectionCode,
												coordinateRectanglePtr);

}		// end "ConvertMapRectToLatLongRect" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void ConvertMapRectToLatLongRect
//
//	Software purpose:	This routine converts the input rectangle map coordinates to 
//							lat-long coordinates.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/14/2005
//	Revised By:			Larry L. Biehl			Date: 04/26/2012		

void ConvertMapRectToLatLongRect (
				MapProjectionInfoPtr				mapProjectionInfoPtr, 
				SInt16								projectionCode,
				DoubleRect*							coordinateRectanglePtr)

{
	if (projectionCode == kTransverseMercatorCode)
		{
		ConvertTransverseMercatorToLatLong (mapProjectionInfoPtr,
															&coordinateRectanglePtr->left, 
															&coordinateRectanglePtr->top);
		ConvertTransverseMercatorToLatLong (mapProjectionInfoPtr,
															&coordinateRectanglePtr->right, 
															&coordinateRectanglePtr->bottom);
		
		}		// end "if (projectionCode == kTransverseMercatorCode)"
		
	else if (projectionCode == kAlbersConicalEqualAreaCode)
		{
		ConvertAlbersEqualAreaToLatLong (mapProjectionInfoPtr,
																	&coordinateRectanglePtr->left, 
																	&coordinateRectanglePtr->top);
		ConvertAlbersEqualAreaToLatLong (mapProjectionInfoPtr,
																	&coordinateRectanglePtr->right, 
																	&coordinateRectanglePtr->bottom);
																	
		}		// end "else if (projectionCode == kLambertAzimuthalEqualAreaCode)"
		
	else if (projectionCode == kEquirectangularCode)
		{
		ConvertEquirectangularToLatLong (mapProjectionInfoPtr,
																	&coordinateRectanglePtr->left, 
																	&coordinateRectanglePtr->top);
		ConvertEquirectangularToLatLong (mapProjectionInfoPtr,
																	&coordinateRectanglePtr->right, 
																	&coordinateRectanglePtr->bottom);
																	
		}		// end "else if (projectionCode == kEquirectangularCode)"
		
	else if (projectionCode == kLambertAzimuthalEqualAreaCode)
		{
		ConvertLambertAzimuthalEqualAreaToLatLong (mapProjectionInfoPtr,
																	&coordinateRectanglePtr->left, 
																	&coordinateRectanglePtr->top);
		ConvertLambertAzimuthalEqualAreaToLatLong (mapProjectionInfoPtr,
																	&coordinateRectanglePtr->right, 
																	&coordinateRectanglePtr->bottom);
																	
		}		// end "else if (projectionCode == kLambertAzimuthalEqualAreaCode)"
		
	else if (projectionCode == kLambertConformalConicCode)
		{
		ConvertLambertConformalConicToLatLong (mapProjectionInfoPtr,
																	&coordinateRectanglePtr->left, 
																	&coordinateRectanglePtr->top);
		ConvertLambertConformalConicToLatLong (mapProjectionInfoPtr,
																	&coordinateRectanglePtr->right, 
																	&coordinateRectanglePtr->bottom);
																	
		}		// end "else if (projectionCode == kLambertAzimuthalEqualAreaCode)"
					
	else if (projectionCode == kOrthographicCode)
		{
		ConvertOrthographicToLatLong (mapProjectionInfoPtr,
													&coordinateRectanglePtr->left, 
													&coordinateRectanglePtr->top);
		ConvertOrthographicToLatLong (mapProjectionInfoPtr,
													&coordinateRectanglePtr->right, 
													&coordinateRectanglePtr->bottom);
																	
		}		// end "else if (projectionCode == kOrthographicCode)"
					
	else if (projectionCode == kCylindricalEqualAreaCode)
		{
		ConvertCylindricalEqualAreaToLatLong (mapProjectionInfoPtr,
															&coordinateRectanglePtr->left, 
															&coordinateRectanglePtr->top);
		ConvertCylindricalEqualAreaToLatLong (mapProjectionInfoPtr,
															&coordinateRectanglePtr->right, 
															&coordinateRectanglePtr->bottom);
																	
		}		// end "else if (projectionCode == kCylindricalEqualAreaCode)"	
					
	else if (projectionCode == kMercatorCode)
		{	
		ConvertMercatorToLatLong (mapProjectionInfoPtr,
												&coordinateRectanglePtr->left, 
												&coordinateRectanglePtr->top);
		ConvertMercatorToLatLong (mapProjectionInfoPtr,
												&coordinateRectanglePtr->right, 
												&coordinateRectanglePtr->bottom);
																	
		}		// end "else if (projectionCode == kMercatorCode)"	
					
	else if (projectionCode == kSinusoidalCode)
		{
		ConvertSinusoidalToLatLong (mapProjectionInfoPtr,
												&coordinateRectanglePtr->left, 
												&coordinateRectanglePtr->top);
		ConvertSinusoidalToLatLong (mapProjectionInfoPtr,
												&coordinateRectanglePtr->right, 
												&coordinateRectanglePtr->bottom);
															
		}		// end "else if (projectionCode == kSinusoidalCode)"
					
	else if (projectionCode == kKrovakCode)
		{
		ConvertKrovakToLatLong (mapProjectionInfoPtr,
											&coordinateRectanglePtr->left, 
											&coordinateRectanglePtr->top);
		ConvertKrovakToLatLong (mapProjectionInfoPtr,
											&coordinateRectanglePtr->right, 
											&coordinateRectanglePtr->bottom);
															
		}		// end "else if (projectionCode == kKrovakCode)"
					
	else if (projectionCode == kPolarStereographicCode)
		{
		ConvertPolarStereographicToLatLong (mapProjectionInfoPtr,
											&coordinateRectanglePtr->left, 
											&coordinateRectanglePtr->top);
		ConvertPolarStereographicToLatLong (mapProjectionInfoPtr,
											&coordinateRectanglePtr->right, 
											&coordinateRectanglePtr->bottom);
															
		}		// end "else if (projectionCode == kKrovakCode)"
		
}		// end "ConvertMapRectToLatLongRect" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertMercatorToLatLong
//
//	Software purpose:			This routine handles conversions from Mercator meter values
//									to latitude and longitude.  This routine is based
//									on code from John Snyder's Map Projections - A Working
//									Manual. It is for the
//									sphere.  An approximation is made for the ellipsoid. This
//									needs to be changed in the future.	
//
//	Parameters in:		xCoordinateValuePtr - input Longitude	
//							yCoordinateValuePtr - input Latitude	
//
//	Parameters out:	xCoordinateValuePtr - output Mercator horizontal meter coordinate	
//							yCoordinateValuePtr - output Mercator vertical meter coordinate
//
// Value Returned:	None	
//												
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/26/2012
//	Revised By:			Larry L. Biehl			Date: 04/26/2012			

Boolean ConvertMercatorToLatLong (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								centerLongitude,
											latitude,
											longitude;
											
	Boolean								returnFlag = TRUE;
																								

	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
		
			// Get input lat-long
			
	longitude = *xCoordinateValuePtr * kDegreesToRadians;
	latitude = *yCoordinateValuePtr * kDegreesToRadians;
	
	centerLongitude = 
			mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian * kDegreesToRadians;
		
	*xCoordinateValuePtr = centerLongitude +
					*xCoordinateValuePtr/mapProjectionInfoPtr->geodetic.radiusSpheroid;
	
	*yCoordinateValuePtr = atan (
				sinh(*yCoordinateValuePtr/mapProjectionInfoPtr->geodetic.radiusSpheroid));
										
   *yCoordinateValuePtr *= kRadiansToDegrees;
   *xCoordinateValuePtr *= kRadiansToDegrees;
		   
   return (returnFlag);
			
}		// end "ConvertMercatorToLatLong" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertSinusoidalToLatLong
//
//	Software purpose:			This routine handles a conversions from Sinusoidal meter 
//									values to latitude/longitude values.
//
//	Parameters in:		xCoordinateValuePtr - input Sinusoidal horizontal meter coordinate
//							yCoordinateValuePtr - input Sinusoidal vertical meter coordinate
//
//	Parameters out:	xCoordinateValuePtr - output Longitude
//							yCoordinateValuePtr - output Latitude
//
// Value Returned:	None	
//												
// Called By:			UpdateCursorCoordinates in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 11/29/2006
//	Revised By:			Larry L. Biehl			Date: 06/30/2009			

Boolean ConvertSinusoidalToLatLong (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
			// Using formula on Page 248 of Snyder's Map Projections - Working Manual
			
	double								a,
											cosLat,
											e1,
											e1To2,
											e1To3,
											e1To4,
											e2,
											J1,
											J2,
											J3,
											J4,
											long0,
											M,
											mu,
											sqrtOneMinusEsquared,
											sqrtOneMinusE2SinSquared,
											sinLat;
											

	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
   								
   		// Get the major and minor axes
   		
   a = mapProjectionInfoPtr->geodetic.semiMajorAxis;
	long0 = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian * 
																						kDegreesToRadians;
			// Calculate the Meridional Arc			

	e2 = mapProjectionInfoPtr->geodetic.eSquared;
	M = *yCoordinateValuePtr;
	
			// Calculate Footprint Latitude
			
	mu = M/(a*(1 - e2*(.25 + e2*((double)3/64 + e2*(double)5/256))));
	
	sqrtOneMinusEsquared = sqrt(1-e2);
	e1 = (1 - sqrtOneMinusEsquared)/(1 + sqrtOneMinusEsquared);
	
			// Calculate Latitude
	
	e1To2 = e1 * e1;
	e1To3 = e1 * e1To2;
	e1To4 = e1 * e1To3;
	J1 = 3*e1/2 - 27*e1To3/32;
	J2 = 21*e1To2/16 - 55*e1To4/32;
	J3 = 151*e1To3/96;
	J4 = 1097*e1To4/512;
	*yCoordinateValuePtr = mu + J1*sin(2*mu) + J2*sin(4*mu) + J3*sin(6*mu) + J4*sin(8*mu);
	
			// Calculate Longitude
			
	sinLat = sin(*yCoordinateValuePtr);
	cosLat = cos(*yCoordinateValuePtr);
	sqrtOneMinusE2SinSquared = sqrt(1 - e2 * sinLat * sinLat);
	
	*xCoordinateValuePtr = long0 + *xCoordinateValuePtr * sqrtOneMinusE2SinSquared / 
																									(a * cosLat);
   
   *yCoordinateValuePtr *= kRadiansToDegrees;
   *xCoordinateValuePtr *= kRadiansToDegrees;
	   
   if (*xCoordinateValuePtr < -180)
//		{
//		if (*xCoordinateValuePtr < -180.5)
//			*xCoordinateValuePtr += 360;
//		
//		if (*xCoordinateValuePtr < -180)
			*xCoordinateValuePtr = -180;
		
//		}		// end "if (*xCoordinateValuePtr < -180)"
		
   else if (*xCoordinateValuePtr > 180)
//		{
//		if (*xCoordinateValuePtr > 180.5)
//			*xCoordinateValuePtr -= 360;
//		
//		if (*xCoordinateValuePtr > 180)
			*xCoordinateValuePtr = 180;
		
//		}		// end "else if (*xCoordinateValuePtr > 180)"
   
   return (TRUE);
			
}		// end "ConvertSinusoidalToLatLong"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertTransverseMercatorToLatLong
//
//	Software purpose:			This routine handles a conversions from UTM meter
//									values to latitude and longitude.  This routine is based
//									on code at a web site by Steven Dutch of the University of
//									Wisconsin.
//
//	Parameters in:		xCoordinateValuePtr - input UTM horizontal meter coordinate	
//							yCoordinateValuePtr - input UTM vertical meter coordinate	
//
//	Parameters out:	xCoordinateValuePtr - output Longitude	
//							yCoordinateValuePtr - output Latitude	
//
// Value Returned:	None	
//												
// Called By:			UpdateCursorCoordinates in multiSpec.c
//
//	Coded By:			Dan Steinwand			Date: 04/02/2004
//	Revised By:			Larry L. Biehl			Date: 11/25/2009			

Boolean ConvertTransverseMercatorToLatLong (
				MapProjectionInfoPtr				mapProjectionInfoPtr, 
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								a,
//											b,
											C1,
											C1Squared,
											cosfp,
											D,
											DSquared,
											DToFourth,
											e1,
											e1To2,
											e1To3,
											e1To4,
											e2,
//											e,
											ePrimedSquared,
											fp,
											J1,
											J2,
											J3,
											J4,
											k0,
											long0,
											M,
											mu,
											N1,
											projectionXeasting,
											projectionYnorthing,
											Q1,
											Q2,
											Q3,
											Q4,
											Q5,
											Q6,
											Q7,
											R1,
											sqrtOneMinusEsquared,
											sqrtOneMinusE2SinSquared,
											sinfp,
											T1,
											tanfp;
											
//	SInt16								gridZone;

																								
	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
	
			// Convert UTM meters location to projection space meters and convert to 	
			// lat/long. 					

//   projectionXeasting =  *xCoordinateValuePtr - 500000;
   projectionXeasting =  
   			*xCoordinateValuePtr - mapProjectionInfoPtr->gridCoordinate.falseEasting;
   
   		// Try to handle the southern hemisphere. Not sure how all UTM data for 
   		// southern hemisphere comes in.  If southern hemisphere, then 10,000,000 
   		// meters needs to be subtracted from the northing.
   		
   projectionYnorthing = 
				*yCoordinateValuePtr - mapProjectionInfoPtr->gridCoordinate.falseNorthing;
   		
//	gridZone = mapProjectionInfoPtr->gridCoordinate.gridZone;
//   if (gridZone < 0 && projectionYnorthing > 0)
//   	projectionYnorthing -= mapProjectionInfoPtr->gridCoordinate.falseNorthing;
   								
   		// Get the major and minor axes
   		
   a = mapProjectionInfoPtr->geodetic.semiMajorAxis;
//	b = mapProjectionInfoPtr->geodetic.semiMinorAxis;
	long0 = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian * 
																						kDegreesToRadians;
	
			// Get the central meridian

			// Calculate the Meridional Arc			

//	e2 = 1 - b*b/(a*a);
	e2 = mapProjectionInfoPtr->geodetic.eSquared;
//	e = sqrt(e2);
//	k0 = 0.9996;
	k0 = mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian;
	M = mapProjectionInfoPtr->geodetic.M0 + projectionYnorthing/k0;
	
			// Calculate Footprint Latitude
			
	mu = M/(a*(1 - e2*(.25 + e2*((double)3/64 + e2*(double)5/256))));
	
	sqrtOneMinusEsquared = sqrt(1-e2);
	e1 = (1 - sqrtOneMinusEsquared)/(1 + sqrtOneMinusEsquared);
	
	e1To2 = e1 * e1;
	e1To3 = e1 * e1To2;
	e1To4 = e1 * e1To3;
	J1 = 3*e1/2 - 27*e1To3/32;
	J2 = 21*e1To2/16 - 55*e1To4/32;
	J3 = 151*e1To3/96;
	J4 = 1097*e1To4/512;
	fp = mu + J1*sin(2*mu) + J2*sin(4*mu) + J3*sin(6*mu) + J4*sin(8*mu);
	
			// Calculate Latitude and Longitude
			
	sinfp = sin(fp);
	cosfp = cos(fp);
	tanfp = tan(fp);
	sqrtOneMinusE2SinSquared = sqrt(1 - e2 * sinfp * sinfp);
	ePrimedSquared = e2/(1-e2);
	C1 = ePrimedSquared*cosfp*cosfp;
	T1 = tanfp*tanfp;
	R1 = a*(1-e2)/(sqrtOneMinusE2SinSquared*sqrtOneMinusE2SinSquared*sqrtOneMinusE2SinSquared);
	N1 = a/sqrtOneMinusE2SinSquared;
	D = projectionXeasting/(N1*k0);
	
	C1Squared = C1 * C1;
	DSquared = D * D;
	DToFourth = DSquared * DSquared;
	
	Q1 = N1 * tanfp/R1;
	Q2 = DSquared/2;
	Q3 = (5 + 3*T1 + 10*C1 - 4*C1Squared - 9*ePrimedSquared) * DToFourth/24;
	Q4 = (61 + 90*T1 + 298*C1 + 45*T1*T1 - 3*C1Squared - 252*ePrimedSquared) *
																			DToFourth*DSquared/720;
	
	*yCoordinateValuePtr = fp - Q1*(Q2 - Q3 + Q4);
	
	Q5 = D;
	Q6 = (1 + 2*T1 + C1) * D*DSquared/6;
	Q7 = (5 - 2*C1 + 28*T1 - 3*C1Squared + 8*ePrimedSquared + 24*T1*T1) *
																				D*DToFourth/120;
	
	*xCoordinateValuePtr = long0 + (Q5 - Q6 + Q7)/cosfp;
   
   *yCoordinateValuePtr *= kRadiansToDegrees;
   *xCoordinateValuePtr *= kRadiansToDegrees;
	   
   if (*xCoordinateValuePtr < -180)
   	*xCoordinateValuePtr += 360;
   else if (*xCoordinateValuePtr > 180)
   	*xCoordinateValuePtr -= 360;
   
   return (TRUE);
			
}		// end "ConvertTransverseMercatorToLatLong" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertOrthographicToLatLong
//
//	Software purpose:			This routine handles a conversions from Orthographic meter
//									values to latitude and longitude.  This routine is based
//									on code from John Snyder's Map Projections - A Working
//									Manual.
//
//	Parameters in:		xCoordinateValuePtr - input Orthographic horizontal meter coordinate	
//							yCoordinateValuePtr - input Orthographic vertical meter coordinate	
//
//	Parameters out:	xCoordinateValuePtr - output Longitude	
//							yCoordinateValuePtr - output Latitude	
//
// Value Returned:	None	
//												
// Called By:			UpdateCursorCoordinates in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 05/10/2005
//	Revised By:			Larry L. Biehl			Date: 02/22/2007			

Boolean ConvertOrthographicToLatLong (
				MapProjectionInfoPtr				mapProjectionInfoPtr, 
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								a,
											c,
											centerLatitude,
											centerLongitude,
											cosC,
											cosCenterLatitude,
											p,
											sinC,
											sinCenterLatitude,
											
											xDistanceCoordinate,
											yDistanceCoordinate;
											

	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
	
	xDistanceCoordinate = *xCoordinateValuePtr;
	yDistanceCoordinate = *yCoordinateValuePtr;
	
	centerLatitude = 
				mapProjectionInfoPtr->gridCoordinate.latitudeOrigin * kDegreesToRadians;
	centerLongitude = 
			mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian * kDegreesToRadians;
	
	
	p = sqrt(xDistanceCoordinate*xDistanceCoordinate + 
											yDistanceCoordinate*yDistanceCoordinate);
											
	if (p <= mapProjectionInfoPtr->geodetic.radiusSpheroid)
		{
		c = asin(p/mapProjectionInfoPtr->geodetic.radiusSpheroid);
		
		cosC = cos(c);
		sinC = sin(c);
		
		cosCenterLatitude = cos(centerLatitude);
		sinCenterLatitude = sin(centerLatitude);
		
		if (p != 0)
			{
			*yCoordinateValuePtr = asin(cosC*sinCenterLatitude + 
		     								yDistanceCoordinate * sinC * cosCenterLatitude/p);
		     								
			if (mapProjectionInfoPtr->gridCoordinate.latitudeOrigin == 90)
				*xCoordinateValuePtr = centerLongitude + 
														atan(xDistanceCoordinate/-yDistanceCoordinate);
			
			else if (mapProjectionInfoPtr->gridCoordinate.latitudeOrigin == -90)
				*xCoordinateValuePtr = centerLongitude + 
														atan(xDistanceCoordinate/yDistanceCoordinate);
			
			else		// latitudeOrigin != +-90 degrees
				{	
				a = p*cosCenterLatitude*cosC - yDistanceCoordinate*sinCenterLatitude*sinC;
				
				*xCoordinateValuePtr = centerLongitude + atan(xDistanceCoordinate * sinC/a);
					
				}		// else latitudeOrigin != +-90 degrees
			
			}		// end "if (p != 0)"
		     								
		else		// p == 0
			{
			*xCoordinateValuePtr = centerLongitude;
			*yCoordinateValuePtr = centerLatitude;
			
			}		// end "else p == 0"
	   
	   *yCoordinateValuePtr *= kRadiansToDegrees;
	   *xCoordinateValuePtr *= kRadiansToDegrees;
	   
	   if (*xCoordinateValuePtr < -180)
	   	*xCoordinateValuePtr += 360;
	   else if (*xCoordinateValuePtr > 180)
	   	*xCoordinateValuePtr -= 360;
	   
	   return (TRUE);
			
		}		// end "if (p <= mapProjectionInfoPtr->geodetic.radiusSpheroid)"
		
	else		// p > mapProjectionInfoPtr->geodetic.radiusSpheroid
		{
	   *xCoordinateValuePtr = -1000;
	   *yCoordinateValuePtr = -1000;
	   
	   return (FALSE);
		
		}		// end "else p > mapProjectionInfoPtr->geodetic.radiusSpheroid"
			
}		// end "ConvertOrthographicToLatLong" 



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:				Boolean ConvertPolarStereographicToLatLong
//
//	Software purpose:			This routine handles a conversions from Polar Stereographic
//									meter values to latitude and longitude.  This routine is based
//									on code from John Snyder's Map Projections - A Working
//									Manual.
//
//	Parameters in:		xCoordinateValuePtr - input Orthographic horizontal meter coordinate	
//							yCoordinateValuePtr - input Orthographic vertical meter coordinate	
//
//	Parameters out:	xCoordinateValuePtr - output Longitude	
//							yCoordinateValuePtr - output Latitude	
//
// Value Returned:	None	
//												
// Called By:			UpdateCursorCoordinates in multiSpec.c
//
//	Coded By:			Larry L. Biehl			Date: 12/07/2011
//	Revised By:			Larry L. Biehl			Date: 12/07/2011			

Boolean ConvertPolarStereographicToLatLong (
				MapProjectionInfoPtr				mapProjectionInfoPtr, 
				double* 								xCoordinateValuePtr, 
				double* 								yCoordinateValuePtr)

{
	double								centerLatitude,
											centerLongitude,
											e,
											e4,
											fac,
											ind,
											rh,							// height above ellipsiod
											temp,							// temporary variable
											ts,							// small value t
											x,
											xDistanceCoordinate,
											y,
											yDistanceCoordinate;
											
	SInt32								flag;							// error flag
											

	if (mapProjectionInfoPtr == NULL)
																							return (FALSE);
											
	if (mapProjectionInfoPtr->geodetic.spheroidCode <= kNoEllipsoidDefinedCode)
																							return (FALSE);
	
	xDistanceCoordinate = *xCoordinateValuePtr;
	yDistanceCoordinate = *yCoordinateValuePtr;
	
	centerLatitude = 
				mapProjectionInfoPtr->gridCoordinate.latitudeOrigin * kDegreesToRadians;
	centerLongitude = 
			mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian * kDegreesToRadians;
		
	e = mapProjectionInfoPtr->geodetic.e;
	e4 = mapProjectionInfoPtr->geodetic.e4;
	fac = mapProjectionInfoPtr->geodetic.fac;
	ind = mapProjectionInfoPtr->geodetic.ind;
	
/*	ind = 0;
	if (fabs(fabs(centerLongitude) - kHALF_PI) > kEPSLN)
		{
		ind = 1;
		con1 = fac * centerLatitude; 
		cosphi = cos (con1);
		sinphi = sin (con1);
		mcs = msfnz (e, sinphi, cosphi);
		tcs = tsfnz (e, con1, sinphi);
		
		}		// end "if (fabs(fabs(longitudeCentralMeridian) - kHALF_PI) > kEPSLN)"
*/	
	x = (xDistanceCoordinate - mapProjectionInfoPtr->gridCoordinate.falseEasting) * fac;
	y = (yDistanceCoordinate - mapProjectionInfoPtr->gridCoordinate.falseNorthing) * fac;
	rh = sqrt (x*x + y*y);
	
	if (ind != 0)
		ts = rh * mapProjectionInfoPtr->geodetic.tcs /
				(mapProjectionInfoPtr->geodetic.semiMajorAxis * mapProjectionInfoPtr->geodetic.mcs);
	else
		ts = rh * e4/(mapProjectionInfoPtr->geodetic.semiMajorAxis * 2.0);
	  
	*yCoordinateValuePtr = fac * phi2z (e, ts, &flag);
	if (flag != 0)
																							return (FALSE);
		
	if (rh == 0)
		*xCoordinateValuePtr = fac * centerLongitude;
	else
		{
		temp = atan2(x, -y);
		*xCoordinateValuePtr = adjust_lon (fac*temp + centerLongitude);
		}
	   
	*yCoordinateValuePtr *= kRadiansToDegrees;
	*xCoordinateValuePtr *= kRadiansToDegrees;
		
	return (TRUE);
			
}		// end "ConvertPolarStereographicToLatLong" 




//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean DetermineIfInverseLatLongPossible
//
//	Software purpose:	This routine will determine given latitude and longitude whether
//							it is possible to compute map coordinates and therefore line
//							columns.
//
//	Parameters in:		Handle to the window information for the given image window.	
//
//	Parameters out:	None		
//
// Value Returned:	True: it is possible to calculate latitude/longitud for a pixel
//							False:  it is not possible
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/17/2005
//	Revised By:			Larry L. Biehl			Date: 04/26/2012			

Boolean DetermineIfInverseLatLongPossible (
				Handle								windowInfoHandle)

{		
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	Handle								mapProjectionHandle;
	
	SInt16								projectionCode,
											gridZone,
											planarMapUnitsCode,
											spheroidCode;
										
	Boolean								enableInverseLatLongUnitsMenuItemFlag;

		
	mapProjectionHandle = 
								GetFileMapProjectionHandle2 (windowInfoHandle);
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer(
												mapProjectionHandle, kNoLock, kNoMoveHi);
								
	enableInverseLatLongUnitsMenuItemFlag = FALSE;				
	if (mapProjectionInfoPtr != NULL)
		{
		projectionCode = mapProjectionInfoPtr->gridCoordinate.projectionCode;
		planarMapUnitsCode = mapProjectionInfoPtr->planarCoordinate.mapUnitsCode;
		spheroidCode = mapProjectionInfoPtr->geodetic.spheroidCode;
		gridZone = mapProjectionInfoPtr->gridCoordinate.gridZone;
		
		if (planarMapUnitsCode == kDecimalDegreesCode)
			enableInverseLatLongUnitsMenuItemFlag = TRUE;
		 		 
		else if (projectionCode == kAlbersConicalEqualAreaCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableInverseLatLongUnitsMenuItemFlag = TRUE;
		 
		else if (projectionCode == kEquirectangularCode && 
//							spheroidCode == kSphereEllipsoidCode &&
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableInverseLatLongUnitsMenuItemFlag = TRUE; 
		 
		else if (projectionCode == kCylindricalEqualAreaCode && 
							spheroidCode == kSphereEllipsoidCode &&
														planarMapUnitsCode != kUnknownCode)
			enableInverseLatLongUnitsMenuItemFlag = TRUE;  
		 
		else if (projectionCode == kKrovakCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableInverseLatLongUnitsMenuItemFlag = TRUE; 
		 
		else if (projectionCode == kLambertAzimuthalEqualAreaCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableInverseLatLongUnitsMenuItemFlag = TRUE; 
		 
		else if (projectionCode == kLambertConformalConicCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableInverseLatLongUnitsMenuItemFlag = TRUE; 
		 
		else if (projectionCode == kMercatorCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableInverseLatLongUnitsMenuItemFlag = TRUE; 
		 
		else if (projectionCode == kOrthographicCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableInverseLatLongUnitsMenuItemFlag = TRUE; 
		 
		else if (projectionCode == kPolarStereographicCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableInverseLatLongUnitsMenuItemFlag = TRUE; 
		 
		else if (projectionCode == kSinusoidalCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableInverseLatLongUnitsMenuItemFlag = TRUE;
		 
		else if (projectionCode == kTransverseMercatorCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableInverseLatLongUnitsMenuItemFlag = TRUE;
			
		if (enableInverseLatLongUnitsMenuItemFlag)
			{
					// Now verify that the pixel sizes are not 0 or if they are
					// that the pixels map coordinates can be computed using
					// a polynomial transformation
					
			if ( (mapProjectionInfoPtr->planarCoordinate.verticalPixelSize == 0 ||
					mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize == 0) &&
						mapProjectionInfoPtr->planarCoordinate.polynomialOrder <= 0)
				enableInverseLatLongUnitsMenuItemFlag = FALSE;
			
			}		// end "if (enableInverseLatLongUnitsMenuItemFlag)"
			
		}		// end "if (mapProjectionInfoPtr != NULL)"
		
	return (enableInverseLatLongUnitsMenuItemFlag);
			
}		// end "DetermineIfInverseLatLongPossible"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean DetermineIfLatLongPossible
//
//	Software purpose:	This routine will determine if it is possible to compute
//							latitude/longitude for a given point in the image
//
//	Parameters in:		Handle to the window information for the given image window.	
//
//	Parameters out:	None		
//
// Value Returned:	True: it is possible to calculate latitude/longitud for a pixel
//							False:  it is not possible
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/15/2005
//	Revised By:			Larry L. Biehl			Date: 04/26/2012			

Boolean DetermineIfLatLongPossible (
				Handle								windowInfoHandle)

{		
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	Handle								mapProjectionHandle;
	
	SInt16								projectionCode,
											gridZone,
											planarMapUnitsCode,
											spheroidCode;
										
	Boolean								enableLatLongUnitsMenuItemFlag;

		
	mapProjectionHandle = 
								GetFileMapProjectionHandle2 (windowInfoHandle);
	
	mapProjectionInfoPtr = (MapProjectionInfoPtr)GetHandlePointer(
												mapProjectionHandle, kNoLock, kNoMoveHi);
								
	enableLatLongUnitsMenuItemFlag = FALSE;				
	if (mapProjectionInfoPtr != NULL)
		{
		projectionCode = mapProjectionInfoPtr->gridCoordinate.projectionCode;
		planarMapUnitsCode = mapProjectionInfoPtr->planarCoordinate.mapUnitsCode;
		spheroidCode = mapProjectionInfoPtr->geodetic.spheroidCode;
		gridZone = mapProjectionInfoPtr->gridCoordinate.gridZone;
			
		if (planarMapUnitsCode == kDecimalDegreesCode)
			enableLatLongUnitsMenuItemFlag = TRUE;
		 
		else if (projectionCode == kAlbersConicalEqualAreaCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableLatLongUnitsMenuItemFlag = TRUE;
		 
		else if (projectionCode == kCylindricalEqualAreaCode && 
							spheroidCode == kSphereEllipsoidCode &&
														planarMapUnitsCode != kUnknownCode)
			enableLatLongUnitsMenuItemFlag = TRUE;  
		 
		else if (projectionCode == kEquirectangularCode && 
//							spheroidCode == kSphereEllipsoidCode &&
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableLatLongUnitsMenuItemFlag = TRUE; 
		 
		else if (projectionCode == kKrovakCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableLatLongUnitsMenuItemFlag = TRUE; 
		 
		else if (projectionCode == kLambertAzimuthalEqualAreaCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableLatLongUnitsMenuItemFlag = TRUE; 
		 
		else if (projectionCode == kLambertConformalConicCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableLatLongUnitsMenuItemFlag = TRUE;  
		 
		else if (projectionCode == kMercatorCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableLatLongUnitsMenuItemFlag = TRUE; 
		 
		else if (projectionCode == kOrthographicCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableLatLongUnitsMenuItemFlag = TRUE;  
		 
		else if (projectionCode == kPolarStereographicCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableLatLongUnitsMenuItemFlag = TRUE;
		 
		else if (projectionCode == kSinusoidalCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableLatLongUnitsMenuItemFlag = TRUE; 
		 
		else if (projectionCode == kTransverseMercatorCode && 
							spheroidCode != kNoEllipsoidDefinedCode &&
														planarMapUnitsCode != kUnknownCode)
			enableLatLongUnitsMenuItemFlag = TRUE;
			
		if (enableLatLongUnitsMenuItemFlag)
			{
					// Now verify that the pixel sizes are not 0 or if they are
					// that the pixels map coordinates can be computed using
					// a polynomial transformation
					
			if ( (mapProjectionInfoPtr->planarCoordinate.verticalPixelSize == 0 ||
					mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize == 0) &&
						mapProjectionInfoPtr->planarCoordinate.polynomialOrder <= 0)
				enableLatLongUnitsMenuItemFlag = FALSE;
			
			}		// end "if (enableLatLongUnitsMenuItemFlag)"
			
		}		// end "if (mapProjectionInfoPtr != NULL)"
		
	return (enableLatLongUnitsMenuItemFlag);
			
}		// end "DetermineIfLatLongPossible"

	

//------------------------------------------------------------------------------------
//								 Copyright (1988-2015)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		Boolean GetProjectionParameters
//
//	Software purpose:	This routine returns the parameters specific for the input
// 						projection code.
//
//	Parameters in:					
//
//	Parameters out:				
//
//	Value Returned:	None
//
// Called By:			LoadGeoKeyDirectory in SSaveWrt.cpp
//							ListMapParameters in SOther.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/10/2006
//	Revised By:			Larry L. Biehl			Date: 12/22/2015

void GetProjectionParameters (
				MapProjectionInfoPtr				mapProjectionInfoPtr,
				Boolean								includeUTMFlag,
				double*								projStdParallel1Ptr,
				double*								projStdParallel2Ptr,
				double*								projNatOriginLongPtr,
				double*								projNatOriginLatPtr,
				double*								projFalseEastingPtr,
				double*								projFalseNorthingPtr,
				double*								projFalseOriginLongPtr,
				double*								projFalseOriginLatPtr,
				double*								projFalseOriginEastingPtr,
				double*								projFalseOriginNorthingPtr,
				double*								projCenterLongPtr,
				double*								projCenterLatPtr,
				double*								projScaleAtNatOriginPtr,
				double*								projAzimuthAnglePtr,
				double*								projStraightVertPoleLongGeoKeyPtr)

{ 
	SInt16							projectionCode;
	
	
			// Get parameters for the projection
			
	*projStdParallel1Ptr = -9999;
	*projStdParallel2Ptr = -9999;
	*projNatOriginLatPtr = -9999;
	*projNatOriginLongPtr = -9999;
	*projFalseEastingPtr = -9999;
	*projFalseNorthingPtr = -9999;
	*projFalseOriginLongPtr = -9999;
	*projFalseOriginLatPtr = -9999;
	*projFalseOriginEastingPtr = -9999;
	*projFalseOriginNorthingPtr = -9999;
	*projCenterLongPtr = -9999;
	*projCenterLatPtr = -9999;
	*projScaleAtNatOriginPtr = -9999;
	*projAzimuthAnglePtr = -9999;
	*projStraightVertPoleLongGeoKeyPtr = -9999;
	
	projectionCode = mapProjectionInfoPtr->gridCoordinate.projectionCode;
	
	switch (projectionCode)
		{
//		case kUTMCode:
//		case kStatePlaneCode:
//			if (!includeUTMFlag)
//				break;
			
		case kAlaskaConformalCode:
		case kMercatorCode:
		case kPolyconicCode:
		case kTransverseMercatorCode:
		case kTransvMercatorSouthOrientedCode:
			*projNatOriginLongPtr = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
			*projNatOriginLatPtr = mapProjectionInfoPtr->gridCoordinate.latitudeOrigin;
			*projScaleAtNatOriginPtr = mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian;
			*projFalseEastingPtr = mapProjectionInfoPtr->gridCoordinate.falseEasting;
			*projFalseNorthingPtr = mapProjectionInfoPtr->gridCoordinate.falseNorthing;
			break;
			
		case kAzimuthalEquidistantCode:
		case kGnomonicCode:
		case kLambertAzimuthalEqualAreaCode:
		case kMillerCylindricalCode:
		case kOrthographicCode:
			*projCenterLongPtr = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
			*projCenterLatPtr = mapProjectionInfoPtr->gridCoordinate.latitudeOrigin;
			*projFalseEastingPtr = mapProjectionInfoPtr->gridCoordinate.falseEasting;
			*projFalseNorthingPtr = mapProjectionInfoPtr->gridCoordinate.falseNorthing;
			break;
			
		case kEquirectangularCode:
			*projCenterLongPtr = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
			*projStdParallel1Ptr = mapProjectionInfoPtr->gridCoordinate.standardParallel1;
			break;
			
		case kAlbersConicalEqualAreaCode:
		case kEquidistantConicCode:
			*projStdParallel1Ptr = mapProjectionInfoPtr->gridCoordinate.standardParallel1;
			*projStdParallel2Ptr = mapProjectionInfoPtr->gridCoordinate.standardParallel2;
			*projNatOriginLongPtr = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
			*projNatOriginLatPtr = mapProjectionInfoPtr->gridCoordinate.latitudeOrigin;
			*projFalseEastingPtr = mapProjectionInfoPtr->gridCoordinate.falseEasting;
			*projFalseNorthingPtr = mapProjectionInfoPtr->gridCoordinate.falseNorthing;
			break;
			
		case kLambertConformalConicCode:
			*projStdParallel1Ptr = mapProjectionInfoPtr->gridCoordinate.standardParallel1;
			*projStdParallel2Ptr = mapProjectionInfoPtr->gridCoordinate.standardParallel2;
			*projFalseOriginLongPtr = mapProjectionInfoPtr->gridCoordinate.falseOriginLongitude;
			*projFalseOriginLatPtr = mapProjectionInfoPtr->gridCoordinate.falseOriginLatitude;
			*projFalseOriginEastingPtr = mapProjectionInfoPtr->gridCoordinate.falseOriginEasting;
			*projFalseOriginNorthingPtr = mapProjectionInfoPtr->gridCoordinate.falseOriginNorthing;
			break;
			
		case kStereographicCode:
			*projCenterLongPtr = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
			*projCenterLatPtr = mapProjectionInfoPtr->gridCoordinate.latitudeOrigin;
			*projScaleAtNatOriginPtr = mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian;
			*projFalseEastingPtr = mapProjectionInfoPtr->gridCoordinate.falseEasting;
			*projFalseNorthingPtr = mapProjectionInfoPtr->gridCoordinate.falseNorthing;
			break;
			
		case kPolarStereographicCode:
			*projStraightVertPoleLongGeoKeyPtr = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
			*projCenterLatPtr = mapProjectionInfoPtr->gridCoordinate.latitudeOrigin;
			*projFalseEastingPtr = mapProjectionInfoPtr->gridCoordinate.falseEasting;
			*projFalseNorthingPtr = mapProjectionInfoPtr->gridCoordinate.falseNorthing;
			break;
		
		case kMollweideCode:
//		case kSinusoidalCode:
		case kVanderGrintenICode:
			*projCenterLongPtr = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
			*projFalseEastingPtr = mapProjectionInfoPtr->gridCoordinate.falseEasting;
			*projFalseNorthingPtr = mapProjectionInfoPtr->gridCoordinate.falseNorthing;
			break;
			
		case kNewZealandMapGridCode:
			*projNatOriginLongPtr = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
			*projNatOriginLatPtr = mapProjectionInfoPtr->gridCoordinate.latitudeOrigin;
			*projFalseEastingPtr = mapProjectionInfoPtr->gridCoordinate.falseEasting;
			*projFalseNorthingPtr = mapProjectionInfoPtr->gridCoordinate.falseNorthing;
			break;
			
		case kCylindricalEqualAreaCode:
			*projStdParallel1Ptr = mapProjectionInfoPtr->gridCoordinate.standardParallel1;
			*projNatOriginLongPtr = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
			*projFalseEastingPtr = mapProjectionInfoPtr->gridCoordinate.falseEasting;
			*projFalseNorthingPtr = mapProjectionInfoPtr->gridCoordinate.falseNorthing;
			break;
			
		case kSpaceObliqueMercatorCode:
			*projFalseEastingPtr = mapProjectionInfoPtr->gridCoordinate.falseEasting;
			*projFalseNorthingPtr = mapProjectionInfoPtr->gridCoordinate.falseNorthing;
			*projCenterLongPtr = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
			*projCenterLatPtr = mapProjectionInfoPtr->gridCoordinate.latitudeOrigin;
			*projScaleAtNatOriginPtr = mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian;
			*projAzimuthAnglePtr = mapProjectionInfoPtr->gridCoordinate.projAzimuthAngle;
			break;
		
		case kSinusoidalCode:
			*projCenterLongPtr = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
			break;
			
		case kKrovakCode:
			*projFalseEastingPtr = mapProjectionInfoPtr->gridCoordinate.falseEasting;
			*projFalseNorthingPtr = mapProjectionInfoPtr->gridCoordinate.falseNorthing;
			*projCenterLongPtr = mapProjectionInfoPtr->gridCoordinate.longitudeCentralMeridian;
			*projCenterLatPtr = mapProjectionInfoPtr->gridCoordinate.latitudeOrigin;
			*projScaleAtNatOriginPtr = mapProjectionInfoPtr->gridCoordinate.scaleFactorOfCentralMeridian;
			*projAzimuthAnglePtr = mapProjectionInfoPtr->gridCoordinate.projAzimuthAngle;
			*projStdParallel1Ptr = mapProjectionInfoPtr->gridCoordinate.standardParallel1;
			break;
		
		}		// end "switch (projectionCode)"
		
			// The false easting and northing is always stored in meters for calculations.
			// Convert to feet if that is what the planar units are in for interface to users.
				
	if (*projFalseEastingPtr != -9999)
		ConvertMetersToFeet (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode, 
										projFalseEastingPtr, 
										projFalseNorthingPtr);
				
	if (*projFalseOriginEastingPtr != -9999)
		ConvertMetersToFeet (mapProjectionInfoPtr->planarCoordinate.mapUnitsCode, 
										projFalseOriginEastingPtr, 
										projFalseOriginNorthingPtr);
		
}		// end "GetProjectionParameters" 



// Function to compute the constant small m which is the radius of
//   a parallel of latitude, phi, divided by the semimajor axis.

double msfnz (
				double								eccent,
				double								sinphi,
				double								cosphi)
				
{
	double 								con;

   con = eccent * sinphi;
   return((cosphi / (sqrt (1.0 - con * con))));
      
}		// end "msfnz"



// Function to compute phi1, the latitude for the inverse of the
//   Albers Conical Equal-Area projection.

double phi1z (
				double								eccent,	// Eccentricity angle in radians
				double								qs,		// Angle in radians
				long*									flag)		// Error flag number
{
	double eccnts;
	double dphi;
	double con;
	double com;
	double sinpi;
	double cospi;
	double phi;
	long i;

   phi = asinz(.5 * qs);
   if (eccent < kEPSLN) 
      return(phi);
   eccnts = eccent * eccent; 
   for (i = 1; i <= 25; i++)
     {
     cospi = cos(phi);
     sinpi = sin(phi);
     con = eccent * sinpi; 
     com = 1.0 - con * con;
     dphi = .5 * com * com / cospi * (qs / (1.0 - eccnts) - sinpi / com + 
            .5 / eccent * log ((1.0 - con) / (1.0 + con)));
    	phi = phi + dphi;
    	if (fabs(dphi) <= 1e-7)
       	return(phi);
      }
      
//  p_error ("Convergence error","phi1z-conv");
  *flag = 001;
  return (0);		// ERROR
  
}		// end "phi1z"

/* Function to compute the latitude angle, phi2, for the inverse of the
   Lambert Conformal Conic and Polar Stereographic projections.
----------------------------------------------------------------*/
double phi2z (
				double								eccent,		// Spheroid eccentricity
				double								ts,			// Constant value t
				SInt32*								flagPtr)		// Error flag number
{
	double									eccnth,
												phi,
												con,
												dphi,
												sinpi;
												
	SInt32									i;


	*flagPtr = 0;
	eccnth = .5 * eccent;
	phi = kHALF_PI - 2 * atan(ts);
	for (i = 0; i <= 15; i++)
		{
		sinpi = sin(phi);
		con = eccent * sinpi;
		dphi = kHALF_PI - 2 * atan(ts *(pow(((1.0 - con)/(1.0 + con)),eccnth))) - phi;
		phi += dphi; 
		if (fabs(dphi) <= .0000000001)
																				return(phi);
			
		}		// end "for (i = 0; i <= 15; i++)"
		
			// Convergence error
			
  *flagPtr = 002;
  return(002);
  
}		// end "phi2z"



// Function to compute constant small q which is the radius of a 
//   parallel of latitude, phi, divided by the semimajor axis. 

double qsfnz (
				double								eccent,
				double								sinphi,
				double								cosphi)
{
	double 								con;

   if (eccent > 1.0e-7)
		{
		con = eccent * sinphi;
		return (( 1.0- eccent * eccent) * (sinphi /(1.0 - con * con) - (.5/eccent)*
             log((1.0 - con)/(1.0 + con))));
		}
		
   else
     return(2.0 * sinphi);
   
}		// end "qsfnz"



// Function to return the sign of an argument
//  ------------------------------------------
int sign(
				double 		x)
								 
{
	if (x < 0.0) 
		return(-1); 
		
	else 
		return(1);
		
}		// end "sign"



//		Function to compute the constant small t for use in the forward
//   computations in the Lambert Conformal Conic and the Polar
//   Stereographic projections.
// --------------------------------------------------------------*/
double tsfnz(
				double eccent,	// Eccentricity of the spheroid
				double phi,		// Latitude phi
				double sinphi)	// Sine of the latitude
{
  double					con,
							com;
  
  con = eccent * sinphi;
  com = .5 * eccent; 
  con = pow(((1.0 - con) / (1.0 + con)),com);
  return (tan(.5 * (kHALF_PI - phi))/con);
  
}		// end "tsfnz"
