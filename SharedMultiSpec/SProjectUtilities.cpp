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
//	File:						SProjectUtilities.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			11/25/2019
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file is to provide utility
//								type functions for projects in MultiSpec.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 

#if defined multispec_wx
	#include "xClassWeightsDialog.h"
	#include "xClassPairWeightsDialog.h"
	#include "xImageView.h"
#endif

#if defined multispec_mac || defined multispec_mac_swift
	#define IDOK								1

	#define IDC_NewWeight					9
	#define IDC_AddButton					10
	#define IDC_RemoveButton				11
	#define IDC_DefaultWeight				14

	#define IDC_EnterNewWeightButton		8
	#define IDC_Weight						9
	#define IDC_EqualWeightButton			10
	#define IDC_UnitsRelative				12
	#define IDC_UnitsPercent				13
	#define IDC_WeightTotal					15
#endif	// defined multispec_mac || defined multispec_mac_swift 

#if defined multispec_win 
	#include "WImageView.h"
	#include "WClassPairWeightsDialog.h"
	#include "WClassWeightsDialog.h"
	#include "WStatisticsDoc.h"
	#include "WStatisticsView.h"

	extern void LGetCell (
					char*									stringPtr,
					SInt16*								stringLengthPtr,
					Cell									cell,
					CListBox*							listBoxCPtr);

	extern void SetPort (
					DialogPtr							dialogPtr);
#endif	// defined multispec_win 



		// Prototypes for routines in this file that are only called by
		// other routines in this file.		

Boolean CheckClassStats (
				UInt32*								numberClasses,
				SInt16*								classPtr,
				SInt16								statisticsType,
				SInt16								covarianceStatsToUse,
				Boolean								checkOnlyFlag,
				SInt32*								minimumNumberTrainPixelsPtr,
				Boolean*								computeCommonCovarianceFlagPtr);

Boolean ClassToBeUsed (
				SInt16								classIndex,
				SInt16*								classPtr,
				UInt16								numberClasses);

Boolean ClassPairWeightsDialogModal (
				DialogPtr							dialogPtr,
				UInt16								numberOfClassesToUse);

Boolean ClassWeightsDialogModal (
				DialogPtr							dialogPtr,
				UInt16								numberOfClassesToUse,
				double*								weightSum);

Boolean IsClassData (
				AreaDescriptionPtr projectAreaDescriptionPtr,
				UInt32								classNumber,
				UInt32								pixelLine,
				UInt32								pixelColumn);

Boolean IsFieldData (
				AreaDescriptionPtr				projectAreaDescriptionPtr,
				SInt16								fieldNumber,
				UInt32								pixelLine,
				UInt32								pixelColumn);

Boolean ListFieldsTitle (
				CMFileStream*						outputFilePtr,
				SInt16*								outputCodePtr,
				SInt16								stringNumber,
				SInt16								classNumber,
				SInt16								fieldTypeCode);

void LoadSymbolList (
				DialogPtr							dialogPtr,
				ListHandle							dialogListHandle);

Boolean ModalSymbolsDialog (
				DialogPtr							dialogPtr,
				UInt16								numberOfClassesToUse);

#if defined multispec_mac
	pascal Boolean SymbolsDialogFilter (
					DialogPtr							theDialog,
					EventRecord*						theEvent,
					SInt16*								itemHitPtr);
#endif	// defined multispec_mac

Boolean VerifyAreaDescription (
				AreaDescriptionPtr				areaDescriptionPtr);



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void Area_Of_SND_by_Direct_Calculation
//
//	Software purpose:	The purpose of this routine is to compute the 
//							area under the standard normal distribution
//							from x=0 to x=r.
//									
//							Area (z) = 1/K Integral[exp (-t*t/2)] for t=0 to t=z.
//										where K = sqrt (2PI).
//
//							Note:  the error function is defined as:
//										erf (z) = 2/sqrt (kPI) Integral[exp (-t*t) for t=0 to z.
//									
//										Area (z) = erf (z/sqrt (2))/2 and
//										erf (z) = 2 * Area (sqrt (2) * z)
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:		
//
//	Coded By:			Chulhee Lee				Date: ?
//	Revised By:			Larry L. Biehl			Date: 03/29/1994	

void Area_Of_SND_by_Direct_Calculation (
				double								r,
				double*								ret)

 {
   double								incre = 0.02,
											localR,
											offset,
											rStop,
											start,
											t,
											x;


   if (r >= 8.327) 
		{
      *ret = .5;
      return;

		}	// end "if (r >= 8.327)" 

   if (r >= 3.5)
		{
      if (r >= 5.5)
			{
         incre = 0.04;

         if (r >= (start = 8.00))
            offset = .4999999999999994;

         else if (r >= (start = 7.50))
            offset = .4999999999999681;

         else if (r >= (start = 7.00))
            offset = .4999999999987202;

         else if (r >= (start = 6.50))
            offset = .4999999999598400;

         else if (r >= (start = 6.00))
            offset = .4999999990134124;

         else if (r >= (start = 5.50))
            offset = .4999999810104375;

			}	// end "if (r >= 5.5)"

      else	// r < 5.5 
			{
         if (r >= (start = 5.00))
            offset = .4999997133484281;

         else if (r >= (start = 4.75))
            offset = .4999989829167574;

         else if (r >= (start = 4.50))
            offset = .4999966023268753;

         else if (r >= (start = 4.25))
            offset = .4999893114742251;

         else if (r >= (start = 4.00))
            offset = .4999683287581669;

         else if (r >= (start = 3.75))
            offset = .4999115827147992;

         else if (r >= (start = 3.5))
            offset = .4997673709209645;

			}	// end "else r < 5.5"

		}	// end "if (r >= 3.5)"

   else	// r < 3.5 
		{
      if (r >= 1.75)
			{
         if (r >= (start = 3.25))
            offset = .4994229749576092;

         else if (r >= (start = 3))
            offset = .4986501019683699;

         else if (r >= (start = 2.75))
            offset = .4970202367649454;

         else if (r >= (start = 2.5))
            offset = .4937903346742238;

         else if (r >= (start = 2.25))
            offset = .4877755273449553;

         else if (r >= (start = 2))
            offset = .4772498680518208;

         else if (r >= (start = 1.75))
            offset = .4599408431361829;

			}	// end "if (r >= 1.75)"

      else	// r < 1.75 
			{
         if (r >= (start = 1.5))
            offset = .4331927987311419;

         else if (r >= (start = 1.25))
            offset = .3943502263331447;

         else if (r >= (start = 1))
            offset = .3413447460685429;

         else if (r >= (start = 0.75))
            offset = .2733726476231318;

         else if (r >= (start = 0.5))
            offset = .1914624612740131;

         else if (r >= (start = 0.25))
            offset = .09870632568292374;

         else if (r >= (start = 0))
            offset = .0;

			}	// end "else r < 1.75"

		}	// end "else r < 3.5"

   t = 0;
   localR = start - .5 * incre;
   rStop = r - incre;
   for (x = start; x < rStop; x += incre)
		{
      localR += incre;
      t += exp (-localR * localR / 2);

		}	// end "for (x=start; x<rStop; x+=incre)"

   t *= incre;

			// Get last piece of numerical integration.

   localR += 0.5 * (r + incre - x);
   t += exp (-localR * localR / 2) * (r - x);

   *ret = offset + t / kSQRT2PI;

}	// end "Area_Of_SND_by_Direct_Calculation"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean AssignClassInfoMemory
//
//	Software purpose:	The purpose of this routine is to assign memory for the
//							requested parameters.
//							Codes are:
//									0 = no memory to be assigned.
//									1 = square matrix of numberFeatures size
//									2 = triangular matrix of numberFeatures size
//									3 = 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			C.H. Lee					Date: 07/02/1996
//	Revised By:			Larry L. Biehl			Date: 08/23/2010	

Boolean AssignClassInfoMemory (
				ClassInfoPtr						classInfoPtr,
				SInt16*								classPtr,
				SInt32								numberClasses,
				SInt32								numberChannels,
				SInt32								numberFeatures,
				SInt16								floatDataValueCode,
				SInt16								covarianceCode,
				SInt16								inverseCode,
				SInt16								meanCode,
				SInt16								transformedCovCode,
				SInt16								transformedMeanCode)

{
	SInt64								numberPixels;

   SInt32								matrixSize,
											numberCovarianceEntries,
											numberFloatDataValueEntries,
											numberInverseEntries,
											numberMeanEntries,
											numberTransformedCovEntries,
											numberTransformedMeanEntries;

   UInt32								tNumberEntries;

   SInt16								statClassNumber;

   Boolean								continueFlag = TRUE;


			// Get memory for class covariances. Allow for needing to
			// read in the entire class covariance (all numberChannels)
			// and then tranforming the covariance to 'numberFeatures'. 

   if (continueFlag && covarianceCode > 0)
		{
      matrixSize = numberFeatures;
      if (covarianceCode & kNumberChannels)
         matrixSize = numberChannels;

      if (covarianceCode & kSquare)
         numberCovarianceEntries = matrixSize * matrixSize;
      else	// !(covarianceCode & kSquare)
         numberCovarianceEntries = matrixSize * (matrixSize + 1) / 2;

      tNumberEntries = numberCovarianceEntries * numberClasses;

      if (covarianceCode & kAllowLoadingSquare)
         tNumberEntries += (numberChannels * numberChannels - numberCovarianceEntries);

      classInfoPtr[0].covariancePtr = (HDoublePtr)MNewPointer (
																	tNumberEntries * sizeof (double));
      continueFlag = (classInfoPtr[0].covariancePtr != NULL);

		}	// end "if (continueFlag && ...)"

			// Get memory for classInfo structure parameters

   if (continueFlag && inverseCode > 0)
		{
      matrixSize = numberFeatures;
      if (inverseCode & kNumberChannels)
         matrixSize = numberChannels;

      if (inverseCode & kSquare)
         numberInverseEntries = matrixSize * matrixSize;
      else	// !(inverseCode & kSquare)
         numberInverseEntries = matrixSize * (matrixSize + 1) / 2;

      tNumberEntries = numberInverseEntries * numberClasses;

      if (inverseCode & kAllowLoadingSquare)
         tNumberEntries += (numberChannels * numberChannels - numberInverseEntries);

      classInfoPtr[0].inversePtr = (HDoublePtr)MNewPointer (
																	tNumberEntries * sizeof (double));
      continueFlag = (classInfoPtr[0].inversePtr != NULL);

		}	// end "if (continueFlag && ...)"

   if (continueFlag && meanCode > 0)
		{
      numberMeanEntries = numberFeatures;
      if (meanCode & kNumberChannels)
         numberMeanEntries = numberChannels;

      classInfoPtr[0].meanPtr = (HDoublePtr)MNewPointer (
								(UInt32)numberClasses * numberMeanEntries * sizeof (double));
      continueFlag = (classInfoPtr[0].meanPtr != NULL);

		}	// end "if (continueFlag && ...)"

   if (continueFlag && transformedCovCode > 0)
		{
      matrixSize = numberFeatures;
      if (transformedCovCode & kNumberChannels)
         matrixSize = numberChannels;

      if (transformedCovCode & kSquare)
         numberTransformedCovEntries = matrixSize * matrixSize;
      else	// !(transformedCovCode & kSquare)
         numberTransformedCovEntries = matrixSize * (matrixSize + 1) / 2;

      tNumberEntries = numberTransformedCovEntries * numberClasses;

      if (transformedCovCode & kAllowLoadingSquare)
         tNumberEntries +=
							(numberChannels * numberChannels - numberTransformedCovEntries);

      classInfoPtr[0].transformedCovPtr = (HDoublePtr)MNewPointer (
																	tNumberEntries * sizeof (double));
      continueFlag = (classInfoPtr[0].transformedCovPtr != NULL);

		}	// end "if (continueFlag && ...)"

			// Get memory for transformed class means and covariances.

   if (continueFlag && transformedMeanCode > 0)
		{
      numberTransformedMeanEntries = numberFeatures;
      if (transformedMeanCode & kNumberChannels)
         numberTransformedMeanEntries = numberChannels;

      classInfoPtr[0].transformedMeanPtr = (HDoublePtr)MNewPointer (
				(UInt32)numberClasses * numberTransformedMeanEntries * sizeof (double));
      continueFlag = (classInfoPtr[0].transformedMeanPtr != NULL);

		}	// end "if (continueFlag && ...)"

			// Now assign memory pointers for rest of classes.

   if (continueFlag)
		{
      SInt16				classStorage,
								index;


      numberFloatDataValueEntries = numberFeatures;
      if (floatDataValueCode & kNumberChannels)
         numberFloatDataValueEntries = numberChannels;

      for (index=0; index<numberClasses; index++)
			{
         if (floatDataValueCode > 0)
				{
						// Get memory for training pixels if needed.

            if (classPtr == NULL)
               statClassNumber = index;

            else	//	classPtr != NULL	
               statClassNumber = classPtr[index] - 1;

						// Get the class storage number.

            classStorage = gProjectInfoPtr->storageClass[statClassNumber];

            numberPixels =
						gProjectInfoPtr->classNamesPtr[classStorage].numberStatisticsPixels;

            if (numberPixels > 0)
					{
               classInfoPtr[index].dataValuesPtr = (HDoublePtr) MNewPointer (
								numberPixels * numberFloatDataValueEntries * sizeof (double));

               continueFlag = (classInfoPtr[index].dataValuesPtr != NULL);

					}	// end "if (numberPixels > 0)"

				}	// end "if (floatDataValueCode > 0)"

         if (!continueFlag)
            break;

         if (covarianceCode > 0)
            classInfoPtr[index].covariancePtr =
						&classInfoPtr[0].covariancePtr[index * numberCovarianceEntries];

         if (inverseCode > 0)
            classInfoPtr[index].inversePtr =
						&classInfoPtr[0].inversePtr[index * numberInverseEntries];

         if (meanCode > 0)
            classInfoPtr[index].meanPtr =
						&classInfoPtr[0].meanPtr[index * numberMeanEntries];

         if (transformedCovCode > 0)
            classInfoPtr[index].transformedCovPtr =
									&classInfoPtr[0].transformedCovPtr[
															index * numberTransformedCovEntries];

         if (transformedMeanCode > 0)
            classInfoPtr[index].transformedMeanPtr =
									&classInfoPtr[0].transformedMeanPtr[
														index * numberTransformedMeanEntries];

			}	// end "for (index=0; index<..."

		}	// end "if (continueFlag)"

   return (continueFlag);

}	// end "AssignClassInfoMemory"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double Bhattacharyya
//
//	Software purpose:	The purpose of this routine is to calculate
//							the Bhattacharyya distance between the two classes 
//							represented by their input covariance matrices, 
//							inverse covariance matrices and mean vectors.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			C.H. Lee					Date: 12/04/1988
//	Revised By:			Larry L. Biehl			Date: 06/30/1994	

double Bhattacharyya (
				HDoublePtr							mean1Ptr, // mean vector of class 1
				HDoublePtr							mean2Ptr, // mean vector of class 2
				HDoublePtr							cov1Ptr, // covariance matrix of class 1
				HDoublePtr							cov2Ptr, // covariance matrix of class 2
				double								logDeterminant1, // log of determinant of
																				//	class 1 covariance matrix
				double								logDeterminant2, // log of determinant of
																				//	class 2 covariance matrix
				HDoublePtr							meanDifPtr, // vector storage of class 1 and
																				// class 2 mean difference.
				SInt16								numberFeatures) // number of features (channels)
																				// represented in the mean vector
																				// and covariance and inverse  	
																				// covariance matrices.																						
{
	double								bhattacharyya1,
											bhattacharyya2,
											//determinant,
											logDeterminant,
											meanDifValue;

   HDoublePtr							temp1Ptr,
											temp2Ptr,
											tInversePtr;

   UInt32								channel1,
											channel2,
											tmpIndex1,
											tmpIndex2;


			// Get 1/2 of the sum of the two covariances.
			// The 1/2 will be taken care of in the mean part by only dividing
			// by 4 instead of 8 and in the covariance part by subtracting the
			// constant 'numberFeature * ln (2)'.						

   tInversePtr = gInverseMatrixMemory.inversePtr;
   temp1Ptr = cov1Ptr;
   temp2Ptr = cov2Ptr;

   for (channel1=0; channel1<(UInt32)numberFeatures; channel1++)
		{
      tmpIndex1 = (UInt32)channel1*numberFeatures;
      tmpIndex2 = channel1;

      for (channel2 = 0; channel2 < channel1; channel2++)
			{
         tInversePtr[tmpIndex1] = tInversePtr[tmpIndex2] = (*temp1Ptr + *temp2Ptr);

         temp1Ptr++;
         temp2Ptr++;

         tmpIndex1++;
         tmpIndex2 += numberFeatures;

			}	// end "for (channel2=1; channel2<channel1; channel2++)"

      tInversePtr[tmpIndex1] = (*temp1Ptr + *temp2Ptr);

      temp1Ptr++;
      temp2Ptr++;

		}	// end "for (channel1=1; channel1<=numOutFeatures; channel1++)"

   InvertSymmetricMatrix (tInversePtr,
									numberFeatures,
									gInverseMatrixMemory.pivotPtr,
									gInverseMatrixMemory.indexRowPtr,
									gInverseMatrixMemory.indexColPtr,
									gInverseMatrixMemory.ipvotPtr,
									NULL,
									&logDeterminant,
									kReturnMatrixInverse);

   if (gOperationCanceledFlag)
																								return (0);

			// Get the mean difference vector.
			// Don't get confused with tInversePtr being used here.  This is		
			// temporary to take advantage of the register variable pointer.		

   tInversePtr = meanDifPtr;
   temp1Ptr = mean1Ptr;
   temp2Ptr = mean2Ptr;

   for (channel1=0; channel1<(UInt32)numberFeatures; channel1++)
		{
      *tInversePtr = *temp1Ptr - *temp2Ptr;
      tInversePtr++;
      temp1Ptr++;
      temp2Ptr++;

		}	// end "for (channel1=0; channel1<numberFeatures; channel1++)"

			// Get the i=j component items of the mean part of the Bhattacharyya
			// distance.

   tInversePtr = gInverseMatrixMemory.inversePtr;
   temp1Ptr = meanDifPtr;
   bhattacharyya1 = 0;
   tmpIndex2 = numberFeatures + 1;
   for (channel1 = 0; channel1 < (UInt32)numberFeatures; channel1++)
		{
      meanDifValue = *temp1Ptr;
      bhattacharyya1 += *tInversePtr * meanDifValue * meanDifValue;
      tInversePtr += tmpIndex2;
      temp1Ptr++;

		}	// end "for (channel1=0; channel1<numberFeatures; channel1++)"

   bhattacharyya1 *= .25;

			// Get the i!=j component items of the mean part of the
			// Bhattacharyya distance.		
			// Note that the i!=j component is double the value computed for
			// bhattacharyya2. This is taken care of by dividing by 2 instead of 4 at
			// the end of the computation.

   tInversePtr = &gInverseMatrixMemory.inversePtr[numberFeatures];
   temp1Ptr = meanDifPtr + 1;
   bhattacharyya2 = 0;
   tmpIndex2 = numberFeatures - 1;
   for (channel1=1; channel1<(UInt32)numberFeatures; channel1++)
		{
      meanDifValue = *temp1Ptr;
      temp2Ptr = meanDifPtr;
      for (channel2 = 0; channel2 < channel1; channel2++)
			{
         bhattacharyya2 += *tInversePtr * meanDifValue * *temp2Ptr;
         tInversePtr++;
         temp2Ptr++;

			}	// end "for (channel2=0; channel2<numberFeatures; channel2++)"

      temp1Ptr++;
      tInversePtr += tmpIndex2;
      tmpIndex2--;

		}	// end "for (channel1=1; channel1<numberFeatures; channel1++)"

   bhattacharyya1 += .5 * (bhattacharyya2 + logDeterminant -
						numberFeatures * kLN2 - .5 * (logDeterminant1 + logDeterminant2));

   return (bhattacharyya1);

}	// end "Bhattacharyya"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ChangeProjectAssociatedImageItem
//
//	Software purpose:	The purpose of this routine is to change the window
//							file information items due to the selection of the
//							Associated Image menu item under the Project menu.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 05/11/1990
//	Revised By:			Larry L. Biehl			Date: 10/09/2015	

void ChangeProjectAssociatedImageItem (
				Handle								windowInfoHandle)

{
	WindowInfoPtr						windowInfoPtr;


   if (windowInfoHandle != NULL)
		{
      windowInfoPtr = (WindowInfoPtr)GetHandlePointer (windowInfoHandle,
																			kLock);

      if (!windowInfoPtr->projectBaseImageFlag)
			{
         gUpdateProjectMenuItemsFlag = TRUE;

         if (windowInfoPtr->projectWindowFlag)
				{
            windowInfoPtr->projectWindowFlag = FALSE;

						// Force any field boundaries to be removed.

            if (gProjectInfoPtr->outlineFieldType != 0)
					{
					#if defined multispec_mac  || defined multispec_wx
						InvalidateWindow (gActiveImageWindow, kImageFrameArea, FALSE);
					#endif	// defined multispec_mac || defined multispec_wx

					#if defined multispec_win
						gActiveImageViewCPtr->Invalidate ();
					#endif	// defined multispec_win

					}	// end "if (gProjectInfoPtr->outlineFieldType != 0)"

				}	// end "if (windowInfoPtr->projectWindowFlag)"

         else	// !windowInfoPtr->projectWindowFlag 
				{
            windowInfoPtr->projectWindowFlag = TRUE;

						// Force the fields to be outlined in the image window.

				#if defined multispec_mac
					OutlineFieldsControl (gProjectInfoPtr->statsWindowMode,
													gActiveImageWindow,
													windowInfoHandle,
													1);
				#endif	// defined multispec_mac

				#if defined multispec_win
					gActiveImageViewCPtr->Invalidate ();
				#endif	// defined multispec_win
				
				#if defined multispec_wx
					InvalidateWindow (gActiveImageViewCPtr, kImageFrameArea, FALSE);
				#endif	// defined multispec_wx

				}	// else !windowInfoPtr->projectWindowFlag

			}	// end "if (!windowInfoPtr->projectBaseImageFlag)"

      CheckAndUnlockHandle (windowInfoHandle);

		}	// end "if (windowInfoH != NULL)"

}	// end "ChangeProjectAssociatedImageItem"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckClassEnhancedStats
//
//	Software purpose:	The purpose of this routine is to check that the enhanced
//							statistics exist for each of the requested classes. If
//							not a message is listed in the output window indicating
//							to the user that they do not.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/17/1997
//	Revised By:			Larry L. Biehl			Date: 04/17/1997	

Boolean CheckClassEnhancedStats (
				UInt32								numberClasses,
				SInt16*								classPtr)

{
	HPClassNamesPtr					classNamesPtr;

   SInt16								classNumber,
											classStorage;

   UInt16								index;

   Boolean								continueFlag = TRUE;


   if (classPtr != NULL)
		{
				// Initialize local variables.

      classNamesPtr = gProjectInfoPtr->classNamesPtr;

      for (index=0; index<numberClasses; index++)
			{
         classNumber = classPtr[index] - 1;

					// Get the class storage number.

         classStorage = gProjectInfoPtr->storageClass[classNumber];

         if (!gProjectInfoPtr->classNamesPtr[classStorage].modifiedStatsFlag)
            continueFlag = ListSpecifiedStringNumber (
											kProjectStrID,
											IDS_Project45,
											NULL,
											gOutputForce1Code,
											gProjectInfoPtr->classNamesPtr[classStorage].name,
											continueFlag);

			}	// end "for (index=0;..."

		}	// end "if (classPtr != NULL)"

   return (continueFlag);

}	// end "CheckClassEnhancedStats"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckClassStats
//
//	Software purpose:	The purpose of this routine is to check the class statistics and
//							make certain that only those classes with valid statistics are
//							included in the class useage vector.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			VerifyProjectStatsUpToDate in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/15/1989
//	Revised By:			Larry L. Biehl			Date: 05/03/2019

Boolean CheckClassStats (
				UInt32*								numberClassesPtr,
				SInt16*								classPtr,
				SInt16								statisticsType,
				SInt16								covarianceStatsToUse,
				Boolean								checkOnlyFlag,
				SInt32*								minimumNumberTrainPixelsPtr,
				Boolean*								computeCommonCovarianceFlagPtr)

{
	HPClassNamesPtr					classNamesPtr;

   SInt16								classNumber,
											classStorage,
											lCovarianceStatsToUse;

   UInt16								index,
											newIndex;

   Boolean								checkFlag = FALSE,
											computeCommonCovarianceFlag,
											continueFlag,
											okayFlag;


   *computeCommonCovarianceFlagPtr = FALSE;

			// Initialize local variables.												

   classNamesPtr = gProjectInfoPtr->classNamesPtr;
   lCovarianceStatsToUse = covarianceStatsToUse;

   if (minimumNumberTrainPixelsPtr != NULL)
      *minimumNumberTrainPixelsPtr = INT_MAX;

			// If the input covariance to use parameter is set to 'mixed stats' then
			// change it to 'no statistics used' so that the class covariance to
			// use parameter will be used instead.

   if (covarianceStatsToUse == kMixedStats)
      covarianceStatsToUse = kNoStatisticsUsed;

   if (*numberClassesPtr > 0)
      checkFlag = TRUE;

   newIndex = 0;
   continueFlag = TRUE;
   for (index=0; index<*numberClassesPtr; index++)
		{
      if (classPtr != NULL)
			{
         classNumber = classPtr[index] - 1;
         classStorage = gProjectInfoPtr->storageClass[classNumber];

			}	// end "if (classPtr != NULL)"

      else	// classPtr == NULL
			{
         continueFlag = FALSE;
         classStorage = gProjectInfoPtr->storageClass[index];
         if (classNamesPtr[classStorage].numberOfTrainFields > 0 ||
								classNamesPtr[classStorage].numberStatisticsPixels > 0)
            continueFlag = TRUE;

			}	// end "else classPtr == NULL"

      if (continueFlag)
			{
         if (covarianceStatsToUse == kNoStatisticsUsed)
            lCovarianceStatsToUse = classNamesPtr[classStorage].covarianceStatsToUse;

         okayFlag = DetermineIfSpecifiedStatisticsExist (
																	&classNamesPtr[classStorage],
																	statisticsType,
																	lCovarianceStatsToUse,
																	&computeCommonCovarianceFlag);

         if (okayFlag)
				{
            if (checkOnlyFlag)
					{
               if (!gProjectInfoPtr->classNamesPtr[classStorage].statsUpToDate)
                  checkFlag = FALSE;

					}	// end "if (checkOnlyFlag)"

            else if (!computeCommonCovarianceFlag) // && !checkOnlyFlag
					{
							// Update the class vector if given as input.

               if (classPtr != NULL)
                  classPtr[newIndex] = classNumber + 1;

               newIndex++;

               if (minimumNumberTrainPixelsPtr != NULL)
                  *minimumNumberTrainPixelsPtr = MIN (
								*minimumNumberTrainPixelsPtr,
								(SInt32)classNamesPtr[classStorage].numberStatisticsPixels);

					}	// end "else !checkOnlyFlag && !computeCommonCovarianceFlag"

				}	// end "if (okayFlag)"

         else	// !okayFlag
            checkFlag = FALSE;

         if (computeCommonCovarianceFlag)
            *computeCommonCovarianceFlagPtr = TRUE;

			}	// end "if (continueFlag)"

		}	// end "for (index=0; ..."

   if (!checkOnlyFlag)
      *numberClassesPtr = newIndex;

   if (minimumNumberTrainPixelsPtr != NULL && *minimumNumberTrainPixelsPtr == INT_MAX)
      *minimumNumberTrainPixelsPtr = 0;

   return (checkFlag);

}	// end "CheckClassStats"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean CheckNumberOfPixelsInClass
//
//	Software purpose:	The purpose of this routine is to verify that the 
//							number of pixels in the class is at one more than 
//							the number of channels to be used.  If not then list 
//							those classes that do not meet the criterion.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/25/1993
//	Revised By:			Larry L. Biehl			Date: 02/28/2013

Boolean CheckNumberOfPixelsInClass (
				UInt32								numberClasses,
				SInt16*								classPtr,
				SInt16								numberFeatures)

{
	CMFileStream*						resultsFileStreamPtr;

   HPClassNamesPtr					classNamesPtr;

   UInt32								index;

   SInt16								classNumber,
											classStorage,
											strLength;

   Boolean								continueFlag,
											returnFlag;


   returnFlag = FALSE;

   if (classPtr != NULL)
		{
				// Initialize local variables.

      classNamesPtr = gProjectInfoPtr->classNamesPtr;
      returnFlag = TRUE;

				// Now verify that the number of pixels in the class is at one more than
				// the number of channels to be used.  If not then list those classes that
				// do not meet the criterion

      resultsFileStreamPtr = GetResultsFileStreamPtr (0);
      continueFlag = TRUE;

      for (index=0; index<numberClasses; index++)
			{
         classNumber = classPtr[index] - 1;

					// Get the class storage number.

         classStorage = gProjectInfoPtr->storageClass[classNumber];

         if (classNamesPtr[classStorage].numberStatisticsPixels < numberFeatures + 1 &&
							classNamesPtr[classStorage].covarianceStatsToUse == kOriginalStats)
				{
            pstr ((char*)gTextString2,
							(char*)&gProjectInfoPtr->classNamesPtr[classStorage].name,
							&strLength);

            if (returnFlag)
					{
               sprintf ((char*)gTextString,
									"%s    The number of pixels in the following classes needs "
									"to be at least%s     one more than the %hd features being "
									"used.%s      %s with %lld pixels%s",
									gEndOfLine,
									gEndOfLine,
									numberFeatures,
									gEndOfLine,
									gTextString2,
									classNamesPtr[classStorage].numberStatisticsPixels,
									gEndOfLine);

               SysBeep (10);

					}	// end "if (returnFlag)"

            else	// !returnFlag 
               sprintf ((char*)gTextString,
								"      %s with %lld pixels%s",
								gTextString2,
								classNamesPtr[classStorage].numberStatisticsPixels,
								gEndOfLine);

            continueFlag = OutputString (resultsFileStreamPtr,
														(char*)gTextString,
														0,
														gOutputForce1Code,
														continueFlag);

            returnFlag = FALSE;

				}	// end "if (classNamesPtr[classNumber]... && ..."

			}	// end "for (index=0;..."

      if (!returnFlag)
					// Insert a blank line after the list if there is one.
         continueFlag = OutputString (resultsFileStreamPtr,
													gEndOfLine,
													gNumberOfEndOfLineCharacters,
													gOutputForce1Code,
													continueFlag);

		}	// end "if (classPtr != NULL)"

   return (returnFlag);

}	// end "CheckNumberOfPixelsInClass"



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ClassDialog
//
//	Software purpose:	The purpose of this routine is to display a dialog
//							with a list of the available classes that the user
//							can select from. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/15/1989
//	Revised By:			Larry L. Biehl			Date: 06/12/1998	

SInt16 ClassDialog (
				UInt32*								numberOutputClassesPtr,
				SInt16*								classPtr,
				SInt16								minimumNumberClasses,
				SInt16								numberInputClasses,
				SInt16								currentSelection,
				Handle								parentOKHandle)

