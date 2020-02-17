//                                     MultiSpec
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use
// this file except in compliance with the License. You may obtain a copy of the
// License at:  https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
// language governing permissions and limitations under the License.
//
// MultiSpec is curated by the Laboratory for Applications of Remote Sensing at
// Purdue University in West Lafayette, IN and licensed by Larry Biehl.
//
//	File:						SRegistration.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			12/21/2017
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems	
//
//	Brief description:	The routines in this file get the polynomial model to
//								relate line/column to map location. It is used by the
//								GDAL interface file and the MultiSpec GeoTIFF file.
//
//								Some of the code in this file comes from the routines for
//								GRASS:
//									CRS.C - Center for Remote Sensing rectification routines
//
//									Written By: Brian J. Buckley
//
//									At: The Center for Remote Sensing
//									Michigan State University
//									302 Berkey Hall
//									East Lansing, MI  48824
//									(517)353-7195
//
//									Written: 12/19/91
//									Last Update:  1/27/92 Brian J. Buckley
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#if defined multispec_wx
#endif
	
#if defined multispec_mac 
#endif	// defined multispec_mac   
                             
#if defined multispec_win
#endif	// defined multispec_win

struct MATRIX
{
	UInt32 n;     // SIZE OF THIS MATRIX (N x N)
	double *v;
};


		// CALCULATE OFFSET INTO ARRAY BASED ON R/C

#define M(row,col) m->v[(((row)-1)*(m->n))+(col)-1]

#define MSUCCESS     1 /* SUCCESS */
#define MNPTERR      0 /* NOT ENOUGH POINTS */
#define MUNSOLVABLE -1 /* NOT SOLVABLE */
#define MMEMERR     -2 /* NOT ENOUGH MEMORY */
#define MPARMERR    -3 /* PARAMETER ERROR */
#define MINTERR     -4 /* INTERNAL ERROR */

#define MAXORDER 3
								

		// Prototypes for routines in this file that are only called by
		// other routines in this file.				

SInt16 	ComputeGeorefCoefficients (
				ControlPointsPtr 					controlPoints,
				double 								E12[], 
				double 								N12[], 
				double 								E21[], 
				double 								N21[], 
				SInt16								order);									

SInt16	CalculateCoefficients (
				ControlPointsPtr 					controlPointsPtr,
				double 								E[], 
				double 								N[], 
				SInt16								order);

SInt16	CalculateWithLeastSquares	(
				ControlPointsPtr 					controlPointsPtr,
				struct MATRIX 						*m,
				double 								a[],
				double 								b[],
				double 								E[],
				double 								N[]);

SInt16	ExactDetermination (
				ControlPointsPtr 					controlPointsPtr,
				struct MATRIX 						*m,
				double 								a[],
				double 								b[],
				double 								E[],
				double 								N[]);

SInt16	SolveMatrix (
				struct MATRIX 						*m,
				double 								a[], 
				double 								b[], 
				double 								E[], 
				double 								N[]);

double	TermValue (
				UInt32								term,
				double 								e, 
				double 								n);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CalculateCoefficients
//
//	Software purpose:	The purpose of this routine is to compute the georeferencing
//							coefficients based on the input set of control points.
//
//							This code is based on a routine in the GRASS crs.c 
//							CRS.C - Center for Remote Sensing rectification routines
//							Written By: Brian J. Buckley
//							At: The Center for Remote Sensing
//							Michigan State University
//							302 Berkey Hall
//							East Lansing, MI  48824
//							(517)353-7195
//							Written: 12/19/91
//							Last Update:  1/27/92 Brian J. Buckley
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/08/2007
//	Revised By:			Larry L. Biehl			Date: 02/10/2007	

SInt16 CalculateCoefficients (
				ControlPointsPtr 					controlPointsPtr, 
				double 								E[], 
				double 								N[], 
				SInt16								order)

