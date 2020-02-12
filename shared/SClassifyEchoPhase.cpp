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
//	File:						SClassifyEchoPhase.cpp
//
//	Authors:					Byeungwoo Jeon
//								Larry L. Biehl
//
//	Revision date:			12/21/2017
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	This file contains functions that control the
//								echo classification processes.
//
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"

#if defined multispec_wx
#endif

#include "SEcho.h"

				
SInt16 ClassifyNonHomogeneousCells (
				SInt32								numberLines,
				statistics*							class_stat,
				HDoublePtr							si_ptr1,
				HSInt32Ptr 							epix_ptr,
				HDoublePtr							wk1,
				double*								thresholdTablePtr,
				HUCharPtr							probabilityBufferPtr,
				AreaDescriptionPtr 				areaDescriptionPtr);
			
void classify_pixel_using_ML (
				statistics*							class_stat,
				SInt16	 							nband,
				UInt32	 							ncls,
				HDoublePtr							si_ptr1,
				double*								wk3,
				HSInt32Ptr 							epix_ptr,
				HDoublePtr							wk1);
					
					

//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		SInt16 phase1
//
//	Software purpose:	Compute Cell Likelihood Values and Check Homogeneity
// 						This routine reads "cell_width" lines of data  at one time.
// 						Homogeneous Cell:	
//								FLDCLS - FLD's Class
// 		   				IBUF   - FLD Number
// 		   				Since field is not assigned yet, it is 0.
// 
// 						NON.Homog Cell: 	
//								FLDCLS - FLD's Class
// 		  					IBUF   - (Minus) Pixel's Class
// 		  
// IBUF is stored in the epix_ptr vector.  Note that the high order bit
// (the sign bit) signifies whether the pixel is part of a homogeneous cell
// (greater than 0) or not a part of one.  The next high order bit signifies
// whether the pixel is thresholded (1 signifies yes, 0 signifies no).  Because
// of the use of these 2 bits, the maximum number of fields that are possible
// is 1,073,741,824.
// ###
// Data read mechanism must be refined.
// change the function loglik_echo (), classify_pixel_using_ML ()  into macro
// ###
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	error_code			
// 
// Called By:			EchoClassifier in SClassifyEcho.cpp
//
// Coded By: 			Byeungwoo Jeon		Date: 01/01/1989
// Revised By: 		Byeungwoo Jeon		Date: 09/28/1991
// Revised By: 		Larry Biehl			Date: 02/14/2014
//------------------------------------------------------------------------------------

SInt16 phase1 (
				FileIOInstructionsPtr		fileIOInstructionsPtr,
				statistics*						class_stat, 
				EchoClassifierVar*			echo_info, 
				double*							pixelThresholdTablePtr, 
				HUCharPtr		 				probabilityBufferPtr, 
				AreaDescriptionPtr			areaDescriptionPtr)
				