{
	DialogPtr							dialogPtr;

   SInt16								returnCode;

   Boolean								returnFlag;


			// Get the modal dialog for the reformat specification

   dialogPtr = LoadRequestedDialog (kChannelSpecificationID, kCopyScrap, 1, 2);
   if (dialogPtr != NULL)
		{
				// Unhilite the OK control in the parent dialog box if it exists.

      if (parentOKHandle != NULL)
         HiliteControl ((ControlHandle)parentOKHandle, 255);

				// If all classes is the current setting, then assume that
				// the subset is the single active project class.

      if (currentSelection == kAllMenuItem && gProjectInfoPtr->currentClass >= 0)
			{
         *numberOutputClassesPtr = 1;
         classPtr[0] = gProjectInfoPtr->currentClass + 1;

			}	// end "if (currentSelection = kAllMenuItem && ..."

				// Get a handle to a one column list.

      gDialogListHandle = GetOneColumnDialogListHandle (dialogPtr, 5);

				//	Load address of routine that will load the class list.

      SetDialogItemDrawRoutine (dialogPtr, 5, gCreateOneColumnList1Ptr);

				//	Item 6: Load title of class list dialog.
				//	Item 7: Load title for class list.											

      SetWTitle (GetDialogWindow (dialogPtr), "\pSelect Classes");
      LoadDItemString (dialogPtr, 7, (Str255*)"\pClass List");

				// Initialize the class range text edit items.

      LoadDItemValue (dialogPtr, 9, (SInt32)1);
      LoadDItemValue (dialogPtr, 11, (SInt32)numberInputClasses);
      LoadDItemValue (dialogPtr, 13, (SInt32)1);
      SetDLogControlHilite (dialogPtr, 14, 0);

				// Center the dialog and then show it.

      ShowDialogWindow (dialogPtr, kChannelSpecificationID, kDoNotSetUpDFilterTable);

				// Continue if there is a valid dialog list handle.

      returnFlag = FALSE;
      if (gDialogListHandle != NULL)
			{
					// Load the class names into the list.

         LSetDrawingMode (FALSE, gDialogListHandle);

         LoadClassNamesIntoList (gDialogListHandle);

         SInt32 numberSelectedItems = SetClassListSelections (
																		gDialogListHandle,
																		(SInt32)numberInputClasses,
																		(SInt32)*numberOutputClassesPtr,
																		(UInt16*)classPtr);

					// Set the number of selected items.

         LoadDItemValue (dialogPtr, 16, numberSelectedItems);

					// Turn list drawing mode back on.

         LSetDrawingMode (TRUE, gDialogListHandle);

         LUpdate (GetPortVisibleRegion (GetDialogPort (dialogPtr), gTempRegion1),
							gDialogListHandle);
         SetEmptyRgn (gTempRegion1);

					// Handle modal dialog selections for one column class list.

         returnFlag = ModalOneColListDialog (dialogPtr,
															numberInputClasses,
															numberOutputClassesPtr,
															classPtr,
															minimumNumberClasses);

         if (gDialogListHandle != NULL)
            LDispose (gDialogListHandle);
         gDialogListHandle = NULL;

			}	// end "if (gDialogListHandle != NULL)"

      CloseRequestedDialog (dialogPtr, kDoNotSetUpDFilterTable);

				// Hilite the OK control again in the parent dialog box if it exists.

      if (parentOKHandle != NULL)
         HiliteControl ((ControlHandle)parentOKHandle, 0);

      if (!returnFlag && currentSelection == kAllMenuItem)
         *numberOutputClassesPtr = numberInputClasses;

		}	// end "if (dialogPtr != NULL)"

   returnCode = kSubsetMenuItem;
   if (*numberOutputClassesPtr == numberInputClasses)
      returnCode = kAllMenuItem;

   return (returnCode);

}	// end "ClassDialog"
#endif	// defined multispec_mac  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ClassPairWeightsDialog
//
//	Software purpose:	The purpose of this routine is to display a dialog
//							to allow the user to change the weights to be used
//							for class pairs.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			SeparabilityDialog   in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/14/1990
//	Revised By:			Larry L. Biehl			Date: 12/28/1999	

SInt16 ClassPairWeightsDialog (
				UInt16								numberOfClassesToUse,
				SInt16**								weightsListPtrPtr,
				SInt16								interClassWeightsSelection,
				SInt16*								defaultClassPairWeightPtr)
 {
	#if defined multispec_mac
		DialogPtr							dialogPtr;

		Boolean								continueFlag;


				// Get the modal dialog for the reformat specification

		dialogPtr = LoadRequestedDialog (kClassPairWeightsDialogID, kCopyScrap, 1, 2);
		if (dialogPtr == NULL)
																							return (FALSE);

				// Get the list handle for the class-weight list.

		gDialogListHandle = GetOneColumnDialogListHandle (dialogPtr, 6);

				// Get the list handle for the weight list.

		if (gDialogListHandle != NULL)
			gDialogListHandle2 = GetOneColumnDialogListHandle (dialogPtr, 7);

				// Continue if there is a valid dialog list handle.

		if (gDialogListHandle != NULL && gDialogListHandle2 != NULL)
			{
					// Set some list parameters

			(*gDialogListHandle)->selFlags = lNoRect + lNoExtend + lUseSense;
			(*gDialogListHandle2)->selFlags = lOnlyOne;

					//	Load address of routine that will load the class-symbol list.

			SetDialogItemDrawRoutine (dialogPtr, 6, gCreateOneColumnList1Ptr);

					//	Load address of routine that will load the class pair-weight list.

			SetDialogItemDrawRoutine (dialogPtr, 7, gCreateOneColumnList2Ptr);

					// Initialize some dialog parameters.

			ClassPairWeightsDialogInitialize (dialogPtr,
															*defaultClassPairWeightPtr,
															defaultClassPairWeightPtr);

					//	Load 0 in for new weight.

			LoadDItemValue (dialogPtr, 9, 0);

					//	Load in the default weight.

			LoadDItemValue (dialogPtr, 14, (SInt32)*defaultClassPairWeightPtr);

					// Center the dialog and then show it.

			ShowDialogWindow (
						dialogPtr, kClassPairWeightsDialogID, kDoNotSetUpDFilterTable);

					// Set default text selection to first edit text item.

			SelectDialogItemText (dialogPtr, 9, 0, INT16_MAX);

					// Get deactivate event for the window that is now behind the
					// dialog and handle it.														

			CheckSomeEvents (activMask);

					// Load the class list.

			LoadClassList (
					dialogPtr, gDialogListHandle, numberOfClassesToUse, NULL, NULL, FALSE);

					// Load the class weight group list if any exist.

			LoadClassWeightGroups (dialogPtr, gDialogListHandle2, *weightsListPtrPtr);

					// Handle modal dialog selections for one column class list.

			continueFlag = FALSE;
			if (gMemoryError == noErr)
				continueFlag =
							ClassPairWeightsDialogModal (dialogPtr, numberOfClassesToUse);

			if (continueFlag)
				{
				ClassPairWeightsDialogOK (dialogPtr,
													gDialogListHandle2,
													weightsListPtrPtr,
													&interClassWeightsSelection,
													(SInt32)GetDItemValue (dialogPtr, 14),
													defaultClassPairWeightPtr);

				}	// end "if (continueFlag)"

					// Make certain that global memory error is set back to 'noErr'.

			gMemoryError = noErr;

			}	// end "if (gDialogListHandle != NULL && ...)"

		if (gDialogListHandle != NULL)
			LDispose (gDialogListHandle);
		gDialogListHandle = NULL;

		if (gDialogListHandle2 != NULL)
			LDispose (gDialogListHandle2);
		gDialogListHandle2 = NULL;

		CloseRequestedDialog (dialogPtr, kDoNotSetUpDFilterTable);
	#endif	// defined multispec_mac

	#if defined multispec_win
		CMClassPairWeightDlg* classPairWeightDialogPtr = NULL;

		TRY
			{
			classPairWeightDialogPtr = new CMClassPairWeightDlg ();

			interClassWeightsSelection = classPairWeightDialogPtr->DoDialog (
																		numberOfClassesToUse,
																		weightsListPtrPtr,
																		interClassWeightsSelection,
																		defaultClassPairWeightPtr);

			if (classPairWeightDialogPtr != NULL)
				delete classPairWeightDialogPtr;

			}

		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		END_CATCH_ALL
	#endif	// defined multispec_win

	#if defined multispec_wx
		CMClassPairWeightDlg* classPairWeightDialogPtr = NULL;

		try
			{
			classPairWeightDialogPtr = new CMClassPairWeightDlg ();

			interClassWeightsSelection = classPairWeightDialogPtr->DoDialog (
																		numberOfClassesToUse,
																		weightsListPtrPtr,
																		interClassWeightsSelection,
																		defaultClassPairWeightPtr);

			if (classPairWeightDialogPtr != NULL)
				delete classPairWeightDialogPtr;

			}

		catch (int e)
			{
			MemoryMessage (0, kObjectMessage);
			}
	#endif	// defined multispec_wx
   
   return (interClassWeightsSelection);

}	// end "ClassPairWeightsDialog"		



//------------------------------------------------------------------------------------
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ClassPairWeightsDialogChangeWeight
//
//	Software purpose:	The purpose of this routine is to handle the selection of the
//							"Add" new class pair weight button.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			ClassWeightsDialog in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1999
//	Revised By:			Larry L. Biehl			Date: 12/28/1999
//	Revised By:			Wei-Kang Hsu			Date: 12/21/2016
void ClassPairWeightsDialogChangeWeight (
				DialogPtr							dialogPtr,
				#if defined multispec_wx
					wxListBox*							classListHandle,
					wxListBox*							weightListHandle,
				#else
					ListHandle							classListHandle,
					ListHandle							weightListHandle,
				#endif
				SInt16								newWeight)

{
	Cell									cell;

   SInt16								stringLength;


   sprintf ((char*)gTextString, "%3d-", newWeight);
   stringLength = 4;

   cell.h = 0;
   cell.v = 0;
   while (LGetSelect (TRUE, &cell, classListHandle))
		{
				// After incrementing, cell.v will be the class
				// number and cell.v will be ready for getting the	
				// next hilited list line.									

      cell.v++;

				// Convert the class number to a string.

      NumToString ((UInt32)cell.v, gTextString2);
      stringLength += gTextString2[0] + 1;
      if (stringLength > 255)
         break;

				// Add class number to the list.

      strncat ((char*)gTextString,
						(char*)&gTextString2[1],
						gTextString2[0]);

				// Append a blank character to the list.

      strncat ((char*)gTextString, " ", 1);
      
		}	// end "while (LGetSelect (TRUE, ...))"

   stringLength--;
	#if defined multispec_mac
		TextFont	(gWindowTextFont); // monaco
	#endif	// defined multispec_mac

	#if defined multispec_wx
		SInt16 numberrow =  (SInt16)weightListHandle->GetCount (); //modified Oct 12 2015
		cell.v = LAddRow (1, numberrow, weightListHandle);
	#else
		cell.v = LAddRow (1, SHRT_MAX, weightListHandle);
	#endif
   LSetCell ((char*)gTextString, stringLength, cell, weightListHandle);

			// Unhilite this button until another set of classes has been selected.

   SetDLogControlHilite (dialogPtr, IDC_AddButton, 255);

}	// end "ClassPairWeightsDialogChangeWeight"



//------------------------------------------------------------------------------------
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ClassPairWeightsDialogClassSelectionChange
//
//	Software purpose:	The purpose of this routine is to handle a change in the 
//							class list.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			ClassWeightsDialog in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1999
//	Revised By:			Larry L. Biehl			Date: 12/28/1999

SInt16 ClassPairWeightsDialogClassSelectionChange (
				DialogPtr							dialogPtr,
				#if defined multispec_wx
					wxListBox*							listHandle,
				#else
					ListHandle							listHandle,
				#endif
				SInt16								newWeight)

{
	Cell									cell;

   SInt16								item,
											minimumItemsRequired = 2,
											selectedCell;

   Boolean								hiliteFlag;


   cell.h = 0;
   cell.v = 0;

   hiliteFlag = TRUE;
   for (item=1; item<=minimumItemsRequired; item++)
		{
      if (!LGetSelect (TRUE, &cell, listHandle))
         hiliteFlag = FALSE;

      else	// LGetSelect (TRUE, &cell, ... 
			{
         if (item == 1)
            selectedCell = cell.v;

         cell.v++;

			}	// end "else LGetSelect (TRUE, &cell, ..."

		}	// end "for (item=1; ..."

   if (newWeight < 0)
      hiliteFlag = FALSE;

   if (hiliteFlag)
      SetDLogControlHilite (dialogPtr, IDC_AddButton, 0);

   else	// !hiliteFlag 
		{
      SetDLogControlHilite (dialogPtr, IDC_AddButton, 255);
      selectedCell = -1;

		}	// end "else !hiliteFlag"

   return (selectedCell);

}	// end "ClassPairWeightsDialogClassSelectionChange"



//------------------------------------------------------------------------------------
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ClassPairWeightsDialogInitialize
//
//	Software purpose:	The purpose of this routine is to initialize the class weights
//							dialog box and parameters.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			ClassWeightsDialog in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/20/1999
//	Revised By:			Larry L. Biehl			Date: 12/28/1999

void ClassPairWeightsDialogInitialize (
				DialogPtr							dialogPtr,
				SInt16								defaultClassPairWeight,
				SInt16*								localDefaultClassPairWeightPtr)
 {
			//	Unhilite the 'Add' button.

   SetDLogControlHilite (dialogPtr, IDC_AddButton, 255);

			//	Unhilite the 'Remove' button.

   SetDLogControlHilite (dialogPtr, IDC_RemoveButton, 255);

   *localDefaultClassPairWeightPtr = defaultClassPairWeight;

}	// end "ClassPairWeightsDialogInitialize"



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ClassPairWeightsDialogModal
//
//	Software purpose:	The purpose of this routine is to handle the user
//							selections in the class pair weights dialog that is used
//							to allow the user to select weights for each class pair. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	 Value Returned:  None
//
// Called By:			ClassPairWeightsDialog   in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/14/1990
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

Boolean ClassPairWeightsDialogModal (
				DialogPtr							dialogPtr,
				UInt16								numberOfClassesToUse)

{
	Rect									theBox;

   Cell									cell;

   Handle								okHandle,
											theHandle;

   SInt32								newWeight,
											saveDefaultWeight,
											saveNewWeight;

   SInt16								//defaultEqualWeight = 10,
											itemHit = 0,
											//minimumItemsRequired = 2,
											returnCode,
											selectedCell,
											selectedWeightGroupCell,
											theType;

   Boolean								modalDone;


			// Continue if dialogPtr and gDialogListHandle are not NULL.

   if (dialogPtr != NULL &&
			gDialogListHandle != NULL &&
				gDialogListHandle2 != NULL)
		{
				// Get the "handle" for the OK button for use later.

      GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);

      cell.h = 0;
      modalDone = FALSE;
      saveNewWeight = 0;
      selectedCell = -1;
      itemHit = 0;
      do {
         //ModalDialog (gTwoListDialogFilterPtr, &itemHit);
         ModalDialog (gProcessorDialogFilterPtr, &itemHit);
         if (itemHit > 2)
				{
            switch (itemHit)
					{
               case 6:
                  selectedCell = ClassPairWeightsDialogClassSelectionChange (
																						dialogPtr,
																						gDialogListHandle,
																						saveNewWeight);
							break;

               case 7:
                  selectedWeightGroupCell =
											ClassPairWeightsDialogWeightSelectionChange (
																						dialogPtr,
																						gDialogListHandle2);
                  break;

               case 9: // New weight value.										
                  GetDialogItem (dialogPtr, 9, &theType, &theHandle, &theBox);
                  GetDialogItemText (theHandle, gTextString);
                  if (gTextString[0] > 0)
							{
                     newWeight = GetDItemValue (dialogPtr, 9);
                     if (newWeight > 99 || newWeight < 0)
                        NumberErrorAlert (saveNewWeight, dialogPtr, 9);

                     else	// newWeight >= 0 && newWeight <= 99 
								{
                        saveNewWeight = newWeight;
                        if (selectedCell >= 0)
                           SetDLogControlHilite (dialogPtr, 10, 0);

								}	// end "else newWeight >= 0 && ..."

							}	// end "if (gTextString[0] < 0)"

                  else	// gTextString[0] <= 0 
							{
                     saveNewWeight = -1;
                     SetDLogControlHilite (dialogPtr, 10, 255);

							}	// end "else gTextString[0] <= 0"
                  break;

               case 10: // Add selected class pair weight set.
                  ClassPairWeightsDialogChangeWeight (dialogPtr,
																		gDialogListHandle,
																		gDialogListHandle2,
																		saveNewWeight);
                  break;

               case 11: // Remove selected class pair weight sets.
                  TextFont (gWindowTextFont); // monaco  

                  selectedWeightGroupCell =
										ClassPairWeightsDialogRemoveWeightSelection	(
																			dialogPtr,
																			gDialogListHandle2,
																			selectedWeightGroupCell);
                  break;

               case 12: // Help
                  HiliteControl ((ControlHandle)okHandle, 255);
                  returnCode = DisplayAlert (1152, 0, kAlertStrID, 9, 0, NULL);

                  TextFont (gWindowTextFont); // monaco
                  LUpdate (GetPortVisibleRegion (GetDialogPort (dialogPtr),
																	gTempRegion1),
																	gDialogListHandle);
                  SetEmptyRgn (gTempRegion1);
                  LUpdate (GetPortVisibleRegion (GetDialogPort (dialogPtr),
																	gTempRegion1),
																	gDialogListHandle2);
                  SetEmptyRgn (gTempRegion1);
                  HiliteControl ((ControlHandle)okHandle, 0);
                  break;

               case 14: // Enter edit weight value.								
                  GetDialogItem (dialogPtr, 14, &theType, &theHandle, &theBox);
                  GetDialogItemText (theHandle, gTextString);
                  if (gTextString[0] > 0)
							{
                     newWeight = GetDItemValue (dialogPtr, 14);
                     if (newWeight > 99 || newWeight < 0)
                        NumberErrorAlert (saveDefaultWeight, dialogPtr, 14);

                     else	// newWeight >= 0 && newWeight <= 99
								{
                        saveDefaultWeight = newWeight;

								}	// end "else newWeight >= 0 && ..."

							}	// end "if (gTextString[0] < 0)"

                  else	// gTextString[0] <= 0 
							{
                     saveDefaultWeight = -1;

							}	// end "else gTextString[0] <= 0"
                  break;

					}	// end "switch (itemHit)"

				}	// end "if (itemHit > 2)"

         else if (itemHit > 0) // and itemHit <= 2
						// itemHit == 1 is OK, itemHit == 2 is Cancel.
            modalDone = TRUE;

			} while (!modalDone);

		}	// end "if (dialogPtr != NULL && ...)"

   return (itemHit == 1);

}	// end "ClassPairWeightsDialogModal"
#endif	// defined multispec_mac



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ClassPairWeightsDialogOK
//
//	Software purpose:	The purpose of this routine is to handle the user
//							selections in the class pair weights dialog that is used
//							to allow the user to select weights for each class pair. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	 Value Returned:  None
//
// Called By:			ClassPairWeightsDialog   in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1999
//	Revised By:			Larry L. Biehl			Date: 12/28/1999	

void ClassPairWeightsDialogOK (
				DialogPtr							dialogPtr,
				#if defined multispec_wx
					wxListBox*							listHandle,
				#else
					ListHandle							listHandle,
				#endif
				SInt16**								weightsListPtrPtr,
				SInt16*								interClassWeightsSelectionPtr,
				SInt16								localDefaultClassPairWeight,
				SInt16*								defaultClassPairWeightPtr)

{
   Cell									cell;

   char*									stringPtr;

   SInt32								bytesNeeded;

   SInt16								index,
											numberOfClassesInWeightGroups,
											numberWeightGroups,
											returnCode,
											saveWeightListIndex,
											stringLength,
											weightListIndex;

   Boolean								changedFlag,
											continueFlag;


			// Initialize local variables.

   continueFlag = TRUE;

			// Get the specified class pair weights.


	#if defined multispec_mac
		numberWeightGroups = ((ListPtr)*listHandle)->dataBounds.bottom;
	#endif	// defined multispec_mac

	#if defined multispec_win  || defined multispec_wx
		numberWeightGroups = listHandle->GetCount ();
	#endif	// defined multispec_win || defined multispec_wx

   if (numberWeightGroups > 0)
		{
				// Get the amount of memory needed for the class pair
				// groups weights list.

      cell.h = 0;
      numberOfClassesInWeightGroups = 0;
      for (index=0; index<numberWeightGroups; index++)
			{
         stringLength = 255;
         cell.v = index;
         LGetCell ((char*)gTextString, &stringLength, cell, listHandle);
         gTextString[stringLength] = 0;
         stringPtr = (char*)&gTextString[3];

					// Find the number of blanks after the dash.  This will
					// be the number of classes in the group less one.

         do {
            stringPtr++;
            numberOfClassesInWeightGroups++;
            stringPtr = strchr (stringPtr, ' ');

				} while (stringPtr != NULL); // end "do ..."

			}	// end "for (index=0; index<numberWeightGroups; ..."

      bytesNeeded = sizeof (SInt16) *
					(1 + 2 * numberWeightGroups + numberOfClassesInWeightGroups);
      continueFlag = CheckPointerSize ((Ptr*)weightsListPtrPtr,
														bytesNeeded,
														&changedFlag);

      if (continueFlag)
			{
					// Load the class pair weight groups in from the list.

         cell.h = 0;
         (*weightsListPtrPtr)[0] = numberWeightGroups;
         weightListIndex = 1;

         for (index=0; index<numberWeightGroups; index++)
				{
            cell.v = index;
            stringLength = 255;
            LGetCell ((char*)gTextString, &stringLength, cell, listHandle);
            gTextString[stringLength] = 0;

						// Get the weight to use.

            strncpy ((char*)gTextString2, (char*)gTextString, 3);
            gTextString2[3] = 0;
            (*weightsListPtrPtr)[weightListIndex] = atoi ((char*)gTextString2);
            weightListIndex++;

            saveWeightListIndex = weightListIndex;
            weightListIndex++;

						// Get the class numbers.

            stringPtr = (char*)&gTextString[3];
            returnCode = sscanf (
								stringPtr, "%hd", &(*weightsListPtrPtr)[weightListIndex]);

						// Find the number of blanks after the dash.  This
						// will be the number of classes in the group less 	
						// one.																

            do {
               stringPtr++;
               returnCode = sscanf (
								stringPtr, "%hd", &(*weightsListPtrPtr)[weightListIndex]);
               weightListIndex++;

               stringPtr = strchr (stringPtr, ' ');

					} while (stringPtr != NULL); // end "do ..."

            (*weightsListPtrPtr)[saveWeightListIndex] =
														weightListIndex - saveWeightListIndex - 1;

				}	// end "for (index=0; index<numberWeightGroups; ..."

         *interClassWeightsSelectionPtr = kUnequalWeightMenuItem;

			}	// end "if (continueFlag)"

		}	// end "if (numberWeightGroups > 0)"

   else	// numberWeightGroups == 0 
      *interClassWeightsSelectionPtr = kEqualWeightMenuItem;

			// Default class weight.

   if (continueFlag)
      *defaultClassPairWeightPtr = localDefaultClassPairWeight;

}	// end "ClassPairWeightsDialogOK"



//------------------------------------------------------------------------------------
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ClassPairWeightsDialogRemoveWeightSelection
//
//	Software purpose:	The purpose of this routine is to handle a change in the class
//							pair weight list.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			ClassWeightsDialog in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1999
//	Revised By:			Larry L. Biehl			Date: 12/28/1999

SInt16 ClassPairWeightsDialogRemoveWeightSelection (
				DialogPtr							dialogPtr,
				#if defined multispec_wx
					wxListBox*							listHandle,
				#else
					ListHandle							listHandle,
				#endif
				SInt16								selectedWeightGroupCell)

{
   LDelRow (1, selectedWeightGroupCell, listHandle);
   selectedWeightGroupCell = -1;
   SetDLogControlHilite (dialogPtr, IDC_RemoveButton, 255);

   return (selectedWeightGroupCell);

}	// end "ClassPairWeightsDialogRemoveWeightSelection"



//------------------------------------------------------------------------------------
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ClassPairWeightsDialogWeightSelectionChange
//
//	Software purpose:	The purpose of this routine is to handle a change in the class
//							pair weight list.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			ClassWeightsDialog in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1999
//	Revised By:			Larry L. Biehl			Date: 12/28/1999

SInt16 ClassPairWeightsDialogWeightSelectionChange (
				DialogPtr							dialogPtr,
				#if defined multispec_wx
					wxListBox*							listHandle)
				#else
					ListHandle							listHandle)
				#endif
{
	Cell									cell;

   SInt16								selectedWeightGroupCell;


   cell.h = 0;
   cell.v = 0;

   if (LGetSelect (TRUE, &cell, listHandle))
		{
      selectedWeightGroupCell = cell.v;
      SetDLogControlHilite (dialogPtr, IDC_RemoveButton, 0);

		}	// end "if (hilite)"

   else	// !LGetSelect (TRUE, &cell, ... 
		{
      SetDLogControlHilite (dialogPtr, IDC_RemoveButton, 255);
      selectedWeightGroupCell = -1;

		}	// end "else !LGetSelect (TRUE, &cell, ..."

   return (selectedWeightGroupCell);

}	// end "ClassPairWeightsDialogWeightSelectionChange"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ClassToBeUsed
//
//	Software purpose:	The purpose of this routine is to determine if the input
//							class is one of those in the input list.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:	TRUE if class is part of the list
//							FALSE if class is not part of the list.
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 06/14/1990
//	Revised By:			Larry L. Biehl			Date: 06/18/1990

Boolean ClassToBeUsed (
				SInt16								classIndex,
				SInt16*								classPtr,
				UInt16								numberClasses)

{
   UInt16								index;


			// Check input parameters.

   if (classPtr == NULL)
																							return (FALSE);

   for (index=0; index<numberClasses; index++)
      if (classIndex == classPtr[index])
																							return (TRUE);

   return (FALSE);

}	// end "ClassToBeUsed"



//------------------------------------------------------------------------------------
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ClassWeightsDialog
//
//	Software purpose:	The purpose of this routine is to display a dialog
//							with a list of the classes and weights that the user
//							can set for the classify processor.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			ClassifyDialog   in SClassifyDialogs.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/22/1990
//	Revised By:			Larry L. Biehl			Date: 12/20/1999

SInt16 ClassWeightsDialog (
				UInt16								numberOfClassesToUse,
				SInt16*								classPtr,
				float*								weightsPtr,
				SInt16								weightsSelection,
				Boolean								useEnhancedStatFlag)

{
	#if defined multispec_mac
		double								weightSum;

		DialogPtr							dialogPtr;

		SInt16								weightUnits;

		Boolean								returnFlag;


				// Get the modal dialog for the reformat specification

		dialogPtr = LoadRequestedDialog (kClassifyWeightsDialogID, kCopyScrap, 1, 2);
		if (dialogPtr == NULL)
																							return (FALSE);

				// Get the display rectangle for the item to place the list into.

		gDialogListHandle = GetOneColumnDialogListHandle (dialogPtr, 6);

				// Continue if there is a valid dialog list handle.

		if (gDialogListHandle != NULL)
			{
			(*gDialogListHandle)->selFlags = lNoRect + lNoExtend + lUseSense;

					//	Load address of routine that will load the class and weight list.

			SetDialogItemDrawRoutine (dialogPtr, 6, gCreateOneColumnList1Ptr);

					// Set routine to draw outline on "OK" box.

			SetDialogItemDrawRoutine (dialogPtr, 16, gHiliteOKButtonPtr);

					// Initialize some dialog parameters.

			ClassWeightsDialogInitialize (dialogPtr, &weightUnits);

			SetDLogControl (dialogPtr, 12, (weightUnits == 0));
			SetDLogControl (dialogPtr, 13, (weightUnits != 0));

					// Center the dialog and then show it.

			ShowDialogWindow (dialogPtr, kClassifyWeightsDialogID, kSetUpDFilterTable);

			gDialogItemDescriptorPtr[9] = kDItemReal;

					// Get deactivate event for the window that is now behind the
					// dialog and handle it.

			CheckSomeEvents (activMask);

					// Make certain that the graph port is set to the classify
					// weights dialog window.

			SetPortDialogPort (dialogPtr);

					// Load the class names to be used into the list.

			LSetDrawingMode (FALSE, gDialogListHandle);

			weightSum = LoadClassWeightsIntoList (gDialogListHandle,
																numberOfClassesToUse,
																classPtr,
																weightsPtr,
																useEnhancedStatFlag);

					//	Load class weight total.

			LoadDItemRealValue (dialogPtr, 15, weightSum, 3);

					// Turn list drawing mode back on.

			LSetDrawingMode (TRUE, gDialogListHandle);

					// Set the text font for the window and draw the list.

			TextFont (gWindowTextFont); // monaco
			LUpdate (GetPortVisibleRegion (GetDialogPort (dialogPtr), gTempRegion1),
														gDialogListHandle);
			SetEmptyRgn (gTempRegion1);

					// Handle modal dialog selections for one column class list.

			returnFlag = FALSE;
			if (gMemoryError == noErr)
				returnFlag = ClassWeightsDialogModal (dialogPtr,
																	numberOfClassesToUse,
																	&weightSum);

			if (returnFlag)
				{
				weightsSelection = ClassWeightsDialogOK (dialogPtr,
																		gDialogListHandle,
																		numberOfClassesToUse,
																		classPtr,
																		weightsPtr);

				}	// end "if (returnFlag)"

					// Make certain that global memory error is set back to 'noErr'.

			gMemoryError = noErr;

			if (gDialogListHandle != NULL)
				LDispose (gDialogListHandle);
			gDialogListHandle = NULL;

			}	// end "if (gDialogListHandle != NULL)"

		CloseRequestedDialog (dialogPtr, kSetUpDFilterTable);
	#endif	// defined multispec_mac

	#if defined multispec_win
		CMClassWeightsDlg* classWeightsDialogPtr = NULL;

		TRY
			{
			classWeightsDialogPtr = new CMClassWeightsDlg ();

			weightsSelection = classWeightsDialogPtr->DoDialog (numberOfClassesToUse,
																					classPtr,
																					weightsPtr,
																					weightsSelection,
																					useEnhancedStatFlag);

			if (classWeightsDialogPtr != NULL)
				delete classWeightsDialogPtr;
			}

		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
	
		END_CATCH_ALL
	#endif	// defined multispec_win

	#if defined multispec_wx
		CMClassWeightsDlg* classWeightsDialogPtr = NULL;
		classWeightsDialogPtr = new CMClassWeightsDlg ();

		weightsSelection = classWeightsDialogPtr->DoDialog (numberOfClassesToUse,
																				classPtr,
																				weightsPtr,
																				weightsSelection,
																				useEnhancedStatFlag);

		if (classWeightsDialogPtr != NULL)
			delete classWeightsDialogPtr;
	#endif	// defined multispec_win

   return (weightsSelection);

}	// end "ClassWeightsDialog"



//------------------------------------------------------------------------------------
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ClassWeightsDialogClassSelectionChange
//
//	Software purpose:	The purpose of this routine is to initialize the class weights
//							dialog box and parameters.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			ClassWeightsDialog in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/17/1999
//	Revised By:			Larry L. Biehl			Date: 12/17/1999

SInt16 ClassWeightsDialogClassSelectionChange (
				DialogPtr							dialogPtr,
				#if defined multispec_wx
					wxListBox*							listHandle,
				#else
					ListHandle							listHandle,
				#endif
				double								newWeight)

{
	Cell									cell;

   SInt16								selectedCell;


   cell.h = 0;
   cell.v = 0;

   if (LGetSelect	(TRUE, &cell, listHandle))
		{
      selectedCell = cell.v;

      if (newWeight >= 0)
			SetDLogControlHilite (dialogPtr, IDC_EnterNewWeightButton, 0);

      SetDLogControlHilite (dialogPtr, IDC_EqualWeightButton, 0);

		}	// end "if (hilite)"

   else	// !LGetSelect (TRUE, &cell, ... 
		{
      SetDLogControlHilite (dialogPtr, IDC_EnterNewWeightButton, 255);
      SetDLogControlHilite (dialogPtr, IDC_EqualWeightButton, 255);
      selectedCell = -1;

		}	// end "else !LGetSelect (TRUE, &cell, ..."

   return (selectedCell);

}	// end "ClassWeightsDialogClassSelectionChange"



//------------------------------------------------------------------------------------
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double ClassWeightsDialogChangeWeights
//
//	Software purpose:	The purpose of this routine is to initialize the class weights
//							dialog box and parameters.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			ClassWeightsDialog in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/17/1999
//	Revised By:			Larry L. Biehl			Date: 12/17/1999

double ClassWeightsDialogChangeWeights (
				DialogPtr							dialogPtr,
				#if defined multispec_wx
					wxListBox*							listHandle,
				#else
					ListHandle							listHandle,
				#endif
				double*								weightSumPtr,
				double								newWeight,
				SInt16								okItemNumber)

{
   Cell									cell;

   SInt16								fixedCellWidth = 40,
											stringLength;


   sprintf ((char*)&gTextString3[1], "%7.3f", newWeight);

   gTextString3[8] = kNullTerminator;
   newWeight = (double)atof ((char*)&gTextString3[1]);

   cell.h = 0;
   cell.v = 0;
   while (LGetSelect (TRUE, &cell, listHandle))
		{
      stringLength = fixedCellWidth;
      LGetCell ((char*)gTextString2, &stringLength, cell, listHandle);

      if (stringLength == fixedCellWidth)
			{
					// Update the sum of the weights.

         BlockMoveData ((char*)&gTextString2[33], (char*)gTextString, 7);
         gTextString [7] = kNullTerminator;
         *weightSumPtr -= (double)atof ((char*)gTextString);
         *weightSumPtr += newWeight;

					// Update the weight and flag in the list.

         gTextString2[0] = ' ';
         BlockMoveData ((char*)&gTextString3[1], (char*)&gTextString2[33], 7);

         LSetCell ((char*)gTextString2, stringLength, cell, listHandle);

			#if defined multispec_win
				LSetSelect (TRUE, cell, listHandle);
			#endif	// defined multispec_win

			}	// end "if (stringLength == ..."

      cell.v++;

		}	// end "while (LGetSelect (TRUE, ...))"
   
   sprintf ((char*)&gTextString3[1], "%.3f", newWeight);
   gTextString3[0] = (UChar)strlen ((char*)&gTextString3[1]);
   LoadDItemString (dialogPtr, IDC_Weight, (Str255*)gTextString3);

   SelectDialogItemText (dialogPtr, IDC_Weight, 0, SHRT_MAX);

   LoadDItemRealValue (dialogPtr, IDC_WeightTotal, *weightSumPtr, 3);

			// Make certain that at least some weights exist.

   if (*weightSumPtr <= 0)
      SetDLogControlHilite (dialogPtr, okItemNumber, 255);

   else	// *weightSumPtr > 0 
      SetDLogControlHilite (dialogPtr, okItemNumber, 0);

   return (newWeight);

}	// end "ClassWeightsDialogChangeWeights"