{
	struct MATRIX 						m;
	
	double 								*a,
			 								*b;
			 								
	UInt32								numactive,   // Number of active control points
		 									i;

	SInt16								status = MUNSOLVABLE;


			// Calculate the number of valid control points

	for (i=numactive=0; i<(UInt32)controlPointsPtr->count; i++)
		{
		if (controlPointsPtr->statusPtr[i] > 0)
			numactive++;
			
		}	// end "for (i=numactive=0; i<(UInt32)controlPointsPtr->count; i++)"

			// Calculate the minimum number of control points needed to 
			// determine a transformation of this order.

	m.n = ((order + 1) * (order + 2)) / 2;

	if (numactive < m.n)
																						return MNPTERR;

			// Initialize matrix

	m.v = (double*)MNewPointerClear (m.n * m.n * sizeof (double));
	a = (double*)MNewPointerClear (m.n * sizeof (double));
	b = (double*)MNewPointerClear (m.n * sizeof (double));

	if (numactive == m.n)
		status = ExactDetermination (controlPointsPtr, &m, a, b, E, N);
	
	else	// numactive > m.n
		status = CalculateWithLeastSquares (controlPointsPtr, &m, a, b, E, N);

	CheckAndDisposePtr (m.v);
	CheckAndDisposePtr (a);
	CheckAndDisposePtr (b);

	return (status);
	
}	// end "CalculateCoefficients"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 CalculateWithLeastSquares
//
//	Software purpose:	The purpose of this routine is to calculate the
//							transformation coefficients with more than the minimum
//							number of control points required for this transformation
//							order.  This routine uses the least squares method to
//							compute the coefficients.
//
//							This code is based on a routine in the GRASS crs.c 
//							CRS.C - Center for Remote Sensing rectification routines
//							Written By: Brian J. Buckley
//							At: The Center for Remote Sensing
//							Michigan State University
//							302 Berkey Hall
//							East Lansing, MI  48824
//							(517)353-7195
//							Written: 12/19/91
//							Last Update:  1/27/92 Brian J. Buckley
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/08/2007
//	Revised By:			Larry L. Biehl			Date: 02/10/2007	

SInt16 CalculateWithLeastSquares (
				ControlPointsPtr 					controlPointsPtr,
				struct MATRIX 						*m,
				double 								a[],
				double 								b[],
				double 								E[],     // easting coefficients
				double 								N[])     // northing coefficients

{
	UInt32								i, 
											j,
											n, 
											numactive = 0;
	
	
			// Initialize the upper half of the matrix and the two column vectors.
			// The array and vector have already been initialized to 0.
	/*
	for (i=1; i<= m->n; i++)
		{
		for (j = i; j <= m->n; j++)
			M (i,j) = 0.0;

		a[i-1] = b[i-1] = 0.0;

		}	// end "for (i=1; i<= m->n; i++)"
	*/
			// Sum the upper half of the atrix and the column vectors according
			// to the least squares method of solving over determined systems.
	
	for (n=0; n<(UInt32)controlPointsPtr->count; n++)
		{
		if (controlPointsPtr->statusPtr[n] > 0)
			{
			numactive++;
			for (i=1; i<=m->n; i++)
				{
				for (j = i; j <= m->n; j++)
					M (i,j) += TermValue (i,
												controlPointsPtr->easting1Ptr[n], 
												controlPointsPtr->northing1Ptr[n]) * 
											TermValue (j,
															controlPointsPtr->easting1Ptr[n],
															controlPointsPtr->northing1Ptr[n]);
				
				a[i-1] += controlPointsPtr->easting2Ptr[n] * TermValue (
																i,
																controlPointsPtr->easting1Ptr[n],
																controlPointsPtr->northing1Ptr[n]);
				
				b[i-1] += controlPointsPtr->northing2Ptr[n] * TermValue (
																i,
																controlPointsPtr->easting1Ptr[n],
																controlPointsPtr->northing1Ptr[n]);
				
				}	// end "for (i=1; i<=m->n; i++)"
			
			}	// end "if (controlPointsPtr->status[n] > 0)"
		
		}	// end "for (n=0; n<controlPointsPtr->count; n++)"
	
	if (numactive <= m->n)
																							return MINTERR;
	
			// Transpose values in upper half of M to other half.
	
	for (i=2; i<=m->n; i++)
		for (j=1; j<i; j++)
			M (i,j) = M (j,i);
	
	return SolveMatrix (m, a, b, E, N);
	
}	// end "CalculateWithLeastSquares"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ComputeGeorefCoefficients
//
//	Software purpose:	The purpose of this routine is to compute the georeferencing
//							coefficients based on the input set of control points.
//
//							This code is based on a routine in the GRASS crs.c 
//							CRS.C - Center for Remote Sensing rectification routines
//							Written By: Brian J. Buckley
//							At: The Center for Remote Sensing
//							Michigan State University
//							302 Berkey Hall
//							East Lansing, MI  48824
//							(517)353-7195
//							Written: 12/19/91
//							Last Update:  1/27/92 Brian J. Buckley
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/08/2007
//	Revised By:			Larry L. Biehl			Date: 02/08/2007	