{
	double							annexationThreshold_derived,
										lik_ratio,
										smax,
										threshold,
										xlik;
   									
	//HEchoFieldInfoPtr			fieldInfoPtr;
   
   FILE*								fp;
   
   HFldLikPtr						auxlikn[4],
										baseFieldLikePtr,
										cellLikPtr,
										*fieldLikeIndicesPtr,
										tAuxLikPtr,
										tFldLikPtr;
   			
   HPtr								fieldLikeFlagsPtr;
   
   double							*cellThresholdTablePtr,
										*ex;
   							
   HDoublePtr						ex2;
	
   HSInt16Ptr						cellClassPtr,
										fieldClassNumberPtr;
   							
   HDoublePtr						si_ptr1;
   							
   HSInt32Ptr						epixCurrentLineCell0Ptr,	// epix_ptr0,
										epixCurrentLineCellPtr,		// epix_ptr;
										epixPreviousLineCell0Ptr,
										epixPreviousLineCellPtr,
										savedLineCell0Ptr;
   
   HUInt32Ptr						fieldLikelihoodTableIndexPtr,
										field_number_table;

   SInt32							cellBuf,                   // May be negative
										cellOrder[4] = {3,1,0,2},
										inx,
										ibuf,                   	// May be negative
										ibufn[4],
										iy,								// from echo Variables 
										lastcol_echo,
										line_x,
										ncl_icel,
										num_col,                              
										ncl_rem_icel, 
										nrw_rem_icel,
										nhd2,
										numberMoveBytes;
   
   UInt32							count,
										epixLineCellBufferCount,
										icel_linlen,                	
										ix,
										iyy,
										lengthFieldLikeList,
										lineNumber,
										ncls,
										newfield,
										nextEmptySlot,
										nhd0,
										nhd1,
										//nhd2,
										nhd3,
										nrw_icel,		// from echo Variables,
										unit_x_icel;
   
   SInt16							auxcls,
										cell_size,
										classNumber, 
										error_code,
										nband; 
   
   UInt16							auxcls_temp,
										cellLocation,
										cell_width,
										fldcls,
										fldclsn;
   
   Boolean							combineLikeFieldsFlag,
										maxRatioFoundFlag,
										mixCellsFlag,
										useTempDiskFileFlag;
   
   char								likeFlag1,
										likeFlag2,
										savedLikeFlag;
   

   error_code = 0;
   mixCellsFlag = echo_info->mixCellsFlag;
   combineLikeFieldsFlag = echo_info->combineLikeFieldsFlag;
	useTempDiskFileFlag = echo_info->useTempDiskFileFlag;
  
			// Set up Local variables related to image Specification 				

   nband   	= echo_info->nband;
   num_col 	= echo_info->num_col;
   
   fieldClassNumberPtr  = echo_info->fieldClassNumberPtr;
   fieldLikelihoodTableIndexPtr  = echo_info->fieldLikelihoodTableIndexPtr;
   field_number_table 	= echo_info->field_number_table;
   
			// Set up Local variables related to Statistics Specification			

   ncls 	= echo_info->ncls;
   numberMoveBytes = sizeof (FldLik) * ncls;
   
			// Set up Local variables related to Echo Specification					

   annexationThreshold_derived = echo_info->annexationThreshold_derived;

   cell_width   = echo_info->cell_width;
   cell_size 	= echo_info->cell_size;
   nrw_icel	= echo_info->nrw_icel;
   ncl_icel	= echo_info->ncl_icel;
   nrw_rem_icel	= echo_info->nrw_rem_icel;
   ncl_rem_icel = echo_info->ncl_rem_icel;
   icel_linlen  = echo_info->icel_linlen;
   lastcol_echo	= echo_info->lastcol_echo;
   
   cellThresholdTablePtr = &pixelThresholdTablePtr[gNumberProbabilityClasses];
  	
   		// Derivations of the above  													
   
   line_x		= num_col * nband;
   unit_x_icel = cell_width * nband;
   
   nhd0 = cell_width;	// icel_linlen + cell_width;
   nhd1 = 0;				// icel_linlen;
   nhd2 = -cell_width;	// icel_linlen - cell_width;
   nhd3 = cell_width;

			// Initialize output Echo Parameters											

   newfield = 0;
   echo_info->field_size = 0;
   echo_info->number_of_fields = 0;
   echo_info->number_of_homogeneous_cells = 0;

			// Memory Allocation																	

   ex2 = echo_info->work2;
   ex = echo_info->work1;
   cellClassPtr = echo_info->cellClassPtr;
   fieldLikeIndicesPtr = echo_info->fieldLikeIndicesPtr;
   fieldLikeFlagsPtr = echo_info->fieldLikeFlagsPtr;
   
   lengthFieldLikeList = echo_info->ncl_fldlik * 2;
   
			// Compute a Cell Likelihood and check Homogeneity							

	//epix_ptr0   	= echo_info->epix_ibufPtr;
	epixCurrentLineCell0Ptr = echo_info->epix_ibufPtr;
	epixPreviousLineCell0Ptr = echo_info->epix_ibuf2Ptr;
	epixLineCellBufferCount = num_col * cell_width * sizeof (SInt32);
	
   lineNumber  	= 0;
   auxlikn[0]     = (HFldLikPtr)ex2;
   auxlikn[1]     = auxlikn[0] + ncls;
   auxlikn[2]     = auxlikn[1] + ncls;
   auxlikn[3]     = auxlikn[2] + ncls;
   nextEmptySlot 	= 0;
   
   		// Initialize field likelihood indices and flag vectors.					
   	
   tFldLikPtr = &echo_info->fldlikPtr[echo_info->line_fldlik];
   
   likeFlag1 = 0x01;
   likeFlag2 = 0x02;
   for (iyy=0; iyy<lengthFieldLikeList; iyy++)
   	{
   	fieldLikeIndicesPtr[iyy] = tFldLikPtr;
   	fieldLikeFlagsPtr[iyy] = likeFlag1 + likeFlag2;
   	tFldLikPtr += ncls;
   	
   	}	// end "for (iyy=0; iyy<bytes ;iyy++)" 
   	
	fp = (FILE*)fileIOInstructionsPtr;
	
   for (ix=1; ix<=nrw_icel; ix+=cell_width)
   	{
   	cellLikPtr = echo_info->fldlikPtr;
      lineNumber += cell_width;
      	
		if (TickCount () >= gNextStatusTime)
			{
     		LoadDItemValue (gStatusDialogPtr, 
     								IDC_Status18, 
     								(SInt32)lineNumber);
    		LoadDItemValueWithCommas (gStatusDialogPtr, 
    											IDC_Status22, 
    											echo_info->number_of_fields);
			gNextStatusTime = TickCount () + gNextStatusTimeOffset;
		
			}	// end "if (TickCount () >= gNextStatusTime)" 
   
      		// Read "Cell_width" Lines of Data											
   	
      if (error_code == 0) 
			error_code = read_lines_of_data1 (fp, cell_width);
   			
      if (error_code != 0) 
      	break;	// Out of "for (ix=1;ix.." Loop 
      
      si_ptr1     = (HDoublePtr)gOutputBufferPtr;
		//epix_ptr   = epix_ptr0;
		epixCurrentLineCellPtr = epixCurrentLineCell0Ptr;
		epixPreviousLineCellPtr = epixPreviousLineCell0Ptr;

				// Scan line from left to right assigning homogeneous cells to		
				// nearest field if possible.  Do not create any new fields			
				// during this pass.																
		
      for (iy=1; iy<=(SInt32)ncl_icel; iy+=cell_width)
      	{
					// Compute Cell Likelihood Value 										

			loglik_echo (class_stat,
								nband,
								ncls,
								si_ptr1,
								cellLikPtr,
								line_x,
								ex2,
								ex,
								&xlik,
								&classNumber,
								cell_width);
								
			threshold = *(echo_info->thresholds_phase1 + classNumber - 1);
			*cellClassPtr = classNumber;
	
					// Check Homogeneity															

			if (xlik > threshold) 
				{		// Homogeneous Cell 														
	   	
	  			echo_info->number_of_homogeneous_cells++;
	  			
						// Check Annexation with four surrounding cells around the target 
						// cell. The cells are:
						// | S(0) | S(1) | S(2) |
						// | S(3) | T |												

			   ibuf = ibufn[0] = ibufn[1] = ibufn[2] = ibufn[3] = 0;
			   if (ix > 1) 
			   	{
					//ibufn[1] = *(epix_ptr - nhd1) & 0xbfffffff;
					ibufn[1] = *(epixPreviousLineCellPtr - nhd1) & 0xbfffffff;
					if (iy > 1) 
						//ibufn[0] = *(epix_ptr - nhd0) & 0xbfffffff;
						ibufn[0] = *(epixPreviousLineCellPtr - nhd0) & 0xbfffffff;
					if (iy < (SInt32)ncl_icel-cell_width) 
						//ibufn[2] = *(epix_ptr - nhd2) & 0xbfffffff;
						ibufn[2] = *(epixPreviousLineCellPtr - nhd2) & 0xbfffffff;
					
			  	 	}	//	"if (ix > 1)"	
	   
	  			if (iy > 1) 
					//ibufn[3] = *(epix_ptr - nhd3) & 0xbfffffff;
	  				ibufn[3] = *(epixCurrentLineCellPtr - nhd3) & 0xbfffffff;

	   		smax = -DBL_MAX;
	  			auxcls = -1;
  			
	  					// Do some analysis of the 4 cells around the target cell.
	  					// If the class of the target cell is the same as the field 
	  					// class of one of the 4 surrounding cells then that 
	  					// surrounding cell/field will have the max likelihood ratio.
	  					// The others do not need to be checked.
	   
	   		maxRatioFoundFlag = FALSE;
			   for (iyy=0; iyy<4; iyy++)
			   	{
			   	cellLocation = (SInt16)cellOrder[iyy];
			   	cellBuf = ibufn[cellLocation];
			   	
			   	if (maxRatioFoundFlag)
			   		ibufn[cellLocation] = 0;
			   	
			   	else	// !maxRatioFoundFlag
			   		{
				   	if (cellBuf > 0) 
				   		{
							inx  = field_number_table[cellBuf];
							fldcls = fieldClassNumberPtr[inx];
						
							if (fldcls == (UInt16)*cellClassPtr)
								{
								maxRatioFoundFlag = TRUE;
								
										// Now "turn off" any cell locations before
										// this one.
										
								for (cellBuf=0; cellBuf<(SInt32)iyy; cellBuf++)
									{
			   					cellLocation = (SInt16)cellOrder[cellBuf];
			   					ibufn[cellLocation] = 0;
									
									}	// end "for (cellBuf=0; cellBuf<iyy; ..."
								
								}	// end "if (fldcls == (UInt16)*cellClassPtr)"
								
							}	// end "if (cellBuf > 0)"
							
						}	// end "else !maxRatioFoundFlag"
	   		
	   			}	// end "for (iyy=0; iyy<4; iyy++)"
	   
			   for (iyy=0; iyy<4; iyy++)
			   	{
			   	cellLocation = (SInt16)cellOrder[iyy];
			   	cellBuf = ibufn[cellLocation];
			   	
			   	if ((cellBuf > 0) && (cellBuf != ibuf))
			   		{
						inx  = field_number_table[cellBuf];
						fldcls = fieldClassNumberPtr[inx];
					
						if (mixCellsFlag || fldcls == (UInt16)*cellClassPtr)
							{
							inx = fieldLikelihoodTableIndexPtr[inx];
			       		lik_ratio = log_lik_ratio (
								       				fieldLikeIndicesPtr[inx],
														cellLikPtr,
														auxlikn[cellLocation],
														fldcls,
														*cellClassPtr,
														&auxcls_temp,
														ncls);
														
				       	if ((lik_ratio > smax) && 
				       				(lik_ratio > annexationThreshold_derived))
								{
			   				ibuf   = cellBuf;
			   				auxcls = auxcls_temp;
			   				tAuxLikPtr = auxlikn[cellLocation];
			   				smax = lik_ratio;
			   
								}	// "if ((lik_ratio > smax) ..."
							
							}	// end "if ((mixCellsFlag || ..."
		
	   				}	// "if ((cellBuf > 0) && (cellBuf != ibuf))"
	   		
	   			}	// end "for (iyy=0; iyy<4; iyy++)"

				if (ibuf > 0)  
					{
							// Assigning to field
					
					inx = field_number_table[ibuf];
					fieldClassNumberPtr[inx] = auxcls;
					inx = fieldLikelihoodTableIndexPtr[inx];
					
					fieldLikeFlagsPtr[inx] &= likeFlag1;
					tFldLikPtr = fieldLikeIndicesPtr[inx];
					
					echo_info->field_size += cell_size;
						
					BlockMoveData (tAuxLikPtr, tFldLikPtr, numberMoveBytes);

					stuffing (epixCurrentLineCellPtr, ibuf, cell_width, cell_width, num_col);
		
					}	// end "if (ibuf > 0)" 
		
				else	// ibuf == 0 
							// Indicate that cell has not been assigned to a field	
							// yet.  It will be done during right to left pass.		
		
					stuffing (epixCurrentLineCellPtr, 0, cell_width, cell_width, num_col);
	
				} 
				
			else	// xlik <= threshold
				{
						// Non-Homogeneous Cells
				
				stuffing (epixCurrentLineCellPtr, -1, cell_width, cell_width, num_col);
				
      		}	// end else xlik <= threshold

      	si_ptr1 += unit_x_icel;	// cell_width * nband 
      	epixCurrentLineCellPtr += cell_width; 
      	epixPreviousLineCellPtr += cell_width;
     		cellLikPtr += ncls;
    		cellClassPtr++;

					// Exit routine if user has "command period" down						
	
	 		if (TickCount () >= gNextTime)
				{
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
					{
					error_code = 1;
					break;
		
					}	// end "if (!CheckSomeEvents..."
		
				}	// end "if (TickCount () >= gNextTime)"
      
     		}	// for iy 
     		
     	if (error_code == 1)
     		break;

				// Initialize the right remaining boundaries as non-homogeneous	
				// pixels.																			
				
		stuffing (epixCurrentLineCellPtr, -1, cell_width, (SInt16)ncl_rem_icel, num_col);
		
				// Now scan line from right to left assigning homogeneous cells 	
				// to nearest field if possible.  If there are no nearest			
				// then create a new field.													
				// The nearest fields to check will be itself if assigned			
				// during left to right pass and the field to the right.				
		
   	for (iy=lastcol_echo; iy>=0; iy-=cell_width) 
   		{
      	cellClassPtr--;
      	cellLikPtr -= ncls;
      	epixCurrentLineCellPtr -= cell_width;
	
					// Determine if cell is homogeneous.									
		
			if (*epixCurrentLineCellPtr >= 0)
				{
  				cellBuf = 0;
  				ibuf = *epixCurrentLineCellPtr;
				if (iy < lastcol_echo) 
					cellBuf = *(epixCurrentLineCellPtr + cell_width);
			
						// Get Cell Likelihood Value											
						// Check right most cell.												
	   	
	   		if ((cellBuf > 0) && (*epixCurrentLineCellPtr != (SInt16)cellBuf))
	   			{
					inx  = field_number_table[cellBuf];
					fldcls = fieldClassNumberPtr[inx];
			
					if (mixCellsFlag || fldcls == (UInt16)*cellClassPtr)
						{
						inx = fieldLikelihoodTableIndexPtr[inx];
	       			lik_ratio = log_lik_ratio (
	       									fieldLikeIndicesPtr[inx],
												cellLikPtr,
												auxlikn[0],
												fldcls,
												*cellClassPtr,
												&auxcls_temp,
												ncls);
		        		if (lik_ratio > annexationThreshold_derived)
							{
		   				auxcls = auxcls_temp;
		   				tAuxLikPtr = auxlikn[0];
  							ibuf = cellBuf;
		   
							}	// end "if (lik_ratio > annexationThreshold_derived)" 
					
						}	// end "if (mixCellsFlag || ..." 
				
	   			if (ibuf > 0 && *epixCurrentLineCellPtr > 0)
	   				{
	   						// Check if right cell has a higher ratio than than 	
	   						// for current assignment.										
	   				
						inx  = field_number_table[*epixCurrentLineCellPtr];
						fldcls = fieldClassNumberPtr[inx]-1;
						inx = fieldLikelihoodTableIndexPtr[inx];
						tFldLikPtr = fieldLikeIndicesPtr[inx];
	   				smax = cellLikPtr[fldcls] - cellLikPtr[*cellClassPtr-1]; 
	   						
	   				if (lik_ratio > smax)
	   			   	{
	   			    			// Reassign the cell remove from current field.		
	   			    	
				   		inx = field_number_table[*epixCurrentLineCellPtr];
				    		fieldClassNumberPtr[inx] = 
	   			    					subtract_log_lik (tFldLikPtr,cellLikPtr,ncls);
				   		echo_info->field_size -= cell_size;
	   			   	*epixCurrentLineCellPtr = 0;
	   			    	
	   			    	}	// end "if (lik_ratio > smax)"
	   			    
	   				}	// end "if (ibuf > 0 && *epixCurrentLineCellPtr > 0)" 
		
	   			}	// "if ((cellBuf > 0) && (cellBuf != ibuf))" 

	   		if (ibuf == 0) 
					{	
							// New field
						
					if (newfield >= (UInt32)echo_info->current_max_field_number)
																								return (3);
						
	   			ibuf = ++newfield;
   				echo_info->number_of_fields++;
   				field_number_table[ibuf] = ibuf;
   				tAuxLikPtr = cellLikPtr;
   				auxcls = *cellClassPtr;
   			
   						// Get next empty location for field likelihood values.	
   			
   				for (iyy=0; iyy<lengthFieldLikeList; iyy++)
   					{	
   					if (nextEmptySlot >= lengthFieldLikeList)
   						nextEmptySlot = 0;
   					if (fieldLikeFlagsPtr[nextEmptySlot] & likeFlag1)
   						break;
   					nextEmptySlot++;
   					
   					}	// end "for (iyy=0, iyy..." 
   				
   				if (iyy >= lengthFieldLikeList)
   																							return (3);
   								
					fieldLikelihoodTableIndexPtr[ibuf] = (UInt32)nextEmptySlot;
					fieldLikeFlagsPtr[nextEmptySlot] &= likeFlag2;
					nextEmptySlot++;
		
					}	// if (ibuf == 0) 

				if (*epixCurrentLineCellPtr == 0)
					{
					inx = field_number_table[ibuf];
					fieldClassNumberPtr[inx] = auxcls;
					inx = fieldLikelihoodTableIndexPtr[inx];
			
					fieldLikeFlagsPtr[inx] &= likeFlag1;
			
					echo_info->field_size += cell_size;
						
					BlockMoveData (tAuxLikPtr, fieldLikeIndicesPtr[inx], numberMoveBytes);
	
					stuffing (epixCurrentLineCellPtr, ibuf, cell_width, cell_width, num_col);
			
					}	// end "if (*epixCurrentLineCellPtr == 0)" 
		
				}	// end "if (ibuf >= 0)" 	
	
			}	// end "for (iy=lastcol_echo; iy>=0; iy-=cell_width)"

				// Exit routine if user has "command period" down						

 		if (TickCount () >= gNextTime)
			{
			if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
				{
				error_code = 1;
				break;
	
				}	// end "if (!CheckSomeEvents..."
	
			}	// end "if (TickCount () >= gNextTime)"

    	if (combineLikeFieldsFlag && ix > 1)
    		{
					// Scan line from left to right combining like fields.		
					// NOTE??!!																	
					// The annexation threshold may need to be revised to take	
					// into account the different number of cells in each field.
	
			epixPreviousLineCellPtr = epixPreviousLineCell0Ptr;
    		for (iy=1; iy<=ncl_icel; iy+=cell_width) 
    	   	{
	    		ibuf = *epixCurrentLineCellPtr;
	
	    		if (ibuf > 0)
					{
					ibuf  = field_number_table[ibuf];
					fldcls = fieldClassNumberPtr[ibuf];
					inx = fieldLikelihoodTableIndexPtr[ibuf];
	       		baseFieldLikePtr = fieldLikeIndicesPtr[inx];
		
					ibufn[0] = ibufn[1] = ibufn[2] = ibufn[3] = 0;
	   
					ibufn[1] = *(epixPreviousLineCellPtr - nhd1) & 0xbfffffff;
					if (iy > 1)
		    			{
		    			ibufn[0] = *(epixPreviousLineCellPtr - nhd0) & 0xbfffffff;
		    			ibufn[3] = *(epixCurrentLineCellPtr - nhd3) & 0xbfffffff;
		
		    			}	// end "if (iy > 1)" 
		
					if (iy < ncl_icel-cell_width) 
		    			ibufn[2] = *(epixPreviousLineCellPtr - nhd2) & 0xbfffffff;
	   
					for (iyy=0; iyy<4; iyy++)
		    			{
	   	   		cellLocation = (SInt16)cellOrder[iyy];
	   	    		cellBuf = ibufn[cellLocation];
	   		
	   	    		if (cellBuf > 0)
	   					cellBuf = field_number_table[ibufn[cellLocation]];
	   	
	   	    		if ((cellBuf > 0) && (cellBuf != ibuf)) 
	   					{
							fldclsn = fieldClassNumberPtr[cellBuf];
			
							if (fldcls == fldclsn)
			    				{
			    				inx = fieldLikelihoodTableIndexPtr[cellBuf];
	       		   		lik_ratio = log_lik_ratio (
	       											fieldLikeIndicesPtr[inx],
														baseFieldLikePtr,
														auxlikn[0],
														fldcls,
														fldclsn,
														&auxcls_temp,
														ncls);
						
		       	    		if (lik_ratio > annexationThreshold_derived) 
									{
									fieldLikeFlagsPtr[inx] |= likeFlag2;
	   	    					for (inx=1; inx<=(SInt32)newfield; inx++)
	   	    		   			{
				    					if (field_number_table[inx] == (UInt32)cellBuf)
				    						field_number_table[inx] = ibuf;
				    	
				    					}	// end "for (inx=1; inx<=..." 
		
   								echo_info->number_of_fields--;
						
									BlockMoveData (
												auxlikn[0], baseFieldLikePtr, numberMoveBytes);
		   
									}	// "if (lik_ratio > ..."
					
			    				}	// end "if (fldcls == fldclsn)"
		
	   					}	// "if ((cellBuf > 0) && (cellBuf != ibuf))"
	   		
	  					}	// end "for (iyy=0; iyy<4; iyy++)"
	   	
	   			}	// end "if (ibuf > 0)" 
	   	
     			epixCurrentLineCellPtr += cell_width;
				epixPreviousLineCellPtr += cell_width;
	
	    		}	// end "for (iy=1;iy <= ncl_icel; iy += cell_width)"
	    
			}	// end "if (combineLikeFieldsFlag && ix > 1)"
			
				// Classify the non-homogeneous cells/pixels and write the			
				// probability information if requested.									
		
		areaDescriptionPtr->line = fileIOInstructionsPtr->lineStart;		
		error_code = ClassifyNonHomogeneousCells (
										cell_width, 
										class_stat, 
										(HDoublePtr)gOutputBufferPtr,
										epixCurrentLineCell0Ptr,	// epix_ptr0, 
										ex2,
										pixelThresholdTablePtr,
										probabilityBufferPtr,
										areaDescriptionPtr);
     		
 				// Write of probability information for the line(s) if 				
				// requested.	 																	
    	
		if (probabilityBufferPtr != NULL && error_code == 0)
			{
			error_code = SaveProbabilityInformation (cell_width,
															cellThresholdTablePtr,
															class_stat,
															probabilityBufferPtr,
															cellLikPtr,
															epixCurrentLineCell0Ptr,	// epix_ptr0,
   														fieldClassNumberPtr,
   														field_number_table,
   														areaDescriptionPtr);
   														
   		}	// end "if (probabilityBufferPtr != NULL && error_code == 0)"
		
		if (useTempDiskFileFlag && error_code == 0)
			{
					// Write the current line-cell data to the temp file.
			
			count = epixLineCellBufferCount;
			error_code = MWriteData (echo_info->tempFileStreamPtr, 
												&count,
												epixCurrentLineCell0Ptr,
												kErrorMessages);
			
			savedLineCell0Ptr = epixCurrentLineCell0Ptr;
			epixCurrentLineCell0Ptr = epixPreviousLineCell0Ptr;
			epixPreviousLineCell0Ptr = savedLineCell0Ptr;
			
			}	// end "if (useTempDiskFileFlag && error_code == 0)"
		
		else	// !useTempDiskFileFlag
			{
			//epix_ptr0 += icel_linlen;	// num_col * cell_width
			epixPreviousLineCell0Ptr = epixCurrentLineCell0Ptr;
			epixCurrentLineCell0Ptr += icel_linlen;	// num_col * cell_width
			
			}	// else !useTempDiskFileFlag

				// Exchange likelihood list flags so that alternating lists will be
				// used.  And intialize the list for the next line set.

   	savedLikeFlag = likeFlag1;
   	likeFlag1 = likeFlag2;
   	likeFlag2 = savedLikeFlag;
   	for (iyy=0; iyy<lengthFieldLikeList; iyy++)
   		{
   		fieldLikeFlagsPtr[iyy] |= likeFlag2;
   	
   		}	// end "for (iyy=0; iyy<lengthFieldLikeList ;iyy++)" 
		
   	if (error_code != noErr)
			break;
		
   	}	// end "for (ix=1; ix<=nrw_icel; ix+=cell_width)" 

			// Perform maximum likelihood classification of remaining rows, 		
			// if any 																				

   if ((error_code == 0) && (nrw_rem_icel > 0)) 
   	{
   	epixCurrentLineCellPtr = epixCurrentLineCell0Ptr;
   	iyy = nrw_rem_icel * num_col;
		for (ix=0; ix<iyy; ix++)
			*epixCurrentLineCellPtr++ = -1;
			
      error_code = read_lines_of_data1 (fp, nrw_rem_icel);
		
				// Classify the non-homogeneous cells/pixels and write the			
				// probability information if requested.									
				
		areaDescriptionPtr->line = fileIOInstructionsPtr->lineStart;
      if (error_code == 0)
			error_code = ClassifyNonHomogeneousCells (
										nrw_rem_icel, 
										class_stat, 
										(HDoublePtr)gOutputBufferPtr,
										epixCurrentLineCell0Ptr,		// epix_ptr0, 
										ex2,
										pixelThresholdTablePtr,
										probabilityBufferPtr,
										areaDescriptionPtr);

				// Write of probability information for the line(s) if 				
				// requested.	 																	
    	
		if (probabilityBufferPtr != NULL && error_code == 0)
			error_code = SaveProbabilityInformation (nrw_rem_icel,
																	NULL,
																	class_stat,
																	probabilityBufferPtr,
																	cellLikPtr, 
																	epixCurrentLineCell0Ptr, // epix_ptr0,
																	fieldClassNumberPtr,
																	field_number_table,
																	areaDescriptionPtr);
		
		if (useTempDiskFileFlag && error_code == 0)
			{
					// Write the current line-cell data to the temp file.
			
			count = epixLineCellBufferCount;
			error_code = MWriteData (echo_info->tempFileStreamPtr, 
											 &count, 
											 epixCurrentLineCell0Ptr,
											 kErrorMessages);
			
			}	// end "if (useTempDiskFileFlag && error_code == 0)"
		
   	}	// "if ((error_code == 0) && (nrw_rem_icel > 0))"

   LoadDItemValue (gStatusDialogPtr, IDC_Status18, (SInt32)lineNumber);
   LoadDItemValueWithCommas (gStatusDialogPtr, 
   									IDC_Status22, 
    									(SInt64)echo_info->number_of_fields);

			// Save maximum field number.														
	
   echo_info->current_max_field_number = newfield;

   return (error_code);
   
}	// end "phase1 