//------------------------------------------------------------------------------------
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ClassWeightsDialogInitialize
//
//	Software purpose:	The purpose of this routine is to initialize the class weights
//							dialog box and parameters.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			ClassWeightsDialog in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/16/1999
//	Revised By:			Larry L. Biehl			Date: 12/16/1999

void ClassWeightsDialogInitialize (
				DialogPtr							dialogPtr,
				SInt16*								weightUnitsPtr)

{
			//	Unhilite the 'Enter New Weight' button.

   SetDLogControlHilite (dialogPtr, IDC_EnterNewWeightButton, 255);

			//	Load blank in for edit weight.

   LoadDItemString (dialogPtr, IDC_Weight, (Str255*)"\0\0");

			//	Unhilite the 'Add to Equal Weight Set' button.

   SetDLogControlHilite (dialogPtr, IDC_EqualWeightButton, 255);

			//	Set units radio controls.

   *weightUnitsPtr = 0;
   SetDLogControlHilite (dialogPtr, IDC_UnitsPercent, 255);

}	// end "ClassWeightsDialogInitialize"



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ClassWeightsDialogModal
//
//	Software purpose:	The purpose of this routine is to handle the user
//							selections in the classify weights dialog that is used
//							to allow the user the weights for each class. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			ClassWeightsDialog   in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/23/1990
//	Revised By:			Larry L. Biehl			Date: 02/27/2018

Boolean ClassWeightsDialogModal (
				DialogPtr							dialogPtr,
				UInt16								numberOfClassesToUse,
				double*								weightSumPtr)

{
   double								newWeight,
											saveNewWeight,
											defaultEqualWeight = 10.;

   Rect									theBox;

   Cell									cell;

   Handle								okHandle,
											theHandle;

   SInt16								itemHit = 0,
											returnCode,
											selectedCell,
											theType;

   Boolean								modalDone;


   // Continue if dialogPtr and gDialogListHandle are not NULL.			

   if (dialogPtr != NULL && gDialogListHandle != NULL)
		{
				// Get the "handle" for the OK button for use later.

      GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);

      cell.h = 0;
      modalDone = FALSE;
      saveNewWeight = -1;
      selectedCell = -1;
      itemHit = 0;
      do {
         ModalDialog (gProcessorDialogFilterPtr, &itemHit);
         if (itemHit > 2)
				{
            switch (itemHit)
					{
               case 3: // Help 
                  HiliteControl ((ControlHandle)okHandle, 255);
                  returnCode = DisplayAlert (1152, 0, kAlertStrID, 7, 0, NULL);

                  TextFont (gWindowTextFont); // monaco
                  LUpdate (GetPortVisibleRegion (GetDialogPort (dialogPtr),
																	gTempRegion1),
																	gDialogListHandle);
                  SetEmptyRgn (gTempRegion1);
                  HiliteControl ((ControlHandle) okHandle, 0);
                  break;

               case 6:
                  selectedCell = ClassWeightsDialogClassSelectionChange (
																						dialogPtr,
																						gDialogListHandle,
																						saveNewWeight);
                  break;

               case 8: // Change the weight for each class selected.		
                  TextFont (gWindowTextFont); // monaco  

                  saveNewWeight = ClassWeightsDialogChangeWeights (
																						dialogPtr,
																						gDialogListHandle,
																						weightSumPtr,
																						saveNewWeight,
																						1);
                  break;

               case 9: // Enter edit weight value.								
                  GetDialogItem (dialogPtr, 9, &theType, &theHandle, &theBox);
                  GetDialogItemText (theHandle, gTextString);
                  if (gTextString[0] > 0)
							{
                     newWeight = GetDItemRealValue (dialogPtr, 9);
                     if (newWeight > 999.999 || newWeight < 0)
                        RealNumberErrorAlert (saveNewWeight, dialogPtr, 9, 3);

                     else	// newWeight >= 0 && newWeight <= 999.9 
								{
                        saveNewWeight = newWeight;
                        if (selectedCell >= 0)
                           SetDLogControlHilite (dialogPtr, 8, 0);

								}	// end "else newWeight >= 0 && ..."

							}	// end "if (gTextString[0] < 0)"

                  else	// gTextString[0] <= 0 
							{
                     saveNewWeight = -1;
                     SetDLogControlHilite (dialogPtr, 8, 255);

							}	// end "else gTextString[0] <= 0"

                  break;

               case 10: // Add to equal weight set.								
                  TextFont (gWindowTextFont); // monaco  

                  ClassWeightsDialogSetEqualWeights (dialogPtr,
																		gDialogListHandle,
																		weightSumPtr,
																		defaultEqualWeight,
																		1);
                  break;

               case 12:
                  break;

               case 13:
                  break;

					}	// end "switch (itemHit)"

				}	// end "if (itemHit > 2)"

         else if (itemHit > 0) // and itemHit <= 2 
						// itemHit == 1 is OK, itemHit == 2 is Cancel.
            modalDone = TRUE;

			} while (!modalDone);

		}	// end "if (dialogPtr != NULL && gDialogListHandle != NULL)"

   return (itemHit == 1);

}	// end "ClassWeightsDialogModal"
#endif	// defined multispec_mac 



//------------------------------------------------------------------------------------
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 ClassWeightsDialogOK
//
//	Software purpose:	The purpose of this routine is to get the weights for the 
//							classes from the class weight list in the dialog box.
//							It will check if the weights are equal and set the parameter
//							indicating that equal weights are to be used.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			ClassWeightsDialog in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/20/1999
//	Revised By:			Larry L. Biehl			Date: 12/20/1999

SInt16 ClassWeightsDialogOK (
				DialogPtr							dialogPtr,
				#if defined multispec_wx
					wxListBox*							listHandle,
				#else
					ListHandle							listHandle,
				#endif
				UInt16								numberOfClassesToUse,
				SInt16*								classPtr,
				float*								weightsPtr)

{
	Cell									cell;

   UInt32								index;

   SInt16								classIndex,
											classStorage,
											fixedCellWidth = 40,
											stringLength,
											weightsSelection;


			// Get the new weight variables.
			// Also reset weightsSelection in case all classes belong to	
			// equal weight set.															

   weightsSelection = kEqualWeightMenuItem;
   cell.h = 0;
   cell.v = 0;
   for (index=0; index<numberOfClassesToUse; index++)
		{
      classIndex = classPtr[index] - 1;
      classStorage = gProjectInfoPtr->storageClass[classIndex];

      stringLength = fixedCellWidth;
      LGetCell ((char*)gTextString2, &stringLength, cell, listHandle);

      if (stringLength == fixedCellWidth)
			{
         BlockMoveData ((char*)&gTextString2[33], (char*)gTextString, 7);
         gTextString[7] = kNullTerminator;

         weightsPtr[classIndex] = (float)atof ((char*)gTextString);

         if (gTextString2[0] == ' ')
				{
            weightsPtr[classIndex] = -weightsPtr[classIndex];
            weightsSelection = kUnequalWeightMenuItem;

				}	// end "if (gTextString2[0] == ' ')"

			}	// end "if (stringLength == ..."

      cell.v++;

		}	// end "for (index=0; index<numberOfClassesToUse; ...)"

   return (weightsSelection);

}	// end "ClassWeightsDialogOK"



//------------------------------------------------------------------------------------
//
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ClassWeightsDialogSetEqualWeights
//
//	Software purpose:	The purpose of this routine is to set the weights for
//							each class to an equal weight.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			ClassWeightsDialog in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/17/1999
//	Revised By:			Larry L. Biehl			Date: 12/17/1999

void ClassWeightsDialogSetEqualWeights (
				DialogPtr							dialogPtr,
				#if defined multispec_wx
					wxListBox*							listHandle,
				#else
					ListHandle							listHandle,
				#endif
				double*								weightSumPtr,
				double								defaultEqualWeight,
				SInt16								okItemNumber)

{
   Cell									cell;

   SInt16								fixedCellWidth = 40,
											stringLength;


   sprintf ((char*)gTextString3, "%7.3f", defaultEqualWeight);

   cell.v = 0;
   cell.h = 0;
   while (LGetSelect (TRUE, &cell, listHandle))
		{
      stringLength = fixedCellWidth;
      LGetCell ((char*)gTextString2, &stringLength, cell, listHandle);

      if (stringLength == fixedCellWidth)
			{
					// Update the sum of the weights.

         BlockMoveData ((char*)&gTextString2[33], (char*)gTextString, 7);
         gTextString[7] = kNullTerminator;
         *weightSumPtr -= (double)atof ((char*)gTextString);
         *weightSumPtr += defaultEqualWeight;

					// Update the weight and flag in the list.

			#if defined multispec_mac
				gTextString2[0] = '.';
			#endif	// defined multispec_mac

			#if defined multispec_win ||multispec_wx
				gTextString2[0] = '#';
			#endif	// defined multispec_win

         BlockMoveData ((char*)gTextString3, (char*)&gTextString2[33], 7);

         LSetCell ((char*)gTextString2, stringLength, cell, listHandle);

			#if defined multispec_win || defined multispec_wx
				LSetSelect (TRUE, cell, listHandle);
			#endif	// defined multispec_win || defined multispec_wx

			}	// end "if (stringLength == ...)"

      cell.v++;

		}	// end "while (LGetSelect (TRUE, ...))"

   SelectDialogItemText (dialogPtr, IDC_Weight, 0, SHRT_MAX);

   LoadDItemRealValue (dialogPtr, IDC_WeightTotal, *weightSumPtr, 3);

   if (*weightSumPtr > 0)
      SetDLogControlHilite (dialogPtr, okItemNumber, 0);

}	// end "ClassWeightsDialogSetEqualWeights"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ClearGlobalAlertVariables
//
//	Software purpose:	The purpose of this routine is to clear the global
//							variables that are used for the alert that asks the
//							user whether to stop and application at once or
//							complete the current iteration.
//
//	Parameters in:		.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/07/1994
//	Revised By:			Larry L. Biehl			Date: 01/07/1994

void ClearGlobalAlertVariables (void)

{
   gAlertId = 0;
   gAlertStrID = 0;
   gAlertStringNumber = 0;
   gAlertReturnCode = 0;

}	// end "ClearGlobalAlertVariables"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ComputeChiSquaredConstants
//
//	Software purpose:	The purpose of this routine is to compute the constants
//							needed in computing the Chi Squared values.. 
//
//	Parameters in:		number degrees of freedom
//
//	Parameters out:	factor1
//							factor2
//							one divided by gamma of half of degrees of freedom.
//
// Value Returned:  	None
//
// Called By:			CreateChiSquareThresholdTable in SClassify.cpp
//							GetChiSquaredValue in SProjectUtilities.cpp
//	Coded By:			Larry L. Biehl			Date: 12/20/1993
//	Revised By:			Larry L. Biehl			Date: 12/20/1993	

void ComputeChiSquaredConstants (
				SInt32								degreesOfFreedom,
				double*								factor1Ptr,
				double*								factor2Ptr,
				double*								oneOverGammaOfHalfDFPtr)

{
			// Get constants for calculations that only depend on the number of
			// degrees of freedom.																

   if (degreesOfFreedom < kDegreesFreedomCrossover)
		{
				// Get constants for algorithm to be used when the number of
				// degrees of freedom is less than degreesFreedomCrossover.			
				// Get the gamma of one half the number of degrees of freedom and	
				// then invert it.																

      ComputeGammaFunctionHalfValue ((SInt16)degreesOfFreedom,
													oneOverGammaOfHalfDFPtr);
      *oneOverGammaOfHalfDFPtr = 1. / (*oneOverGammaOfHalfDFPtr);

				// Get the intregral factor.

      *factor1Ptr = *oneOverGammaOfHalfDFPtr * (double)2 /
													pow ((double)2, (double)degreesOfFreedom / 2);
      *factor2Ptr = 1.;

		}	// end "if (degreesOfFreedom < kDegreesFreedomCrossover)"

   else	// degreesOfFreedom >= kDegreesFreedomCrossover 
		{
				// Get the constants for the algorithm to be used when the number
				// of degrees of freedom is degreesFreedomCrossover or more.		

      *factor1Ptr = (double)2. / (9 * degreesOfFreedom);
      *factor2Ptr = sqrt (*factor1Ptr);

		}	// end "else degreesOfFreedom >= kDegreesFreedomCrossover"

}	// end "ComputeChiSquaredConstants"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double ComputeChiSquaredValue
//
//	Software purpose:	The purpose of this routine is compute the chi squared
//							value for the input degrees of freedom and percent
//							probability.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			FS_decision_boundary in SFeatureExtractionMath.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/20/1993
//	Revised By:			Larry L. Biehl			Date: 04/03/1997	

double ComputeChiSquaredValue (
				SInt32								degreesOfFreedom,
				double								factor1,
				double								factor2,
				double								oneOverGammaOfHalfDF,
				double								threshold_probability)

{
   double								chiSquaredValue,
											rIncrement,
											smallestValue,
											tempProbability,
											tempVariable,
											tempVariable2,
											thresholdPrecision;


			// Initialize local variables.

   rIncrement = 2;
   chiSquaredValue = 0.;

   if (threshold_probability <= 0.)
																				return (chiSquaredValue);

   if (threshold_probability >= 1.)
		{
      chiSquaredValue = DBL_MAX;
																				return (chiSquaredValue);

		}	// end "if (threshold_probability >= 1.)"

	#ifdef _80BitDoubles_
		#ifdef _MC68881_
			smallestValue = 1e-18;
		#endif
		#ifndef _MC68881_
					// This was changed from 1e-18 to 1e-15 on 3/3/1997 so that
					// the program would not go into an infinite loop when
					// running under MAE.
			smallestValue = 1e-15;
		#endif
	#else
		smallestValue = 1e-15;
	#endif

   if (threshold_probability >= .99)
		{
      thresholdPrecision = (1. - threshold_probability) / 1000000;

      if (thresholdPrecision < smallestValue)
         thresholdPrecision = smallestValue;

		}	// end "if (threshold_probability >= .99)"

   else	// threshold_probability < .99
      thresholdPrecision = 0.00000001;

   if (degreesOfFreedom < kDegreesFreedomCrossover)
		{
      FindRThreshold ((SInt16)degreesOfFreedom,
								&chiSquaredValue,
								threshold_probability,
								rIncrement,
								thresholdPrecision,
								factor1,
								oneOverGammaOfHalfDF);

      chiSquaredValue *= chiSquaredValue;

		}	// end "if (degreesOfFreedom < kDegreesFreedomCrossover)"

   else	// degreesOfFreedom >= kDegreesFreedomCrossover 
		{
				// Approximate the chi squared value with the following
				// formula when the number of degrees of freedom is more than	
				// the the crossover number .. somewhere around 30-40.			

      tempProbability = threshold_probability;
      if (threshold_probability > 0.5)
         tempProbability = 1 - threshold_probability;

      if (tempProbability < smallestValue)
         tempProbability = smallestValue;

      tempVariable2 = -2 * log (tempProbability);
      tempVariable = sqrt (tempVariable2);

      chiSquaredValue = tempVariable -
				(2.515517 + 0.802853 * tempVariable + 0.010328 * tempVariable2) /
					(1.0 + 1.432788 * tempVariable + 0.189269 * tempVariable2 +
							0.001308 * tempVariable * tempVariable2);

      if (threshold_probability <= 0.5)
         chiSquaredValue = -chiSquaredValue;

      tempVariable = 1.0 - factor1 + chiSquaredValue*factor2;

      chiSquaredValue = degreesOfFreedom * tempVariable * tempVariable * tempVariable;

		}	// end "else degreesOfFreedom >= kDegreesFreedomCrossover"

   return (chiSquaredValue);

}	// end "ComputeChiSquaredValue"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ComputeGammaFunctionHalfValue
//
//	Software purpose:	The purpose of this routine is to compute the 
//							Gamma function value for half of the given input 
//							value.  The even input values the gamma is
//							(n/2-1)!.  For odd input values the gamma is
//							(b/2-1)(n/2-2) ... 5/2 3/2 1/2 sqrt (PI).
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:	
//
//	Coded By:			Chulhee Lee				Date: ?
//	Revised By:			Larry L. Biehl			Date: 02/19/1990	

void ComputeGammaFunctionHalfValue (
				SInt16								n,
				double*								ret)

{
   double								x;

   UInt32								i,
											local_n,
											nd2;


   local_n = n;
   nd2 = local_n / 2;

   if (local_n == nd2 * 2)
		{
      *ret = 1;
      for (i=1; i<nd2; i++)
         *ret *= i;

		}	// end "if (local_n == nd2*2)"

   else	// local_n!= nd2*2 
		{
      *ret = kSQRTPI;

      for (x=.5; x<nd2; x+=1)
         *ret *= x;

		}	// end "else n!= n/2*2"

}	// end "ComputeGammaFunctionHalfValue"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void CreateFieldRgn
//
//	Software purpose:	The purpose of this routine is to create a region
//							based on the polygon points that describe a field.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			GetPolygonLabelPoint in SOutlineFields.cpp
//							GetFieldBoundary in SProjectUtilities.cpp
//							GetSelectionBoundary in SSelectionUtility.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/16/1988
//	Revised By:			Larry L. Biehl			Date: 10/27/2015	

void CreateFieldRgn (
				SInt16								numberPoints,
				HPFieldPointsPtr					fieldPointsPtr,
				RgnHandle*							rgnHandlePtr,
				SInt32								columnOffset,
				SInt32								lineOffset)
				
{
	*rgnHandlePtr = NULL;
	
			// Make certain that input values seem valid.								

	if (numberPoints >= 3) 
		{
      SInt16		i;

		#if defined multispec_mac
			*rgnHandlePtr = NewRgn ();

			if (*rgnHandlePtr != NULL) 
				{
				OpenRgn ();
				MoveTo ((SInt16)(fieldPointsPtr[numberPoints - 1].col + columnOffset),
							(SInt16)(fieldPointsPtr[numberPoints - 1].line + lineOffset));

				for (i=0; i<numberPoints; i++)
					{
					if (gMemoryError == noErr)
						LineTo ((SInt16)(fieldPointsPtr[i].col + columnOffset),
									(SInt16)(fieldPointsPtr[i].line + lineOffset));

					}	// end "for (i=0; i<numberPoints; i++)"

				CloseRgn (*rgnHandlePtr);

				}	// end "if (*rgnHandle != NULL)"
		#endif	// defined multispec_mac

		#if defined multispec_win
			POINT* pointsPtr = NULL;

			pointsPtr = (POINT*)MNewPointer ((SInt64)numberPoints * sizeof (POINT));
			if (pointsPtr != NULL)
				*rgnHandlePtr = new CRgn;

			if (*rgnHandlePtr != NULL) 
				{
				for (i=0; i<numberPoints; i++)
					{
					pointsPtr[i].x = (int)(fieldPointsPtr[i].col + columnOffset);
					pointsPtr[i].y = (int)(fieldPointsPtr[i].line + lineOffset);

					}	// end "for (i=0; i<numberPoints; i++)" 

				if (!(*rgnHandlePtr)->CreatePolygonRgn ((tagPOINT*)pointsPtr,
																		numberPoints,
																		WINDING)) 
					{
					delete *rgnHandlePtr;
					*rgnHandlePtr = NULL;

					}	// end "if (!(*rgnHandlePtr)->CreatePolygonRgn (..."

				}	// end "if (*rgnHandlePtr != NULL)"

			CheckAndDisposePtr ((Ptr)pointsPtr);
		#endif	// defined multispec_win
		
		#if defined multispec_wx
			wxPoint* pointsPtr = (wxPoint*)MNewPointer (
															(SInt64)numberPoints * sizeof (wxPoint));
			if (pointsPtr != NULL)
				{
				for (i=0; i<numberPoints; i++)
					{
					pointsPtr[i].x = (int)(fieldPointsPtr[i].col + columnOffset);
					pointsPtr[i].y = (int)(fieldPointsPtr[i].line + lineOffset);

					}	// end "for (i=0; i<numberPoints; i++)"
					
				//*rgnHandlePtr = new wxRegion (numberPoints, pointsPtr, wxWINDING_RULE);
				*rgnHandlePtr = new wxRegion (numberPoints, pointsPtr, wxODDEVEN_RULE);

				}	// end "if (pointsPtr != NULL)"

			CheckAndDisposePtr ((Ptr)pointsPtr);
		#endif	// defined multispec_wx

		}	// end "if (numberPoints >= 3)"

}	// end "CreateFieldRgn"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 DetermineFieldTypes
//
//	Software purpose:	The purpose of this routine is to determine if any
//							of the classes contain fields which are not
//							cluster fields.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	Binary SInt16 - 1's column set if non-
//										cluster, training field exists; 2's column
//										set if non-cluster, testing field exists,
//										4's column is set if a cluster field exists.
// 
// Called By:			ListResultsDialog in SListResults.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/31/1991
//	Revised By:			Larry L. Biehl			Date: 12/30/1998	

SInt16 DetermineFieldTypes (void)

{
	HPClassNamesPtr					classNamesPtr;
   HPFieldIdentifiersPtr			fieldIdentPtr;

   SInt16								classStorage,
											clusterField,
											fieldNumber,
											nonClusterTrainField,
											nonClusterTestField;

   UInt16								classIndex;


   if (gProjectInfoPtr == NULL)
																							return (0);

			// Initialize local variables.													

   classNamesPtr = gProjectInfoPtr->classNamesPtr;
   fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
   clusterField = 0;
   nonClusterTrainField = 0;
   nonClusterTestField = 0;


   for (classIndex=0;
			classIndex<gProjectInfoPtr->numberStatisticsClasses;
			classIndex++) 
		{
				// Get the class storage number.												

      classStorage = gProjectInfoPtr->storageClass[classIndex];

      fieldNumber = classNamesPtr[classStorage].firstFieldNumber;

      while (fieldNumber != -1) 
			{
					// If the field is not a cluster type of field, then set		
					// appropriate variable to indicate that a non cluster type	
					// field exists.															

         if (fieldIdentPtr[fieldNumber].pointType != kClusterType) 
				{
            if (fieldIdentPtr[fieldNumber].fieldType & kTrainingType)
               nonClusterTrainField = 1;

            if (fieldIdentPtr[fieldNumber].fieldType & kTestingType)
               nonClusterTestField = 2;

				}	// end "if (fieldIdentPtr[fieldNumber].pointType < ..." 

         else	// fieldIdentPtr[fieldNumber].pointType == kClusterType 
            clusterField = 4;

         fieldNumber = fieldIdentPtr[fieldNumber].nextField;

			}	// end "while (fieldNumber != -1)" 

		}	// end "for (classIndex=0; classIndex<..."

   return (clusterField + nonClusterTrainField + nonClusterTestField);

}	// end "DetermineFieldTypes" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		short int DiskFilePopUpMenu
//
//	Software purpose:	The purpose of this routine is to handle a 
//							mousedown in a 'Disk File Format' pop up 
//							menu location.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			ClassifyDialog in SClassifyDialogs.cpp	
//
//	Coded By:			Larry L. Biehl			Date: 02/10/1990
//	Revised By:			Larry L. Biehl			Date: 04/18/2013	

SInt16 DiskFilePopUpMenu (
				DialogPtr							dialogPtr,
				MenuHandle							diskFileMenu,
				SInt16								dialogItem)

{
   SInt16								returnCode;


			// Initialize local variables.

   returnCode = 0;

   if (gOutputFormatCode > 0)
      returnCode = StandardPopUpMenu (dialogPtr,
													dialogItem - 1,
													dialogItem,
													diskFileMenu,
													gOutputFormatCode,
													kPopUpDiskFileMenuID);

   return (returnCode);

}	// end "DiskFilePopUpMenu"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawChannelsPopUp2
//
//	Software purpose:	The purpose of this routine is to draw the channel
//							prompt and channel set selection for channel
//							pop up menu.  This routine will also draw a drop
//							shadow box around the pop up selection.  The
//							difference between this routine and DrawChannelPopUp
//							is that selection 1 is 'All available' not 'All'.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/16/1989
//	Revised By:			Larry L. Biehl			Date: 10/30/1990	

pascal void DrawChannelsPopUp2 (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.

   DrawPopUpMenuItem (dialogPtr,
								itemNumber,
								gPopUpAllAvailableSubsetMenu,
								gChannelSelection,
								TRUE);

}	// end "DrawChannelsPopUp2"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawInterClassWeightsPopUp
//
//	Software purpose:	The purpose of this routine is to draw the weights
//							prompt and weight set selection for class weights
//							pop up menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/12/1993
//	Revised By:			Larry L. Biehl			Date: 01/12/1993	

pascal void DrawInterClassWeightsPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.

   DrawPopUpMenuItem (dialogPtr,
								itemNumber,
								gPopUpWeightsMenu,
								gInterClassWeightsSelection,
								TRUE);

}	// end "DrawInterClassWeightsPopUp"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawProjectChangesPopUp
//
//	Software purpose:	The purpose of this routine is to draw the channel
//							prompt and channel set selection for channel
//							pop up menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 10/05/1989
//	Revised By:			Larry L. Biehl			Date: 03/25/2017

pascal void DrawProjectChangesPopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)
 {
			// Declare local variables and structures
	/*
	Rect									popUpBox;
   Handle								theHandle;

   SInt16								bottomSpace = 6,
											leftSpace = 4,
											theType;
	*/

			// Use the generic pop up drawing routine.
 
   DrawPopUpMenuItem (dialogPtr, itemNumber, NULL, 1, TRUE);
 
	/*
			// Make certain that the correct graph port is set up.

   SetPortDialogPort (dialogPtr);

			// Get the rectangle for the pop up box.

   GetDialogItem (dialogPtr, itemNumber, &theType, &theHandle, &popUpBox);

			// Clear the pop up box.

   EraseRect (&popUpBox);

			// Draw the current channels selection.

   MoveTo (popUpBox.left + leftSpace, popUpBox.bottom - bottomSpace);

   DrawString ("\pProject Changes");

			// Draw the pop up box around currently selected channel option.

   DrawDropShadowBox (popUpBox);

   DrawDropDownTriangle (&popUpBox);
	*/

}	// end "DrawProjectChangesPopUp"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		pascal void DrawSeparabilityDistancePopUp
//
//	Software purpose:	The purpose of this routine is to draw the 
//							separability measure prompt and channel combinations
//							set selection for channel combinations
//							pop up menu.  This routine will also draw a drop
//							shadow box around the pop up selection.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 04/02/1993
//	Revised By:			Larry L. Biehl			Date: 04/02/1993	

pascal void DrawSeparabilityDistancePopUp (
				DialogPtr							dialogPtr,
				SInt16								itemNumber)

{
			// Use the generic pop up drawing routine.

   DrawPopUpMenuItem (dialogPtr,
								itemNumber,
								gPopUpSeparabilityDistanceMenu,
								gSeparabilityDistance,
								TRUE);

}	// end "DrawSeparabilityDistancePopUp"
#endif	// defined multispec_mac



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void FindRThreshold
//
//	Software purpose:	The purpose of this routine is to compute the 
//							threshold radius such that F(rThreshold) is within
//							the thresholdPrecision of thresholdProbability. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			CreateChiSquareThresholdTable in SClassify.cpp
//
//	Coded By:			Chulhee Lee				Date: ?
//	Revised By:			Larry L. Biehl			Date: 03/12/1990	

void FindRThreshold (
				SInt16								degreesOfFreedom,
				double*								rThreshold,
				double								thresholdProbability,
				double								rIncrementStart,
				double								thresholdPrecision,
				double								factor,
				double								oneOverGammaOfHalfDF)

{
   double								endProbability,
											lowestR = 0.0000,
											probability,
											probabilityDifference,
											r,
											rIncrement,
											rStart,
											startProbability;

   Boolean								haveEndFlag,
											haveStartFlag;


			// Check for invalid input value.

   if (*rThreshold < 0)
																									return;

			// Initialize local variables.

   rStart = (*rThreshold == 0) ? rIncrementStart : *rThreshold - rIncrementStart;
   rStart = MAX (lowestR, rStart);
   rIncrement = rIncrementStart;
   haveStartFlag = FALSE;
   haveEndFlag = FALSE;

   do {
      for (r=rStart;; r+=rIncrement)
			{
         Intg_Normal (degreesOfFreedom, r, &probability, factor, oneOverGammaOfHalfDF);

         probabilityDifference = fabs (probability - thresholdProbability);
         if (probabilityDifference < thresholdPrecision)
            break;

         if (probability >= thresholdProbability)
				{
            haveEndFlag = TRUE;
            endProbability = probability;

            if (!haveStartFlag)
					{
               rStart -= rIncrement;
               break;

					}	// end "if (!haveStartFlag)"

				}	// end "if (probability >= thresholdProbability)"

         else	// probability < thresholdProbability 
				{
            rStart = r;
            haveStartFlag = TRUE;
            startProbability = probability;

				}	// end "probability < thresholdProbability"

         if (haveStartFlag && haveEndFlag)
				{
            rStart = rStart + (thresholdProbability - startProbability) /
												(endProbability - startProbability) * rIncrement;

            rIncrement /= 5;
            if (rIncrement < 1e-19)
               probabilityDifference = 0;


            rStart -= rIncrement;
            rStart = MAX (lowestR, rStart);

            haveStartFlag = FALSE;
            haveEndFlag = FALSE;

            break;

				}	// end "if (haveStartFlag && haveEndFlag)"

			}	// end "for (r=rStart; ; r+=rIncrement)"

		} while (probabilityDifference > thresholdPrecision);

   *rThreshold = r;

}	// end "FindRThreshold"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ForceProjectUtilityCodeResourceLoad
//
//	Software purpose:	The purpose of this is to force the 'Project Utility' code
//							resource to be loaded into memory.  It is called by
//							routines in the 'Project' code resource.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 11/19/1992
//	Revised By:			Larry L. Biehl			Date: 11/19/1992

void ForceProjectUtilityCodeResourceLoad (void)

{
			// If spare memory had to be used to load code resources, then exit routine.

   if (gMemoryTypeNeeded < 0)
																									return;

			// Code resources loaded okay, so set flag back for non-Code
			// resources.

   gMemoryTypeNeeded = 0;

}	// end "ForceProjectUtilityCodeResourceLoad"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetChiSquaredValue
//
//	Software purpose:	The purpose of this routine is compute the constants
//							needed for the chi squared algorithm and the chi
//							squared value itself for the input percent probability.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			FS_decision_boundary in SFeatureExtractionMath.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/22/1992
//	Revised By:			Larry L. Biehl			Date: 12/20/1993	

double GetChiSquaredValue (
				SInt32								degreesOfFreedom,
				double								threshold_probability)

{
   double								chiSquaredValue,
											factor1,
											factor2,
											oneOverGammaOfHalfDF;


			// Intialize the nextTime variable to indicate when the next check
			// should occur for a command-.

   gNextTime = TickCount ();

			// Get constants for calculations that only depend on the number of
			// degrees of freedom.

   ComputeChiSquaredConstants (degreesOfFreedom,
											&factor1,
											&factor2,
											&oneOverGammaOfHalfDF);

   chiSquaredValue = ComputeChiSquaredValue (degreesOfFreedom,
															factor1,
															factor2,
															oneOverGammaOfHalfDF,
															threshold_probability);

   return (chiSquaredValue);

}	// end "GetChiSquaredValue"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		UInt32 GetClassNameMaxLength
//
//	Software purpose:	The purpose of this routine is to get the length of the 
//							longest class name in the input vector of classes.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:
//
//	Called by:			ListClusterStatistics in SCluster.cpp
//							GetTableColumnWidths in SListResults.cpp
//							ListClassificationSummary in SListResults.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/11/1997
//	Revised By:			Larry L. Biehl			Date: 01/09/1998

UInt32 GetClassNameMaxLength (
				UInt32								numberClasses,
				SInt16*								classPtr,
				HUInt32Ptr							nameLengthVectorPtr)
 {
   Ptr									namePtr;

   UInt32								classIndex,
											classStorage,
											maxNameLength,
											statClassNumber;


   maxNameLength = 0;

   if (gProjectInfoPtr != NULL)
		{
      for (classIndex=0; classIndex<numberClasses; classIndex++)
			{
         if (classPtr != NULL)
            statClassNumber = classPtr[classIndex] - 1;

         else	// classPtr != NULL
            statClassNumber = classIndex;

					// Get the class storage number.

         classStorage = gProjectInfoPtr->storageClass[statClassNumber];

         namePtr = (char*)&gProjectInfoPtr->classNamesPtr[classStorage].name;

         maxNameLength = MAX ((UInt32)namePtr[0], maxNameLength);

         if (nameLengthVectorPtr != NULL)
            nameLengthVectorPtr[statClassNumber] = (UInt32)namePtr[0];

			}	// end "for (classIndex=0; classIndex<numberLines; classIndex++)"

		}	// end "if (gProjectInfoPtr != NULL)"

   return (maxNameLength);

}	// end "GetClassNameMaxLength"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		ClassInfoPtr GetClassInfoStructure
//
//	Software purpose:	The purpose of this routine is to allocate the memory
//							for a class info structure for the input number of classes
//							and intialize the structure.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:
//
//	Coded By:			Larry L. Biehl			Date: 07/02/1996
//	Revised By:			Larry L. Biehl			Date: 07/02/1996