SInt16 ComputeGeorefCoefficients (
				ControlPointsPtr 					controlPointsPtr,
				double 								E12[], 
				double 								N12[], 
				double 								E21[], 
				double 								N21[], 
				SInt16								order)

{
	double 								*tempptr;
	
	SInt16								status;
	
	
	if (order >= 1 && order <= MAXORDER)
		{
				// Calculate the forward transformation coefficients
		
		status = CalculateCoefficients (controlPointsPtr, E12, N12, order);
		
		if (status == MSUCCESS)
			{
					// Switch the 1 and 2 easting and northing arrays
			
			tempptr = controlPointsPtr->easting1Ptr; 
			controlPointsPtr->easting1Ptr = controlPointsPtr->easting2Ptr; 
			controlPointsPtr->easting2Ptr = tempptr;
			
			tempptr = controlPointsPtr->northing1Ptr; 
			controlPointsPtr->northing1Ptr = controlPointsPtr->northing2Ptr; 
			controlPointsPtr->northing2Ptr = tempptr;
			
					// Calculate the backward transformation coefficients
			
			status = CalculateCoefficients (controlPointsPtr, E21, N21, order);
			
					// Switch the 1 and 2 easting and northing arrays back
			
			tempptr = controlPointsPtr->easting1Ptr; 
			controlPointsPtr->easting1Ptr = controlPointsPtr->easting2Ptr; 
			controlPointsPtr->easting2Ptr = tempptr;
			
			tempptr = controlPointsPtr->northing1Ptr; 
			controlPointsPtr->northing1Ptr = controlPointsPtr->northing2Ptr; 
			controlPointsPtr->northing2Ptr = tempptr;
			
			}	// end "if (status == MSUCCESS)"
		
		}	// end "if (order >= 1 && order <= MAXORDER)"
	
	else	// end "order < 1 || order > MAXORDER"
		status = MPARMERR;
	
	return (status);
	
}	// end "ComputeGeorefCoefficients"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ExactDetermination
//
//	Software purpose:	The purpose of this routine is to calculate the
//							transformation coefficients with exactly the minimum
//							number of control points required for this 
//							transformation order.
//
//							This code is based on a routine in the GRASS crs.c 
//							CRS.C - Center for Remote Sensing rectification routines
//							Written By: Brian J. Buckley
//							At: The Center for Remote Sensing
//							Michigan State University
//							302 Berkey Hall
//							East Lansing, MI  48824
//							(517)353-7195
//							Written: 12/19/91
//							Last Update:  1/27/92 Brian J. Buckley
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/08/2007
//	Revised By:			Larry L. Biehl			Date: 02/08/2007	

SInt16 ExactDetermination (
				ControlPointsPtr 					controlPointsPtr, 
				struct MATRIX 						*m,
				double 								a[],
				double 								b[],
				double 								E[],     /* EASTING COEFFICIENTS */
				double 								N[])     /* NORTHING COEFFICIENTS */

{
	UInt32 								pntnow,
											currow, 
											j;


	currow = 1;
	for (pntnow=0; pntnow<(UInt32)controlPointsPtr->count; pntnow++)
		{
		if (controlPointsPtr->statusPtr[pntnow] > 0)
			{
					// Populate matrix M

			for (j=1; j<=m->n; j++)
				M (currow,j) = TermValue (j,
													controlPointsPtr->easting1Ptr[pntnow],
													controlPointsPtr->northing1Ptr[pntnow]);

					// Populate matrix A and B

			a[currow-1] = controlPointsPtr->easting2Ptr[pntnow];
			b[currow-1] = controlPointsPtr->northing2Ptr[pntnow];

			currow++;
			
			}	// end "if (controlPointsPtr->statusPtr[pntnow] > 0)"
			
		}	// end "for (pntnow = 0; pntnow < cp->count; pntnow++)"

	if (currow - 1 != m->n)
		return MINTERR;

	return SolveMatrix (m, a, b, E, N);
	
}	// end "ExactDetermination"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetPolynomialModel
//
//	Software purpose:	The purpose of this routine is to determine if a polynomial
//							model is need and if so get the coefficients for that model.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/09/2007
//	Revised By:			Larry L. Biehl			Date: 12/21/2010	

SInt16 GetPolynomialModel (
				FileInfoPtr							fileInfoPtr)