//------------------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void ClassifyNonHomogeneousCells
//
//	Software purpose:	The purpose of this routine is to classify the
//							the non-homogeneous cells and save the probability
//							indeces for the non-homogenous cells if requested
//							for the pixels in 'cell_width' lines.
//
//	Parameters in:				
//
//	Parameters out:	None
//
// Value Returned:	None			
// 
// Called By:
//
//	Coded By:			Larry L. Biehl			Date: 05/25/1993
//	Revised By:			Larry L. Biehl			Date: 12/29/2005

SInt16 ClassifyNonHomogeneousCells (
				SInt32								numberLines, 
				statistics*							class_stat, 
				HDoublePtr							si_ptr1,
				HSInt32Ptr							epix_ptr, 
				HDoublePtr							wk1, 			// nband by nband work space
				double*								thresholdTablePtr, 
				HUCharPtr 							probabilityBufferPtr, 
				AreaDescriptionPtr				areaDescriptionPtr)

	
{                  
	double								rrDivide2;
									
	Point									point;
	RgnHandle							rgnHandle;
   								
	HUCharPtr 							savedProbabilityBufferPtr;
	
	SInt16*								thresholdProbabilityPtr;
   
	SInt32								column,
											line,
   										numberColumns;
   								
	UInt32								numberClasses;

   SInt16								numberChannels,
											thresholdCode;
   								
	Boolean								polygonFieldFlag;

		
			// Set up local variables.														

   numberClasses		= gEchoClassifierVariablePtr->ncls;
	numberColumns		= gEchoClassifierVariablePtr->num_col;
   numberChannels   	= gEchoClassifierVariablePtr->nband;
   
	polygonFieldFlag 	= areaDescriptionPtr->polygonFieldFlag;
	rgnHandle 			= areaDescriptionPtr->rgnHandle;
	point.v 				= (SInt16)areaDescriptionPtr->line;
		
	if (polygonFieldFlag && rgnHandle == NULL)
																							return (1);
   
   thresholdCode = 0;
	if (gClassifySpecsPtr->thresholdFlag)
		thresholdCode = gClassifySpecsPtr->probabilityThresholdCode;
	
	thresholdProbabilityPtr = gClassifySpecsPtr->thresholdProbabilityPtr;
	savedProbabilityBufferPtr = probabilityBufferPtr;
		
   for (line=0; line<numberLines; line++) 
   	{
		point.h = (SInt16)areaDescriptionPtr->columnStart;
      for (column=0; column<numberColumns; column++) 
      	{
      			// Classify any non-homogeneous pixels using the maximum			
      			// likelihood classifier.													
      			
      	if (*epix_ptr < 0)
      		{
				if (!polygonFieldFlag || PtInRgn (point, rgnHandle))
					{
	         	classify_pixel_using_ML (class_stat,
														numberChannels,
														numberClasses,
														si_ptr1,
														&rrDivide2,
														epix_ptr,
														wk1);
					
							// Fill probability buffer if needed.							
						
					if (savedProbabilityBufferPtr)
						{
			   		//maxClass = (SInt16)(labs (*epix_ptr) - 1);
	
	   						// Cell likehood value is  classConstant - r**2/2 		
		   					// But threshold is for the value r**2/2					
		   				
						//dValue = class_stat[maxClass].classConstantML - xlik[maxClass];
				   	
				   			// Get the threshold table index.							
						
						*probabilityBufferPtr = 
										(UInt8)GetThresholdClass (rrDivide2, 
																			thresholdTablePtr);
										
 						gTempDoubleVariable1 += 
										thresholdProbabilityPtr[*probabilityBufferPtr];
										
						if (*probabilityBufferPtr > thresholdCode)
							*epix_ptr &= 0xbfffffff;
					
						}	// end "if (savedProbabilityBufferPtr)"
						
					else	// !savedProbabilityBufferPtr 
						*epix_ptr &= 0xbfffffff;
						
					}	// end "if (!polygonFieldFlag || PtInRgn (..."
		   
				else	// polygonFieldFlag && !PtInRgn (point, rgnHandle)
					{
				   if (savedProbabilityBufferPtr)
						*probabilityBufferPtr = 0;
					
					}	// end "else polygonFieldFlag && !PtInRgn (..."
					
				}	// end "if (*epix_ptr < 0)"

	   	epix_ptr++;
	   	si_ptr1 += numberChannels;
			probabilityBufferPtr++;
			point.h++;

					// Exit routine if user has "command period" down						
	
	 		if (TickCount () >= gNextTime)
				{
				if (!CheckSomeEvents (osMask+keyDownMask+updateMask+mDownMask+mUpMask))
																								return (1);
		
				}	// end "if (TickCount () >= gNextTime)"
	   	
      	}	// end "for (column=0; column<numberColumns; column++)"
      	
      point.v++;
      	
   	}	// end "for (line=0; line<numberLines; line++)"
   	
   return (0); 

}	// end "ClassifyNonHomogeneousCells"