ClassInfoPtr GetClassInfoStructure (
				UInt16								numberClasses)

{
   ClassInfoPtr						classInfoPtr;


			// Get memory for the structure which contains pointers to the
			// class statistics and data values.

   classInfoPtr = (ClassInfoPtr)MNewPointer (numberClasses * sizeof (ClassInfo));

   if (classInfoPtr != NULL)
		{
      UInt16 index;

				// Initialize quickClassInfor structure.

      for (index = 0; index < numberClasses; index++)
			{
         classInfoPtr[index].determinant = 0;
         classInfoPtr[index].logDeterminant = 0;
         classInfoPtr[index].no_samples = 0;
         classInfoPtr[index].covariancePtr = NULL;
         classInfoPtr[index].dataValuesPtr = NULL;
         classInfoPtr[index].inversePtr = NULL;
         classInfoPtr[index].meanPtr = NULL;
         classInfoPtr[index].transformedCovPtr = NULL;
         classInfoPtr[index].transformedMeanPtr = NULL;

			}	// end "for (index=0; index<..."

		}	// end "if (classInfoPtr != NULL)"

   return (classInfoPtr);

}	// end "GetClassInfoStructure"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetClassWeightsIndex
//
//	Software purpose:	The purpose of this routine is to get the index to
//							be used for the class weights depending on 
//							whether original or enhanced statistics 
//							and whether equal or unequal weights are to be used.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:	GetProjectClassWeightsIndex in SProjectUtilities.cpp
//							UpdateProjectClassWeights in SProjectUtilities.cpp
//							LoadStatEnhanceClassStatistics in SStatisticsEnhancement.cpp
//							StatisticsEnhanceControl in SStatisticsEnhancement.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/27/1993
//	Revised By:			Larry L. Biehl			Date: 01/06/1994

SInt16 GetClassWeightsIndex (
				Boolean								useEnhancedStatisticsFlag,
				Boolean								equalWeightsFlag)

{
   SInt16								weightIndex;


   weightIndex = 0;

   if (equalWeightsFlag)
      weightIndex = 2;

   else	// !equalWeightsFlag 
		{
      if (useEnhancedStatisticsFlag)
         weightIndex = 1;

		}	// end "else !equalWeightsFlag

   return (weightIndex);

}	// end "GetClassWeightsIndex"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		float* GetClassWeightsPtr
//
//	Software purpose:	The purpose of this routine is to get the pointer
//							to the appropriate class weights vector depending
//							upon whether the pointer is requested for the
//							non-modifed or modified statistics.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/27/1993
//	Revised By:			Larry L. Biehl			Date: 12/29/1993

float* GetClassWeightsPtr (
				float*								weightsPtr,
				Boolean								useEnhancedStatisticsFlag,
				Boolean								equalWeightsFlag)

{
   if (equalWeightsFlag)
      weightsPtr = &weightsPtr[2 * gProjectInfoPtr->numberStatisticsClasses];

   else	// !equalWeightsFlag 
		{
      if (useEnhancedStatisticsFlag)
         weightsPtr = &weightsPtr[gProjectInfoPtr->numberStatisticsClasses];

		}	// end "else !equalWeightsFlag

   return (weightsPtr);

}	// end "GetClassWeightsPtr"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double GetClassWeightValue
//
//	Software purpose:	The purpose of this routine is to determine the
//							weight value for the specified class given
//							the total weight of all classes being used.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/06/1994
//	Revised By:			Larry L. Biehl			Date: 09/19/1997	

double GetClassWeightValue (
				UInt16								statClassNumber,
				UInt32								weightsIndex,
				double								totalProbability)

{
   double								weightValue;

   float*								classWeightsPtr;

   SInt16								classStorage;


   classStorage = gProjectInfoPtr->storageClass[statClassNumber];

   if (weightsIndex <= 3)
		{
      classWeightsPtr =
         gProjectInfoPtr->classNamesPtr[classStorage].priorWeights;

      weightValue =
         fabs ((double) classWeightsPtr[weightsIndex]) / totalProbability;

		}	// end "if (weightsIndex <= 3)"

   else	// weightsIndex == 4
      weightValue =
				gProjectInfoPtr->classNamesPtr[classStorage].numberStatisticsPixels /
																							totalProbability;

   return (weightValue);

}	// end "GetClassWeightValue" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetCommonCovarianceWeightsIndex
//
//	Software purpose:	The purpose of this routine is to get the index to
//							be used for the class weights depending on 
//							whether original or enhanced statistics 
//							and whether equal or unequal weights are to be used.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:	GetCommonCovariance in SProjectComputeStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/19/1997
//	Revised By:			Larry L. Biehl			Date: 12/16/1999

UInt16 GetCommonCovarianceWeightsIndex ()

{
	UInt16								weightIndex;


			// equal weighting
			
   weightIndex = 2;

   if (gProjectInfoPtr->commonCovarianceWeightSet == kUnequalWeights)
				// user supplied weighting
      weightIndex = 3;

   else if (gProjectInfoPtr->commonCovarianceWeightSet == kNumberSampleWeights)
				// weight by number of samples
      weightIndex = 4;

   return (weightIndex);

}	// end "GetCommonCovarianceWeightsIndex" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetFieldBoundary
//
//	Software purpose:	The purpose of this routine is to get the line and
//							column coordinates for the specified field from
//							the project file information that is in memory.  If
//							the field of polygon type then the region is 
//							setup for the polygon.
//
//	Parameters in:		
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			ClassifyTrainTestFields in SClassify.cpp
//							GetNextClusterArea in SCluster.cpp
//							GetNumberOfClusterPixels in SCluster.cpp
//							ConvertImagePixelsToClassNumbers in SFieldsToThematicFile.cpp
//							ListFieldData in SListData.cpp
//							ListResultsTrainTestFields in SListResults.cpp
//							GetTotalSumSquares in SMatrixUtilities.cpp
//							CompareImageDimensionsWithProjectFields in SProject.cpp
//							UpdateFieldStats in SProjectComputeStatistics.cpp
//							HistogramFieldStats in SProjectHistogramStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/15/1988
//	Revised By:			Larry L. Biehl			Date: 10/27/2015

void GetFieldBoundary (
				ProjectInfoPtr						classProjectInfoPtr,
				AreaDescriptionPtr				areaDescriptionPtr,
				SInt16								fieldNumber)

{
	HPFieldIdentifiersPtr			fieldIdentPtr;
   HPFieldPointsPtr					fieldPointsPtr;

   SInt32								previousLineStart;

   UInt32								imageStartColumn,
											imageStartLine;

   SInt16								classStorage,
											pointIndex;

   Boolean								definedAreaFlag;


			// Initialize local variables.													

   fieldIdentPtr = &classProjectInfoPtr->fieldIdentPtr[fieldNumber];
   fieldPointsPtr = classProjectInfoPtr->fieldPointsPtr;
   definedAreaFlag = TRUE;

			// Get index of first point that describes the field.						

   pointIndex = fieldIdentPtr->firstPolygonPoint;

			// Initialize the mask info structure pointer to NULL.

   areaDescriptionPtr->maskInfoPtr = NULL;

			// Load the point type;

   areaDescriptionPtr->pointType = fieldIdentPtr->pointType;

			// If the line and column intervals are 0, then initialize them to 1.

   if (areaDescriptionPtr->lineInterval <= 0)
      areaDescriptionPtr->lineInterval = 1;

   if (areaDescriptionPtr->columnInterval <= 0)
      areaDescriptionPtr->columnInterval = 1;

			// Determine if field is described by a polygon or a rectangle.  If	
			// the field is described by a polygon then create a region for the	
			// field.																				

   if (fieldIdentPtr->pointType == kPolygonType) 
		{
      LongRect				boundingRectangle;
      RgnHandle			rgnHandle;


      areaDescriptionPtr->polygonFieldFlag = TRUE;

				// Allow for the first two points in the polygon point list to
				// represent the bounding rectangle.

      pointIndex += 2;

      CreateFieldRgn (fieldIdentPtr->numberOfPolygonPoints,
								&fieldPointsPtr[pointIndex],
								&rgnHandle,
								areaDescriptionPtr->columnOffset,
								areaDescriptionPtr->lineOffset);
		
      if (rgnHandle != NULL) 
			{
         GetBoundingRegionRectangle (rgnHandle,
													&boundingRectangle,
													(UInt32)fieldIdentPtr->numberOfPolygonPoints,
													&fieldPointsPtr[pointIndex],
													areaDescriptionPtr->columnOffset,
													areaDescriptionPtr->lineOffset);

         areaDescriptionPtr->lineStart = boundingRectangle.top;
         areaDescriptionPtr->lineEnd = boundingRectangle.bottom;

         areaDescriptionPtr->columnStart = boundingRectangle.left;
         areaDescriptionPtr->columnEnd = boundingRectangle.right;

					// If offset were applied, verify that the field area is still within 
					// image area. If not, set the flag indicating that the area is not 
					// defined.

         if (areaDescriptionPtr->applyOffsetFlag)
            definedAreaFlag = VerifyAreaDescription (areaDescriptionPtr);

         if (gMemoryError != noErr || !definedAreaFlag) 
				{
            DisposeRgn (rgnHandle);
            rgnHandle = NULL;
            definedAreaFlag = FALSE;

				}	// end "if (gMemoryError != noErr || !definedAreaFlag)"

         areaDescriptionPtr->rgnHandle = rgnHandle;

			}	// end "if (rgnHandle != NULL)"

		}	// end "if (fieldIdentPtr..." 

   else	// The field is not polygonal type 
		{
      areaDescriptionPtr->polygonFieldFlag = FALSE;
      areaDescriptionPtr->rgnHandle = NULL;

      if (fieldIdentPtr->pointType == kRectangleType) 
			{
         areaDescriptionPtr->lineStart = fieldPointsPtr[pointIndex].line;
         areaDescriptionPtr->lineEnd = fieldPointsPtr[pointIndex + 1].line;

         areaDescriptionPtr->columnStart = fieldPointsPtr[pointIndex].col;
         areaDescriptionPtr->columnEnd = fieldPointsPtr[pointIndex + 1].col;

         if (areaDescriptionPtr->applyOffsetFlag) 
				{
						// Adjust for any offset between this image and the project 
						// base image.

            areaDescriptionPtr->lineStart += areaDescriptionPtr->lineOffset;
            areaDescriptionPtr->lineEnd += areaDescriptionPtr->lineOffset;
            areaDescriptionPtr->columnStart += areaDescriptionPtr->columnOffset;
            areaDescriptionPtr->columnEnd += areaDescriptionPtr->columnOffset;

						// Now verify that the field area is still within image area.
						// If not, set the flag indicating that the area is not defined.

            definedAreaFlag = VerifyAreaDescription (areaDescriptionPtr);

				}	// end "if (areaDescriptionPtr->applyOffsetFlag)"

			}	// end "if (fieldIdentPtr->pointType == kRectangleType)" 

      else if (fieldIdentPtr->pointType == kMaskType) 
			{
					// Set the area to be checked as that defined by the mask.

					// Initialize the line and column end parameters to zero. This will 
					// force the output line and column parameters to be based on the 
					// size of the mask itself.

         areaDescriptionPtr->lineStart = 1;
         areaDescriptionPtr->lineEnd = 0;
         areaDescriptionPtr->columnStart = 1;
         areaDescriptionPtr->columnEnd = 0;

         if (areaDescriptionPtr->applyOffsetFlag) 
				{
						// Allow for offset in image which may be different than that
						// for the project base image.

            imageStartLine = (UInt32)(classProjectInfoPtr->startLine -
																	areaDescriptionPtr->lineOffset);

            imageStartColumn = (UInt32)(classProjectInfoPtr->startColumn -
																	areaDescriptionPtr->columnOffset);

				}	// end "if (areaDescriptionPtr->applyOffsetFlag"

         else	// !areaDescriptionPtr->applyOffsetFlag)
				{
						// Get line and column start values for the project base image.

            imageStartLine = classProjectInfoPtr->startLine;
            imageStartColumn = classProjectInfoPtr->startColumn;

				}	// end "else !areaDescriptionPtr->applyOffsetFlag"

					// Determine if any of the requested lines and columns are within
					// the area represented by the mask.

         definedAreaFlag = GetMaskArea (
												NULL,
												fieldIdentPtr->fieldType,
												classProjectInfoPtr,
												NULL,
												imageStartLine,
												imageStartColumn,
												areaDescriptionPtr->lineInterval,
												areaDescriptionPtr->columnInterval,
												(UInt32*)&areaDescriptionPtr->lineStart,
												(UInt32*)&areaDescriptionPtr->lineEnd,
												(UInt32*)&areaDescriptionPtr->columnStart,
												(UInt32*)&areaDescriptionPtr->columnEnd,
												(UInt32*)&areaDescriptionPtr->maskLineStart,
												(UInt32*)&areaDescriptionPtr->maskColumnStart);

         if (areaDescriptionPtr->applyOffsetFlag)
            definedAreaFlag = VerifyAreaDescription (areaDescriptionPtr);

					// Get the mask buffer and value-to-field vector if needed.

         if (definedAreaFlag) 
				{
            areaDescriptionPtr->maskInfoPtr = GetMaskInfoPointer (
																			fieldIdentPtr->fieldType,
																			classProjectInfoPtr,
																			NULL);

            UInt32 maskValueRequest = 0;
            if (areaDescriptionPtr->maskInfoPtr != NULL)
               maskValueRequest = (UInt32)fieldIdentPtr->maskValue;

            if (maskValueRequest != 0) 
					{
               areaDescriptionPtr->maskValueRequest = maskValueRequest;

               previousLineStart = areaDescriptionPtr->lineStart;

               areaDescriptionPtr->lineStart = GetFirstMaskLine (
																areaDescriptionPtr->maskInfoPtr,
																(UInt16)maskValueRequest,
																areaDescriptionPtr->maskLineStart,
																areaDescriptionPtr->maskColumnStart,
																areaDescriptionPtr->lineStart,
																areaDescriptionPtr->lineEnd,
																areaDescriptionPtr->lineInterval,
																areaDescriptionPtr->columnStart,
																areaDescriptionPtr->columnEnd,
																areaDescriptionPtr->columnInterval);

               if (areaDescriptionPtr->lineStart > 0) 
						{
								// Update the mask line start to reflect a possible change in
								// the image line start.

                  areaDescriptionPtr->maskLineStart +=
											areaDescriptionPtr->lineStart - previousLineStart;

						}	// end "if (areaDescriptionPtr->lineStart > 0)"

               else	// areaDescriptionPtr->lineStart == 0
                  maskValueRequest = 0;

					}	// end "if (maskValueRequest	!= 0)"

            if (maskValueRequest == 0) 
					{
               areaDescriptionPtr->maskInfoPtr = NULL;
               definedAreaFlag = FALSE;

					}	// end "if (maskValueRequest == 0)"

				}	// end "if (definedAreaFlag)"

			}	// end "if (fieldIdentPtr->pointType == kMaskType)" 

      else	// field is of clustertype 
			{
         definedAreaFlag = FALSE;

			}	// end "else field is of cluster type" 

		}	// end "else the field is not polygonal type" 

   if (!definedAreaFlag) 
		{
      areaDescriptionPtr->lineStart = 0;
      areaDescriptionPtr->lineEnd = 0;

      areaDescriptionPtr->columnStart = 0;
      areaDescriptionPtr->columnEnd = 0;

      areaDescriptionPtr->numberLines = 0;
      areaDescriptionPtr->numSamplesPerChan = 0;

      areaDescriptionPtr->pointType = kRectangleType;

		}	// end "if (!definedAreaFlag)" 

   classStorage = fieldIdentPtr->classStorage;
   areaDescriptionPtr->classNumber =
      classProjectInfoPtr->classNamesPtr[classStorage].classNumber;
   areaDescriptionPtr->fieldNumber = fieldNumber + 1;

   if (areaDescriptionPtr->lineStart > 0) 
		{
      areaDescriptionPtr->numberLines =
						areaDescriptionPtr->lineEnd - areaDescriptionPtr->lineStart + 1;

      if (areaDescriptionPtr->columnInterval > 0)
         areaDescriptionPtr->numSamplesPerChan =
						(areaDescriptionPtr->columnEnd - areaDescriptionPtr->columnStart +
								areaDescriptionPtr->columnInterval) /
										areaDescriptionPtr->columnInterval;

   }	// end "if (areaDescriptionPtr->lineStart > 0)" 

}	// end "GetFieldBoundary" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean GetListResultsFlag
//
//	Software purpose:	The purpose of this routine is to return the current
//							setting of the list flag for the input area and information
//							type.
//
//	Parameters in:		area code, list information code.		
//
//	Parameters out:	None
//
// Value Returned:	TRUE if information is to be listed and
//							FALSE if otherwise.			
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/08/1997
//	Revised By:			Larry L. Biehl			Date: 04/08/1997

Boolean GetListResultsFlag (
				SInt16								fieldTypeCode,
				SInt16								listInformationCode)

{
	Boolean								listFlag = FALSE;


   if (fieldTypeCode & kTrainingType &&
					gProjectInfoPtr->listResultsTrainingCode & listInformationCode)
      listFlag = TRUE;

   if (fieldTypeCode & kTestingType &&
						gProjectInfoPtr->listResultsTestCode & listInformationCode)
      listFlag = TRUE;

   return (listFlag);

}	// end "GetListResultsFlag" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt64 GetMaximumPixelsPerClass
//
//	Software purpose:	The purpose of this routine is to determine the number of
//							training and/or test pixels in the class in the input class
//							list that has the most pixels. The user can control
//							whether cluster fields are to be considered and the classes
//							to be considered that are using the input covariance code.
//								
//
//	Parameters in:		Class list pointer. If NULL, all project classes will be used.
//							The number of classes in the input class list.
//							The field type code to be considered - training and/or test
//								kTrainingType
//								kTestingType
//								kBothFieldTypes
//							The covariance code to be considered
//								kOriginalStats
//								kLeaveOneOutStats
//								kEnhancedStats
//							Flag indicating whether cluster fields are to be considered
//								kDontIncludeClusterFields or
//								kIncludeClusterFields	
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			UpdateProjectLOOStats in SLOOCovariance.cpp
//
//	Coded By:			Larry L. Biehl			Date: 09/17/1997
//	Revised By:			Larry L. Biehl			Date: 08/26/2010

SInt64 GetMaximumPixelsPerClass (
				UInt16*								classPtr,
				UInt32								numberClasses,
				SInt16								fieldTypeCode,
				UInt16								covarianceStatsToUse,
				Boolean								includeClusterFieldsFlag)

{
	SInt64								maxNumberPixelsPerClass,
											numberPixelsInClass;

   HPClassNamesPtr					classNamesPtr;
   HPFieldIdentifiersPtr			fieldIdentPtr;

   UInt32								classIndex,
											classNumber;

   SInt16								classStorage,
											fieldCount,
											fieldNumber;


			// Check parameters.																	

   if (gProjectInfoPtr == NULL)
																							return (0);

			// Initialize local variables.													

   classNamesPtr = gProjectInfoPtr->classNamesPtr;
   fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;
   fieldCount = 0;
   maxNumberPixelsPerClass = 0;

   if (classPtr == NULL)
      numberClasses = gProjectInfoPtr->numberStatisticsClasses;

   for (classIndex = 0; classIndex < numberClasses; classIndex++) 
		{
      if (classPtr != NULL)
         classNumber = (UInt32)classPtr[classIndex];

      else	// classPtr == NULL 
         classNumber = classIndex;

				// Get the class storage number and first field number for the class.												

      classStorage = gProjectInfoPtr->storageClass[classNumber];

				// Consider this class only if it satisfies the requested covariance
				// statistics to be used.

      if (classNamesPtr[classStorage].covarianceStatsToUse & covarianceStatsToUse) 
			{
         fieldNumber = classNamesPtr[classStorage].firstFieldNumber;

         numberPixelsInClass = 0;
         while (fieldNumber != -1) 
				{
						// Make certain that field is of the requested type (s).			

            if (fieldIdentPtr[fieldNumber].fieldType & fieldTypeCode) 
					{
               if (fieldIdentPtr[fieldNumber].pointType != kClusterType ||
							(includeClusterFieldsFlag &&
								fieldIdentPtr[fieldNumber].pointType == kClusterType &&
														fieldIdentPtr[fieldNumber].statsUpToDate))

								// Now add in the number of pixels for this field.

                  numberPixelsInClass += fieldIdentPtr[fieldNumber].numberPixels;

					}	// end "if (fieldIdentPtr[fieldNumber].fieldType ..." 

            fieldNumber = fieldIdentPtr[fieldNumber].nextField;

				}	// end "while (fieldNumber != -1)" 

         maxNumberPixelsPerClass = MAX (maxNumberPixelsPerClass, numberPixelsInClass);

			}	// end "if (...[classStorage].covarianceStatsToUse & covarianceStatsToUse)"

		}	// end "for (classIndex=0; ... 

   return (maxNumberPixelsPerClass);

}	// end "GetMaximumPixelsPerClass" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		short int GetNextFieldArea
//
//	Software purpose:	The purpose of this routine is to return the
//							field number for the next area in the list of
//							requested classes and fields to be used.
//
//	Parameters in:		Project information pointer
//							Class list pointer. If NULL, all project classes will be used.
//							The number of classes in the input class list.
//							Index of last class that was used
//							Index of last field that was used
//							The field type code to be considered - training and/or test
//								kTrainingType
//								kTestingType
//								kBothFieldTypes
//							Flag indicating whether cluster fields are to be considered
//								kDontIncludeClusterFields or
//								kIncludeClusterFields	
//
//	Parameters out:	None
//
// Value Returned:	-1 if there is no field area of the requested type
//								available.  Otherwise the next field number is returned
//								which is of the correct type and is not a cluster
//								field.			
// 
// Called By:			ClassifyDialog in SClassifyDialogs.cpp
//							ClusterDialog in SCluster.cpp
//							ClusterClassification in SCluster.cpp
//							GetNextClusterArea in SCluster.cpp
//							GetNumberOfClusterPixels in SCluster.cpp
//							ISODATAClusterDialog in SClusterIsodata.cpp
//							OnePassClusterDialog in SClusterSinglePass.cpp
//							GetTotalCovariance in SMatrixUtilities.cpp
//							CreateStatisticsImages in SStatisticsImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/08/1990
//	Revised By:			Larry L. Biehl			Date: 12/22/1998

SInt16 GetNextFieldArea (
				ProjectInfoPtr						projectClassInfoPtr,
				SInt16*								classPtr,
				UInt32								numberClasses,
				SInt16*								lastClassIndex,
				SInt16								lastField,
				SInt16								fieldTypeCode,
				Boolean								includeClusterFieldsFlag)

{
	SInt16								classStorage;

   HPFieldIdentifiersPtr			fieldIdentPtr;

   SInt16								classIndex,
											fieldNumber,
											nextFieldNumber;


			// Check parameters.																	

   if (projectClassInfoPtr == NULL || numberClasses <= 0)
																								return (-1);

			// Initialize local variables.													

   nextFieldNumber = -1;
   fieldNumber = lastField;
   fieldIdentPtr = projectClassInfoPtr->fieldIdentPtr;

   while (nextFieldNumber == -1 && *lastClassIndex < (SInt16)numberClasses) 
		{
      if (fieldNumber < 0) 
			{
         (*lastClassIndex)++;

         if (*lastClassIndex < (SInt16)numberClasses) 
				{
            if (classPtr != NULL)
               classIndex = classPtr[*lastClassIndex];

            else	// classPtr == NULL 
               classIndex = *lastClassIndex + 1;

            classStorage = projectClassInfoPtr->storageClass[classIndex - 1];
            fieldNumber =
               projectClassInfoPtr->classNamesPtr[classStorage].firstFieldNumber;

				}	// end "if (*lastClassIndex < numberClasses)" 

			}	// end "if (fieldNumber == -1)" 

      else	// fieldNumber >= 0 
         fieldNumber = fieldIdentPtr[fieldNumber].nextField;

				// Make certain that field is of the requested type(s).				

      if (fieldNumber >= 0 &&
								(fieldIdentPtr[fieldNumber].fieldType & fieldTypeCode)) 
			{
         if (fieldIdentPtr[fieldNumber].pointType != kClusterType ||
						(includeClusterFieldsFlag &&
							fieldIdentPtr[fieldNumber].pointType == kClusterType &&
								fieldIdentPtr[fieldNumber].statsUpToDate))
            nextFieldNumber = fieldNumber;

			}	// end "if (fieldNumber >= 0 && ..." 

		}	// end "while (fieldNumber == -1 && ...)" 

   return (nextFieldNumber);

}	// end "GetNextFieldArea" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 GetNumberOfAreas
//
//	Software purpose:	The purpose of this routine is to determine the total
//							number of areas that are specified by the class list
//							and field type.
//
//	Parameters in:		Class list pointer. If NULL, all project classes will be used.
//							The number of classes in the input class list.
//							The field type code to be considered - training and/or test
//								kTrainingType
//								kTestingType
//								kBothFieldTypes
//							Flag indicating whether cluster fields are to be considered
//								kDontIncludeClusterFields or
//								kIncludeClusterFields
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			LoadClassifySpecs in SClassify.cpp
//							InitializeClusterMemory in SCluster.cpp
//							GetTotalSumSquares in SMatrixUtilities.cpp
//							CreateStatisticsImages in SStatisticsImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/08/1990
//	Revised By:			Larry L. Biehl			Date: 12/28/1998

SInt16 GetNumberOfAreas (
				SInt16*								classPtr,
				UInt32								numberClasses,
				SInt16								fieldTypeCode,
				Boolean								includeClusterFieldsFlag)

{
   SInt16								classStorage,
											fieldCount;

   HPClassNamesPtr					classNamesPtr;
   HPFieldIdentifiersPtr			fieldIdentPtr;

   SInt16								classNumber,
											fieldNumber;

   UInt16								classIndex;


			// Check parameters.																	

   if (gProjectInfoPtr == NULL || numberClasses <= 0)
																								return (0);

			// Initialize local variables.													

   classNamesPtr = gProjectInfoPtr->classNamesPtr;
   fieldCount = 0;
   classNumber = 0;

   if (classPtr == NULL)
      numberClasses = gProjectInfoPtr->numberStatisticsClasses;

   for (classIndex = 0; classIndex < numberClasses; classIndex++) 
		{
      if (classPtr != NULL)
         classNumber = classPtr[classIndex];

      else	// classPtr == NULL 
         classNumber++;

				// Get the class storage number.												

      classStorage = gProjectInfoPtr->storageClass[classNumber - 1];

      fieldNumber = classNamesPtr[classStorage].firstFieldNumber;

      while (fieldNumber != -1) 
			{
         fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];

					// Make certain that field is of the requested type(s).			

         if (fieldIdentPtr->fieldType & fieldTypeCode) 
				{
            if (fieldIdentPtr->pointType != kClusterType ||
						(includeClusterFieldsFlag &&
											fieldIdentPtr->pointType == kClusterType &&
																	fieldIdentPtr->statsUpToDate))
               fieldCount++;

				}	// end "if (fieldIdentPtr->fieldType ..." 

         fieldNumber = fieldIdentPtr->nextField;

			}	// end "while (fieldNumber != -1)" 

		}	// end "for (classIndex=0; ... 

   return (fieldCount);

}	// end "GetNumberOfAreas" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		UInt32 GetNumberOfCombinations
//
//	Software purpose:	The purpose of this routine is to determine the
//							number of combinations of "m" groups with x contiguous
//							items per group out of a total of "n" items.
//									
//							NumberOfCombinations =  (n-m(x-1))!/[m! (n-m(x-1)-m)!]
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:	
//
//	Coded By:			C.H. Lee					Date: 12/04/1988
//	Revised By:			Larry L. Biehl			Date: 05/05/1998	

UInt32 GetNumberOfCombinations (
				SInt32								total,
				SInt32								subset,
				SInt32								numberContiguousPerGroup,
				Boolean								stepSearchFlag,
				SInt16*								returnCodePtr)

{
			// Declare local variables and structures										

   double								numberCombinations;

   SInt32								adjustedTotal,
											endLoop,
											i;


   numberCombinations = 0;

   if (!stepSearchFlag) 
		{
      adjustedTotal = total - subset * (numberContiguousPerGroup - 1);

      if (adjustedTotal >= subset &&
							subset > 0 && numberContiguousPerGroup > 0) 
			{
         numberCombinations = 1;
         endLoop = MAX (subset, adjustedTotal - subset);

         for (i = adjustedTotal; i > endLoop; i--) 
				{
            numberCombinations *= i;
            if (numberCombinations > FLT_MAX) 
					{
               *returnCodePtr = -2;
																							return (0);

					}	// end "if (numberCombinations > FLT_MAX)"

				}	// end "for (i=total; i>endLoop; i--)" 

         endLoop = adjustedTotal - endLoop;
         for (i = 1; i <= endLoop; i++)
            numberCombinations /= i;

			}	// end "if (adjustedTotal >= subset &&..." 

		}	// end "if (!stepSearchFlag)" 

   else	// stepSearchFlag 
		{
      if (total >= subset) 
			{
         numberCombinations =
            2 * (double) (total + 1) - numberContiguousPerGroup * (subset + 1);
         numberCombinations *= subset;
         numberCombinations /= 2;

			}	// end "if (total >= subset)"

		}	// end "else stepSearchFlag" 

   if (numberCombinations > gMaxNumberChannelCombinations) 
		{
      *returnCodePtr = -2;
																								return (0);

		}	// end "if (numberCombinations > FLT_MAX)"

   else	// numberCombinations <= gMaxNumberChannelCombinations
		{
      *returnCodePtr = 0;
																return ((UInt32)numberCombinations);

		}	// end "else numberCombinations <= gMaxNumberChannelCombinations"

}	// end "GetNumberOfCombinations"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetProjectFieldsBoundingArea
//
//	Software purpose:	The purpose of this routine is to determine the bounding area
//							of the requested project fields
//
//	Parameters in:		fieldTypeCode: code indicating which field types are to be
//								considered.
//							classPtr: vector of the classes to be used
//							numberClasses: number of classes in the class vector
//							areaLineStart: line start for the user requested area
//							areaLineEnd: line end for the user requested area
//							areaColumnStart: column start for the user requested area
//							areaColumnEnd: column end for the user requested area
//
//	Parameters out:	longRectPtr: bounding area in the image which includes all
//								requested fields and/or user requested area
//
// Value Returned:	= 0, No error
//							> 0, Error determining the bounding area for the requested
//									project fields.
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 01/14/2003
//	Revised By:			Larry L. Biehl			Date: 12/31/2003

SInt16 GetProjectFieldsBoundingArea (
				SInt16								fieldTypeCode,
				SInt16*								classPtr,
				UInt32								numberClasses,
				SInt32								areaLineStart,
				SInt32								areaLineEnd,
				SInt32								areaColumnStart,
				SInt32								areaColumnEnd,
				LongRect*							longRectPtr)