{
	ControlPointsPtr		 			controlPointsPtr;
	MapProjectionInfoPtr				mapProjectionInfoPtr;
	
	Handle								mapProjectionHandle;
	
	UInt32								i;
	
	SInt16								numberActiveControlPoints,
											numberCoefficients,
											order,
											returnCode = noErr;
	
	
	mapProjectionHandle = GetFileMapProjectionHandle (fileInfoPtr);
	
	if (mapProjectionHandle != NULL && fileInfoPtr->controlPointsHandle != NULL)
		{ 								
		mapProjectionInfoPtr = (MapProjectionInfoPtr)
		GetHandlePointer (mapProjectionHandle,
								kLock);
		
		controlPointsPtr = GetControlPointVectorPointers (
																	fileInfoPtr->controlPointsHandle,
																	kLock);
						
				// Get the number of valid control points
		
		numberActiveControlPoints = 0;
		for (i=0; i<(UInt32)controlPointsPtr->count; i++)
			{
			if (controlPointsPtr->statusPtr[i] > 0)
				numberActiveControlPoints++;
				
			}	// end "for (i=0; i<(UInt32)controlPointsPtr->count; i++)"
					
		if (numberActiveControlPoints >= 3)
			{
					// Get the order of the default order of the polynomial
			
			order = 0;
			if (numberActiveControlPoints < 6)
				order = 1;
			
			else if (numberActiveControlPoints < 10)
				order = 2;
			
			else	// numberActiveControlPoints >= 10
				order = 3;
			
			do 
				{
				numberCoefficients = (order+1) * (order+2) / 2;
				
						// Get memory to store the coefficients in.
						// Be sure any existing memory is released.
				
				UnlockAndDispose (mapProjectionInfoPtr->coefficientsHandle);
				mapProjectionInfoPtr->coefficientsHandle = MNewHandle (
														4 * numberCoefficients * sizeof (double));
				mapProjectionInfoPtr->numberCoefficients = numberCoefficients;
				
				GetCoefficientsVectorPointers (mapProjectionInfoPtr);
				
				returnCode = ComputeGeorefCoefficients (
							controlPointsPtr,
							mapProjectionInfoPtr->planarCoordinate.easting1CoefficientsPtr, 
							mapProjectionInfoPtr->planarCoordinate.northing1CoefficientsPtr, 
							mapProjectionInfoPtr->planarCoordinate.easting2CoefficientsPtr, 
							mapProjectionInfoPtr->planarCoordinate.northing2CoefficientsPtr, 
							order);
				
				if (returnCode != MSUCCESS)
					order--;
				
				}	while ((order > 0) && (returnCode != MSUCCESS));
			
			CloseCoefficientsVectorPointers (mapProjectionInfoPtr);
			
			if (returnCode == MSUCCESS)
				{
				mapProjectionInfoPtr->planarCoordinate.polynomialOrder = order;
				
						// Force the horizontal and vertical pixel sizes to be 0 so that
						// distances are "measured" with the spatial model.
				
				mapProjectionInfoPtr->planarCoordinate.horizontalPixelSize = 0;
				mapProjectionInfoPtr->planarCoordinate.verticalPixelSize = 0;
				
				}	// end "if (returnCode == MSUCCESS)"
			
			else	// returnCode != MSUCCESS
				{
				UnlockAndDispose (mapProjectionInfoPtr->coefficientsHandle);
				mapProjectionInfoPtr->numberCoefficients = 0;
					
				}	// end "returnCode != MSUCCESS"
			
			}	// end "if (numberActiveControlPoints >= 3"
			
		CheckAndUnlockHandle (mapProjectionHandle); 
		CloseControlPointVectorPointers (fileInfoPtr->controlPointsHandle);
			
		}	// end "if (mapProjectionHandle != NULL)"
	
	return (returnCode);
	
}	// end "GetPolynomialModel"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 SolveMatrix
//
//	Software purpose:	The purpose of this routine is to solve for the 'E' and 'N'
//							coefficients by using a somewhat modified gaussian 
//							elimination method.
//
//							| M11 M12 ... M1n | | E0   |   | a0   |
//							| M21 M22 ... M2n | | E1   | = | a1   |
//							|  .   .   .   .  | | .    |   | .    |
//							| Mn1 Mn2 ... Mnn | | En-1 |   | an-1 |
//							
//							and
//							
//							| M11 M12 ... M1n | | N0   |   | b0   |
//							| M21 M22 ... M2n | | N1   | = | b1   |
//							|  .   .   .   .  | | .    |   | .    |
//							| Mn1 Mn2 ... Mnn | | Nn-1 |   | bn-1 |
//							
//							This code is based on a routine in the GRASS crs.c 
//							CRS.C - Center for Remote Sensing rectification routines
//							Written By: Brian J. Buckley
//							At: The Center for Remote Sensing
//							Michigan State University
//							302 Berkey Hall
//							East Lansing, MI  48824
//							(517)353-7195
//							Written: 12/19/91
//							Last Update:  1/27/92 Brian J. Buckley
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/08/2007
//	Revised By:			Larry L. Biehl			Date: 02/10/2007	