//------------------------------------------------------------------------------------
// FUNCTION : classify_pixel_using_ML
// Purpose  : Classify given pixel using pixelwise maximum 
// 		likelihhod classifier.
// 	      Pixel data is in "unsigned char" format.
// Coded By: 			Byeungwoo Jeon			Date: 01/01/1989
// Revised By: 		Byeungwoo Jeon			Date: 09/28/1991
// Revised By: 		Larry Biehl				Date: 12/29/2005
//------------------------------------------------------------------------------------

void classify_pixel_using_ML (
				statistics*							stat_ptr, 
				SInt16								nband, 
				UInt32								ncls, 
				HDoublePtr							si_ptr1, 
				double*								rrDivide2Ptr, 
				HSInt32Ptr							epix_ptr, 
				HDoublePtr							wk1)

{
   double							discriminantMax,
   									dValue,
   									rrDivide2,
   									rrDivide2Max;
	
	HDoublePtr						d_ptr,
										mean_ptr,
										tsi_ptr;
   
   UInt32							class_assigned,
   									classNumber;
   
   SInt16							channel;
 
		
	discriminantMax	= -DBL_MAX;
	class_assigned 	= 0;
	   
	for (classNumber=1; classNumber<=ncls; classNumber++) 
	   {
				// Compute Quadratic Term												

		d_ptr = wk1;
		mean_ptr = stat_ptr->vmean;
		tsi_ptr = si_ptr1;
		for (channel=0; channel<nband; channel++)
   		*d_ptr++ = *tsi_ptr++ - *mean_ptr++;

   	symtrix (wk1, stat_ptr->covinv, &rrDivide2, nband);
   	rrDivide2 /= 2;

		dValue = -rrDivide2 + stat_ptr->classConstantML;

		if (dValue > discriminantMax)
			{
   		discriminantMax = dValue;
   		rrDivide2Max = rrDivide2;
   		class_assigned = classNumber;
   		
			}	// "if (dValue > discriminantMax) "

		stat_ptr++;
		
   	}	// for class

	*epix_ptr = -(SInt32)class_assigned;
	*rrDivide2Ptr = rrDivide2Max;
   
}	// end "classify_pixel_using_ML" 