{
	AreaDescription					areaDescription;

   SInt32								areaNumber;

   SInt16								fieldNumber,
											lastClassIndex,
											lastFieldIndex,
											returnCode;


			// Check parameters.	

   returnCode = 1;

   if (fieldTypeCode < kTrainingType ||
						fieldTypeCode > kTrainingType + kTestingType + kAreaType)
																						return (returnCode);

   if (fieldTypeCode & kAreaType) 
		{
				// Just use the input settings.

      longRectPtr->top = areaLineStart;
      longRectPtr->bottom = areaLineEnd;
      longRectPtr->left = areaColumnStart;
      longRectPtr->right = areaColumnEnd;

		}	// end "if (fieldTypeCode & kAreaType)"

   else	//  !(fieldTypeCode & kAreaType)
		{
				// Initialize in case needed for training and/or test fields.

      longRectPtr->top = SInt32_MAX;
      longRectPtr->bottom = 0;
      longRectPtr->left = SInt32_MAX;
      longRectPtr->right = 0;

		}	// end "if (fieldTypeCode & kAreaType)"

   if (fieldTypeCode & (kTrainingType + kTestingType)) 
		{
      if (gProjectInfoPtr == NULL || numberClasses <= 0)
																						return (returnCode);

      InitializeAreaDescription (&areaDescription);

      lastClassIndex = -1;
      lastFieldIndex = -1;

				// Loop by number of areas.													

      for (areaNumber = 0; areaNumber < kMaxNumberStatFields; areaNumber++) 
			{
					// Get the next field number.												

         fieldNumber = GetNextFieldArea (gProjectInfoPtr,
														classPtr,
														numberClasses,
														&lastClassIndex,
														lastFieldIndex,
														fieldTypeCode,
														kDontIncludeClusterFields);

         if (fieldNumber < 0)
            break;

         lastFieldIndex = fieldNumber;

					// Get the field coordinates.				

         GetFieldBoundary (gProjectInfoPtr, &areaDescription, fieldNumber);

         longRectPtr->top = MIN (longRectPtr->top, areaDescription.lineStart);
         longRectPtr->bottom = MAX (longRectPtr->bottom, areaDescription.lineEnd);
         longRectPtr->left = MIN (longRectPtr->left, areaDescription.columnStart);
         longRectPtr->right = MAX (longRectPtr->right, areaDescription.columnEnd);

					// Dispose of the region if needed.		

         CloseUpAreaDescription (&areaDescription);

			}	// end "for (areaNumber=0; ... 

		}	// end "if (fieldTypeCode & (kTrainingType+kTrainingType))" 


   if (longRectPtr->top <= longRectPtr->bottom ||
													longRectPtr->left <= longRectPtr->right)
      returnCode = 0;

   return (returnCode);

}	// end "GetProjectFieldsBoundingArea"	 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:				short int GetProjectClassWeightsIndex
//
//	Software purpose:	The purpose of this routine is to get the index
//							to the appropriate project class weights 
//							depending upon whether original statistics or 
//							enhanced statistics and equal or unequal weighting
//							are to be used.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:  
//
//	Calls To:				
//
// Called By:			ClassifyDialog in SClassifyDialogs.cpp
//							FeatureExtractionDialog in SFeatureExtraction.cpp
//							LoadClassWtVector in SProjectUtilities.cpp
//							StatisticsEnhanceDialog in SStatisticsEnhancement.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1993
//	Revised By:			Larry L. Biehl			Date: 01/06/1994

SInt16 GetProjectClassWeightsIndex (void)

{
   SInt16								weightsIndex;
   Boolean								equalWeightsFlag;


   if ((gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats &&
			gProjectInfoPtr->modifiedClassWeightSet == kUnequalWeights) ||
						(gProjectInfoPtr->covarianceStatsToUse != kEnhancedStats &&
										gProjectInfoPtr->classWeightSet == kUnequalWeights))
      equalWeightsFlag = FALSE;

   else	// equal weights to be used. 
      equalWeightsFlag = TRUE;

   weightsIndex = GetClassWeightsIndex (
									gProjectInfoPtr->covarianceStatsToUse == kEnhancedStats,
									equalWeightsFlag);

   return (weightsIndex);

}	// end "GetProjectClassWeightsIndex" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetProjectWindowTitle
//
//	Software purpose:	The purpose of this routine is to get the
//							title of the project window.
//
//	Parameters in:		
//
//	Parameters out:	None.
//
//	Value Returned:	None 				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 12/14/2000
//	Revised By:			Larry L. Biehl			Date: 05/06/2017 

void GetProjectWindowTitle (
				UCharPtr								titleStringPtr)

{
	if (gProjectWindow != NULL)
		{
		#if defined multispec_mac  	   
			GetWTitle (gProjectWindow, titleStringPtr);
		#endif	// defined multispec_mac 

		#if defined multispec_win
			USES_CONVERSION;

			CMStatisticsDoc* documentCPtr =
									((CMStatisticsForm*)gProjectWindow)->GetDocument ();
			CString titleString = documentCPtr->GetTitle ();

			UInt16 titleLength = titleString.GetLength ();
			LPTSTR titleBufferPtr = titleString.GetBuffer (titleLength);
		
				// Convert unicode to char string
			strcpy ((char*)&titleStringPtr[1], T2A (titleBufferPtr));
			titleString.ReleaseBuffer ();

					// Move the window title making sure that there are no
					// spaces in the name.

			UInt16		inIndex,
							outIndex = 1;

			for (inIndex=1; inIndex<=titleLength; inIndex++)
				{
				if (titleStringPtr[inIndex] != ' ')
					{
					titleStringPtr[outIndex] = (UChar)titleStringPtr[inIndex];
					outIndex++;

					}	// end "if (titleBufferPtr[inIndex] != ' ')"

				}	// end "for (inIndex=0; inIndex<titleLength; inIndex++)"

					// Make the string a pascal string and terminate it with as
					// if it were a c string.

			titleStringPtr[0] = (UInt8) (outIndex - 1);
			titleStringPtr[outIndex] = 0;
		#endif	// defined multispec_win 

		}	// end "if (gProjectWindow != NULL)"

	else
		titleStringPtr[0] = sprintf ((char*)&titleStringPtr[1], "New Project");

}	// end "GetProjectWindowTitle" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void GetBoundingRegionRectangle
//
//	Software purpose:	The purpose of this routine is to get the bounding region
//							rectangle from the input region handle.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:		
//
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/08/1998
//	Revised By:			Larry L. Biehl			Date: 11/05/2015

void GetBoundingRegionRectangle (
				RgnHandle							rgnHandle,
				LongRect*							boundingRectPtr,
				UInt32								numberPoints,
				HPFieldPointsPtr					fieldPointsPtr,
				SInt32								columnOffset,
				SInt32								lineOffset)

{
	#if defined multispec_mac  
		UInt32 i;


		//boundingRectPtr->top = (*rgnHandle)->rgnBBox.top;
		//boundingRectPtr->bottom = (*rgnHandle)->rgnBBox.bottom;

		//boundingRectPtr->left = (*rgnHandle)->rgnBBox.left;
		//boundingRectPtr->right = (*rgnHandle)->rgnBBox.right;

		GetRegionBounds (rgnHandle, &gTempRect);

		boundingRectPtr->top = gTempRect.top;
		boundingRectPtr->bottom = gTempRect.bottom;
		boundingRectPtr->left = gTempRect.left;
		boundingRectPtr->right = gTempRect.right;

		if (boundingRectPtr->left == 0) 
			{
					// The region is empty so rgnBBox is not correct. Go through
					// the points to get the bounding rectangle.

			boundingRectPtr->top = SInt32_MAX;
			boundingRectPtr->left = SInt32_MAX;
			for (i=0; i<numberPoints; i++) 
				{
				boundingRectPtr->left = MIN (boundingRectPtr->left,
														fieldPointsPtr[i].col + columnOffset);
				boundingRectPtr->top = MIN (boundingRectPtr->top,
														fieldPointsPtr[i].line + lineOffset);

				boundingRectPtr->right = MAX (boundingRectPtr->right,
														fieldPointsPtr[i].col + columnOffset);
				boundingRectPtr->bottom = MAX (boundingRectPtr->bottom,
														fieldPointsPtr[i].line + lineOffset);

				}	// end "for (i=0; i<numberPoints; i++)"

			}	// end "if (boundingRectPtr->left == 0)"
	#endif	// defined multispec_mac

	#if defined multispec_win
		Rect		rectangle;


		SInt16 returnValue = rgnHandle->GetRgnBox ((tagRECT*)&rectangle);

		if (returnValue == SIMPLEREGION || returnValue == COMPLEXREGION) 
			{
			boundingRectPtr->top = rectangle.top;
			boundingRectPtr->bottom = rectangle.bottom;

			boundingRectPtr->left = rectangle.left;
			boundingRectPtr->right = rectangle.right;

			}	// end "if (returnValue == SIMPLEREGION || ..."

		else	// returnValue != SIMPLEREGION && ...
			{
			rgnHandle->DeleteObject ();
			rgnHandle = NULL;

			}	// end "else returnValue != SIMPLEREGION && ..."		
	#endif	// defined multispec_win
	
	#if defined multispec_wx
		wxRect rectangle = rgnHandle->GetBox ();

		if (rectangle.width > 0 && rectangle.height > 0) 
			{
			boundingRectPtr->left = rectangle.x;
			boundingRectPtr->right = rectangle.x + rectangle.width;	
	
			boundingRectPtr->top = rectangle.y;
			boundingRectPtr->bottom = rectangle.y + rectangle.height;
			
					// in wxWidgets, the bounding rectangle seems to leave out
					// the first left and top values. Subtract one off here.
					
			boundingRectPtr->left = MAX (rectangle.x-1, 1);
			boundingRectPtr->top = MAX (rectangle.y-1, 1);

			}	// end "if (rectangle.width != 0 & rectangle.height != 0"
			
		else	// rectangle.width == 0 || rectangle.height == 0
			{
					// The region is empty so rgnHandle is not correct. Go through
					// the points to get the bounding rectangle.
					
			UInt32 i;

			boundingRectPtr->top = SInt32_MAX;
			boundingRectPtr->left = SInt32_MAX;
			for (i=0; i<numberPoints; i++) 
				{
				boundingRectPtr->left = MIN (boundingRectPtr->left,
														fieldPointsPtr[i].col + columnOffset);
				boundingRectPtr->top = MIN (boundingRectPtr->top,
														fieldPointsPtr[i].line + lineOffset);

				boundingRectPtr->right = MAX (boundingRectPtr->right,
														fieldPointsPtr[i].col + columnOffset);
				boundingRectPtr->bottom = MAX (boundingRectPtr->bottom,
														fieldPointsPtr[i].line + lineOffset);

				}	// end "for (i=0; i<numberPoints; i++)"
				
			}	// end "else rectangle.width == 0 || rectangle.height == 0"
	#endif	// defined multispec_wx					
	
}	// end "GetBoundingRegionRectangle"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		float* GetTempClassWeightsPtr
//
//	Software purpose:	The purpose of this routine is to get a pointer
//							to memory to use for a temporary class weight vector
//							for unequal weights for original and enhanced statistics.
//							Also the current project unequal class weights are
//							copied to the temporary vector.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:		
//
// Called By:			ClassifyDialog in SClassifyDialogs.cpp
//							FeatureExtractionDialog in SFeatureExtraction.cpp
//							StatisticsEnhanceDialog in SStatisticsEnhancement.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1993
//	Revised By:			Larry L. Biehl			Date: 01/06/1994

float* GetTempClassWeightsPtr (void)

{
   HPClassNamesPtr					classNamesPtr;
   float*								classWeightsPtr;

   SInt32								bytesNeeded;

   SInt16								index,
											numberClasses,
											classStorage;


			// Get memory for the temporary class weights vector.					

   numberClasses = gProjectInfoPtr->numberStatisticsClasses;

   bytesNeeded = (SInt32)2 * numberClasses * sizeof (float);
   classWeightsPtr = (float*)MNewPointer (bytesNeeded);

   if (classWeightsPtr) 
		{
				// Initialize the temporary class weights.							

      classNamesPtr = gProjectInfoPtr->classNamesPtr;

      for (index=0; index<numberClasses; index++) 
			{
         classStorage = gProjectInfoPtr->storageClass[index];

         classWeightsPtr[index] =
            classNamesPtr[classStorage].priorWeights[0];

         classWeightsPtr[index + numberClasses] =
            classNamesPtr[classStorage].priorWeights[1];

			}	// end "for (index=0; index<numberClasses; index++)" 

		}	// end "if (classWeightsPtr)" 

   return (classWeightsPtr);

}	// end "GetTempClassWeightsPtr"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double GetTotalProbability
//
//	Software purpose:	The purpose of this routine is to determine the
//							the total of the probability values for the classes
//							being used.
//
//	Parameters in:					
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/06/1994
//	Revised By:			Larry L. Biehl			Date: 09/19/1997	

double GetTotalProbability (
				UInt16*								classPtr,
				UInt32								numberClasses,
				UInt32								weightsIndex)

{
	double								totalProbability;

   float*								classWeightsPtr;

   UInt32								classStorage,
											index,
											statClassNumber;


   totalProbability = 0.;

   for (index=0; index<numberClasses; index++) 
		{
      if (classPtr != NULL)
         statClassNumber = classPtr[index] - 1;

      else	// classPtr == NULL
         statClassNumber = index;

      classStorage = gProjectInfoPtr->storageClass[statClassNumber];

      if (weightsIndex <= 3) 
			{
         classWeightsPtr =
            gProjectInfoPtr->classNamesPtr[classStorage].priorWeights;

         totalProbability +=
            fabs ((double) classWeightsPtr[weightsIndex]);

			}	// end "if (weightsIndex <= 3)"

      else	// weightsIndex <== 4
         totalProbability +=
					gProjectInfoPtr->classNamesPtr[classStorage].numberStatisticsPixels;

		}	// end "for (index=0; index<numberClasses; index++)" 

   return (totalProbability);

}	// end "GetTotalProbability" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void InitializeGlobalAlertVariables
//
//	Software purpose:	The purpose of this routine is to initialize the global
//							variables that are used for the alert that asks the
//							user whether to stop and application at once or
//							complete the current iteration.
//
//	Parameters in:		.
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/07/1994
//	Revised By:			Larry L. Biehl			Date: 01/07/1994

void InitializeGlobalAlertVariables (
				SInt16								alertId,
				SInt16								alertStrID,
				SInt16								alertStringNumber)

{
   gAlertId = alertId;
   gAlertStrID = alertStrID;
   gAlertStringNumber = alertStringNumber;
   gAlertReturnCode = 0;

}	// end "InitializeGlobalAlertVariables"  



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void Intg_Normal
//
//	Software purpose:	The purpose of this routine is to compute the 
//							cummulative distribution function value for
//							the Chi-distribution function for the specified
//							degrees of freedom and r value. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:	
//
//	Coded By:			Chulhee Lee				Date: ?
//	Revised By:			Larry L. Biehl			Date: 03/12/1990	

void Intg_Normal (
				SInt16								degreesOfFreedom,
				double								r,
				double*								returnPtr,
				double								factor,
				double								oneOverGammaOfHalfDF)

{
   double								expMinusHalfRSquared;


			// Note that if r is greater than about 45 for the PowerPC architecture
			// then exp (-r*r/2) will underflow. This should not happen as long
			// as direct calculation is not done for degrees of freedom greater than
			// 30.

   expMinusHalfRSquared = exp (-r * r / 2);

   *returnPtr = Intg_Normal_2 (
					degreesOfFreedom - 1, r, oneOverGammaOfHalfDF, expMinusHalfRSquared);

   *returnPtr *= factor;

}	// end "Intg_Normal" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		double Intg_Normal_2
//
//	Software purpose:	This routine is called by Intg_Normal to compute the 
//							cummulative distribution function value for
//							the Chi-distribution function for the specified
//							degrees of freedom and r value.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			Intg_Normal
//
//	Coded By:			Chulhee Lee				Date: ?
//	Revised By:			Larry L. Biehl			Date: 03/12/1990	

double Intg_Normal_2 (
				SInt16								degreesOfFreedom,
				double								r,
				double								input,
				double								expMinusHalfRSquared)

{
   double								x,
											returnValue;


   if (degreesOfFreedom == 0) 
		{
      Area_Of_SND_by_Direct_Calculation (r, &x);

      returnValue = x * kSQRT2PI;

		}	// end "if (degreesOfFreedom == 0)" 

   else	// degreesOfFreedom != 0 
		{
      if (degreesOfFreedom == 1)
         returnValue = 1 - expMinusHalfRSquared;

      else	// degreesOfFreedom > 1 
			{
         returnValue = Intg_Normal_2 (
            degreesOfFreedom - 2, r, input, expMinusHalfRSquared);
         returnValue *= (degreesOfFreedom - 1);
         returnValue -=
						pow (r, (double) (degreesOfFreedom - 1)) * expMinusHalfRSquared;

			}	// end "else degreesOfFreedom > 1" 

		}	// end "else degreesOfFreedom != 0" 

   return (returnValue);

}	// end "Intg_Normal_2" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean IsClassData
//
//	Software purpose:	The purpose of this routine is to check if the input pixel is 
//							part of the given class
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			IsProjectData in SStatisticsEnhancement.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/26/1996
//	Revised By:			Larry L. Biehl			Date: 11/08/1999	

Boolean IsClassData (
				AreaDescriptionPtr				projectAreaDescriptionPtr,
				UInt32								classNumber,
				UInt32								pixelLine,
				UInt32								pixelColumn)

{
   HPClassNamesPtr					classNamesPtr;
   HPFieldIdentifiersPtr			fieldIdentPtr;

   SInt16								classStorage,
											fieldNumber;


			// Initialize local variables.													

   classNamesPtr = gProjectInfoPtr->classNamesPtr;
   fieldIdentPtr = gProjectInfoPtr->fieldIdentPtr;

			// Get the class storage number.													

   classStorage = gProjectInfoPtr->storageClass[classNumber];

			// Continue only if the number of fields in the class is one or more.

   if (classNamesPtr[classStorage].numberOfTrainFields > 0) 
		{
      fieldNumber = classNamesPtr[classStorage].firstFieldNumber;

      while (fieldNumber != -1) 
			{
					// Make certain that field is training field.						

         if (fieldIdentPtr[fieldNumber].fieldType & kTrainingType) 
				{
						// List the requested data for the field.							

            if (IsFieldData (projectAreaDescriptionPtr,
										fieldNumber,
										pixelLine,
										pixelColumn))
																							return (TRUE);

				}	// end "if (fieldIdentPtr[fieldNumber].field..." 

         fieldNumber = fieldIdentPtr[fieldNumber].nextField;

			}	// end "while (fieldNumber != -1)" 

		}	// end "if (classNamesPtr[classStorage].number ..." 

			// Indicate that the input point is not part of the given class.							

   return (FALSE);

}	// end "IsClassData" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean IsFieldData ()
//
//	Software purpose:	The purpose of this routine is to check if the input pixel is 
//							part of the given field.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			IsClassData in SStatisticsEnhancement.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/26/1996
//	Revised By:			Larry L. Biehl			Date: 11/08/1999	

Boolean IsFieldData (
				AreaDescriptionPtr				projectAreaDescriptionPtr,
				SInt16								fieldNumber,
				UInt32								pixelLine,
				UInt32								pixelColumn)

{
   Point									point;

   RgnHandle							rgnHandle;

   SInt16								pointType;

   Boolean								returnFlag;


			// Make certain that input values make sense									

   if (fieldNumber <= -1)
																						return (FALSE);
   if (gProjectInfoPtr != NULL &&
					fieldNumber >= gProjectInfoPtr->numberStorageFields)
																						return (FALSE);

			// Exit if this is a cluster type field.										

   pointType = gProjectInfoPtr->fieldIdentPtr[fieldNumber].pointType;
   if (pointType >= kClusterType)
																						return (FALSE);

			// Initialize local variables.													

   returnFlag = FALSE;

			// Determine if field is described by a polygon or a rectangle.  If	
			// the field is described by a polygon then create a region for the	
			// field.			

   GetFieldBoundary (gProjectInfoPtr, projectAreaDescriptionPtr, fieldNumber);

   if (pointType != kPolygonType) 
		{
      if (pixelLine >= (UInt32)projectAreaDescriptionPtr->lineStart &&
				pixelLine <= (UInt32)projectAreaDescriptionPtr->lineEnd &&
					pixelColumn >= (UInt32)projectAreaDescriptionPtr->columnStart &&
						pixelColumn <= (UInt32)projectAreaDescriptionPtr->columnEnd)
			returnFlag = TRUE;

		}	// end "if (pointType != kPolygonType)"

   else	// pointType == kPolygonType
		{
      point.v = (SInt16)pixelLine;
      point.h = (SInt16)pixelColumn;

      rgnHandle = projectAreaDescriptionPtr->rgnHandle;

				// Check if point is within the region.										

      if (PtInRgn (point, rgnHandle))
         returnFlag = TRUE;

      CloseUpAreaDescription (projectAreaDescriptionPtr);

		}	// end "else pointType == kPolygonType" 

   return (returnFlag);

}	// end "IsFieldData" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean IsProjectData ()
//
//	Software purpose:	The purpose of this routine is to check if the input
//							pixel is part of the given project.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:			GetFieldDataValues in SProjectMatrixUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/26/1996
//	Revised By:			Larry L. Biehl			Date: 11/08/1999	

Boolean IsProjectData (
				AreaDescriptionPtr				maskAreaDescriptionPtr,
				AreaDescriptionPtr				projectAreaDescriptionPtr,
				UInt32								pixelLine,
				UInt32								pixelColumn)

{
   HPClassNamesPtr					classNamesPtr;
   HPFieldIdentifiersPtr			fieldIdentPtr;

   UInt32								classNumber,
											numberClasses,
											offset;

   SInt16								classStorage,
											fieldNumber;

   UInt16								maskValue;


			// Initialize local variables.													

   numberClasses = gProjectInfoPtr->numberStatisticsClasses;

			// Continue only if number of classes is one or more.						

   if (numberClasses > 0) 
		{
      if (maskAreaDescriptionPtr->maskInfoPtr != NULL) 
			{
					// First check if pixel is part of the mask area.

         if (pixelLine >= (UInt32)maskAreaDescriptionPtr->lineStart &&
					pixelLine <= (UInt32)maskAreaDescriptionPtr->lineEnd &&
						pixelColumn >= (UInt32)maskAreaDescriptionPtr->columnStart &&
							pixelColumn <= (UInt32)maskAreaDescriptionPtr->columnEnd)
				{
						// Get the line and column offset into the mask buffer vector.

            offset = (pixelLine - maskAreaDescriptionPtr->lineStart) *
									(maskAreaDescriptionPtr->maskInfoPtr->numberColumns + 1);

            offset += pixelColumn - maskAreaDescriptionPtr->columnStart +
														maskAreaDescriptionPtr->maskColumnStart;

            maskValue = maskAreaDescriptionPtr->maskInfoPtr->maskPointer[offset];

						// Check if point belongs to the class.								

            classNamesPtr = gProjectInfoPtr->classNamesPtr;

            for (classNumber=0; classNumber<numberClasses; classNumber++) 
					{
							// Get the class storage number.													

               classStorage = gProjectInfoPtr->storageClass[classNumber];

							// Continue only if the number of fields in the class is one 
							// or more.

               if (classNamesPtr[classStorage].numberOfTrainFields > 0) 
						{
                  fieldNumber = classNamesPtr[classStorage].firstFieldNumber;

                  while (fieldNumber != -1) 
							{
                     fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];

									// Make certain that field is training field.						

                     if ((fieldIdentPtr->fieldType & kTrainingType) &&
														(fieldIdentPtr->pointType == kMaskType)) 
								{
                        if (maskValue == fieldIdentPtr->maskValue)
																							return (TRUE);

								}	// end "if (fieldIdentPtr[fieldNumber].field..." 

                     fieldNumber = fieldIdentPtr->nextField;

							}	// end "while (fieldNumber != -1)" 

						}	// end "if (classNamesPtr[classStorage].number ..." 

					}	// end "for (classNumber=0; ... 

				}	// end "if (pixelLine >= maskAreaPtr->lineStart && ..."

			}	// end "if (maskAreaDescription.maskInfoPtr != NULL)"

				// Now check if pixels is part of any rectangular or polygon
				// training areas.

      for (classNumber = 0; classNumber < numberClasses; classNumber++) 
			{
					// Check if point belongs to the class.											

         if (IsClassData (projectAreaDescriptionPtr,
									classNumber,
									pixelLine,
									pixelColumn))
																							return (TRUE);

			}	// end "for (classNumber=0; ... 

		}	// end "if (numberClasses > 0)" 

			// Indicate that the input point does not belong to the
			// training statistics.									

   return (FALSE);

}	// end "IsProjectData" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListClassesUsed
//
//	Software purpose:	The purpose of this routine is to list the classes
//							used for the processor.  The output will be listed
//							in the output text window and/or the outputdisk file 
//							as specified.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ClassifyAreasControl in SClassify.cpp
//							ListDataControl in SListData.cpp
//							ListResultsControl in SListResults.cpp
//							PrincipalComponentControl in SPrincipalComponents.cpp
//							SeparabilityControl in SFeatureSelection.cpp
//							HistogramProjectStats in SProjectHistogramStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/09/1989
//	Revised By:			Larry L. Biehl			Date: 02/24/2000	

Boolean ListClassesUsed (
				ClassifierVar*						clsfyVariablePtr,
				FileInfoPtr							fileInfoPtr,
				SInt16*								classPtr,
				unsigned char*						symbolsPtr,
				SInt16								weightsIndex,
				UInt32								numberClasses,
				CMFileStream*						outputFilePtr,
				SInt16*								outputCodePtr,
				Boolean								thresholdFlag)

{
			// Declare local variables and structures	

	double								mixingParameter;

   char									*groupNamePtr,
											*stringPtr;

   HPClassNamesPtr					classNamesPtr;

   SInt16*								classToGroupPtr;

   SInt32								stringLength;

   UInt32								index;

   SInt16								backgroundIndexOffset,
											classStorage,
											groupIndex,
											stringNumber,
											strLength;

   UInt16								classNumber;

   Boolean								continueFlag,
											groupOrderedFlag,
											listGroupColumnFlag;


			// Initialize local variables.													

   continueFlag = TRUE;
   listGroupColumnFlag = FALSE;
   backgroundIndexOffset = 0;
   stringNumber = IDS_Project2;

			// Do setup for variables that are associated with the List	Results	
			// processor.																			

   if (gProcessorCode == kListResultsProcessor) 
		{
      if (clsfyVariablePtr != NULL)
         backgroundIndexOffset = clsfyVariablePtr->backgroundIndexOffset;

      listGroupColumnFlag = (clsfyVariablePtr->summaryCode & kGroupSummary) ||
													(clsfyVariablePtr->tableType & kGroupTable);

      groupOrderedFlag = (clsfyVariablePtr->tableType & kGroupTable);

      if (groupOrderedFlag || listGroupColumnFlag) 
			{
         classToGroupPtr = GetClassToGroupPointer (fileInfoPtr);

					// Get a pointer to the group names information.					

         groupNamePtr = (char*)GetHandlePointer (fileInfoPtr->groupNameHandle);

			}	// end "if (groupOrderedFlag || listGroupColumnFlag)" 

				// Get pointer to the properly ordered list of classes if 		
				// this is a group type table.											
		/*
      if (groupOrderedFlag)
      	{
      	if (classPtr != NULL && (clsfyVariablePtr->tableType & kGroupTable))
      		classPtr = &classPtr [gProjectInfoPtr->numberStatisticsClasses];
		 
      	}	// end "if (groupOrderedFlag)"
		*/
      stringNumber = IDS_Project1;

		}	// end "if (gProcessorCode == kListResultsProcessor)" 

			// Get the basic string to use.													
			// Classes used:  or  Project Classes used:									

   MGetString (gTextString, kProjectStrID, stringNumber);

   stringLength = gTextString[0];
   stringPtr = (char*)&gTextString[stringLength + 1];

			// Add group column if appropriate.												

   if (listGroupColumnFlag) 
		{
      sprintf (stringPtr, "\t   Group                       ");
      stringPtr += 32;

		}	// end "groupOrderedFlag" 

			// Add symbol column if appropriate.											

   if (symbolsPtr != NULL) 
		{
      sprintf (stringPtr, "\tSymbol");
      stringPtr += 7;

		}	// end "if (symbolsPtr != NULL)" 

			// Add weight column if appropriate.											

   if (weightsIndex >= 0) 
		{
      sprintf (stringPtr, "\t Weight");
      stringPtr += 8;

		}	// end "if (weightsIndex >= 0)" 

			// Add weight column if appropriate.											

   if (gProjectInfoPtr->covarianceStatsToUse == kLeaveOneOutStats ||
										gProjectInfoPtr->covarianceStatsToUse == kMixedStats)
		{
      if (gProjectInfoPtr->mixingParameterCode == kComputedOptimum)
         sprintf (stringPtr, "\t LOOC ");

      else if (gProjectInfoPtr->mixingParameterCode == kUserSet)
         sprintf (stringPtr, "\t User ");

      if (gProjectInfoPtr->mixingParameterCode != kIdentityMatrix) 
			{
         stringPtr += 7;
         sprintf (stringPtr, "Mixing Parameter");

         stringPtr += 16;

			}	// end "if (...->mixingParameterCode != kIdentityMatrix)"

		}	// end "if (...->covarianceStatsToUse == kLeaveOneOutStats || ..." 

   sprintf (stringPtr, "%s", gEndOfLine);

   stringLength =
				(Ptr)stringPtr - (Ptr)&gTextString + gNumberOfEndOfLineCharacters - 1;
   continueFlag = OutputString (outputFilePtr,
											(char*)&gTextString[1],
											stringLength,
											*outputCodePtr,
											continueFlag);

			// List line for threshold class if needed.									

   if (thresholdFlag) 
		{
      sprintf ((char*)gTextString,
					"       0: Threshold (background)         %s",
					gEndOfLine);

				// Add the symbol if available.												

      if (symbolsPtr != NULL)
         sprintf ((char*)&gTextString[41], " ' '%s", gEndOfLine);

      continueFlag = OutputString (outputFilePtr,
												(char*)gTextString,
												0,
												*outputCodePtr,
												continueFlag);

		}	// end "if (thresholdFlag)" 	

   classNamesPtr = gProjectInfoPtr->classNamesPtr;

   index = 0;
   for (classNumber=1;
				classNumber<=gProjectInfoPtr->numberStatisticsClasses;
						classNumber++)
		{
				// Get the class storage number.												

      classStorage = gProjectInfoPtr->storageClass[classNumber - 1];

      sprintf ((char*)gTextString2, "                               ");
      pstr ((char*)gTextString2,
				(char*)&classNamesPtr[classStorage].name,
				&strLength);

      if (classNumber != (UInt16)classPtr[index]) 
			{
         strLength = MIN (strLength, 22);
         sprintf ((char*)&gTextString2[strLength], "-not used");
         strLength += 9;

			}	// end "if (classNumber != classPtr[index])" 

      gTextString2[strLength] = ' ';
      gTextString2[kMaxClassFieldNameLength] = kNullTerminator;

      stringPtr = (char*)&gTextString;
      sprintf (stringPtr, "    %4d: %s", classNumber, gTextString2);
      stringPtr += 41;

      if (classNumber == (UInt16)classPtr[index]) 
			{
					// Add group name if appropriate.										

         if (listGroupColumnFlag) 
				{
            sprintf ((char*)gTextString2, "                               ");

            if ((UInt32)(classNumber + backgroundIndexOffset) <=
																			fileInfoPtr->numberClasses) 
					{
               groupIndex = classToGroupPtr[classNumber - 1 + backgroundIndexOffset];
               pstr ((char*)gTextString2, &groupNamePtr[groupIndex * 32], &strLength);
               gTextString2[strLength] = ' ';
               gTextString2[kMaxClassFieldNameLength] = kNullTerminator;
               sprintf (stringPtr, "\t%3d: %s", groupIndex + 1, gTextString2);

					}	// end "if (classNumber+backgroundIndexOffset <= ..." 

            else	// classNumber+backgroundIndexOffset > ... 
               sprintf (stringPtr, "\t     %s", gTextString2);

            stringPtr += 37;

				}	// end "if (listGroupColumnFlag)" 

					// Add symbol column if appropriate.									

         if (symbolsPtr != NULL) 
				{
            sprintf (stringPtr, "\t %c    ", symbolsPtr[classNumber]);
            stringPtr += 7;

				}	// end "if (symbolsPtr != NULL)" 

					// Add the weights if available.											

         if (weightsIndex >= 0) 
				{
            sprintf (stringPtr,
							"\t%7.3f",
							fabs ((double) gProjectInfoPtr->classNamesPtr[
							classStorage].priorWeights[weightsIndex]));
            stringPtr += 8;

				}	// end "if (weightsIndex >= 0)" 

					// Add the mixing parameter if needed.											

         if (classNamesPtr[classStorage].covarianceStatsToUse == kLeaveOneOutStats) 
				{
            if (classNamesPtr[classStorage].mixingParameterCode != kIdentityMatrix) 
					{
               mixingParameter = classNamesPtr[classStorage].looCovarianceValue;
               if (classNamesPtr[classStorage].mixingParameterCode == kUserSet)
						mixingParameter = classNamesPtr[classStorage].userMixingParameter;

               sprintf (stringPtr,
								"\t       %8.6f",
								mixingParameter);
               stringPtr += 16;

					}	// end "if (...[classStorage].mixingParameterCode != ..."

				}	// end "if (...covarianceStatsToUse == kLeaveOneOutStats && ..." 

         if (index < numberClasses - 1)
            index++;

			}	// end "if (classNumber == classPtr[index])" 

      else	// classNumber != classPtr[index] 
			{
         sprintf (stringPtr, "\t \t    ");
         stringPtr += 7;

			}	// end "else classNumber != classPtr[index]" 

      sprintf (stringPtr, "%s", gEndOfLine);
      stringLength = (Ptr)stringPtr - (Ptr)&gTextString + gNumberOfEndOfLineCharacters;
      continueFlag = OutputString (outputFilePtr, 
												(char*)gTextString,
												stringLength,
												*outputCodePtr,
												continueFlag);

      if (!continueFlag)
         break;

		}	// end "for (index=0;..." 

			// Insert a blank line after the table.

   continueFlag = OutputString (outputFilePtr,
											gEndOfLine,
											gNumberOfEndOfLineCharacters,
											*outputCodePtr,
											continueFlag);

   return (continueFlag);

}	// end "ListClassesUsed" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListClassFieldsUsed
//
//	Software purpose:	The purpose of this routine is to list the fields
//							used for the given class.  The output will be listed
//							in the output text window and/or the output disk file 
//							as specified.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			HistogramClassStats in SProjectHistogramStatistics.cpp
//							ListProjectFieldsUsed in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/25/1990
//	Revised By:			Larry L. Biehl			Date: 08/30/2006