SInt16 SolveMatrix (
				struct MATRIX 						*m,
				double 								a[], 
				double 								b[], 
				double 								E[], 
				double 								N[])

{
	UInt32								i, 
											j, 
											i2, 
											j2, 
											imark;
											
	double 								factor, 
											temp,
			 								pivot;  // actual value of the largest pivot candidate

	
	for (i=1; i<=m->n; i++)
		{
		j = i;

				// Find row with largest magnitude value for pivot value

		pivot = M (i,j);
		imark = i;
		for (i2=i+1; i2<=m->n; i2++)
			{
			temp = fabs (M (i2,j));
			if (temp > fabs (pivot))
				{
				pivot = M (i2,j);
				imark = i2;
				
				}	// end "if (temp > fabs (pivot))"
				
			}	// end "for (i2=i+1; i2<=m->n; i2++)"

				// if the pivot is very small then the points are nearly co-linear 
				// co-linear points result in an undefined matrix, and nearly 
				// co-linear points results in a solution with rounding error 

		if (pivot == 0.0)
																					return MUNSOLVABLE;

				// if row with highest pivot is not the current row, switch them
 
		if (imark != i)
			{
			for (j2=1; j2<=m->n; j2++)
				{
				temp = M (imark,j2);
				M (imark,j2) = M (i,j2);
				M (i,j2) = temp;
				
				}	// end "for (j2=1; j2<=m->n; j2++)"

			temp = a[imark-1];
			a[imark-1] = a[i-1];
			a[i-1] = temp;

			temp = b[imark-1];
			b[imark-1] = b[i-1];
			b[i-1] = temp;
			
			}	// end "if (imark != i)"

				// compute zeros above and below the pivot, and compute
				// values for the rest of the row as well

		for (i2=1; i2<=m->n; i2++)
			{
			if (i2 != i)
				{
				factor = M (i2,j) / pivot;
				for (j2 = j; j2 <= m->n; j2++)
					M (i2,j2) -= factor * M (i,j2);
				a[i2-1] -= factor * a[i-1];
				b[i2-1] -= factor * b[i-1];
				
				}	// end "if (i2 != i)"
				
			}	// end "for (i2=1; i2<=m->n; i2++)"
			
		}	// end "for (i=1; i<=m->n; i++)"

			// SINCE ALL OTHER VALUES IN THE MATRIX ARE ZERO NOW, CALCULATE THE
			// COEFFICIENTS BY DIVIDING THE COLUMN VECTORS BY THE DIAGONAL VALUES.

	for (i=1; i<=m->n; i++)
		{
		E[i-1] = a[i-1] / M (i,i);
		N[i-1] = b[i-1] / M (i,i);
		
		}	// end "for (i=1; i<=m->n; i++)"

	return MSUCCESS;
	
}	// end "SolveMatrix"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double TermValue
//
//	Software purpose:	The purpose of this routine is to calculate the
//							X/Y term based on the term number.
//							ORDER\TERM   1    2    3    4    5    6    7    8    9   10
//								1        e0n0 e1n0 e0n1
//								2        e0n0 e1n0 e0n1 e2n0 e1n1 e0n2
//								3        e0n0 e1n0 e0n1 e2n0 e1n1 e0n2 e3n0 e2n1 e1n2 e0n3
//
//							This code is based on a routine in the GRASS crs.c 
//							CRS.C - Center for Remote Sensing rectification routines
//							Written By: Brian J. Buckley
//							At: The Center for Remote Sensing
//							Michigan State University
//							302 Berkey Hall
//							East Lansing, MI  48824
//							(517)353-7195
//							Written: 12/19/91
//							Last Update:  1/27/92 Brian J. Buckley
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 02/08/2007
//	Revised By:			Larry L. Biehl			Date: 02/08/2007	

double TermValue (
				UInt32								term,
				double 								e, 
				double 								n)

{
	switch (term)
		{
		case  1: return 1.0;
		case  2: return e;
		case  3: return n;
		case  4: return e*e;
		case  5: return e*n;
		case  6: return n*n;
		case  7: return e*e*e;
		case  8: return e*e*n;
		case  9: return e*n*n;
		case 10: return n*n*n;
		
		}	// end "switch (term)"
	
	return 0.0;
	
}	// end "TermValue"