//------------------------------------------------------------------------------------
// FUNCTION : loglik_echo
// Purpose  : Compute Cell Loglik and find cell class.
// Coded By: 			Byeungwoo Jeon			Date: 01/01/1989
// Revised by: 		Byeungwoo Jeon			Date: 09/28/1991
//	Revised By:			Larry L. Biehl			Date: 12/29/2005
//------------------------------------------------------------------------------------

void loglik_echo (
				statistics*							class_stat,
				SInt16								nband,
				UInt32								ncls,
				HDoublePtr							si_ptr0,
				HFldLikPtr							fldlik_ptr,
				SInt32								line_x,
				HDoublePtr							ex2,
				double*								ex,
				double*								xmax,
				SInt16*								cell_class,
				SInt16								cell_width)
				
{
   double								discriminantMax,
   										dValue1,
   										dValue2;
   
   double*								lexptr;

	statistics*							stat_ptr;
   
   HDoublePtr							d_ptr,
   										d_ptr1,
   										si_ptr1,
   										si_ptr2;
   									
   SInt32								ix,
   										iy,
   										kx,
   										ky;
   									
   UInt32								classIndex;
   
   
			// Initialize ex (nband x 1) vector and ex2 (nband x nband) upper right 
			// triangular matrix.
   
   d_ptr 	= ex;
   d_ptr1 	= ex2;
   
   for (kx=1; kx<=nband; kx++) 
   	{
		*d_ptr = 0;
		d_ptr++;
	
		for (ky=kx; ky<=nband; ky++)
			{
			*d_ptr1 = 0;
			d_ptr1++;
			
			}	// end "for (ky=kx; ky<=nband; ky++)"
	
   	}	// for kx 

			// Compute ex2 (upper right triangular matrix) and ex
			//		ex = sum of xi's for all pixels in cell where i represents band
			//		ex2 = sum of xi*xj for all pixels in cell where i and j represents
			//				band.

   for (ix=0; ix<cell_width; ix++) 
   	{
      si_ptr1  = si_ptr0; 
      for (iy=0; iy<cell_width; iy++) 
      	{
      	lexptr = ex;	
      	d_ptr1 = ex2;
   	
			for (kx=1; kx<=nband; kx++) 
				{		
	   		si_ptr2 = si_ptr1;
	   		*lexptr += dValue1 = *si_ptr1;		// ex 
	   		lexptr++;
	   		si_ptr1++;
	   
	   		for (ky=kx; ky<=nband; ky++)
	   			{
	      		*d_ptr1 += dValue1 * *si_ptr2;	// ex2
	      		d_ptr1++;
	      		si_ptr2++;
	      		
	      		}	// end "for (ky=kx; ky<=nband; ky++)"
	   
   			}	// end "for (kx=1; kx<=nband; kx++)"
	
      	}	// end "for (iy=0; iy<cell_width; iy++)"
      	
      si_ptr0 += line_x;
      
   	}	// end " for (ix=0; ix<cell_width; ix++)"

			// Compute Cell Likelihood Value
   
   *cell_class 		= 0;
   discriminantMax   = -DBL_MAX;
   stat_ptr 			= class_stat;
   
   for (classIndex=0; classIndex<ncls; classIndex++) 
   	{
      dValue1 = 0;
      		
      		// sprd represents 4 * Mt_Invcov_M for given class.
      		
      dValue2 = stat_ptr->sprd;
      
      		// Now calculate Xt_Invcov_X for given class
      		
      d_ptr  = ex2;
      d_ptr1 = stat_ptr->covinv;
      for (kx=1; kx<=nband; kx++) 
      	{
	 		dValue2 += *d_ptr1 * *d_ptr;	 // Diagonal
	 		d_ptr1++;
	 		d_ptr++;
	 
         for (ky=kx+1; ky<=nband; ky++)
         	{
	   		dValue1 += *d_ptr1 * *d_ptr; // Off Diagonal
	   		d_ptr1++;
	   		d_ptr++;
	   		
	   		}	// end "for (ky=kx+1; ky<=nband; ky++)"
	 
	 		d_ptr1 += kx;
	 
      	}	// for kx
      	
      		// Now calculate -.5 * (Xt_Invcov_X + Mt_Invcov_M)

      dValue2 /= 2;
      dValue2 += dValue1;
      dValue2 = -dValue2;
      
      		// Now add in the constant for the class. = cellsize times
      		//  -.5*N * log (2pi) - .5 * log det (cov) + log (prior probability)
      		 
      dValue2 += stat_ptr->classConstant;
      
      		// Now add in Xt * Invcov_M
      		
      d_ptr = stat_ptr->vprd;
      lexptr = ex;	
      for (kx=0; kx<nband; kx++)
      	{ 
			dValue2 += *d_ptr * *lexptr;
			d_ptr++;
			lexptr++;
			
			}	// end "for (kx=0; kx<nband; kx++)"

      fldlik_ptr [classIndex] = dValue2;
      if (dValue2 > discriminantMax) 
      	{
	   	discriminantMax = dValue2;
	   	*cell_class = (SInt16)(classIndex + 1);
	   	
      	}	// "if (dValue2 > discriminantMax)" 
      	
      stat_ptr++;
      
  		}	// for class 
   
   *xmax = discriminantMax;

}	// end "loglik_echo"