Boolean ListClassFieldsUsed (
				SInt16								classNumber,
				SInt16*								fieldPtr,
				SInt16								fieldTypeCode,
				CMFileStream*						outputFilePtr,
				SInt16*								outputCodePtr,
				SInt16								stringNumber,
				SInt16*								fieldLabelNumberPtr,
				Boolean								includeClusterFieldsFlag)

{
			// Declare local variables and structures	

   HPClassNamesPtr					classNamesPtr;
   HPFieldIdentifiersPtr			fieldIdentPtr;
   HPFieldPointsPtr					fieldPointsPtr;
   char*									stringPtr;

   SInt16								classStorage,
											fieldIndex,
											fieldNumber,
											point,
											pointIndex,
											stringLength,
											strLength;

   Boolean								continueFlag,
											useFieldNumberFlag;


			// Initialize local variables.													

   classStorage = gProjectInfoPtr->storageClass[classNumber - 1];
   classNamesPtr = gProjectInfoPtr->classNamesPtr;
   fieldPointsPtr = gProjectInfoPtr->fieldPointsPtr;
   stringLength = 71 + gNumberOfEndOfLineCharacters;
   continueFlag = TRUE;

   useFieldNumberFlag = FALSE;
   if (*fieldLabelNumberPtr <= 0)
      useFieldNumberFlag = TRUE;

			// Get the basic string to use if needed.										
			// "Fields histogramed for class: "												

   if (stringNumber > 0) 
		{
      if (!ListFieldsTitle (outputFilePtr,
										outputCodePtr,
										stringNumber,
										classNumber,
										fieldTypeCode))
																							return (FALSE);

		}	// end "if (stringNumber > 0)" 

   fieldNumber = classNamesPtr[classStorage].firstFieldNumber;

   fieldIndex = 0;
   stringPtr = (char*)&gTextString[47];
   while (fieldNumber != -1) 
		{
      fieldIdentPtr = &gProjectInfoPtr->fieldIdentPtr[fieldNumber];

				// Make certain that field is of the requested type.					

      if (fieldIdentPtr->fieldType & fieldTypeCode) 
			{
         continueFlag = (fieldPtr == NULL);
         if (!continueFlag)
            continueFlag = (fieldNumber == fieldPtr[fieldIndex]);

         if (continueFlag) 
				{
            if (useFieldNumberFlag)
               *fieldLabelNumberPtr = fieldNumber + 1;

						// Get field name in 31 character field.							

            sprintf ((char*)gTextString3, "                               ");
            pstr ((char*)gTextString3, (char*)&fieldIdentPtr->name, &strLength);
            gTextString3[strLength] = ' ';
            gTextString3[kMaxClassFieldNameLength] = kNullTerminator;

            sprintf ((char*)gTextString, 
								"    %4d:\t%s\t%5d",
								*fieldLabelNumberPtr,
								gTextString3,
								classNumber);

						// Get the field boundary.												

            pointIndex = fieldIdentPtr->firstPolygonPoint;

						// Determine if rectangular or polygonal type field.			

            if (fieldIdentPtr->pointType == kRectangleType) 
					{
               sprintf (stringPtr,
									"\t%5u\t%5u\t%5u\t%5u%s",
									(unsigned int)fieldPointsPtr[pointIndex].line,
									(unsigned int)fieldPointsPtr[pointIndex + 1].line,
									(unsigned int)fieldPointsPtr[pointIndex].col,
									(unsigned int)fieldPointsPtr[pointIndex + 1].col,
									gEndOfLine);

               if (!OutputString (outputFilePtr,
												(char*)gTextString,
												stringLength,
												*outputCodePtr,
												TRUE))
																							return (FALSE);

					}	// end "if (fieldIdentPtr->..." 

            else if (fieldIdentPtr->pointType == kPolygonType) 
					{
							// Allow for the first two points which represent the bounding
							// rectangle.

               pointIndex += 2;

							// field is polygon type && listCode is for statistics list. 																

               for (point=0;
								point<fieldIdentPtr->numberOfPolygonPoints;
								point++) 
						{
                  if (point == 1)
                     sprintf ((char*)gTextString,
										"                                               ");

                  sprintf (stringPtr,
										"\t%5u      \t%5u      %s",
										(unsigned int)fieldPointsPtr[pointIndex].line,
										(unsigned int)fieldPointsPtr[pointIndex].col,
										gEndOfLine);

                  if (!OutputString (outputFilePtr,
													(char*)gTextString,
													stringLength,
													*outputCodePtr,
													TRUE))
																							return (FALSE);
                  pointIndex++;

						}	// end "for (point=1; point<fieldIdentPtr->" 

					}	// end "else if (fieldIdentPtr->pointType ..."  

            else if (fieldIdentPtr->pointType == kMaskType) 
					{
							// Field is cluster type. 												

               sprintf (stringPtr, " Mask area.%s", gEndOfLine);

					if (!OutputString (outputFilePtr,
												(char*)gTextString,
												0,
												*outputCodePtr,
												TRUE))
																							return (FALSE);

					}	// end "else if (fieldIdentPtr->..." 

            else if (fieldIdentPtr->pointType == kClusterType) 
					{
							// Field is cluster type. 												

               sprintf (stringPtr, " Cluster area.%s", gEndOfLine);

               if (!includeClusterFieldsFlag ||
									(!gProjectInfoPtr->keepClassStatsOnlyFlag &&
																			!fieldIdentPtr->statsUpToDate))
                  sprintf (&stringPtr[13],
										"; cannot be used since no coordinates are available.%s",
										gEndOfLine);

               if (!OutputString (outputFilePtr,
												(char*)gTextString,
												0,
												*outputCodePtr,
												TRUE))
																							return (FALSE);

					}	// end "if (fieldIdentPtr->..." 

            fieldIndex++;
            (*fieldLabelNumberPtr)++;

				}	// end "if (continueFlag)" 

			}	// end "if (fieldIdentPtr->field..." 

      fieldNumber = fieldIdentPtr->nextField;

		}	// end "while (fieldNumber != -1)" 

			// Insert a blank line after the table.

   continueFlag = OutputString (outputFilePtr,
											gEndOfLine,
											gNumberOfEndOfLineCharacters,
											*outputCodePtr,
											continueFlag);

   return (TRUE);

}	// end "ListClassFieldsUsed" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListClassificationHeaderInfo
//
//	Software purpose:	The purpose of this routine is to list header information
//							at the beginning of an ascii classification listing.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			ClassifyControl   in SClassify.cpp
// 						ClusterControl   in SCluster.cpp
//
//	Coded By:			Larry L. Biehl			Date: 02/13/1991
//	Revised By:			Larry L. Biehl			Date: 03/03/2017	

Boolean ListClassificationHeaderInfo (
				CMFileStream*						resultsFileStreamPtr,
				SInt16*								outputCodePtr,
				SInt16								covarianceStatsToUse,
				Boolean								continueFlag)

{
			// Initialize local variables.													

			// List the header identifier line.												
	/*
   MGetString (gTextString2, kFileIOStrID, IDS_MultiSpecType);  // 42
   sprintf ((char*)gTextString, "%s%s%s", gEndOfLine, &gTextString2[1], gEndOfLine);
   continueFlag = OutputString (resultsFileStreamPtr,
											(char*)gTextString,
											0,
											*outputCodePtr,
											TRUE);
	*/
			// List the processor name, date, time and project info.					

   continueFlag = ListHeaderInfo (
								resultsFileStreamPtr,
								kLImageInfo + kLProjectName + kLProjectImageName + kLStatType,
								outputCodePtr,
								covarianceStatsToUse,
								continueFlag);

			// List the project image start line and column.		

   MGetString (gTextString2, kFileIOStrID, IDS_ImageStartLine); // 36 
   sprintf ((char*)gTextString,
					"    %s %u, %u%s",
					&gTextString2[1],
					(unsigned int)gProjectInfoPtr->startLine,
					(unsigned int)gProjectInfoPtr->startColumn,
					gEndOfLine);
	
   continueFlag = OutputString (resultsFileStreamPtr,
											(char*)gTextString,
											0,
											*outputCodePtr,
											continueFlag);

   return (continueFlag);

}	// end "ListClassificationHeaderInfo" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListFieldsTitle
//
//	Software purpose:	The purpose of this routine is to list the title for
//							the field table listing.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			ListClassFieldsUsed in SProjectUtilities.cpp
//							ListProjectFieldsUsed in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/10/1991
//	Revised By:			Larry L. Biehl			Date: 02/09/1994	

Boolean ListFieldsTitle (
				CMFileStream*						outputFilePtr,
				SInt16*								outputCodePtr,
				SInt16								stringNumber,
				SInt16								classNumber,
				SInt16								fieldTypeCode)

{
   char*									stringPtr;
   HPClassNamesPtr					classNamesPtr;

   SInt16								classStorage,
											strLength;

   Boolean								continueFlag;


			// Initialize local variables.													

   continueFlag = TRUE;
   classNamesPtr = gProjectInfoPtr->classNamesPtr;

			// First get the proper line for the field table title based on the	
			// processor being used.															

   if (stringNumber == 0) 
		{
      stringNumber = IDS_Project11;
      switch (gProcessorCode) 
			{
         case kClusterProcessor:
            break;

         case kSeparabilityProcessor:
            break;

         case kClassifyProcessor:
            stringNumber = IDS_Project20;
            if (fieldTypeCode == kTestingType)
               stringNumber = IDS_Project21;
            break;

         case kListResultsProcessor:
            break;

         case kListDataProcessor:
            break;

         case kPrincipalComponentsProcessor:
            break;

			}	// end "switch (gProcessorCode)" 

      MGetString (gTextString, kProjectStrID, stringNumber);
      stringPtr = (char*)&gTextString[gTextString[0] + 1];

      sprintf (stringPtr,
					"(line interval = %u; column interval = %u)%s",
					(unsigned int)gAreaDescription.lineInterval,
					(unsigned int)gAreaDescription.columnInterval,
					gEndOfLine);

		}	// end "if (stringNumber == 0)" 

   else	// stringNumber != 0 
		{
      classStorage = gProjectInfoPtr->storageClass[classNumber - 1];
      MGetString (gTextString, kProjectStrID, stringNumber);
      stringPtr = (char*)&gTextString[gTextString[0] + 1];

      pstr ((char*)gTextString2,
					(char*)&classNamesPtr[classStorage].name, &strLength);

      sprintf (stringPtr,
					" %s (line interval = %u; column interval = %u)%s",
					gTextString2,
					(unsigned int)gAreaDescription.lineInterval,
					(unsigned int)gAreaDescription.columnInterval,
					gEndOfLine);

		}	// end "else stringNumber != 0" 

   continueFlag = OutputString (outputFilePtr,
											(char*)&gTextString[1],
											0,
											*outputCodePtr,
											continueFlag);

			// Now get the field table header lines.										

   continueFlag = ListSpecifiedStringNumber (kProjectStrID,
															IDS_Project18,
															(unsigned char*)gTextString,
															outputFilePtr,
															*outputCodePtr,
															continueFlag);

   continueFlag = ListSpecifiedStringNumber (kProjectStrID,
															IDS_Project19,
															(unsigned char*)gTextString,
															outputFilePtr,
															*outputCodePtr,
															continueFlag);

   return (continueFlag);

}	// end "ListFieldsTitle" 																					



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListProjectFieldsUsed
//
//	Software purpose:	The purpose of this routine is to list the project fields
//							used for the processor.  The output will be listed
//							in the output text window and/or the outputdisk file 
//							as specified.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			ClassifyAreasControl in SClassify.cpp
//							ListDataControl in SListData.cpp
//							PrincipalComponentControl in SPrincipalComponents.cpp
//							HistogramProjectStats in SProjectHistogramStatistics.cpp
//							StatisticsImageControl in SStatisticsImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/08/1991
//	Revised By:			Larry L. Biehl			Date: 04/08/1997	

Boolean ListProjectFieldsUsed (
				FileInfoPtr							fileInfoPtr,
				SInt16*								classPtr,
				UInt32								numberClasses,
				SInt16*								fieldPtr,
				SInt16								fieldTypeCode,
				CMFileStream*						outputFilePtr,
				SInt16*								outputCodePtr,
				Boolean								includeClusterFieldsFlag)
 {
		// Declare local variables and structures

   UInt32								index;

   SInt16								classNumber,
											fieldLabelNumber;


			// Initialize local variables.

   if (GetListResultsFlag (fieldTypeCode, kAreasUsed)) 
		{
				// List the three header lines.													

      if (!ListFieldsTitle (outputFilePtr,
										outputCodePtr,
										0,
										0,
										fieldTypeCode))
																						return (FALSE);

				// Loop through classes  being used.											

      for (index = 0; index < numberClasses; index++) 
			{
         fieldLabelNumber = 0;
         classNumber = classPtr[index];

         if (!ListClassFieldsUsed (classNumber,
												fieldPtr,
												fieldTypeCode,
												outputFilePtr,
												outputCodePtr,
												0,
												&fieldLabelNumber,
												includeClusterFieldsFlag))
																						return (FALSE);

			}	// end "for (index=0;..." 

		}	// end "if (GetListResultsFlag (...)"

   return (TRUE);

}	// end "ListProjectFieldsUsed" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ListClassInformationMessage
//
//	Software purpose:	This routine lists the specified string number indicating
//							that the class covariance (variance) could not
//							be inverted.
//
//	Parameters in:				
//
//	Parameters out:				
//
// Value Returned:	True if string was listed and/or written to
//													the output file.
//							False if not.
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/26/1994
//	Revised By:			Larry L. Biehl			Date: 02/22/2001			

Boolean ListClassInformationMessage (
				SInt16								strListID,
				SInt16								stringIndex,
				CMFileStream*						resultsFileStreamPtr,
				SInt16								outputCode,
				UInt32								statClassNumber,
				Boolean								continueFlag)
 {
   char*									stringPtr2;

   UInt32								classStorage;

   SInt16								strLength;


   classStorage = gProjectInfoPtr->storageClass[statClassNumber];

   if (gProjectInfoPtr->classNamesPtr[classStorage].listMessageFlag) 
		{
      pstr ((char*)gTextString3,
				(char*)&gProjectInfoPtr->classNamesPtr[classStorage].name,
				&strLength);

      continueFlag = GetSpecifiedStringNumber (
									strListID, stringIndex, gTextString2, continueFlag);

      if (continueFlag) 
			{
         stringPtr2 = (char*)&gTextString2;

         if (stringIndex == IDS_Project31 || stringIndex == IDS_Project32)
            sprintf ((char*)gTextString, (char*)&stringPtr2[1], (char*)gTextString3);

         else if (stringIndex == IDS_Project67)
            sprintf ((char*)gTextString, 
								(char*)&stringPtr2[1], 
								(char*)gTextString3,
								gProjectInfoPtr->zeroVarianceFactor);

         else if (stringIndex == IDS_Project68)
            sprintf ((char*)gTextString, 
								(char*)&stringPtr2[1], 
								gProjectInfoPtr->zeroVarianceFactor);

         else if (stringIndex == IDS_Project69)
            sprintf ((char*)gTextString,
								"%s", (char*)&stringPtr2[1]);

         else if (stringIndex == IDS_Project78)
            sprintf ((char*)gTextString,
							(char*)&stringPtr2[1],
							(char*)gTextString3);

         else	// stringIndex == IDS_Project79
            sprintf ((char*)gTextString, "%s", (char*)&stringPtr2[1]);

         continueFlag = OutputString (resultsFileStreamPtr,
													(char*)gTextString,
													0,
													outputCode,
													continueFlag);

			}	// end "if (continueFlag)" 

		}	// end "if (...->classNamesPtr[classStorage].listMessageFlag)"

   return (continueFlag);

}	// end "ListClassInformationMessage" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadClassList
//
//	Software purpose:	The purpose of this routine is to load a class
//							list into the list pointed to by the dialogListHandle.
//							If 'classPtr' is NULL then all classes for the project
//							are loaded in.  If 'symbolPtr' is NULL, then the class
//							number is add before each class name in the list; otherwise
//							the class symbol is added before each name in the list.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned: 			
//
// Called By:			ClassPairWeightsDialog in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/14/1990
//	Revised By:			Larry L. Biehl			Date: 03/01/2017

void LoadClassList (
				DialogPtr							dialogPtr,
				#if defined multispec_wx
					wxListBox*							dialogListHandle,
				#else
					ListHandle							dialogListHandle,
				#endif
				SInt16								numberOfClassesToUse,
				SInt16*								classPtr,
				unsigned char*						symbolPtr,
				Boolean								clusterFlag)

{
	Cell									cell;

   HPClassNamesPtr					classNamesPtr;

   SInt16								classIndex,
											classStorage,
											index,
											row,
											stringLength,
											textIndex;


			// Check input variables.															

   if (dialogPtr == NULL)
																									return;
   if (dialogListHandle == NULL)
																									return;
   if (gProjectInfoPtr == NULL)
																									return;


			// Initialize local variables.													

   classNamesPtr = gProjectInfoPtr->classNamesPtr;

   if (classPtr == NULL && !clusterFlag)
      numberOfClassesToUse = gProjectInfoPtr->numberStatisticsClasses;

   if (symbolPtr == NULL)
      textIndex = 4;
   else	// symbolPtr != NULL 
      textIndex = 2;

			// Make certain that the graph port is set to the dialog window.		

   SetPortDialogPort (dialogPtr);

			// Load the class names to be used into the list.							

   LSetDrawingMode (FALSE, dialogListHandle);
	#if defined multispec_wx
		row = LAddRow (numberOfClassesToUse, 0, dialogListHandle);
	#else
		row = LAddRow (numberOfClassesToUse, 0, dialogListHandle);
	#endif

   cell.h = 0;
   for (index = 0; index < numberOfClassesToUse; index++) 
		{
      if (classPtr != NULL)
         classIndex = classPtr[index] - 1;

      else	// classPtr == NULL 
         classIndex = index;

				// Place class symbol in the list.											

      if (symbolPtr != NULL)
         sprintf ((char*)gTextString, "%c-", symbolPtr[classIndex + 1]);

      else	// symbolPtr == NULL 	
         sprintf ((char*)gTextString, "%3d-", classIndex + 1);

      if (!clusterFlag) 
			{
					// Get the class storage number.											

         classStorage = gProjectInfoPtr->storageClass[classIndex];

					// Append the class name to to list item.								

         pstr ((char*)&gTextString[textIndex],
					(char*)&classNamesPtr[classStorage].name, &stringLength);
         stringLength += textIndex;

			}	// end "if (!clusterFlag)" 

      else	// clusterFlag 
			{
					// Add cluster class number.												

         sprintf ((char*)&gTextString[textIndex], "%4d", classIndex + 1);
         stringLength = textIndex + 4;

			}	// end "else clusterFlag" 

      cell.v = index;
      LSetCell ((char*)gTextString, stringLength, cell, dialogListHandle);

				// Make certain that we didn't have a memory error with the 		
				// last setting of the list for the cell.									

      if (gMemoryError != noErr)
         break;

		}	// end "for (index=0; ... 

			// Turn list drawing mode back on.												

   LSetDrawingMode (TRUE, dialogListHandle);

	#if defined multispec_mac 
				// Set the text font for the window and draw the list.					

		TextFont (gWindowTextFont); // monaco  
		LUpdate (GetPortVisibleRegion (GetDialogPort (dialogPtr), gTempRegion1),
						dialogListHandle);
		SetEmptyRgn (gTempRegion1);
	#endif	// defined multispec_mac 

}	// end "LoadClassList"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadClassPairWeightVector
//
//	Software purpose:	The purpose of this routine is to load the class
//							pair weight vector with a list of equal weights for 
//							all class pairs in the project.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned: 		
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 06/14/1990
//	Revised By:			Larry L. Biehl			Date: 06/18/1990

void LoadClassPairWeightVector (
				SInt16*								weightsPtr,
				SInt32								numberClassPairs,
				SInt16								defaultWeight,
				SInt32*								weightsSumPtr,
				SInt16								weightSet,
				SInt16*								weightsListPtr,
				SInt16*								classPtr,
				UInt16								numberClasses)
 {
   SInt32								index;

   SInt16								class1,
											class2,
											numberWeightListSets,
											setNumber,
											userWeight,
											weightIndex,
											weightsListIndex,
											weightsListIndex2,
											weightsSetEnd;


			//  Check input variables.													

   if (weightsPtr == NULL)
																									return;

			// Load in the default weight set.										

   for (index=0; index<numberClassPairs; index++)
      weightsPtr[index] = defaultWeight;

			// Add in user selected class pair weights.							

   if (weightsListPtr != NULL && weightSet != kEqualWeights) 
		{
      numberWeightListSets = weightsListPtr[0];

      weightsListIndex = 1;
      for (setNumber=1; setNumber<=numberWeightListSets; setNumber++) 
			{
         userWeight = weightsListPtr[weightsListIndex];

         weightsListIndex++;
         weightsSetEnd = weightsListIndex + weightsListPtr[weightsListIndex];

         weightsListIndex++;
         while (weightsListIndex < weightsSetEnd) 
				{
            class1 = weightsListPtr[weightsListIndex];

            if (ClassToBeUsed (class1, classPtr, numberClasses))
					{
               weightsListIndex2 = weightsListIndex + 1;
               while (weightsListIndex2 <= weightsSetEnd) 
						{
                  class2 = weightsListPtr[weightsListIndex2];

                  if (ClassToBeUsed (class2, classPtr, numberClasses))
							{
                     weightIndex =
									class1 * numberClasses - (class1 * (class1 + 1)) / 2 -
																			numberClasses + class2 - 1;
                     weightsPtr[weightIndex] = userWeight;

							}	// end "if (ClassToBeUsed (class2, ..." 

                  weightsListIndex2++;

						}	// end "while (weightsListIndex2 <= weightsSetEnd)" 

					}	// end "if (ClassToBeUsed (class1, ..." 

            weightsListIndex++;

				}	// end "while (weightsListIndex < weightsSetEnd)" 

         weightsListIndex++;

			}	// end "for (setNumber=1; ..." 

		}	// end "if (weightsListPtr != NULL && ...)" 

			// Now get the weight sum.													

   *weightsSumPtr = 0;
   for (index=0; index<numberClassPairs; index++)
      *weightsSumPtr += weightsPtr[index];

}	// end "LoadClassPairWeightVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadClassSymbolVector
//
//	Software purpose:	The purpose of this routine is to load the class
//							symbols vector with a list of symbols for all 
//							classes in the project.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned: 		
//
// Called By:			LoadClassifySpecs in SClassify.cpp
//
//	Coded By:			Larry L. Biehl			Date: 01/17/1989
//	Revised By:			Larry L. Biehl			Date: 05/01/1998

void LoadClassSymbolVector (
				unsigned char*						symbolsPtr,
				SInt16								stringNumber,
				SInt16								numberClasses)
 {
   UInt32								index,
											index2,
											numberSymbols;


   if (gProjectInfoPtr != NULL) 
		{
      MGetString (gTextString, kSymbolsStrID, stringNumber);

				// Start loading the symbols at location 1 in the vector instead	
				// of 0.  This makes it easier to reference by class number.		
				// Load blank as the very first symbol in the vector for the null	
				// or background class.															

      symbolsPtr[0] = ' ';

      numberSymbols = (UInt32)gTextString[0];
      index2 = 1;
      for (index = 1; index <= (UInt32)numberClasses; index++) 
			{
         symbolsPtr[index] = gTextString[index2];
         index2++;
         if (index2 > numberSymbols)
            index2 = 1;

			}	// end "for (index=1; index<=numberClasses..." 

		}	// end "if (gProjectInfoPtr != NULL)" 

}	// end "LoadClassSymbolVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadClassVector
//
//	Software purpose:	The purpose of this routine is to load the class
//							vector with a list of all class numbers in the 
//							project.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned: 		
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/16/1989
//	Revised By:			Larry L. Biehl			Date: 05/17/1990	

void LoadClassVector (
				UInt32*								numberClassesPtr,
				SInt16*								classPtr)
 {
   UInt32								index,
											numberStatisticsClasses;


   numberStatisticsClasses = gProjectInfoPtr->numberStatisticsClasses;

   for (index=0; index<numberStatisticsClasses; index++)
      classPtr[index] = (SInt16)(index + 1);

   *numberClassesPtr = numberStatisticsClasses;

}	// end "LoadClassVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadClassWeightGroups
//
//	Software purpose:	The purpose of this routine is to load the class
//							weight groups into the list pointed to by the 
//							dialogListHandle.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned: 			
//
// Called By:			ClassPairWeightsDialog in SFeatureSelection.cpp
//
//	Coded By:			Larry L. Biehl			Date: 06/19/1990
//	Revised By:			Larry L. Biehl			Date: 12/21/2016

void LoadClassWeightGroups (
				DialogPtr							dialogPtr,
				#if defined multispec_wx
					wxListBox*							dialogListHandle,
				#else
					ListHandle							dialogListHandle,
				#endif
				SInt16*								weightsListPtr)

{
   Cell									cell;

   SInt16								classIndex,
											numberOfClassesInGroup,
											numberWeightGroups,
											set,
											stringLength,
											weight,
											weightsListIndex;


			// Check input parameters.															

   if (weightsListPtr == NULL)
																									return;

			// Get the number of user specified weight groups.							

   numberWeightGroups = weightsListPtr[0];
	weightsListIndex = 0;

   cell.h = 0;

   for (set=1; set<=numberWeightGroups; set++) 
		{
      weightsListIndex++;
      weight = weightsListPtr[weightsListIndex];
      sprintf ((char*)gTextString, "%3d-", weight);
      stringLength = 4;

      weightsListIndex++;
      numberOfClassesInGroup = weightsListPtr[weightsListIndex];

      for (classIndex = 1; classIndex <= numberOfClassesInGroup; classIndex++) 
			{
         weightsListIndex++;
         NumToString ((UInt32)weightsListPtr[weightsListIndex], gTextString2);
         stringLength += gTextString2[0] + 1;

         if (stringLength > 255)
            break;

					// Add class number to the list.											

         strncat ((char*)gTextString, (char*)&gTextString2[1], gTextString2[0]);

					// Append a blank character to the list.								

         strncat ((char*)gTextString, " ", 1);

			}	// end "for (classIndex=1; ..." 

				// Add the string to the next vertical cell.								

		#if defined multispec_mac 
			TextFont (gWindowTextFont); // monaco 
		#endif	// defined multispec_mac  

      stringLength--;
		
		#if defined multispec_wx
			cell.v = LAddRow (1, set-1, dialogListHandle);
		#else
			cell.v = LAddRow (1, SHRT_MAX, dialogListHandle);
		#endif
      
      LSetCell ((char*)gTextString, stringLength, cell, dialogListHandle);

		}	// end "for (set=1; set<=numberWeightGroups; set++)" 

}	// end "LoadClassWeightGroups"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadClassWeightsIntoList
//
//	Software purpose:	The purpose of this routine is to load the field
//							vector with a list of all field numbers in the 
//							project.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned: 		
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 12/16/1999
//	Revised By:			Larry L. Biehl			Date: 12/16/1999	

double LoadClassWeightsIntoList (
				#if defined multispec_wx
					wxListBox*							listHandle,
				#else
					ListHandle							listHandle,
				#endif
				UInt16								numberOfClassesToUse,
				SInt16*								classPtr,
				float*								weightsPtr,
				Boolean								useEnhancedStatFlag)

{
   double								weightSum;

   Cell									cell;

   SInt16								classIndex,
											classStorage,
											fixedCellWidth = 40,
											index,
											row,
											stringLength;


   weightSum = 0;

   if (listHandle != NULL) 
		{
      row = LAddRow (numberOfClassesToUse, 0, listHandle);
      cell.h = 0;
      weightsPtr = GetClassWeightsPtr (weightsPtr, useEnhancedStatFlag, FALSE);

      for (index = 0; index < numberOfClassesToUse; index++) 
			{
					// Get the class storage number.											

         classIndex = classPtr[index] - 1;
         classStorage = gProjectInfoPtr->storageClass[classIndex];

					// Add name, weight and "equal weight flag" to the class 		
					// list.																			

         pstr ((char*)gTextString2,
					(char*)&gProjectInfoPtr->classNamesPtr[classStorage].name,
					&stringLength);

         gTextString[0] = ' ';
         if (weightsPtr[classIndex] >= 0)
			#if defined multispec_mac
            gTextString[0] = '.';
			#endif	// defined multispec_mac 

			#if defined multispec_win || defined multispec_wx
				gTextString[0] = '#';
			#endif	// defined multispec_win ...
         

         sprintf ((char*)&gTextString[1],
						"%-31s-%7.3f",
						(char*)gTextString2,
						fabs ((double)weightsPtr[classIndex]));

         cell.v = index;
         LSetCell ((char*)gTextString, fixedCellWidth, cell, listHandle);

					// Make certain that we didn't have a memory with the last		
					// setting of the list for the cell.									

         if (gMemoryError != noErr)
            break;

         // Get sum of weights.														

         weightSum += fabs ((double) weightsPtr[classIndex]);

			}	// end "for (index=0; ... 

		}	// end "if (listHandle != NULL)"

   return (weightSum);

}	// end "LoadClassWeightsIntoList"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadFieldVector
//
//	Software purpose:	The purpose of this routine is to load the field
//							vector with a list of all field numbers in the 
//							project.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned: 		
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/24/1990
//	Revised By:			Larry L. Biehl			Date: 06/14/1990	

void LoadFieldVector (
				UInt16*								numberFieldsPtr,
				SInt16*								fieldPtr,
				SInt16								fieldType)
 {
   UInt32								index,
											numberStatFields;


			// Check input variables.															

   if (fieldPtr == NULL || gProjectInfoPtr == NULL)
																									return;

   numberStatFields = 0;

   if (fieldType & kTrainingType)
      numberStatFields += gProjectInfoPtr->numberStatTrainFields;

   if (fieldType & kTestingType)
      numberStatFields += gProjectInfoPtr->numberStatTestFields;

   for (index = 0; index < numberStatFields; index++)
      fieldPtr[index] = (SInt16)(index + 1);

   *numberFieldsPtr = (UInt16)numberStatFields;

}	// end "LoadFieldVector" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadSymbolList
//
//	Software purpose:	The purpose of this routine is to load a symbol
//							list into the list pointed to by the dialogListHandle.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned: 		
//
// Called By:			SymbolsDialog in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/26/1990
//	Revised By:			Larry L. Biehl			Date: 01/28/2003

void LoadSymbolList (
				DialogPtr							dialogPtr,
				ListHandle							dialogListHandle)

{
   Cell									cell;

   SInt16								index,
											numberSymbols,
											row,
											stringLength;


			// Check input variables.															

   if (dialogPtr == NULL || dialogListHandle == NULL)
																									return;

			// Initialize local variables.													

   stringLength = 1;

			// Get the resource with the default symbols.								

   GetIndString (gTextString, kSymbolsStrID, IDS_Symbol1);

   numberSymbols = gTextString[0];

			// Make certain that the graph port is set to the dialog window.		

   SetPortDialogPort (dialogPtr);

			// Load the class names to be used into the list.							

   LSetDrawingMode (FALSE, dialogListHandle);

   row = LAddRow (numberSymbols, 0, dialogListHandle);

   cell.h = 0;
   cell.v = 0;
   for (index=1; index<=numberSymbols; index++) 
		{
				// Place class symbol in the list.											

      LSetCell (&gTextString[index], stringLength, cell, dialogListHandle);

				// Make certain that we didn't have a memory error with the 		
				// last setting of the list for the cell.									

      if (gMemoryError != noErr)
         break;

      cell.v++;

		}	// end "for (index=1; ... 

			// Turn list drawing mode back on.												

   LSetDrawingMode (TRUE, dialogListHandle);

			// Set the text font for the window and draw the list.					

   TextFont (gWindowTextFont); // monaco  
   LUpdate (GetPortVisibleRegion (GetDialogPort (dialogPtr), gTempRegion1),
					dialogListHandle);
   SetEmptyRgn (gTempRegion1);

}	// end "LoadSymbolList"
#endif	// defined multispec_mac 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LoadTrainClassVector
//
//	Software purpose:	The purpose of this routine is to load the class
//							vector with a list of all class numbers in the 
//							project that have statistics available.
//		
//	Parameters in:				
//
//	Parameters out:			
//
// Value Returned:		
//
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 01/16/1989
//	Revised By:			Larry L. Biehl			Date: 05/17/1990	

void LoadTrainClassVector (
				UInt32*								numberClassesPtr,
				SInt16*								classSetPtr,
				SInt16*								classPtr)
 {
   HPClassNamesPtr					classNamesPtr;

   SInt16								classIndex,
											classStorage,
											index,
											numberStatisticsClasses;


			// Initialize local variables.												

   numberStatisticsClasses = gProjectInfoPtr->numberStatisticsClasses;
   classNamesPtr = gProjectInfoPtr->classNamesPtr;
   
   index = 0;
   for (classIndex=0; classIndex<numberStatisticsClasses; classIndex++) 
		{
      classStorage = gProjectInfoPtr->storageClass[classIndex];
      if (classNamesPtr[classStorage].numberOfTrainFields > 0 ||
									classNamesPtr[classStorage].numberStatisticsPixels > 0) 
			{
         classPtr[index] = classIndex + 1;
         index++;

			}	// end "if (classNamesPtr[classIndex]..." 

		}	// end "for (classIndex=0;..." 

   *numberClassesPtr = index;

   *classSetPtr = kAllMenuItem;
   if (*numberClassesPtr != gProjectInfoPtr->numberStatisticsClasses)
      *classSetPtr = kSubsetMenuItem;

}	// end "LoadTrainClassVector" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void LockProjectMemory
//
//	Software purpose:	The purpose of this routine is to lock the memory
//							for the project.  This is usually after memory for
//							the project has changed and the project memory is unlocked.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 03/06/1991
//	Revised By:			Larry L. Biehl			Date: 09/23/1997	

void LockProjectMemory (
				Handle								inputProjectInfoHandle,
				SInt16								lockCode,
				ProjectInfoPtr*					outputProjectInfoPtrPtr)

{
   ProjectInfoPtr						localProjectInfoPtr;


			// Check input variables.															

   if (outputProjectInfoPtrPtr == NULL)
																									return;

			// Initialize local variables.													

   localProjectInfoPtr = *outputProjectInfoPtrPtr;

   if (localProjectInfoPtr == NULL && inputProjectInfoHandle != NULL) 
		{
      *outputProjectInfoPtrPtr = (ProjectInfoPtr)GetHandlePointer (
													inputProjectInfoHandle, kLock, kMoveHi);
      localProjectInfoPtr = *outputProjectInfoPtrPtr;

		}	// end "if (localProjectInfoPtr == NULL && ..." 

   if (localProjectInfoPtr != NULL && !localProjectInfoPtr->handlesLockedFlag) 
		{
				// Lock handle to class list information.	

      localProjectInfoPtr->storageClass = (SInt16*)GetHandlePointer (
									localProjectInfoPtr->storageClassHandle, kLock, kMoveHi);

				// Lock handle to channel information.	

      localProjectInfoPtr->channelsPtr = (UInt16*)GetHandlePointer (
									localProjectInfoPtr->channelHandle, kLock, kMoveHi);

				// Lock handle to class name information.		

      localProjectInfoPtr->classNamesPtr = (HPClassNamesPtr) GetHandlePointer (
									localProjectInfoPtr->classNamesHandle, kLock, kMoveHi);

				// Lock handles to common covariance statistics.
				// Don't need the pointers now.					

      GetHandlePointer (
						localProjectInfoPtr->commonChannelStatsHandle, kLock, kMoveHi);

      GetHandlePointer (
						localProjectInfoPtr->commonCovarianceStatsHandle, kLock, kMoveHi);

				// Lock handle to class channel statistics.					

      localProjectInfoPtr->classChanStatsPtr = 
							(HChannelStatisticsPtr)GetHandlePointer (
														localProjectInfoPtr->classChanStatsHandle,
														kLock, 
														kMoveHi);

				// Lock handle to modified class channel statistics.					

      localProjectInfoPtr->modifiedClassChanStatsPtr =
							(HChannelStatisticsPtr)GetHandlePointer (
											localProjectInfoPtr->modifiedClassChanStatsHandle,
											kLock,
											kMoveHi);

				// Lock handle to modified class covariance statistics.				

      localProjectInfoPtr->modifiedClassCovStatsPtr =
							(HCovarianceStatisticsPtr) GetHandlePointer (
									localProjectInfoPtr->modifiedClassCovStatsHandle,
									kLock,
									kMoveHi);

				// Lock handle to class sum of squares statistics.				

      localProjectInfoPtr->classSumSquaresStatsPtr =
							(HSumSquaresStatisticsPtr) GetHandlePointer (
												localProjectInfoPtr->classSumSquaresStatsHandle,
												kLock,
												kMoveHi);

				// Lock handle to field name information.				

      localProjectInfoPtr->fieldIdentPtr =
							(HPFieldIdentifiersPtr) GetHandlePointer (
												localProjectInfoPtr->fieldIdentifiersHandle,
												kLock,
												kMoveHi);

				// Lock handle to field coordinates.			

      localProjectInfoPtr->fieldPointsPtr =
							(HPFieldPointsPtr) GetHandlePointer (
												localProjectInfoPtr->fieldCoordinatesHandle,
												kLock,
												kMoveHi);

				// Lock handle to field channel statistics.			

      localProjectInfoPtr->fieldChanStatsPtr =
							(HChannelStatisticsPtr) GetHandlePointer (
												localProjectInfoPtr->fieldChanStatsHandle,
												kLock,
												kMoveHi);

				// Lock handle to field covariance statistics.		

      localProjectInfoPtr->fieldSumSquaresStatsPtr =
							(HSumSquaresStatisticsPtr) GetHandlePointer (
												localProjectInfoPtr->fieldSumSquaresStatsHandle,
												kLock,
												kMoveHi);

      localProjectInfoPtr->handlesLockedFlag = TRUE;

		}	// end "if (localProjectInfoPtr != NULL && ..." 

}	// end "LockProjectMemory" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ModalSymbolsDialog 
//
//	Software purpose:	The purpose of this routine is to handle the user
//							selections in the symbol dialog that is used
//							to allow the user to select the symbol for each class. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned: 	None
//
// Called By:			SymbolsDialog   in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/25/1990
//	Revised By:			Larry L. Biehl			Date: 12/16/2016	

Boolean ModalSymbolsDialog (
				DialogPtr							dialogPtr,
				UInt16								numberOfClassesToUse)

{
   Rect									theBox;

   Cell									cell;

   Handle								okHandle,
											theHandle;

   SInt16								fixedCellWidth = 33,
											itemHit = 0,
											newSymbolLength,
											returnCode,
											selectedCell,
											stringLength,
											theType;

   Boolean								modalDone;

   char									saveNewSymbol;


			// Continue if dialogPtr and gDialogListHandle are not NULL.			

   if (dialogPtr != NULL && gDialogListHandle != NULL) 
		{
				// Get the "handle" for the OK button for use later.					

      GetDialogItem (dialogPtr, 1, &theType, &okHandle, &theBox);

      cell.h = 0;
      modalDone = FALSE;
      newSymbolLength = 0;
      selectedCell = -1;
      itemHit = 0;
      do {
         ModalDialog (gProcessorDialogFilterPtr, &itemHit);
         if (itemHit > 2) 
				{
            switch (itemHit) 
					{
               case 5:
                  cell.v = 0;

                  if (LGetSelect (TRUE, &cell, gDialogListHandle))
                     selectedCell = cell.v;

                  else	// !LGetSelect (TRUE, &cell, ... 
                     selectedCell = -1;
                  break;

               case 7: // Change the symbol for each class selected.		
                  TextFont (gWindowTextFont); // monaco  

                  cell.v = 0;
                  while (LGetSelect (TRUE, &cell, gDialogListHandle)) 
							{
                     stringLength = fixedCellWidth;
                     LGetCell (gTextString2, &stringLength, cell, gDialogListHandle);

                     gTextString2[0] = saveNewSymbol;

                     LSetCell (
										gTextString2, stringLength, cell, gDialogListHandle);

                     cell.v++;

							}	// end "while (LGetSelect (TRUE, ...))" 
                  break;

               case 8:
                  GetDialogItem (dialogPtr, itemHit, &theType, &theHandle, &theBox);
                  GetDialogItemText (theHandle, gTextString);

                  newSymbolLength = gTextString[0];
                  saveNewSymbol = gTextString[1];

                  SelectDialogItemText (dialogPtr, 8, 0, INT16_MAX);
                  break;

               case 10:
                  cell.v = 0;

                  if (LGetSelect (TRUE, &cell, gDialogListHandle2)) 
							{
                     stringLength = 1;
                     LGetCell (
									&gTextString2[1], &stringLength, cell, gDialogListHandle2);
                     gTextString2[0] = 1;
                     LoadDItemString (dialogPtr, 8, (Str255*)gTextString2);

                     newSymbolLength = 1;
                     saveNewSymbol = gTextString2[1];

                     SelectDialogItemText (dialogPtr, 8, 0, INT16_MAX);

							}	// end "if (LGetSelect (TRUE, &cell, ..." 

                  break;

               case 12: // Help 
                  HiliteControl ((ControlHandle) okHandle, 255);
                  returnCode = DisplayAlert (1152, 0, kAlertStrID, 13, 0, NULL);

                  TextFont (gWindowTextFont); // monaco  
                  LUpdate (GetPortVisibleRegion (GetDialogPort (dialogPtr),
																	gTempRegion1),
																	gDialogListHandle);
                  SetEmptyRgn (gTempRegion1);
                  LUpdate (GetPortVisibleRegion (GetDialogPort (dialogPtr),
																	gTempRegion1),
																	gDialogListHandle2);
                  SetEmptyRgn (gTempRegion1);
                  HiliteControl ((ControlHandle)okHandle, 0);
                  break;

					}	// end "switch (itemHit)" 

            if (itemHit == 5 || itemHit == 8 || itemHit == 10) 
					{
               if (newSymbolLength == 1 && selectedCell >= 0)
                  SetDLogControlHilite (dialogPtr, 7, 0);

               else	// newSymbolLength != 1 || selectedCell < 0 
                  SetDLogControlHilite (dialogPtr, 7, 255);

					}	// end "if (itemHit == 5 || itemHit == 8 || ..." 

				}	// end "if (itemHit > 2)" 

         else if (itemHit > 0) // and itemHit <= 2 
						// itemHit == 1 is OK, itemHit == 2 is Cancel.					
            modalDone = TRUE;

			} while (!modalDone);

		}	// end "if (dialogPtr != NULL && gDialogListHandle != NULL)" 

   return (itemHit == 1);

}	// end "ModalSymbolsDialog"
#endif	// defined multispec_mac 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void NoClassStatsAlert
//
//	Software purpose:	The purpose of this routine is to display an alert
//							to inform the user that there are no statistics
//							to use to continue the requested operation.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:	None
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date:  1/15/1989
//	Revised By:			Larry L. Biehl			Date:  7/29/1997	

void NoClassStatsAlert (
				UInt32								minimumNumber)

{
   SInt16								returnCode;


   MInitCursor ();

   if (LoadSpecifiedStringNumberLongP (kAlertStrID,
														IDS_Alert40,
														(char*) gTextString,
														(char*) gTextString2,
														TRUE,
														(SInt32)minimumNumber,
														0))
      returnCode = DisplayAlert (kErrorAlertID, 3, 0, 0, 0, gTextString);

}	// end "NoClassStatsAlert" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		short int ProjectChangesPopUpMenu
//
//	Software purpose:	The purpose of this routine is to handle a 
//							mousedown in the project channges pop up menu 
//							location.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/05/1989
//	Revised By:			Larry L. Biehl			Date: 05/13/2016

SInt16 ProjectChangesPopUpMenu (
				DialogPtr							dialogPtr,
				Rect*									popUpProjectChangesBoxPtr,
				Boolean*								newProjectFlag,
				SInt16								currentListType)

{
   Point									popUpLoc;

   SInt32								menuData;

   SInt16								returnCode,
											selection;


			// Get location of project change pop up box and convert it to 		
			// global coordinates.																

   popUpLoc.v = popUpProjectChangesBoxPtr->bottom - 1;
   popUpLoc.h = popUpProjectChangesBoxPtr->left + 2;
   LocalToGlobal (&popUpLoc);

			// Insert the pop up menu and then let user make a selection.			

   InsertMenu (gPopUpProjectChangesMenu, -1);

   if (gProjectInfoPtr->fieldChanStatsHandle == NULL)
      DisableMenuItem (gPopUpProjectChangesMenu, 1);
   else	// gProjectInfoPtr->fieldChanStatsHandle != NULL 
      EnableMenuItem (gPopUpProjectChangesMenu, 1);
	/*
   if (*newProjectFlag) 
		{
      DisableMenuItem (gPopUpProjectChangesMenu, 2);
      CheckMenuItem (gPopUpProjectChangesMenu, 2, TRUE);
	
		}	// end "if (*newProjectFlag)" 

   else	// !*newProjectFlag 
		{
      DisableMenuItem (gPopUpProjectChangesMenu, 2);
      CheckMenuItem (gPopUpProjectChangesMenu, 2, FALSE);

		}	// end "else !*newProjectFlag" 
	*/
			// Invert the menu title.															

   InvertRect (popUpProjectChangesBoxPtr);

   menuData = PopUpMenuSelect (gPopUpProjectChangesMenu,
											popUpLoc.v, 
											popUpLoc.h, 
											1);

			// Invert the prompt box again to make normal and remove the			
			// the channel pop up menu.														

   InvertRect (popUpProjectChangesBoxPtr);
   DeleteMenu (kPopUpProjectChangesMenuID);

			// Invalidate the area within the channel pop up box.						

   InvalWindowRect (GetDialogWindow (dialogPtr), popUpProjectChangesBoxPtr);

			// Get the selection made and handle the request.							

   selection = LoWord (menuData);

   returnCode = currentListType;
   if (selection == 1)
      if (ProjectMenuClearStatistics ())
         returnCode = 1;
	/*
   if (selection == 2) 
		{
      if (CreateNewProject (FALSE)) 
			{
         *newProjectFlag = TRUE;
         returnCode = 1;

			}	// end "if (CreateNewProject ())" 

		}	// end "if (selection == 2)" 
	*/
			// Return the selection that was made.											

   return (returnCode);

}	// end "ProjectChangesPopUpMenu"
#endif	// defined multispec_mac 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean ProjectMenuClearStatistics
//
//	Software purpose:	The purpose of this routine is to handle a 
//							clear project statistics menu item request.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:
//
// Called By:			Menus in MMenus.c
//							ProjectChangesPopUpMenu in SProjectUtilities.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/28/1989
//	Revised By:			Larry L. Biehl			Date: 04/27/2019

Boolean ProjectMenuClearStatistics (void)

{
	WindowInfoPtr						projectWindowInfoPtr;

   SInt16								itemHit,
											numberChannels;

   Boolean								returnFlag;


			// Initialize local variables.

   returnFlag = FALSE;
   itemHit = 0;

			// Give the user a chance to save the current project file with
			// Save As ...

   if (gProjectInfoPtr->numberStatisticsClasses > 0)
		{
      char*			projectFileNameCPtr;

      CMFileStream* projectFileStreamPtr = GetFileStreamPointer (gProjectInfoPtr);

      if (FileExists (projectFileStreamPtr))
         projectFileNameCPtr =
							(char*)GetFileNameCPointerFromProjectInfo (gProjectInfoPtr);

      else	// !FileExists (...
			{
         sprintf ((char*)gTextString3, (char*)"Untitled Project");
         projectFileNameCPtr = (char*)gTextString3;

			}	// end "else !FileExists (..."

      itemHit = 0;
      if (LoadSpecifiedStringNumberStringP (kAlertStrID,
															IDS_Alert41,
															(char*) gTextString,
															(char*) gTextString2,
															TRUE,
															projectFileNameCPtr))
			itemHit = DisplayAlert (kSaveAlertID, 3, 0, 0, 0, gTextString);

      if (itemHit == 1)
         SaveProjectFile (1);

		}	// end "if (gProjectInfoPtr->numberStatisticsClasses > 0)"

   if (itemHit != 2)
		{
      projectWindowInfoPtr = (WindowInfoPtr)GetHandlePointer (
																gProjectInfoPtr->windowInfoHandle);
      numberChannels = projectWindowInfoPtr->totalNumberChannels;

      returnFlag = ChangeProjectChannelsList (numberChannels);

      if (returnFlag)
			{
         ClearProjectStatistics (2);
         gUpdateProjectMenuItemsFlag = TRUE;

			}	// end "if (returnFlag)"

		}	// end "if (itemHit != 2)"

   return (returnFlag);

}	// end "ProjectMenuClearStatistics" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReleaseClassifySpecsMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory that makes up the classification
//							specification structure.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			CloseProjectStructure in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/14/1988
//	Revised By:			Larry L. Biehl			Date: 11/16/1999

void ReleaseClassifySpecsMemory (
				Handle*								classifySpecsHandlePtr)
 {
   ClassifySpecsPtr					classifySpecsPtr;


   if (*classifySpecsHandlePtr != NULL)
		{
				// Get pointer to classification specifications.

      classifySpecsPtr = (ClassifySpecsPtr)GetHandlePointer (
																			*classifySpecsHandlePtr,
																			kLock);

				// Release memory for classification class areas vector if it has
				// been allocated.

      UnlockAndDispose (classifySpecsPtr->classAreaHandle);

				// Release memory for classification classes vector if it has
				// been allocated.

      UnlockAndDispose (classifySpecsPtr->classHandle);

				// Release memory for classification feature pointer vector if it
				// has been allocated.

      UnlockAndDispose (classifySpecsPtr->featureHandle);

				// Release memory for classification channels vector if it has
				// been allocated.

      UnlockAndDispose (classifySpecsPtr->channelsHandle);

				// Release memory for class symbols vector if it has been
				// allocated.

      UnlockAndDispose (classifySpecsPtr->symbolsHandle);

				// Release memory for threshold probability table.

      UnlockAndDispose (classifySpecsPtr->thresholdProbabilityHandle);

				// Release memory for the threshold table.

      UnlockAndDispose (classifySpecsPtr->thresholdTableHandle);

				// Release memory for the CEM parameters if needed.


      CEMParametersPtr cemParametersPtr = (CEMParametersPtr)GetHandlePointer (
																	classifySpecsPtr->cemParametersH,
																	kLock);
      if (cemParametersPtr != NULL)
         UnlockAndDispose (cemParametersPtr->correlationMatrixClassHandle);

      UnlockAndDispose (classifySpecsPtr->cemParametersH);

				// Release memory for the decision tree parameters if needed.

      UnlockAndDispose (classifySpecsPtr->decisionTreeVarH);

				// Release memory for the echo parameters if needed.

      UnlockAndDispose (classifySpecsPtr->echoClassifierVarH);


      *classifySpecsHandlePtr = UnlockAndDispose (*classifySpecsHandlePtr);

		}	// end "if (*classifySpecsHandlePtr != NULL)"

	gClassifySpecsPtr = NULL;

}	// end "ReleaseClassifySpecsMemory" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReleaseClassInfoMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory that was assigned to the class info
//							structure.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 07/02/1996
//	Revised By:			Larry L. Biehl			Date: 06/17/2006

void ReleaseClassInfoMemory (
				ClassInfoPtr						classInfoPtr,
				UInt16								numberClasses)

{
   if (classInfoPtr != NULL)
		{
				// Release memory that was assigned.

      CheckAndDisposePtr (classInfoPtr[0].covariancePtr);

      CheckAndDisposePtr (classInfoPtr[0].inversePtr);

      CheckAndDisposePtr (classInfoPtr[0].meanPtr);

      CheckAndDisposePtr (classInfoPtr[0].transformedCovPtr);

      CheckAndDisposePtr (classInfoPtr[0].transformedMeanPtr);

      UInt16 index;

      for (index=0; index<numberClasses; index++)
         CheckAndDisposePtr (classInfoPtr[index].dataValuesPtr);

		}	// end "if (classInfoPtr)"

   classInfoPtr = (ClassInfoPtr)CheckAndDisposePtr ((Ptr)classInfoPtr);

}	// end "ReleaseClassInfoMemory" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReleaseFeatureExtractionSpecsMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory that makes up the classification
//							specification structure.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			CloseProjectStructure in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/02/1992
//	Revised By:			Larry L. Biehl			Date: 11/16/1999

void ReleaseFeatureExtractionSpecsMemory (
				Handle*								featureExtractionSpecsHandlePtr)

{
	if (*featureExtractionSpecsHandlePtr != NULL)
		{
				// Get pointer to classification specifications.

      gFeatureExtractionSpecsPtr = (FeatureExtractionSpecsPtr)GetHandlePointer (
																*featureExtractionSpecsHandlePtr,
																kLock);

				// Release memory for the preprocessing specifications.

      ProjectionPursuitSpecsPtr projectionPursuitSpecsPtr =
										&gFeatureExtractionSpecsPtr->projectionPursuitParams;

      UnlockAndDispose (projectionPursuitSpecsPtr->bandGroupingHandle);
      UnlockAndDispose (projectionPursuitSpecsPtr->transformValueHandle);
      UnlockAndDispose (projectionPursuitSpecsPtr->transformVectorHandle);

				// Release memory for classes vector if it has been allocated.

      UnlockAndDispose (gFeatureExtractionSpecsPtr->classHandle);

				// Release memory for the eigenvalues handle.

      UnlockAndDispose (gFeatureExtractionSpecsPtr->eigenValueHandle);

				// Release memory for the eigenvector handle.

      UnlockAndDispose (gFeatureExtractionSpecsPtr->eigenVectorHandle);

				// Release memory for feature pointer vector if it
				// has been allocated.

      UnlockAndDispose (gFeatureExtractionSpecsPtr->featureHandle);

				// Release memory for channels vector if it has
				// been allocated.

      UnlockAndDispose (gFeatureExtractionSpecsPtr->channelsHandle);

      *featureExtractionSpecsHandlePtr =
										UnlockAndDispose (*featureExtractionSpecsHandlePtr);

		}	// end "if (*featureExtractionSpecsHandle != NULL)" 

   gFeatureExtractionSpecsPtr = NULL;

}	// end "ReleaseFeatureExtractionSpecsMemory" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReleaseSeparabilitySpecsMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory that makes up the separability specification 
//							structure.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			CloseProjectStructure in SProject.cpp
//
//	Coded By:			Larry L. Biehl			Date: 10/25/1989
//	Revised By:			Larry L. Biehl			Date: 11/16/1999

void ReleaseSeparabilitySpecsMemory (
				Handle*								separabilitySpecsHandlePtr)

{
   SeparabilitySpecsPtr				separabilitySpecsPtr;


   if (*separabilitySpecsHandlePtr != NULL) 
		{
				// Get pointer to separability specifications.							

      separabilitySpecsPtr = (SeparabilitySpecsPtr)GetHandlePointer (
																			*separabilitySpecsHandlePtr,
																			kLock);

				// Release memory for separability feature pointer vector if it 	
				// has been allocated.															

      UnlockAndDispose (separabilitySpecsPtr->featureHandle);

				// Release memory for separability channels vector if it has		
				// been allocated.																

      UnlockAndDispose (separabilitySpecsPtr->channelsHandle);

				// Release memory for separability classes vector if it has			
				// been allocated.																

      UnlockAndDispose (separabilitySpecsPtr->classHandle);

				// Release memory for separability symbols vector if it has been 	
				// allocated.																		

      UnlockAndDispose (separabilitySpecsPtr->symbolsHandle);

				// Release memory for all possible channel combinations vector 	
				// if it has been allocated.													

      UnlockAndDispose (separabilitySpecsPtr->allChanCombinationsHandle);

				// Release memory for separability channel combinations vector 	
				// if it has been allocated.													

      UnlockAndDispose (separabilitySpecsPtr->channelCombinationsHandle);

				// Release memory for separability show combinations vector 		
				// if it has been allocated.													

      UnlockAndDispose (separabilitySpecsPtr->showCombinationsH);

				// Release memory for separability exclude combinations vector 	
				// if it has been allocated.													

      UnlockAndDispose (separabilitySpecsPtr->excludeCombinationsH);

				// Release memory for separability show/exclude combinations  		
				// vector if it has been allocated.											

      UnlockAndDispose (separabilitySpecsPtr->showExcludeCombinationsH);

				// Release memory for any saved separability distance measures.	

      if (separabilitySpecsPtr->channelSetsPtr != NULL)
         CheckAndDisposePtr ((char*)separabilitySpecsPtr->channelSetsPtr);

      if (separabilitySpecsPtr->distancesPtr != NULL)
         CheckAndDisposePtr ((char*)separabilitySpecsPtr->distancesPtr);

      if (separabilitySpecsPtr->distancesSummaryPtr != NULL)
         CheckAndDisposePtr ((char*)separabilitySpecsPtr->distancesSummaryPtr);

      *separabilitySpecsHandlePtr = UnlockAndDispose (*separabilitySpecsHandlePtr);

		}	// end "if (*separabilitySpecsHandlePtr != NULL)"

}	// end "ReleaseSeparabilitySpecsMemory" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReleaseStatHistogramSpecsMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory that makes up the statistics histogram
//							specification structure.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			CloseProjectStructure  in SProject.cpp
//							LoadStatHistogramSpecs in SProjectHistogramStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/09/1990
//	Revised By:			Larry L. Biehl			Date: 11/16/1999

void ReleaseStatHistogramSpecsMemory (
				Handle*								statHistogramSpecsHPtr)

{
   StatHistogramSpecsPtr			statHistogramSpecsPtr;


   if (*statHistogramSpecsHPtr != NULL) 
		{
				// Get pointer to statistics histogram specifications.				

      statHistogramSpecsPtr = (StatHistogramSpecsPtr)GetHandlePointer (
																			*statHistogramSpecsHPtr,
																			kLock);

				// Release memory for statistics histogram classes vector if it has been
				// allocated.

      UnlockAndDispose (statHistogramSpecsPtr->classHandle);

				// Release memory for statistics histogram fields vector if it	has been
				// allocated.

      UnlockAndDispose (statHistogramSpecsPtr->fieldHandle);

				// Release memory for statistics histogram feature pointer vector if it
				// has been allocated.

      UnlockAndDispose (statHistogramSpecsPtr->featureHandle);

				// Release memory for statistics histogram channels vector if it has
				// been allocated.

      UnlockAndDispose (statHistogramSpecsPtr->channelsHandle);

      *statHistogramSpecsHPtr = UnlockAndDispose (*statHistogramSpecsHPtr);

		}	// end "if (*statHistogramSpecsHPtr != NULL)" 

   gStatHistogramSpecsPtr = NULL;

}	// end "ReleaseStatHistogramSpecsMemory" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ReleaseStatisticsEnhanceSpecsMemory
//
//	Software purpose:	The purpose of this routine is to release the 
//							memory that makes up the statistics enhancement
//							specification structure.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None	
// 
// Called By:			CloseProjectStructure  in SProject.cpp
//							LoadStatisticsEnhanceSpecs in SStatisticsEnhancement.cpp
//
//	Coded By:			Larry L. Biehl			Date: 11/30/1993
//	Revised By:			Larry L. Biehl			Date: 11/16/1999

void ReleaseStatisticsEnhanceSpecsMemory (
				Handle*								statisticsEnhanceSpecsHPtr)
 {
   StatEnhanceSpecsPtr				statEnhanceSpecsPtr;


   if (*statisticsEnhanceSpecsHPtr != NULL) 
		{
				// Get pointer to statistics histogram specifications.				

      statEnhanceSpecsPtr = (StatEnhanceSpecsPtr)GetHandlePointer (
																		*statisticsEnhanceSpecsHPtr,
																		kLock);

				// Release memory for statistics histogram classes vector if it 	
				// has been allocated.															

      UnlockAndDispose (statEnhanceSpecsPtr->classHandle);

      *statisticsEnhanceSpecsHPtr = UnlockAndDispose (*statisticsEnhanceSpecsHPtr);

		}	// end "if (*statisticsEnhanceSpecsHPtr != NULL)" 

   gStatEnhanceSpecsPtr = NULL;

}	// end "ReleaseStatisticsEnhanceSpecsMemory" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt32 SetClassListSelections
//
//	Software purpose:	The purpose of this routine is to set the items
//							in the class list that are currently selected.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/30/1998
//	Revised By:			Larry L. Biehl			Date: 04/30/1998

SInt32 SetClassListSelections (
				#if defined multispec_wx
					wxListBox*							listHandle,
				#else
					ListHandle							listHandle,
				#endif
				SInt32								numberInputClasses,
				SInt32								numberSelectedClasses,
				UInt16*								classPtr)
				
{
   Cell									cell;

   SInt32								classIndex,
											index;

   UInt16								classNum;

   cell.h = 0;
   index = 0;
   for (classIndex=0; classIndex<numberInputClasses; classIndex++) 
		{
      cell.v = (SInt16)classIndex;
      classNum = (UInt16)(classIndex + 1);
      if (index < numberSelectedClasses && classPtr[index] == classNum) 
			{
         LSetSelect (TRUE, cell, listHandle);
         index++;

			}	// end "if (index < *numberOutputItems && ...)" 

		}	// end "for (classIndex=0; classIndex<..."

			// Scroll to the hilited list item.												

   LAutoScroll (listHandle);

   return (index);

}	// end "SetClassListSelections" 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 StatHistogramPopUpMenu
//
//	Software purpose:	The purpose of this routine is to handle a 
//							mousedown in the statistics histogram pop up 
//							menu location in the statistics window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 04/18/1990
//	Revised By:			Larry L. Biehl			Date: 12/24/2002

SInt16 StatHistogramPopUpMenu (
				SInt16								statsWindowMode)

{
   Point									histogramPopUpLoc;

   SInt32								menuData;

   UInt32								item,
											numberItems;

   SInt16								returnCode;


			// Get location of histogram pop up box and convert it to global.	

   GetControlBounds (gProjectInfoPtr->histogramControlH, &gTempRect);
   histogramPopUpLoc.v = gTempRect.top + 1;
   histogramPopUpLoc.h = gTempRect.left;
   LocalToGlobal (&histogramPopUpLoc);

			// Delete any menu items already in the popup menu.

   numberItems = CountMenuItems (gPopUpTemporaryMenu);
   for (item=0; item<numberItems; item++)
      DeleteMenuItem (gPopUpTemporaryMenu, 1);

			// Set up the items on the menu.

   switch (statsWindowMode) 
		{
      case 2:
         AppendMenu (gPopUpTemporaryMenu, "\pHistogram Classes");
         AppendMenu (gPopUpTemporaryMenu, "\pHistogram Fields");
         AppendMenu (gPopUpTemporaryMenu, "\pHistogram...");
         numberItems = 3;
         break;

      case 3:
         AppendMenu (gPopUpTemporaryMenu, "\pHistogram Class");
         AppendMenu (gPopUpTemporaryMenu, "\pHistogram Fields");
         AppendMenu (gPopUpTemporaryMenu, "\pHistogram...");
         numberItems = 3;
         break;

      case 4:
         AppendMenu (gPopUpTemporaryMenu, "\pHistogram Field");
         AppendMenu (gPopUpTemporaryMenu, "\pHistogram...");
         numberItems = 2;
         break;

		}	// end "switch (statsWindowMode)"

			// Insert the pop up menu and then let user make a selection.			

   InsertMenu (gPopUpTemporaryMenu, -1);

   menuData = PopUpMenuSelect (gPopUpTemporaryMenu,
											histogramPopUpLoc.v, 
											histogramPopUpLoc.h, 
											1);

			// Now delete the menu items.

   for (item=0; item<numberItems; item++)
      DeleteMenuItem (gPopUpTemporaryMenu, 1);

			// Remove the pop up menu.															

   DeleteMenu (kColorMenuID);

			// Now get the proper code to be return relative to the current statistics 
			// mode.

   switch (statsWindowMode) 
		{
      case 2:
      case 3:
         returnCode = LoWord (menuData);
         break;

      case 4:
         returnCode = LoWord (menuData);
         if (returnCode > 0)
            returnCode += 1;
         break;

		}	// end "switch (statsWindowMode)"

			// Return the selection that was made.											

   return (returnCode);

}	// end "StatHistogramPopUpMenu"
#endif	// defined multispec_mac 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 StatisticsPopUpMenu
//
//	Software purpose:	The purpose of this routine is to handle a 
//							mousedown in the statistics pop up 
//							menu location in the statistics window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:			StatisticsWMouseDn in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/18/1990
//	Revised By:			Larry L. Biehl			Date: 03/08/2000

SInt16 StatisticsPopUpMenu (
				SInt16								statsWindowMode)

{
   Point									statisticsPopUpLoc;

   SInt32								menuData;

   SInt16								classStorage,
											menuItem,
											newMenuItem;

   Boolean								optionKeyFlag;


			// Get location of statistics pop up box and convert it to global	

   statisticsPopUpLoc.v = gProjectInfoPtr->popUpCovarianceToUseBox.top + 1;
   statisticsPopUpLoc.h = gProjectInfoPtr->popUpCovarianceToUseBox.left;
   LocalToGlobal (&statisticsPopUpLoc);

			// Invert the prompt box													

   InvertRect (&gProjectInfoPtr->promptCovarianceToUseBox);

			// Insert the pop up menu and then let user make a selection	

   InsertMenu (gPopUpStatisticsTypeMenu, -1);
   EraseRect (&gProjectInfoPtr->popUpCovarianceToUseBox);

			// Determine if enhanced statistics exist and set the menu item accordingly.

   if (statsWindowMode == kClassListMode) 
		{
      if (gProjectInfoPtr->enhancedStatsExistFlag)
         EnableMenuItem (gPopUpStatisticsTypeMenu, 3);

      else	// !gProjectInfoPtr->enhancedStatsExistFlag 
         DisableMenuItem (gPopUpStatisticsTypeMenu, 3);

				// Add the mixed statistics item if needed for the project specific popup.

      if (CountMenuItems (gPopUpStatisticsTypeMenu) == 3)
         InsertMenuItem (gPopUpStatisticsTypeMenu, "\pMixed", 4);

      DisableMenuItem (gPopUpStatisticsTypeMenu, 4);

				// Get Current selected menu item.

      menuItem = GetProjectStatisticsTypeMenuItem ();

		}	// end "if (statsWindowMode == kClassListMode)"

   else	// statsWindowMode == kFieldListMode
		{
      classStorage = gProjectInfoPtr->storageClass[gProjectInfoPtr->currentClass];

      if (gProjectInfoPtr->classNamesPtr[classStorage].modifiedStatsFlag)
         EnableMenuItem (gPopUpStatisticsTypeMenu, 3);

      else	// !gProjectInfoPtr->classNamesPtr[classStorage].modifiedStatsFlag
         DisableMenuItem (gPopUpStatisticsTypeMenu, 3);

				// Delete the mixed statistics item for the class specific popup.

      DeleteMenuItem (gPopUpStatisticsTypeMenu, 4);

				// Get Current selected menu item.

      menuItem = GetClassStatisticsTypeMenuItem ();

		}	// end "else statsWindowMode == kFieldListMode"

			// If the option key is down, then add "..." to the LOOC to indicate that
			// a dialog box can be opened to enter a LOOC mixing parameter.

   optionKeyFlag = FALSE;
   if ((gEventRecord.modifiers & optionKey) > 0) 
		{
      optionKeyFlag = TRUE;
      SetMenuItemText (gPopUpStatisticsTypeMenu,
								2,
								"\pLOOC ...");

		}	// end "if ((gEventRecord.modifiers & optionKey) > 0)"

   else	// (gEventRecord.modifiers & optionKey) <= 0
      SetMenuItemText (gPopUpStatisticsTypeMenu,
								2,
								"\pLOOC");

			// Now set check marks accordingly.

   CheckMenuItem (gPopUpStatisticsTypeMenu,
						1,
						menuItem == 1);

   CheckMenuItem (gPopUpStatisticsTypeMenu,
						2,
						menuItem == 2);

   CheckMenuItem (gPopUpStatisticsTypeMenu,
						3,
						menuItem == 3);

   if (CountMenuItems (gPopUpStatisticsTypeMenu) == 4)
      CheckMenuItem (gPopUpStatisticsTypeMenu,
							4,
							(menuItem == 4));

   menuData = PopUpMenuSelect (gPopUpStatisticsTypeMenu,
											statisticsPopUpLoc.v,
											statisticsPopUpLoc.h,
											menuItem);

			// Invert the prompt box again to make normal and remove the	
			// the statistics type pop up menu													

   InvertRect (&gProjectInfoPtr->promptCovarianceToUseBox);
   DeleteMenu (kPopUpStatisticsTypeMenuID);

			// Make the selection the new current class if a selection was	
			// made; then redraw the class pop up box to reflect the			
			// new current class in case it is different.						

   newMenuItem = LoWord (menuData);

   InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpCovarianceToUseBox);

   if (optionKeyFlag && newMenuItem == 2) 
		{
      if (!LOOCOptionsDialog (statsWindowMode))
         newMenuItem = menuItem;

      InvalWindowRect (gProjectWindow, &gProjectInfoPtr->popUpCovarianceToUseBox);

		}	// end "if (optionKeyFlag && newMenuItem == 2)"          

			// Return the selection that was made.											

   return (newMenuItem);

}	// end "StatisticsPopUpMenu"
#endif	// defined multispec_mac 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 StatListPopUpMenu
//
//	Software purpose:	The purpose of this routine is to handle a mousedown in the 
//							statistics list pop up menu location in the statistics window.
//
//	Parameters in:					
//
//	Parameters out:				
//
// Value Returned:	
//
// Called By:	
//
//	Coded By:			Larry L. Biehl			Date: 10/09/1997
//	Revised By:			Larry L. Biehl			Date: 10/09/1997

SInt16 StatListPopUpMenu (
				SInt16								statsWindowMode)

{
   Point									listPopUpLoc;

   SInt32								menuData;

   UInt32								item,
											numberItems;

   SInt16								returnCode;


			// Get location of histogram pop up box and convert it to global.		

   GetControlBounds (gProjectInfoPtr->listControlH, &gTempRect);
   listPopUpLoc.v = gTempRect.top + 1;
   listPopUpLoc.h = gTempRect.left;
   LocalToGlobal (&listPopUpLoc);

			// Delete any menu items already in the popup menu.

   numberItems = CountMenuItems (gPopUpTemporaryMenu);
   for (item=0; item<numberItems; item++)
      DeleteMenuItem (gPopUpTemporaryMenu, 1);

			// Set up the items on the menu.

   switch (statsWindowMode) 
		{
      case 2:
         AppendMenu (gPopUpTemporaryMenu, "\pList Classes Stats");
         AppendMenu (gPopUpTemporaryMenu, "\pList Classes & Fields Stats");
         AppendMenu (gPopUpTemporaryMenu, "\pList Fields Stats");
         AppendMenu (gPopUpTemporaryMenu, "\pList Stats...");
         numberItems = 4;
         break;

      case 3:
         AppendMenu (gPopUpTemporaryMenu, "\pList Class Stats");
         AppendMenu (gPopUpTemporaryMenu, "\pList Class & Fields Stats");
         AppendMenu (gPopUpTemporaryMenu, "\pList Fields Stats");
         AppendMenu (gPopUpTemporaryMenu, "\pList Stats...");
         numberItems = 4;
         break;

      case 4:
         AppendMenu (gPopUpTemporaryMenu, "\pList Field Stats");
         AppendMenu (gPopUpTemporaryMenu, "\pList Stats...");
         numberItems = 2;
         break;

		}	// end "switch (statsWindowMode)"

			// Insert the pop up menu and then let user make a selection.			

   InsertMenu (gPopUpTemporaryMenu, -1);

   menuData = PopUpMenuSelect (gPopUpTemporaryMenu, listPopUpLoc.v, listPopUpLoc.h, 1);

			// Now delete the menu items.

   for (item=0; item<numberItems; item++)
      DeleteMenuItem (gPopUpTemporaryMenu, 1);

			// Remove the pop up menu.															

   DeleteMenu (kColorMenuID);

			// Now get the proper code to be return relative to the current statistics 
			// mode.

   switch (statsWindowMode) 
		{
      case 2:
      case 3:
         returnCode = LoWord (menuData);
         break;

      case 4:
         returnCode = LoWord (menuData);
         if (returnCode > 0)
            returnCode += 2;
         break;

		}	// end "switch (statsWindowMode)"

			// Return the selection that was made.											

   return (returnCode);

}	// end "StatListPopUpMenu"
#endif	// defined multispec_mac 



#if defined multispec_mac
//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean SymbolsDialog
//
//	Software purpose:	The purpose of this routine is to display a dialog
//							with a list of the classes and symbols that the user
//							can set. 
//
//	Parameters in:		None
//
//	Parameters out:	None
//
// Value Returned:  	None
//
// Called By:			ClassifyDialog   		in SClassifyDialogs.cpp
//							SeparabilityDialog   in SFeatureSelection.cpp
//							ClusterDialog   		in SCluster.cpp
//
//
//	Coded By:			Larry L. Biehl			Date: 07/25/1990
//	Revised By:			Larry L. Biehl			Date: 07/22/1999	

Boolean SymbolsDialog (
				UInt16								numberOfClassesToUse,
				SInt16*								classPtr,
				unsigned char*						symbolsPtr,
				Boolean								clusterFlag)
 {
   DialogPtr							dialogPtr;

   Cell									cell;

   SInt16								classIndex,
											fixedCellWidth = 33,
											index,
											stringLength;

   Boolean								returnFlag = FALSE;


			// Get the modal dialog for the reformat specification					

   dialogPtr = LoadRequestedDialog (kSymbolsDialogID, kCopyScrap, 1, 2);
   if (dialogPtr == NULL)
																						return (FALSE);

			// Get the list handle for the class-symbol list.

	gDialogListHandle = GetOneColumnDialogListHandle (dialogPtr, 5);

			// Get the list handle for the symbols list.

   if (gDialogListHandle != NULL)
      gDialogListHandle2 = GetOneColumnDialogListHandle (dialogPtr, 10);

			// Continue if there are valid dialog list handles.						

   if (gDialogListHandle != NULL && gDialogListHandle2 != NULL) 
		{
      (*gDialogListHandle)->selFlags = lNoRect + lNoExtend + lUseSense;
      (*gDialogListHandle2)->selFlags = lOnlyOne;

				// Item 4: Load the list header.												

      if (clusterFlag)
         LoadDItemString (dialogPtr, 4, (Str255*)"\pSymbol-Cluster class number");

      else	// !clusterFlag 
         LoadDItemString (dialogPtr, 4, (Str255*)"\pSymbol-Class name");

				//	Item 5: Load address of routine that will load the class 		
				// and symbol list.																

      SetDialogItemDrawRoutine (dialogPtr, 5, gCreateOneColumnList1Ptr);

				//	Item 7: Unhilite the '<-' button.										

      SetDLogControlHilite (dialogPtr, 7, 255);

				//	Item 8: Load blank in for edit symbol.									

      LoadDItemString (dialogPtr, 8, (Str255*) "\p");

				//	Item 10: Load address of routine that will load the	default	
				// symbol list.																	

      SetDialogItemDrawRoutine (dialogPtr, 10, gCreateOneColumnList2Ptr);

				// Item 11: Set routine to draw outline on "OK" box.					

      //SetDialogItemDrawRoutine (dialogPtr, 11, gHiliteOKButtonPtr);

				// Center the dialog and then show it.										

      ShowDialogWindow (dialogPtr, kSymbolsDialogID, kDoNotSetUpDFilterTable);

				// Get deactivate event for the window that is now behind the  	
				// dialog and handle it.														

      CheckSomeEvents (activMask + updateMask);

				// Load the class list.															

      LoadClassList (dialogPtr, 
								gDialogListHandle,
								numberOfClassesToUse, 
								classPtr, 
								symbolsPtr, 
								clusterFlag);

				// Load the symbol list.														

      if (gMemoryError == noErr)
         LoadSymbolList (dialogPtr, gDialogListHandle2);

      gDialogItemDescriptorPtr[8] = kDItemString;

				// Handle modal dialog selections for one column class list.		

      returnFlag = FALSE;
      if (gMemoryError == noErr)
         returnFlag = ModalSymbolsDialog (dialogPtr, numberOfClassesToUse);

      if (returnFlag) 
			{
					// Get the new symbols.														

         cell.h = 0;
         cell.v = 0;
         for (index = 0; index < numberOfClassesToUse; index++) 
				{
            if (classPtr != NULL)
               classIndex = classPtr[index];

            else	// classPtr == NULL 
               classIndex = index + 1;

            stringLength = fixedCellWidth;
            LGetCell (gTextString2, &stringLength, cell, gDialogListHandle);

            BlockMoveData (
							(char*)&gTextString2[0], (char*)&symbolsPtr[classIndex], 1);

            cell.v++;

				}	// end "for (index=0; index<numberOfClassesToUse; ...)" 

			}	// end "if (returnFlag)" 

				// Make certain that global memory error is set back to 'noErr'.	

      gMemoryError = noErr;

		}	// end "if (gDialogListHandle != NULL && ...)" 

   if (gDialogListHandle != NULL)
      LDispose (gDialogListHandle);
   gDialogListHandle = NULL;

   if (gDialogListHandle2 != NULL)
      LDispose (gDialogListHandle2);
   gDialogListHandle2 = NULL;

   CloseRequestedDialog (dialogPtr, kDoNotSetUpDFilterTable);

   return (returnFlag);

}	// end "SymbolsDialog" 
#endif	// defined multispec_mac 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UnlockProjectMemory
//
//	Software purpose:	The purpose of this routine is to unlock the memory
//							for the project.  This is usually done when memory for
//							the project needs to change.  This will allow memory to
// 						move and not fragment it.
//
//	Parameters in:		unlockCode =0 - unlock all project handles and project
//									structure itself.
//							unlockCode =1 - unlock all project handles.
//							unlockCode =2 - unlock class/field/point handles and
//											statistics handles.
//							unlockCode =3 - unlock statistics handles.
//
//	Parameters out:	None
//
// Value Returned:	None				
// 
// Called By:			Menus in MMenus.c
//							SetupModifiedStatsMemory in SProjectComputeStatistics.cpp
//							SetupStatsMemory in SProjectComputeStatistics.cpp
//							AddClassToProject in SStatistics.cpp
//							AddFieldToProject in SStatistics.cpp
//
//	Coded By:			Larry L. Biehl			Date: 03/06/1991
//	Revised By:			Larry L. Biehl			Date: 01/09/1999	

void UnlockProjectMemory (
				ProjectInfoPtr*					inputProjectInfoPtrPtr,
				SInt16								unlockCode,
				Handle*								outputProjectInfoHandlePtr)

{
   ProjectInfoPtr						localProjectInfoPtr;

   Handle								projectInfoHandle;


			// Initialize local variables.													

   localProjectInfoPtr = *inputProjectInfoPtrPtr;

   if (localProjectInfoPtr != NULL) 
		{
      if (unlockCode <= 1) 
			{
					// UnLock handle to class list information.							

         CheckAndUnlockHandle (localProjectInfoPtr->storageClassHandle);
         localProjectInfoPtr->storageClass = NULL;

					// UnLock handle to channel information.								

         CheckAndUnlockHandle (localProjectInfoPtr->channelHandle);
         localProjectInfoPtr->channelsPtr = NULL;

			}	// end "if (unlockCode == 1)" 

      if (unlockCode <= 2) 
			{
					// UnLock handle to class name information.							

         CheckAndUnlockHandle (localProjectInfoPtr->classNamesHandle);
         localProjectInfoPtr->classNamesPtr = NULL;

					// UnLock handle to field name information.							

         CheckAndUnlockHandle (localProjectInfoPtr->fieldIdentifiersHandle);
         localProjectInfoPtr->fieldIdentPtr = NULL;

					// UnLock handle to field coordinates.									

         CheckAndUnlockHandle (localProjectInfoPtr->fieldCoordinatesHandle);
         localProjectInfoPtr->fieldPointsPtr = NULL;

			}	// end "if (unlockCode <= 2)" 

      if (unlockCode <= 3) 
			{
					// UnLock handles to common covariance statistics.							

         CheckSizeAndUnlockHandle (localProjectInfoPtr->commonChannelStatsHandle);
         CheckSizeAndUnlockHandle (localProjectInfoPtr->commonCovarianceStatsHandle);

					// UnLock handle to class channel statistics.						

         CheckSizeAndUnlockHandle (localProjectInfoPtr->classChanStatsHandle);
         localProjectInfoPtr->classChanStatsPtr = NULL;

					// UnLock handle to class covariance statistics.					

         CheckSizeAndUnlockHandle (localProjectInfoPtr->classSumSquaresStatsHandle);
         localProjectInfoPtr->classSumSquaresStatsPtr = NULL;

					// Unlock handle to modified class channel statistics.			

         CheckSizeAndUnlockHandle (localProjectInfoPtr->modifiedClassChanStatsHandle);
         localProjectInfoPtr->modifiedClassChanStatsPtr = NULL;

					// Unlock handle to modified class covariance statistics.		

         CheckSizeAndUnlockHandle (localProjectInfoPtr->modifiedClassCovStatsHandle);
         localProjectInfoPtr->modifiedClassCovStatsPtr = NULL;

					// UnLock handle to field channel statistics.						

         CheckSizeAndUnlockHandle (localProjectInfoPtr->fieldChanStatsHandle);
         localProjectInfoPtr->fieldChanStatsPtr = NULL;

					// UnLock handle to field covariance statistics.					

         CheckSizeAndUnlockHandle (localProjectInfoPtr->fieldSumSquaresStatsHandle);
         localProjectInfoPtr->fieldSumSquaresStatsPtr = NULL;

			}	// end "if (unlockCode <= 3)" 

      localProjectInfoPtr->handlesLockedFlag = FALSE;

      if (unlockCode == 0 && outputProjectInfoHandlePtr) 
			{
					// UnLock handle to project structure.									

         projectInfoHandle = localProjectInfoPtr->projectInfoHandle;
         CheckAndUnlockHandle (projectInfoHandle);

         *outputProjectInfoHandlePtr = projectInfoHandle;
         *inputProjectInfoPtrPtr = NULL;

			}	// end "if (unlockCode == 0 && ...)" 

		}	// end "if (gProjectInfoPtr != NULL)" 

}	// end "UnlockProjectMemory" 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 UpdateDialogClassWeightsInfo
//
//	Software purpose:	The purpose of this routine is to update the
//							information associated with the dialog class weights. The '1'
//							bin for classWeightSet contains the weight code for
//							non-enhanced statistics and the '2' bin contains the code
//							for enhanced statistics. A 0 in the bin represents equal
//							weighting and a 1 represents unequal weighting.
//
//	Parameters in:		.
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:			ClassifyDialog in SClassifyDialogs.cpp
//							FeatureExtractionDialog in SFeatureExtraction.cpp
//							StatisticsEnhanceDialog in SStatisticsEnhancement.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1993
//	Revised By:			Larry L. Biehl			Date: 11/05/1999

SInt16 UpdateDialogClassWeightsInfo (
				SInt16								currentWeightsSelection,
				Boolean								useEnhancedStatisticsFlag,
				SInt16*								classWeightSetCodePtr,
				Boolean								initializeClassWeightSetFlag)

{
	SInt16								classWeightSet,
											newWeightsSelection;


   classWeightSet = kEqualWeights;
   if (classWeightSetCodePtr != NULL)
      classWeightSet = *classWeightSetCodePtr;

			// Initialize the setting for the class weights code if requested.	

   if (initializeClassWeightSetFlag) 
		{
      classWeightSet = 0;

      if (gProjectInfoPtr->modifiedClassWeightSet == kUnequalWeights)
         classWeightSet += 2;

      if (gProjectInfoPtr->classWeightSet == kUnequalWeights)
         classWeightSet += 1;

		}	// end "if (initializeClassWeightSetFlag)"

   if (useEnhancedStatisticsFlag) 
		{
				// Save the previous setting for equal-unequal weights.				

      if (!initializeClassWeightSetFlag) 
			{
					// Clear the previous setting for non-enhanced statistics.

         classWeightSet = (classWeightSet & 0x0002);

					// Now set the current setting for non-enhanced statistics.

         if (currentWeightsSelection == kUnequalWeights)
            classWeightSet += 1;

			}	// end "if (!initializeClassWeightSetFlag)"

				// Initialize class weight variable for using enhanced statistics.																		

      newWeightsSelection = kEqualWeights;
      if (classWeightSet & 0x0002)
         newWeightsSelection = kUnequalWeights;

		}	// end "if (useEnhancedStatisticsFlag)" 

   else	// !useEnhancedStatisticsFlag 
		{
				// Save the previous setting for equal-unequal weights.				

      if (!initializeClassWeightSetFlag) 
			{
					// Clear the previous setting for enhanced statistics.

         classWeightSet = (classWeightSet & 0x0001);

					// Now set the current setting for enhanced statistics.

         if (currentWeightsSelection == kUnequalWeights)
            classWeightSet += 2;

			}	// end "if (!initializeClassWeightSetFlag)"

				// Initialize class weight variable for using non-enhanced statistics.																															

      newWeightsSelection = kEqualWeights;
      if (classWeightSet & 0x0001)
         newWeightsSelection = kUnequalWeights;

		}	// end "else !useEnhancedStatisticsFlag" 

   if (classWeightSetCodePtr != NULL)
      *classWeightSetCodePtr = classWeightSet;

   return (newWeightsSelection);

}	// end "UpdateDialogClassWeightsInfo"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void UpdateProjectClassWeights
//
//	Software purpose:	The purpose of this routine is to update the
//							project class weights with those supplied by the
//							user via a dialog box.
//
//	Parameters in:		.
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			ClassifyDialog in SClassifyDialogs.cpp
//							FeatureExtractionDialog in SFeatureExtraction.cpp
//
//	Coded By:			Larry L. Biehl			Date: 12/28/1993
//	Revised By:			Larry L. Biehl			Date: 03/10/1994

void UpdateProjectClassWeights (
				Boolean								useEnhancedStatisticsFlag,
				SInt16								weightsSelection,
				float*								classWeightsPtr)

{
   HPClassNamesPtr					classNamesPtr;

   SInt16								classStorage,
											weightsIndex;

   UInt16								index;


   if (gProjectInfoPtr == NULL)
																									return;

   weightsSelection = abs (weightsSelection);

   if (useEnhancedStatisticsFlag)
      gProjectInfoPtr->modifiedClassWeightSet = weightsSelection;

   else	// !useEnhancedStatisticsFlag 
      gProjectInfoPtr->classWeightSet = weightsSelection;

   if (weightsSelection == kUnequalWeights) // Unequal weights 
		{
      classNamesPtr = gProjectInfoPtr->classNamesPtr;

				// Move the new weights to the appropriate location.				

      weightsIndex = GetClassWeightsIndex (useEnhancedStatisticsFlag,
															FALSE);

      classWeightsPtr += weightsIndex * gProjectInfoPtr->numberStatisticsClasses;

      for (index=0;
				index<gProjectInfoPtr->numberStatisticsClasses;
					index++) 
			{
         classStorage = gProjectInfoPtr->storageClass[index];

         classNamesPtr[classStorage].priorWeights[weightsIndex] = *classWeightsPtr;

         classWeightsPtr++;

			}	// end "for (index=0; index<numberClasses; index++)" 

		}	// end "if (weightsSelection == kUnequalWeights)" 

}	// end "UpdateProjectClassWeights"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean VerifyAreaDescription
//
//	Software purpose:	The purpose of this routine is to update the
//							project class weights with those supplied by the
//							user via a dialog box.
//
//	Parameters in:		.
//
//	Parameters out:	None
//
// Value Returned:	None		
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 03/20/1999
//	Revised By:			Larry L. Biehl			Date: 03/20/1999

Boolean VerifyAreaDescription (
				AreaDescriptionPtr				areaDescriptionPtr)

{
   Boolean								definedAreaFlag = TRUE;


			// Now verify that the field area is still within image area.
			// If not, set the flag indicating that the area is not defined.

   areaDescriptionPtr->columnStart = MAX (1, areaDescriptionPtr->columnStart);
   areaDescriptionPtr->columnEnd = MIN (areaDescriptionPtr->maxNumberColumns,
														areaDescriptionPtr->columnEnd);

   areaDescriptionPtr->lineStart = MAX (1, areaDescriptionPtr->lineStart);
   areaDescriptionPtr->lineEnd = MIN (areaDescriptionPtr->maxNumberLines,
													areaDescriptionPtr->lineEnd);

   if (areaDescriptionPtr->columnStart > areaDescriptionPtr->columnEnd ||
							areaDescriptionPtr->lineStart > areaDescriptionPtr->lineEnd)
      definedAreaFlag = FALSE;

   return (definedAreaFlag);

}	// end "VerifyAreaDescription"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean VerifyProjectStatsUpToDate
//
//	Software purpose:	The purpose of this routine is to verify that the project
//							statistics are up-to-date.  If they are not the statistics are
//							recalculated to be ready for using in algorithms that are in the
//							process of being used such as classify, feature extraction, etc.
//
//	Parameters in:		Number of classes in the class vector pointer
//							The class vector pointer
//							Minimum number of classes with statistics that are needed
//							Code indicating which covariance statistics are to be used
//							Code indicating whether the global information pointers should
//								be set up.
//
//	Parameters out:	Number of classes in the class vector pointer.  (If smaller than
//								on input, classes in the class vector with no available
//								statistics were removed)
//							The class vector pointer (list may have been revised as described
//								above)
//							Number of training pixels in the smallest class.
//
// Value Returned:	True if update worked okay and statistics are ready to be used. 
//							False if memory was not available for the update or if the
//								number of training classes is less than the minimum number
//								needed.
// 
// Called By:			BiPlotDataControl in SBiPlotData.cpp
//							FeatureExtractionControl in SFeatureExtraction.cpp
//							ClassifyControl in SClassify.cpp
//							ClusterControl in SCluster.cpp
//							SeparabilityControl in SFeatureSelection.cpp
//							EvaluateCovariancesControl in SOther.cpp
//							StatisticsEnhanceControl in SStatisticsEnhancement.cpp
//							StatisticsImageControl in SStatisticsImage.cpp
//
//	Coded By:			Larry L. Biehl			Date: 07/23/1997
//	Revised By:			Larry L. Biehl			Date: 05/03/2019

Boolean VerifyProjectStatsUpToDate (
				UInt32*								numberClassesPtr,
				SInt16*								classPtr,
				UInt32								minimumNumberClasses,
				SInt16								statisticsType,
				SInt16								covarianceStatsToUse,
				SInt16								setupGlobalInfoPointers,
				SInt32*								minimumNumberTrainPixelsPtr)
 {
   UInt32								localNumberClasses,
											savedNumberClasses;

   SInt16								returnCode;

   Boolean								computeCommonCovarianceFlag,
											continueFlag,
											updateStatsFlag;


   if (gProjectInfoPtr == NULL)
																						return (FALSE);

   continueFlag = TRUE;
   returnCode = 1;

			// Determine if the statistics for any of the classes to be used need to 
			// be updated.

   updateStatsFlag = !CheckClassStats (numberClassesPtr,
													classPtr,
													statisticsType,
													covarianceStatsToUse,
													TRUE,
													minimumNumberTrainPixelsPtr,
													&computeCommonCovarianceFlag);

   if (!updateStatsFlag && computeCommonCovarianceFlag) 
		{
				// The common covariance needs to be computed for the LOOC statistics.
				// Verify that all of the statistics classes are up-to-date if not
				// checked already.

      if (*numberClassesPtr < (UInt32)gProjectInfoPtr->numberStatTrainClasses) 
			{
         localNumberClasses = gProjectInfoPtr->numberStatisticsClasses;
         updateStatsFlag = !CheckClassStats (&localNumberClasses,
															NULL,
															statisticsType,
															covarianceStatsToUse,
															TRUE,
															minimumNumberTrainPixelsPtr,
															&computeCommonCovarianceFlag);

			}	// end "if (*numberClassesPtr < ...->numberStatTrainClasses)"

		}	// end "if (!updateStatsFlag && computeCommonCovarianceFlag)"

   if (!updateStatsFlag && computeCommonCovarianceFlag) 
		{
				// Get the common covariance for leave-one-out statistics.

      UpdateProjectLOOStats (kUpdateProject, NULL);

		}	// end "if (!updateStatsFlag && computeCommonCovaranceFlag)"

			// Force any local common covariance that is to be used to be created.

   gProjectInfoPtr->localCommonCovarianceLoadedFlag = FALSE;

   if (updateStatsFlag) 
		{
      returnCode = UpdateStatsControl (kUpdateProject, TRUE, FALSE);

				// Now get the project image file information pointers again. They
				// may have been released in the call to 'UpdateStatsControl'.

      if (!GetProjectImageFileInfo (kDoNotPrompt, setupGlobalInfoPointers))
         returnCode = 2;

      gProjectInfoPtr->moveMemoryFlag = TRUE;

		}	// end "if (if (updateStatsFlag)"

   if (returnCode != 2) 
		{
				// Make certain that the vector of classes to use is		
				// consistent with the statistics that are available. 	
				// It is possible for there to be a class specified 		
				// for use with no statistics.									

      savedNumberClasses = *numberClassesPtr;
      CheckClassStats (numberClassesPtr,
								classPtr,
								statisticsType,
								covarianceStatsToUse,
								FALSE,
								minimumNumberTrainPixelsPtr,
								&computeCommonCovarianceFlag);

				// If number of classes is 0, then put up an alert and skip the operation.									

      if (*numberClassesPtr < minimumNumberClasses) 
			{
         NoClassStatsAlert (minimumNumberClasses);
         if (*numberClassesPtr == 0)
            *numberClassesPtr = savedNumberClasses;
         continueFlag = FALSE;

			}	// end "if (*numberClassesPtr <= 0)" 

		}	// end "if (returnCode != 2)"

   else	// returnCode == 2 
      continueFlag = FALSE;

   return (continueFlag);

}	// end "VerifyProjectStatsUpToDate"



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		Boolean VerifyProjectTrainingPixelsLoaded
//
//	Software purpose:	The purpose of this routine is to verify that the project
//							training pixels have been loaded for use with the SVM or
//							KNN classifiers
//
//	Parameters in:		Number of classes in the class vector pointer
//							The class vector pointer
//							Minimum number of classes with statistics that are needed
//							Code indicating which covariance statistics are to be used
//							Code indicating whether the global information pointers should
//								be set up.
//
//	Parameters out:	Number of classes in the class vector pointer.  (If smaller than
//								on input, classes in the class vector with no available
//								statistics were removed)
//							The class vector pointer (list may have been revised as described
//								above)
//							Number of training pixels in the smallest class.
//
// Value Returned:	True if update worked okay and statistics are ready to be used. 
//							False if memory was not available for the update or if the
//								number of training classes is less than the minimum number
//								needed.
// 
// Called By:			ClassifyControl in SClassify.cpp
//
//	Coded By:			Larry L. Biehl			Date: 04/26/2019
//	Revised By:			Larry L. Biehl			Date: 05/04/2019

Boolean VerifyProjectTrainingPixelsLoaded (
				UInt32*								numberClassesPtr,
				SInt16*								classPtr,
				UInt32								minimumNumberClasses,
				SInt16								setupGlobalInfoPointers,
				SInt32*								minimumNumberTrainPixelsPtr)

{
   UInt32								savedNumberClasses;

   SInt16								returnCode;

   Boolean								computeCommonCovarianceFlag,
											continueFlag;
											//loadPixelDataFlag;


   if (gProjectInfoPtr == NULL)
																						return (FALSE);

   continueFlag = TRUE;
   returnCode = 1;
	//loadPixelDataFlag = TRUE;
	/*
	if (gProjectInfoPtr->pixelDataLoadedFlag)
		{
				// Check if pixels data need to be reloaded because of change in
				// the selection of training pixels.

		loadPixelDataFlag = !CheckClassStats (numberClassesPtr,
														classPtr,
														kPixelValuesOnly,
														kNoStatisticsUsed,
														TRUE,
														minimumNumberTrainPixelsPtr,
														&computeCommonCovarianceFlag);
		
		}	// end "if (gProjectInfoPtr->pixelDataLoadedFlag)"
	*/
	//if (loadPixelDataFlag)
	if (!gProjectInfoPtr->pixelDataLoadedFlag)
		{
		returnCode = UpdateStatsControl (kUpdateProject, FALSE, TRUE);

				// Now get the project image file information pointers again. They
				// may have been released in the call to 'UpdateStatsControl'.

		if (!GetProjectImageFileInfo (kDoNotPrompt, setupGlobalInfoPointers))
			returnCode = 2;

		gProjectInfoPtr->moveMemoryFlag = TRUE;

		}	// end "if (loadPixelDataFlag)"

	if (returnCode != 2)
		{
				// Make certain that the vector of classes to use is		
				// consistent with the statistics that are available. 	
				// It is possible for there to be a class specified 		
				// for use with no statistics.									

		savedNumberClasses = *numberClassesPtr;
		CheckClassStats (numberClassesPtr,
								classPtr,
								kPixelValuesOnly,
								kNoStatisticsUsed,
								FALSE,
								minimumNumberTrainPixelsPtr,
								&computeCommonCovarianceFlag);

				// If number of classes is < minimumNumberClasses, then put up
				// an alert and skip the operation.

		if (*numberClassesPtr < minimumNumberClasses) 
			{
			NoClassStatsAlert (minimumNumberClasses);
			if (*numberClassesPtr == 0)
				*numberClassesPtr = savedNumberClasses;
			continueFlag = FALSE;

			}	// end "if (*numberClassesPtr <= minimumNumberClasses)"

		}	// end "if (returnCode != 2)"

	else	// returnCode == 2 
		continueFlag = FALSE;

   return (continueFlag);

}	// end "VerifyProjectTrainingPixelsLoaded"
