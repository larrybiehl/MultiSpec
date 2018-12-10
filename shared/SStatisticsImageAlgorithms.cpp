//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SStatisticsImageAlgorithms.cpp
//
//	Authors:					Chulhee Lee
//								Larry L. Biehl
//
//	Revision date:			12/21/2017
//
//	Language:				C
//
//	System:					Linux, Macintosh, and Windows Operating Systems
//
//	Brief description:	The purpose of the routines in this file are to create
//								statistics images of the field, class or selected area mean
//								and covariance statistics.  Chulhee Lee developed the idea and
//								the code.
//
//	Functions in file:	FS_gen_make_stat_image_same_scale (USER SPECIFY)
//								FS_make_stat_image_same_scale (DEFAULT)
//								FS_make_cor_mean_std
//
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
/*
	Template for debugging
		int numberChars = sprintf ((char*)gTextString3,
													" SOpnImag: (): %s",
													gEndOfLine);
		ListString ((char*)gTextString3, numberChars, gOutputTextH);	
*/
//------------------------------------------------------------------------------------

#include "SMultiSpec.h" 

#ifdef multispec_lin
	#include "LMultiSpec.h"
#endif


#define HOW_MANY_SD	1.0

#define SPACE_BET_COLOR_CODE 3

long 	WIDTH_COLOR_CODE;
long	TOTAL_WIDTH_COLOR_CODE;
long	COL_NUMBER_HEIGHT;
long	MINIMUM_STI_HEIGHT=100;

long	L_size_col;
long	L_size_row;
long	L_number_enlargement;
long	L_number_width;
long	L_enlargement;
long	L_final_size_row;
long	L_final_size_col;
long	L_total_width;
long	L_color_start_location;
long	L_wave_length_location;
long	L_V_wave_length_location;
long	L_mean_graph_location;
long	L_band_no_location;

#define NO_PALETTE_CLASSES	248
#define PALETTE_OFFSET	2
#define iw 0
#define ib 1
#define ired NO_PALETTE_CLASSES
#define grid_color 120
#define mean_color NO_PALETTE_CLASSES
#define mean_sd_color 2
#define SHIFT_OFFSET3 15

unsigned char  G_white_char=(unsigned char)iw;
unsigned char  G_black_char=(unsigned char)ib;

short int zero[5][3] 	= { 1,1,1,1,0,1,1,0,1,1,0,1,1,1,1};
short int one[5][3] 	= { 0,1,0,0,1,0,0,1,0,0,1,0,0,1,0};
short int two[5][3] 	= { 1,1,1,0,0,1,1,1,1,1,0,0,1,1,1};
short int three[5][3] 	= { 1,1,1,0,0,1,1,1,1,0,0,1,1,1,1};
short int four[5][3] 	= { 1,0,1,1,0,1,1,1,1,0,0,1,0,0,1};
short int five[5][3] 	= { 1,1,1,1,0,0,1,1,1,0,0,1,1,1,1};
short int six[5][3] 	= { 1,1,1,1,0,0,1,1,1,1,0,1,1,1,1};
short int seven[5][3] 	= { 1,1,1,0,0,1,0,0,1,0,0,1,0,0,1};
short int eight[5][3] 	= { 1,1,1,1,0,1,1,1,1,1,0,1,1,1,1};
short int nine[5][3] 	= { 1,1,1,1,0,1,1,1,1,0,0,1,1,1,1};

short int n0[15] 	= { 1,1,1,1,0,1,1,0,1,1,0,1,1,1,1};
short int n1[15] 	= { 0,1,0,0,1,0,0,1,0,0,1,0,0,1,0};
short int n2[15] 	= { 1,1,1,0,0,1,1,1,1,1,0,0,1,1,1};
short int n3[15] 	= { 1,1,1,0,0,1,1,1,1,0,0,1,1,1,1};
short int n4[15] 	= { 1,0,1,1,0,1,1,1,1,0,0,1,0,0,1};
short int n5[15] 	= { 1,1,1,1,0,0,1,1,1,0,0,1,1,1,1};
short int n6[15] 	= { 1,1,1,1,0,0,1,1,1,1,0,1,1,1,1};
short int n7[15] 	= { 1,1,1,0,0,1,0,0,1,0,0,1,0,0,1};
short int n8[15] 	= { 1,1,1,1,0,1,1,1,1,1,0,1,1,1,1};
short int n9[15] 	= { 1,1,1,1,0,1,1,1,1,0,0,1,1,1,1};

short int nA[15]  = { 0,1,0,1,0,1,1,1,1,1,0,1,1,0,1};
short int nB[15]  = { 1,1,0,1,0,1,1,1,1,1,0,1,1,1,0};
short int nC[15]  = { 0,1,1,1,0,0,1,0,0,1,0,0,0,1,1};
short int nD[15]  = { 1,1,0,1,0,1,1,0,1,1,0,1,1,1,0};
short int nE[15]  = { 1,1,1,1,0,0,1,1,0,1,0,0,1,1,1};
short int nF[15]  = { 1,1,1,1,0,0,1,1,0,1,0,0,1,0,0};
short int nG[15]  = { 0,1,1,1,0,0,1,0,1,1,0,1,0,1,1};
short int nH[15]  = { 1,0,1,1,0,1,1,1,1,1,0,1,1,0,1};
short int nI[15]  = { 1,1,1,0,1,0,0,1,0,0,1,0,1,1,1};
short int nJ[15]  = { 0,1,1,0,0,1,0,0,1,1,0,1,0,1,0};
short int nK[15]  = { 1,0,1,1,0,1,1,1,0,1,0,1,1,0,1};
short int nL[15]  = { 1,0,0,1,0,0,1,0,0,1,0,0,1,1,1};
short int nM[15]  = { 1,0,1,1,1,1,1,1,1,1,0,1,1,0,1};
short int nN[15]  = { 1,1,1,1,0,1,1,0,1,1,0,1,1,0,1};
short int nO[15]  = { 0,1,0,1,0,1,1,0,1,1,0,1,0,1,0};
short int nP[15]  = { 0,1,0,1,0,1,1,0,1,1,0,1,0,1,0};
short int nQ[15]  = { 0,1,0,1,0,1,1,0,1,0,1,1,0,0,1};
short int nR[15]  = { 1,1,0,1,0,1,1,1,0,1,0,1,1,0,1};
short int nS[15]  = { 0,1,1,1,0,0,0,1,0,0,0,1,1,1,0};
short int nT[15]  = { 1,1,1,0,1,0,0,1,0,0,1,0,0,1,0};
short int nU[15]  = { 1,0,1,1,0,1,1,0,1,1,0,1,1,1,1};
short int nV[15]  = { 1,0,1,1,0,1,1,0,1,1,0,1,0,1,0};
short int nW[15]  = { 1,0,1,1,0,1,1,1,1,1,1,1,1,0,1};
short int nX[15]  = { 1,0,1,1,0,1,0,1,0,1,0,1,1,0,1};
short int nY[15]  = { 1,0,1,1,0,1,0,1,0,0,1,0,0,1,0};
short int nZ[15]  = { 1,1,1,0,0,1,0,1,0,1,0,0,1,1,1};
short int nSpace[15]  = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // " "
short int nDot[15]  = { 0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}; // "."
short int nUnderScore[15]  = { 0,0,0,0,0,0,0,0,0,0,0,0,1,1,1}; // "_"
short int nMinus[15]  = { 0,0,0,0,0,0,1,1,1,0,0,0,0,0,0}; // "-"

unsigned char mean_bit_map[24] = { ib,iw,ib,ib,ib,iw, iw,ib,iw,ib,iw,ib, iw,ib,iw,ib,iw,ib, iw,ib,iw,ib,iw,ib};
unsigned char sd_bit_map[20] 	= { iw,ib,ib,ib, ib,iw,ib,iw, ib,iw,iw,ib, ib,iw,iw,ib, iw,ib,ib,iw};

typedef struct STI_info_str {
	SInt32	no_wave_length_floating_point;
	SInt32	no_V_wave_length_location;	/* no of vertical wave length locations */
	SInt32	no_H_wave_length_location;	/* no of horizontal wave length locations */
	SInt32	*V_wave_length_location;	/* locations of vertical wave length */
	SInt32	*H_wave_length_location;	/* locations of horizontal wave length */ 
	double	*V_wave_length_value;	/* locations of vertical wave length */
	double	*H_wave_length_value;	/* locations of horizontal wave length */ 


	SInt32	col_grid_space;		/* if col_grid_space>0, overwrite the default column
								   grid space. Set zero to use default value */

	SInt32	row_grid_space_flag;
	double	row_grid_space;		/* if row_grid_space_flag>0, overwrite the default row
								   grid space. Set zero to use default value */
	SInt32	row_grid_space_floating_digit;
	

	SInt32	no_col_grid_indicator;	/* put column grids at the user-specified-locations.
									   no_col_grid_indicator = number of desired column grids.
								       Set zero to use default value */
	SInt32	no_col_grid_indicator_floating_point_digit;	/* number of floating digits of the
	                                                       numbers for column grids. */
	SInt32	*no_col_grid_indicator_location;	/* locations of the desired column grid */
	SInt32	*no_col_grid_indicator_value;		/* numbers for the desired column grid 
												   set MINUS12345 if number is not desired */		/* will be used for graph bit map */
	
	SInt32	upper_lower_bound_flag;		/* overwrite the default upper bound and lower bound.
					  					   Set zero to use default value.
					  					   Will be used for graph bit map */
	double	upper_bound;				/* user specified upper bound */
	double	lower_bound;				/* user specified lower bound */
	
	SInt32	mean_min_max_flag;	/* overwrite the default max & min mean.
					  			   Set zero to use default value.
					  			   Will be used for statistics images */	
	double	meanmax_all;		/* user specified max mean */
	double	meanmin_all;		/* user specified min mean */

	SInt32	std_min_max_flag;	/* overwrite the default max & min standard deviation.
					  			   Set zero to use default value.
					  			   Will be used for statistics images */	
	double	stdmax_all;			/* user specified max sd */
	double	stdmin_all;			/* always zero */

	SInt32	mean_min_max_array_flag;				/* set zero to use default value */
	double	*meanmax;								/* NOT available */
	double	*meanmin;		
	
	SInt32	std_min_max_array_flag;					/* set zero to use default value */
	double	*stdmax;								/* NOT available */
	double	*stdmin;
} STI_INFO_STR;

#define	WAVE_LENGTH_DEFAULT_FLAG 123456789



void		enlarge_cor (
				UInt32								enlargement,
				unsigned char* 					cor,
				UInt32								size_row,
				UInt32								size_col,
				unsigned char* 					final_cor,
				UInt32								final_size_row,
				UInt32								final_size_col);

void 		FS_find_STI_size (
				SInt32								numberFeatures,
				UInt32*								STI_size);

void		FS_gen_make_stat_image_same_scale (
				CLASS_INFO_STR* 					class_info,
				SInt16*								classPtr,
				SInt32								no_class,
				UInt16*								featurePtr,
				SInt32								numberFeatures,
				STI_INFO_STR 						STI_info,
				FileInfoPtr							newFileInfoPtr,
				SInt32*								ERROR_FLAG);

void		FS_make_cor_mean_std (
				UInt16*								featurePtr,
				SInt32								numberFeatures,
				CLASS_INFO_STR* 					class_info,
				SInt16*								classPtr,
				SInt32								no_class,
				FileInfoPtr							newFileInfoPtr,
				double 								max_mean_std_height,
				double 								min_mean_std_height,
				STI_INFO_STR 						STI_info,
				SInt32*								ERROR_FLAG,
				unsigned char                 *cor,
				unsigned char                 *final_cor);

void		FS_make_stat_image_same_scale (
				CLASS_INFO_STR* 					class_info,
				SInt16*								classPtr,
				SInt32								no_class,
				UInt16*								featurePtr,
				SInt32								numberFeatures,
				FileInfoPtr							newFileInfoPtr,
				SInt32*								ERROR_FLAG,
				SInt16								minMaxSettingCode,
				double								user_mean_minimum,
				double								user_mean_maximum);

void		FS_make_half_STI (
				double* 								cov,
				double* 								var,
				unsigned char* 					image,
				SInt32								n);

void		gen_put_number (
				unsigned char* 					final_cor,
				SInt32								final_size_col,
				SInt32								row_pos,
				SInt32								col_pos,
				SInt32								number,
				SInt32								sign,
				SInt32								enlargement);

void		make_even (
				SInt32*								numberPtr);
																
void		put_integer (
				SInt32								start_zero_flag,
				unsigned char* 					final_cor,
				SInt32								final_size_col,
				SInt32								row_pos,
				double								largest_poss_no,
				double								xnumber,
				SInt32								start_col_pos,
				SInt32								max_digit,
				SInt32								enlargement);

void		put_real (
				unsigned char* 					final_cor,
				SInt32								final_size_col,
				SInt32								row_pos,
				double 								largest_poss_no,
				double 								xnumber,
				SInt32								start_col_pos,
				SInt32								max_digit,
				SInt32								enlargement,
				SInt32								number_floating_point);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void enlarge_cor
//
//	Software purpose:	 
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Chulhee Lee				Date: ??/??/19??
//	Revised By:			Larry L. Biehl			Date: 01/15/1998

void enlarge_cor (
				UInt32								enlargement,
				unsigned char* 					cor,
				UInt32								size_row,
				UInt32								size_col,
				unsigned char* 					final_cor,
				UInt32								final_size_row,
				UInt32								final_size_col)
				
{
	unsigned char						corValue;
	
	UInt32 								index_a,
											columnIndex,
											final_corIndex,
											index_b,
											rowIndex;
											
	
	for (rowIndex=0; rowIndex<size_row; rowIndex++)
		{
		for (columnIndex=0; columnIndex<size_col; columnIndex++)
			{
			corValue = cor[rowIndex*size_col + columnIndex];
			
			for (index_a=0; index_a<enlargement; index_a++)
				{
				final_corIndex = (rowIndex*enlargement+index_a)*final_size_col + 
																				columnIndex*enlargement;
							
				for (index_b=0; index_b<enlargement; index_b++)
					{
					final_cor[final_corIndex] = corValue;
					final_corIndex++;
					
					}	// end "for (index_b=0; index_b<enlargement; index_b++)"
															
				}	// end "for (a=0; a<enlargement; a++)"
															
			}	// end "for (j=0; j<size_col; j++)"
															
		}	// end "for (i=0; i<size_row; i++)"

}	// end "enlarge_cor"


 
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FS_find_STI_size
//
//	Software purpose:	 
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Chulhee Lee				Date: ??/??/19??
//	Revised By:			Larry L. Biehl			Date: 08/24/2017

void FS_find_STI_size (
				SInt32								numberFeatures,
				UInt32*								STI_size)
				
				
{
	SInt32 								gap,
											width;
	
	
			// put gap for std and mean
			
	width = (SInt32)(numberFeatures * 0.12);
	gap = (SInt32)(numberFeatures * 0.05 + .5);
	
	if (width<1)
		width = 1;
		
	if (gap<1)
		gap = 1;
	 
	L_total_width = width + gap;
	
	L_size_col = numberFeatures + L_total_width;
	L_size_row = numberFeatures;

			// number size
			// Maybe should consider the number enlargement as an option within the
			// dialog box. Larry Biehl 1.16.97
			
	L_number_enlargement = 1;

	if (numberFeatures>250)
		L_number_enlargement = 2;
	/*		
	if (n>400)
		L_number_enlargement = 4;
		
	if (n>800)
		L_number_enlargement = 8;
	 
	if (n>1600)
		L_number_enlargement = 16;
	 
	if (n>3200)
		L_number_enlargement = 32;
	 
	if (n>6400)
		L_number_enlargement = 64;
	 
	if (n>12800)
		L_number_enlargement = 128;
	*/
	L_number_width = 4 * L_number_enlargement;

			// if dim. is too small, enlarge for mean graph
			
	if (numberFeatures < MINIMUM_STI_HEIGHT)
		L_enlargement=MINIMUM_STI_HEIGHT/L_size_row + 1;
		
	else
		L_enlargement = 1;
	 
			// column number label & color code at the bottom
			
	WIDTH_COLOR_CODE = (long)(0.07*numberFeatures*L_enlargement);
	if (WIDTH_COLOR_CODE < 7)
		WIDTH_COLOR_CODE = 7;
		
	TOTAL_WIDTH_COLOR_CODE = SPACE_BET_COLOR_CODE + WIDTH_COLOR_CODE;
	COL_NUMBER_HEIGHT = 7 * L_number_enlargement;
	
			// assign memory
			
	L_final_size_col = L_size_col * L_enlargement;
   
	L_final_size_row = (long)((L_size_row+numberFeatures)*L_enlargement +
												TOTAL_WIDTH_COLOR_CODE +
														COL_NUMBER_HEIGHT*2 +
																2*L_number_enlargement + 2 +
																			numberFeatures*0.01);
	
	if (STI_size != NULL)
		*STI_size = L_final_size_row * L_final_size_col;

	L_V_wave_length_location = numberFeatures*L_enlargement + 1;
	
			// color code bar in the middle
	/*			
	L_color_start_location=L_size_row*L_enlargement+SPACE_BET_COLOR_CODE;
	L_wave_length_location=L_size_row*L_enlargement+
						   (SPACE_BET_COLOR_CODE+WIDTH_COLOR_CODE)+3+L_number_enlargement;
	L_mean_graph_location=L_final_size_row-COL_NUMBER_HEIGHT;
	L_band_no_location=L_final_size_row-COL_NUMBER_HEIGHT+2*L_number_enlargement;
	*/ 	
			// color code bar at the bottom

	L_wave_length_location = L_size_row*L_enlargement+3+L_number_enlargement;

   L_color_start_location = L_final_size_row-WIDTH_COLOR_CODE + SHIFT_OFFSET3;
	
	L_mean_graph_location = L_final_size_row - COL_NUMBER_HEIGHT -
									(SPACE_BET_COLOR_CODE + WIDTH_COLOR_CODE) + SHIFT_OFFSET3;
									
			// channel number loccation
			
	L_band_no_location = L_final_size_row-COL_NUMBER_HEIGHT + 2*L_number_enlargement -
									(SPACE_BET_COLOR_CODE + WIDTH_COLOR_CODE) + SHIFT_OFFSET3;
													
}	// end "FS_find_STI_size"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FS_gen_make_stat_image_same_scale
//
//	Software purpose:
//							NOTE:
//								stat_image_type=1
//								all classes => same scale
//								mean, standard deviation => same scale
//								all bands of a class => same scale
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Chulhee Lee				Date: ??/??/19??
//	Revised By:			Larry L. Biehl			Date: 12/21/2017

void FS_gen_make_stat_image_same_scale (
				CLASS_INFO_STR* 					class_info,
				SInt16*								classPtr,
				SInt32								no_class,
				UInt16*								featurePtr,
				SInt32								numberFeatures,
				STI_INFO_STR 						STI_info,
				FileInfoPtr							newFileInfoPtr,
				SInt32*								ERROR_FLAG)
								
{
	CMFileStream*						fileStreamPtr;
	
	FileStringPtr						filePathPtr,
											newFileNamePtr;
											
	unsigned char						*cor, *final_cor, erdas_header[128];
	
	double								max_mean_std_height,
											min_mean_std_height,
			 								sd;
	
	UInt32 								i,
											j;
											
	UInt32								count,
											final_cor_size,
											L_image_size_row;
											
	SInt16								errCode;
											
	Boolean								addProjectIdentifierFlag = FALSE;
	
			
				// Make statistics images
				
	if (*ERROR_FLAG == 0)
		{
				// calculate max. and min. of mean and std	
				
		max_mean_std_height= -1e10;
		min_mean_std_height= 1e10;
		for (i=0; i<(UInt32)no_class; i++)
			for (j=0; j<(UInt32)numberFeatures; j++)
				{
				sd = sqrt (fabs ((class_info+i)->fvar[j]));
	  
			 	if ((class_info+i)->fmean[j] +HOW_MANY_SD*sd > max_mean_std_height)
			  		max_mean_std_height = (class_info+i)->fmean[j] +HOW_MANY_SD*sd;
			   
			 	if ((class_info+i)->fmean[j] -HOW_MANY_SD*sd < min_mean_std_height)
			   	min_mean_std_height = (class_info+i)->fmean[j] -HOW_MANY_SD*sd;
			   	
			 	}	// end "for (j=0; j<n; j++)"

				// check user specified value	
			
		if (STI_info.mean_min_max_flag == 3)
			{
			max_mean_std_height = STI_info.meanmax_all;
			min_mean_std_height = STI_info.meanmin_all;
			
			}	// end "if (STI_info.mean_min_max_flag == 3)"
		
				// Initialize some parameters.
				
		FS_find_STI_size (numberFeatures, NULL);
      
				// create memory for statistic image files
				
		final_cor_size = no_class * L_size_row * L_size_col * sizeof (char);
      cor = (unsigned char*)MNewPointer (final_cor_size);
      
      if (cor == NULL)
			{
			*ERROR_FLAG = 110;																				
																										return;
			
			}	// end "if (cor == NULL)"
   
				// Initialize cor memory to be all index to white pixels.
				
		for (i=0; i<final_cor_size; i++)
			cor[i] = G_white_char;

				// Allocate memory for image
				
      L_image_size_row = L_final_size_row + 20;
      final_cor_size = no_class * L_image_size_row * L_final_size_col * sizeof (char);
      final_cor = (unsigned char*)MNewPointer (final_cor_size);
      
      if (final_cor == NULL)
			{
			*ERROR_FLAG = 111;
																										return;
			
			}	// end "if (final_cor == NULL)"
   
				// Initialize final_cor memory to be all index to white pixels.
   
		for (i=0; i<final_cor_size; i++)
			final_cor[i] = G_white_char;
      
      FS_make_cor_mean_std (featurePtr,
										numberFeatures,
										class_info,
										classPtr, 
										(SInt32)no_class,
										newFileInfoPtr,
										max_mean_std_height,
										min_mean_std_height,
										STI_info,
										ERROR_FLAG,
										cor,
										final_cor);
	 
		if (*ERROR_FLAG != 0)
																										return;
			
				// Make ERDAS Header
      
      newFileInfoPtr->numberLines = L_image_size_row;
      newFileInfoPtr->numberColumns = L_final_size_col * no_class;
     
      newFileInfoPtr->numberClasses = NO_PALETTE_CLASSES + 2;
      newFileInfoPtr->mapProjectionHandle = NULL;
     
     
      CMFileStream* newFileStreamPtr = GetFileStreamPointer (newFileInfoPtr);
  
      FileStringPtr newFilePathPtr =
						(FileStringPtr)GetFilePathPPointerFromFileStream (newFileStreamPtr);
      
      if (gProjectInfoPtr != NULL)
			{
         fileStreamPtr = GetFileStreamPointer (gProjectInfoPtr);
         filePathPtr =
						(FileStringPtr)GetFilePathPPointerFromFileStream (fileStreamPtr);
			
			if (filePathPtr[0] == 0)
				{
						// This is case when project has not been saved so there
						// is no name yet. Use the image name.
						
				filePathPtr =
						(FileStringPtr)GetFilePathPPointerFromFileInfo (gImageFileInfoPtr);
				addProjectIdentifierFlag = TRUE;
				
				}	// end "if (filePathPtr[0] == 0)"
				
			}	// end "if (gProjectInfoPtr != NULL)"
    
      else	// gProjectInfoPtr == NULL
         filePathPtr =
						(FileStringPtr)GetFilePathPPointerFromFileInfo (gImageFileInfoPtr);
         
		BlockMoveData ((UCharPtr)filePathPtr,
							newFilePathPtr,
							filePathPtr[0] + 1); 
			
		RemoveSuffix (newFilePathPtr);

		if (addProjectIdentifierFlag)
			{
			sprintf ((char*)&newFilePathPtr[newFilePathPtr[0]+1], (char*)"_project");
			 						
			newFilePathPtr[0] += 8;
			
			}	// end "if (addProjectIdentifierFlag)"
  
		sprintf ((char*)&newFilePathPtr[newFilePathPtr[0]+1], (char*)".sti");
			 						
		newFilePathPtr[0] += 4;
		  
		LoadNewErdasHeader (newFileInfoPtr, (unsigned char*)erdas_header, kErdas74Type);
		  
		#if defined multispec_mac
					// Force the uniFileName to be recreated to match the support file
					// name.
			newFileStreamPtr->uniFileName.length = 0;
		#endif	// defined multispec_mac

				// Now get wide character and unicode names.

		SetFileDoesNotExist (newFileStreamPtr, kKeepUTF8CharName);
		UpdateFileNameInformation (newFileStreamPtr, NULL);
      
      newFileNamePtr =
					(FileStringPtr)GetFileNamePPointerFromFileStream (newFileStreamPtr);

		#if defined multispec_lin
					// For mygeohub version, we need to make sure that we are not trying
					// to write to the default input image file directory since it is read
					// only.
					// For MultiSpec on mygeohub, need to change the working directory for
					// saving files if the current working directory is the read only data
					// directory.
					// Use wxSetWorkingDirectory (wxString) or wxFileDialog::SetDirectory
					// to do this. Use wxGetCwd or wxFileDialog::GetDirectory to determine
					// if the default one is the standard directory.
			#ifndef NetBeansProject
				FileStringPtr			fileNamePtr;
				
				filePathPtr = (FileStringPtr)newFileStreamPtr->GetFilePathPPtr ();
				wxString wxFilePathName (&filePathPtr[1], wxConvUTF8);
				wxFileName fileName;
				fileName.Assign (wxFilePathName);

				if (!fileName.IsDirWritable ())
					{
							// Need to change the directory to the working output directory
							
					fileNamePtr = (FileStringPtr)newFileStreamPtr->GetFileNameCPtr ();
					
					wxString workingDirectory = wxGetCwd ();
					workingDirectory.Append ("/");
					workingDirectory.Append (fileNamePtr);
					wxCharBuffer charWorkingDirectory = workingDirectory.ToAscii ();
		
					newFileStreamPtr->SetFilePathFromCharString (
															(StringPtr)charWorkingDirectory.data (),
															TRUE);
									
					//SetFileDoesNotExist (resultsFileStreamPtr, kKeepUTF8CharName);

					}	// end "if (!fileName.IsDirWritable ())"
					
				else 
					{
							// Directory is writable
					/*
					SInt16 numberChars9 = sprintf ((char*)gTextString3,
															" Directory is writable: %s", 
															gEndOfLine);
					ListString ((char*)gTextString3, numberChars9, gOutputTextH);
					*/
					}
			#endif	// end "#ifndef NetBeansProject"
		#endif	// end "#if defined multispec_lin"
									
      errCode = CreateNewFile (newFileStreamPtr,
											GetVolumeReferenceNumber (newFileStreamPtr),
											gCreator,
											kErrorMessages,
											kReplaceFlag);
      
		if (errCode != noErr)
			*ERROR_FLAG = 200;

      if (errCode == noErr)
			{
         count = 128;
         errCode = MWriteData (newFileStreamPtr, 
											&count,
											erdas_header, 
											kNoErrorMessages);
		
			if (errCode != noErr)
				*ERROR_FLAG = 201;
		
			}	// end "if (errCode == noErr)"
	
      if (errCode == noErr)
			{
         count = L_image_size_row * L_final_size_col *no_class;
			//count = final_cor_size * 2;
      
         errCode = MWriteData (newFileStreamPtr, 
											&count,
											final_cor, 
											kNoErrorMessages);
		
			if (errCode != noErr)
				*ERROR_FLAG = 202;
		
			}	// end "if (errCode == noErr)"
		
		if (*ERROR_FLAG != 200)
			IOCheck (errCode, newFileStreamPtr);
	
				// " Image statistics file, '%s', written to disk.\r", 
				
      ListSpecifiedStringNumber (kStatisticsImageStrID,
											IDS_StatisticsImage19,
											(CMFileStream*)NULL, 
											gOutputForce1Code, 
											newFileNamePtr, 
											*ERROR_FLAG == noErr);

      CloseFile (newFileInfoPtr);
		
				//free memory
      
      CheckAndDisposePtr (final_cor);
      CheckAndDisposePtr (cor);
		/*				
		for (i=0; i<no_class; i++)
			{
		 	FS_make_cor_mean_std (n, 
											class_info,
											i,
											newFileInfoPtr,
											max_mean_std_height,
											min_mean_std_height,
											STI_info,
											ERROR_FLAG);
	 
	 		if (*ERROR_FLAG!=0)
	  			break;
	  	
			}	// end "for (i=0; i<no_class; i++)"
		*/
		}	// end "if (*ERROR_FLAG == 0)"
	
}	// end "FS_gen_make_stat_image_same_scale"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FS_make_cor_mean_std
//
//	Software purpose:	 
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Chulhee Lee				Date: ??/??/19??
//	Revised By:			Larry L. Biehl			Date: 08/24/2017

void FS_make_cor_mean_std (
				UInt16*								featurePtr,
				SInt32								numberFeatures,
				CLASS_INFO_STR* 					class_info,
				SInt16*								classPtr,
				SInt32								no_class,
				FileInfoPtr							newFileInfoPtr,
				double 								max_mean_std_height,
				double 								min_mean_std_height,
				STI_INFO_STR 						STI_info,
				SInt32*								ERROR_FLAG,
            unsigned char                 *cor,
            unsigned char                 *final_cor)
							
{	
	double 								avail_range,
											difference,
											grid_space,
											longest_number,
											low_bound,
											sd,
											upper_bound,
											x,
											y;
									
	double								*fmean;
									
	SInt32								a,
											avail_mean_height,
											bottomGraphLimit,
											col_grid_label_space,
											col_grid_space,
											col_pos,
											col_pos_end,
											col_pos_start,
											cor_index,
											final_total_width,
											grid_dot_space = 3,
											grid_space_floating_digit,
											i,
											imean,
											isd,
											j,
											k,
											max_digit,
											row_pos,
											start_zero_flag=0,
											topGraphLimit;
									
	UInt32								class_index,
											classNameIndex,
											final_cor_size;
											
	SInt16								featureNumber;
	
	UInt16								stddev_color;
	
   
	FS_find_STI_size (numberFeatures, &final_cor_size);
	
			// Initialize; initialization now done in calling routine.
	/*		
   for (i=0; i< (final_cor_size*index); i++)
		final_cor[i] = G_white_char;
   
   for (i=0; i<(L_size_row * L_size_col * index); i++)
		cor[i] = G_white_char;
	*/
	if (STI_info.mean_min_max_flag == 1)
		{
				// The user did not specify the min and max for the plot.
				// Minimums and maximums are based on the data for all classes/field.
				// Allow a little extra space.
			
		if (max_mean_std_height > 0)
			max_mean_std_height *= 1.03;
		else
			max_mean_std_height *= 0.97;
		
		if (min_mean_std_height > 0)
			min_mean_std_height *= 0.97;
		else
			min_mean_std_height *= 1.03;
		
		}	// end "if (STI_info.mean_min_max_flag == 0)"
   
   for (class_index=0; class_index<(UInt32)no_class; class_index++)
		{
      fmean = (class_info+class_index)->fmean;
      
      *ERROR_FLAG = 0;
		
				// Put correlation in color code
	
      for (k=i=0; i<numberFeatures; i++)
			{
         for (j=0; j<=i; j++,k++)
            cor[L_size_col*i+j + class_index * L_size_row * L_size_col] = 
							cor[L_size_col*j+i + class_index * L_size_row * L_size_col] 
									= (class_info+class_index)->half_image[k];
							
			}	// end "for (k=i=0; i<n; i++)"

				// Enlarge correlation matrix if necessary

      enlarge_cor ((UInt32)L_enlargement, 
							cor + (class_index * L_size_row * L_size_col),
							(UInt32)L_size_row,
							(UInt32)L_size_col,
							final_cor + (class_index * L_final_size_col) +
																15 * (no_class * L_final_size_col),
							(UInt32)L_final_size_row,
							(UInt32)(L_final_size_col*no_class));

				// Put color code

      for (j=0; j<numberFeatures*L_enlargement; j++)
			{
         x = j;
         x /= numberFeatures * L_enlargement;
         x *= NO_PALETTE_CLASSES;
         x += PALETTE_OFFSET;
         for (i=L_color_start_location; 
						i<WIDTH_COLOR_CODE+L_color_start_location;
							i++)
            final_cor[i*L_final_size_col*no_class+j+(class_index * L_final_size_col)] =
																							(unsigned char)x;
			
			}	// end "for (j=0; j<n*L_enlargement; j++)"
			
				// find grid space, upper_bound,low_bound
			
      avail_mean_height = numberFeatures * L_enlargement;
						
		if (STI_info.mean_min_max_flag == 2)
			{
					// Per class or field min max to be used
					// Get the min max for this class 
					
			max_mean_std_height= -1e10;
			min_mean_std_height= 1e10;
			for (j=0; j<numberFeatures; j++)
				{
				sd = sqrt (fabs ((class_info+class_index)->fvar[j]));
	  
			 	if ((class_info+class_index)->fmean[j] + HOW_MANY_SD*sd >
																					max_mean_std_height)
			  		max_mean_std_height = (class_info+class_index)->fmean[j] +
																							HOW_MANY_SD*sd;
			   
			 	if ((class_info+class_index)->fmean[j] - HOW_MANY_SD*sd <
																					min_mean_std_height)
			   	min_mean_std_height =
										(class_info+class_index)->fmean[j] - HOW_MANY_SD*sd;
			   	
			 	}	// end "for (j=0; j<n; j++)"
					
					// Allow a little extra space.
				
         if (max_mean_std_height > 0)
            max_mean_std_height *= 1.03;
         else
            max_mean_std_height *= 0.97;
			
         if (min_mean_std_height > 0)
            min_mean_std_height *= 0.97;
         else
            min_mean_std_height *= 1.03;
			
			}	// end "if (STI_info.mean_min_max_flag == 2)"
      
      difference = max_mean_std_height - min_mean_std_height;
      
				// find grid space in the power of 10
			
      grid_space = 1;
      
      if (difference > 1)
			{
         for (i=0; i<100; i++)
				{
            if (difference <10)
               break;
				
            difference /= 10;
            grid_space *= 10;
			
				}	// end "for (i=0; i<100; i++)"
			
			}	// end "if (difference>1)"
			
      else	// if (difference <= 1)
			{
			for (i=0; i<100; i++)
				{
				if (difference >1)
					break;
				
				difference *= 10;
				grid_space /= 10;
				
				}	// end "for (i=0; i<100; i++)"
				
			}	// end "else if (difference <= 1)"
      
				// Find upper bound and lower bound in multiples of grid space

      i = (SInt32)(min_mean_std_height/grid_space);
      
      if (min_mean_std_height > 0)
         low_bound = i*grid_space;
      else
         low_bound = (i-1)*grid_space;

      j = (SInt32)(max_mean_std_height/grid_space);
      
      if (max_mean_std_height > 0)
         upper_bound = (j+1)*grid_space;
      else
         upper_bound = j * grid_space;

				// if grid space is too large, reduce it
			
      difference = max_mean_std_height - min_mean_std_height;
      
      if (grid_space > 0)
			{
         if (difference/grid_space<2)
            grid_space /= 5;
			
         else if (difference/grid_space<5)
            grid_space /= 2;
			
			}	// if (grid_space>0)
      
				// find new upper bound and lower bound with new grid space
			
      for (k=0; k<100; k++)
			{
         if (upper_bound-grid_space<max_mean_std_height)
            break;
         upper_bound -= grid_space;
		
			}
		
      for (k=0; k<100; k++)
			{
         if (low_bound+grid_space>min_mean_std_height)
            break;
         low_bound += grid_space;
		
			}	// end "for (k=0; k<100; k++)"
      
				// check whether it is possible to save one more grid space
			
      if (low_bound + 0.5*grid_space<min_mean_std_height &&
	    						upper_bound - 0.5*grid_space>max_mean_std_height)
			{
         upper_bound -= 0.5*grid_space;
         low_bound += 0.5*grid_space;
		
			}	// end "if (low_bound + ..."

				// col grid space
			
      if (numberFeatures > 12800)
			{
         col_grid_space = 400;
         col_grid_label_space = 3200;
		
			}
		
      else if (numberFeatures > 6400)
			{
         col_grid_space = 200;
         col_grid_label_space = 1600;
		
			}
		
      else if (numberFeatures > 3200)
			{
         col_grid_space = 100;
         col_grid_label_space = 800;
		
			}
		
      else if (numberFeatures > 1600)
			{
         col_grid_space = 50;
         col_grid_label_space = 400;
		
			}
		
      else if (numberFeatures > 800)
			{
         col_grid_space = 20;
         col_grid_label_space = 200;
		
			}
		
      else if (numberFeatures > 400)
			{
         col_grid_space = 20;
         col_grid_label_space = 100;
		
			}
		
      else if (numberFeatures > 150)
			{
         col_grid_space = 10;
         col_grid_label_space = 40;
		
			}
		
      else if (numberFeatures > 100)
			{
         col_grid_space = 10;
         col_grid_label_space = 20;
		
			}
      
      else if (numberFeatures > 20)
			{
         col_grid_space = 5;
         col_grid_label_space = 10;
		
			}
		
      else if (numberFeatures > 10)
			{
         col_grid_space = 2;
         col_grid_label_space = 2;
		
			}
		
      else 
			{
         col_grid_space = 1;	// no grid
         col_grid_label_space = 1;
		
			}
      
				// grid_space_floating_digit
			
      if (grid_space > 1)
         grid_space_floating_digit = 0;
		
      else
         grid_space_floating_digit = (SInt32)(fabs (log10 (grid_space)) + .999);
	 
			// Check user specified upper and lower bound & grid_space
			
      if (STI_info.col_grid_space > 0)
         col_grid_space=STI_info.col_grid_space;
	
      if (STI_info.row_grid_space_flag > 0)
			{
         grid_space=STI_info.row_grid_space;
         grid_space_floating_digit=STI_info.row_grid_space_floating_digit;
		
			}	// end "if (STI_info.row_grid_space_flag > 0)"
		
      if (STI_info.upper_lower_bound_flag > 0)
			{
         upper_bound=STI_info.upper_bound;
         low_bound=STI_info.lower_bound;
		
			}	// end "if (STI_info.upper_lower_bound_flag > 0)"
      
      avail_range = upper_bound-low_bound;

				// put row grid for mean graph	

      final_total_width = L_enlargement*L_total_width;
      max_digit = (final_total_width-2)/L_number_width;
      
      for (x=grid_space; x<upper_bound-low_bound; x+=grid_space)	// row grid
			{
         k = (SInt32)(x/avail_range*avail_mean_height);
         k = L_mean_graph_location-1-k;
         if (k==k/2*2)
            k++;

         for (j=1; j<numberFeatures*L_enlargement; j+=grid_dot_space)
            final_cor[k*L_final_size_col*no_class+j+class_index*L_final_size_col] =
																									grid_color;
			
			}	// end "for (x=grid_space; x<upper_bound-low_bound; x+=grid_space)"
    
				// put column grid for mean graph	

      if (STI_info.no_col_grid_indicator == 0)
			{
         for (i=col_grid_space; i<numberFeatures; i+=col_grid_space)	// col grid
            for (j=0; j<numberFeatures*L_enlargement; j+=grid_dot_space)
               final_cor[
                  (L_mean_graph_location-1-j)*L_final_size_col*no_class+i*L_enlargement
											+ class_index * L_final_size_col] = grid_color;

         row_pos = L_band_no_location;
     
         int textLength;
     
         if (gProjectInfoPtr != NULL)
				{
				classNameIndex = classPtr[class_index] - 1;
            textLength = (int)
						strlen ((char*)gProjectInfoPtr->classNamesPtr[classNameIndex].name);
         
						// block characters
            if (textLength > (L_final_size_col/5))
               textLength = (L_final_size_col/5)-2;
 
						// Generate class text
				
            for (i = 1; i < textLength; i++)
					{     
               gen_put_number (
							final_cor + (class_index * L_final_size_col),
							L_final_size_col*no_class,
							//row_pos + 20,
							5,
									// Make it cental alignment
							((L_final_size_col- (5*textLength))/2-10)+5*(i - 1),
							(SInt32)gProjectInfoPtr->classNamesPtr[classNameIndex].name[i],
							1,
							L_number_enlargement);

					}	// end "for (i = 1; i < textLength; i++)"
					
				}	// end "if (gProjectInfoPtr != NULL)"
   
         for (i=col_grid_label_space; 
						//i<=n-col_grid_label_space;
						i<=numberFeatures;  
							i+=col_grid_label_space)
				{
						// Get the position for the band number. Do this by computing
						// the column position for the beginning and end of the band
						// and position it in the middle of it.
				
						// Start of band position
	 				
            x = i - 1;
            
            j = 1;
            if (i > 1)
               j = (SInt32)((log10 (fabs (x))+.999));
	   		
            j *= 4*L_number_enlargement;
            j /= 2;
            col_pos_start = (i-1)*L_enlargement - j;
	  		
						// End of band position
	 						
  						// Get the actual feature/channel number
						
				featureNumber = featurePtr[i-1] + 1;
				x = featureNumber;
						
            j = 1;
            if (i > 1)
               j = (SInt32)((log10 (fabs (x))+.999));
	   		
            j *= 4*L_number_enlargement;
            j /= 2;
            col_pos_end = i*L_enlargement - j;

            col_pos = (col_pos_start + col_pos_end)/2;
            if (col_pos < 0)
               col_pos = 0;
            
						// Now insert the band number into the image.
						
            put_integer (start_zero_flag,
									final_cor + (class_index * L_final_size_col),
									L_final_size_col*no_class, 
									row_pos, 
									x, 
									x, 
									col_pos, 
									max_digit, 
									L_number_enlargement);
           
										
				}	// "for (i=col_grid_label_space; ..."
			
			}	// if (STI_info.no_col_grid_indicator == 0)
      
      longest_number = MAX (fabs (low_bound+grid_space), fabs (upper_bound));
			
      k=1;
      for (i=0; i<grid_space_floating_digit; i++)
         k *= 10;
		
      for (x=low_bound+grid_space; x<upper_bound; x+=grid_space)
			{
					// find row position for number 
				
         y = x-low_bound;
         i = (SInt32)(y/avail_range*avail_mean_height);
         row_pos = L_mean_graph_location-1-i-2*L_number_enlargement;
 
					// find the number of digit of integer part
 				
         y = log10 (fabs (upper_bound));
         if (y < 0)
            i = 1;
         else
            i = (SInt32)(y + 0.999);
  
         col_pos = numberFeatures*L_enlargement + 1;
         if (L_number_enlargement > 1)
            col_pos = numberFeatures*L_enlargement + 3;
  
         put_real (final_cor + (class_index * L_final_size_col), 
							L_final_size_col*no_class, 
							row_pos,
							longest_number, 
							x, 
							col_pos, 
							max_digit, 
							L_number_enlargement, 
							grid_space_floating_digit);
		
			}	// end "for (x=low_bound+grid_space; x<upper_bound; x+=grid_space)"
    
				//  Put mean graph as bit map	
			
      bottomGraphLimit = L_mean_graph_location - 1;
      topGraphLimit =  L_mean_graph_location - 1 - numberFeatures*L_enlargement;

      for (i=0; i<numberFeatures; i++)
			{
         imean = (SInt32)((fmean[i]-low_bound)/avail_range*avail_mean_height);

			//imean_min = min_mean_std_height;
 
         isd = (SInt32)(HOW_MANY_SD*sqrt ((double)(class_info+class_index)->fvar[i]) /
																		avail_range*avail_mean_height);
 
					// mean +- sd
				
         j = L_mean_graph_location - 1 - imean;
         
         for (k=1; k<=isd; k++)
				{
            stddev_color = (UInt16)(mean_color - 20 - 30.*k/isd);
		
						// Check out of bound
			
            if (j+k < bottomGraphLimit && j+k > topGraphLimit)
					{
               for (a=0; a<L_enlargement; a++)
						{
                  cor_index = (j+k)*L_final_size_col*no_class + i*L_enlargement + a
																	+ class_index * L_final_size_col;
                  
                 
                  if (cor_index >= 0 && cor_index < (final_cor_size*no_class))
                     final_cor[cor_index] = (unsigned char)stddev_color;
						
						}	// end "for (a=0; a<L_enlargement; a++)"
					
					}	// end "if (j+k < bottomGraphLimit && j+k > topGraphLimit)"

						// check out of bound
						
            if (j-k < bottomGraphLimit && j-k > topGraphLimit)
				//if (j+k > L_mean_graph_location-1-n*L_enlargement)
					{
               for (a=0; a<L_enlargement; a++)
						{
                  cor_index = (j-k)*L_final_size_col*no_class + i*L_enlargement + a
																	+ class_index * L_final_size_col;
                  if (cor_index >= 0 && cor_index < (final_cor_size*no_class))
                     final_cor[cor_index] = (unsigned char)stddev_color;
				
						}	// end "for (a=0; a<L_enlargement; a++)"
											
					}	// end "if (j-k < bottomGraphLimit && j-k > topGraphLimit)"
				
				}	// end "for (k=1; k<=isd; k++)"

					// mean
				
			if (imean>0 && imean<numberFeatures*L_enlargement)	// check out of bound
				{
				j = L_mean_graph_location - 1 - imean;
				for (a=0; a<L_enlargement; a++)
					{
					cor_index = j*L_final_size_col*no_class + i*L_enlargement + a +
																		class_index * L_final_size_col;
					if (cor_index >= 0 && cor_index < (final_cor_size*no_class))
						final_cor[cor_index] = (unsigned char)mean_color;
																	
					}	// end "for (a=0; a<L_enlargement; a++)"
																	
				}	// end "if (imean > 0 && ..."
			
			}	// end "for (i=0; i<n; i++)"

				// Put frame	
		
      for (i=0; i<numberFeatures*L_enlargement; i++)
         final_cor[(L_mean_graph_location-1)*L_final_size_col*no_class+i+
																(class_index * L_final_size_col)] =
            final_cor[(L_mean_graph_location-numberFeatures*L_enlargement)*
						L_final_size_col*no_class+i+ (class_index * L_final_size_col)] =
																			grid_color; // G_black_char;
      
      for (i=0; i<numberFeatures*L_enlargement; i++)
         final_cor[(L_mean_graph_location-1-i)*L_final_size_col*no_class+0+
																(class_index * L_final_size_col)] =
            final_cor[(L_mean_graph_location-1-i)*L_final_size_col*
									no_class+numberFeatures*L_enlargement-1 +
																(class_index * L_final_size_col)] =
																			grid_color; // G_black_char;
      
		}	// end for class_index

}	// end "FS_make_cor_mean_std"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FS_make_half_STI
//
//	Software purpose:	
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Chulhee Lee				Date: ??/??/19??
//	Revised By:			Larry L. Biehl			Date: 01/15/1998

void FS_make_half_STI (
				double* 								cov,
				double* 								var,
				unsigned char* 					image,
				SInt32								n)
				
				
{
	double 								x,
											y;
	
	SInt32 								i,
											j,
											k,
											l;
											
											
	for (l=0,i=0; i<n; i++)
		{
		for (j=0; j<=i; j++,l++)
			{
			x = cov[l];
			y = var[i] * var[j];

			y = sqrt (fabs (y));
			if (y <= 0)	// no variation in the band
	   		k = G_black_char;
			
			else
				{
				x /= y;
				k = (SInt32)((x+1) * 125);
				
				}
	   
			if (k >= NO_PALETTE_CLASSES)
				k = NO_PALETTE_CLASSES - 1;
				
			if (k < 0)
				k = 0;

			image[l] = (unsigned char)(k+PALETTE_OFFSET);
	  
			}	// end "for (j=0; j<=i; j++,l++)"
			
		}	// end "for (l=0,i=0; i<n; i++)"
	 
}	// end "FS_make_half_STI"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void FS_make_stat_image_same_scale
//
//	Software purpose:	 
//							NOTE:
//								stat_image_type=1
//								all classes => same scale
//								mean, standard deviation => same scale
//								all bands of a class => same scale
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Chulhee Lee				Date: ??/??/19??
//	Revised By:			Larry L. Biehl			Date: 08/24/2017

void FS_make_stat_image_same_scale (
				CLASS_INFO_STR* 					class_info,
				SInt16*								classPtr,
				SInt32								no_class,
				UInt16*								featurePtr,
				SInt32								numberFeatures,
				FileInfoPtr							newFileInfoPtr,
				SInt32*								ERROR_FLAG,
				SInt16								minMaxSettingCode,
				double								user_mean_minimum,
				double								user_mean_maximum)
				
{
	struct STI_info_str 				STI_info;
	SInt32								i;
	
	
	*ERROR_FLAG = 0;
	
			// Make it all default
			
	STI_info.row_grid_space_flag = 0;
	STI_info.upper_lower_bound_flag = 0;
	STI_info.col_grid_space = 0;
	STI_info.row_grid_space = 0;
	STI_info.no_col_grid_indicator = 0;
	STI_info.no_col_grid_indicator_floating_point_digit = 0;
	STI_info.mean_min_max_flag = (SInt32)minMaxSettingCode;
	STI_info.std_min_max_flag = 0;
	STI_info.mean_min_max_array_flag = 0;
	STI_info.std_min_max_array_flag = 0;
	STI_info.no_V_wave_length_location = 0;
	STI_info.no_H_wave_length_location = 0;
	
	STI_info.no_wave_length_floating_point = 0;
	
	STI_info.no_col_grid_indicator_location = NULL;
	STI_info.no_col_grid_indicator_value = NULL;
	
	STI_info.V_wave_length_location = NULL;
	STI_info.H_wave_length_location = NULL;
	
	STI_info.meanmax = NULL;
	STI_info.meanmin = NULL;
	
	STI_info.stdmax = NULL;
	STI_info.stdmin = NULL;
	
	STI_info.V_wave_length_value = NULL;
	STI_info.H_wave_length_value = NULL;
	
	STI_info.meanmin_all = user_mean_minimum;
	STI_info.meanmax_all = user_mean_maximum;
	
	//STI_info.std_min_max_flag = 1;
	STI_info.stdmax_all = 0;
	STI_info.stdmin_all = 0;
	
			// Assign memory
			
	i = sizeof (SInt32) * numberFeatures;
	
	STI_info.no_col_grid_indicator_location = (SInt32*)MNewPointer (i);
   
	if (STI_info.no_col_grid_indicator_location == NULL)
		*ERROR_FLAG = 100;
		
	if (*ERROR_FLAG == 0)
		{
		STI_info.no_col_grid_indicator_value = (SInt32*)MNewPointer (i);

		if (STI_info.no_col_grid_indicator_value == NULL)
			*ERROR_FLAG = 101;
			
		}	// end "if (*ERROR_FLAG == 0)"
	
	if (*ERROR_FLAG == 0)
		{
		STI_info.V_wave_length_location = (SInt32*)MNewPointer (i);

		if (STI_info.V_wave_length_location == NULL)
			*ERROR_FLAG = 102;
			
		}	// end "if (*ERROR_FLAG == 0)"
		
	if (*ERROR_FLAG == 0)
		{
		STI_info.H_wave_length_location = (SInt32*)MNewPointer (i);

		if (STI_info.H_wave_length_location == NULL)
			*ERROR_FLAG=103;
			
		}	// end "if (*ERROR_FLAG == 0)"
	
	i = sizeof (double) * numberFeatures;
	
	if (*ERROR_FLAG == 0)
		{
		STI_info.meanmax = (double*)MNewPointer (i);
		if (STI_info.meanmax == NULL)
			*ERROR_FLAG = 104;
			
		}	// end "if (*ERROR_FLAG == 0)"
	
	if (*ERROR_FLAG == 0)
		{
		STI_info.meanmin = (double*)MNewPointer (i);
		if (STI_info.meanmin == NULL)
			*ERROR_FLAG = 105;
			
		}	// end "if (*ERROR_FLAG == 0)"
		
	if (*ERROR_FLAG == 0)
		{
		STI_info.stdmax = (double*)MNewPointer (i);
		if (STI_info.stdmax == NULL)
			*ERROR_FLAG = 106;
			
		}	// end "if (*ERROR_FLAG == 0)"
		
	if (*ERROR_FLAG == 0)
		{
		STI_info.stdmin = (double*)MNewPointer (i);
		if (STI_info.stdmin == NULL)
			*ERROR_FLAG = 107;
			
		}	// end "if (*ERROR_FLAG == 0)"
		
	if (*ERROR_FLAG == 0)
		{
		STI_info.V_wave_length_value = (double*)MNewPointer (i);
		if (STI_info.V_wave_length_value == NULL)
			*ERROR_FLAG = 108;
			
		}	// end "if (*ERROR_FLAG == 0)"
		
	if (*ERROR_FLAG == 0)
		{
		STI_info.H_wave_length_value = (double*)MNewPointer (i);
		if (STI_info.H_wave_length_value == NULL)
			*ERROR_FLAG = 109;
			
		}	// end "if (*ERROR_FLAG == 0)"
		
	if (*ERROR_FLAG == 0)
		{
				// For now leave the ability for writing wavelengths out. It is too
				// easy to have the number of channels that match the 12 and 210
				// criterea and not actually have AVIRIS or FLC1 data.
		/*				
				// Default wave length for AVIRIS data
				
		if ((class_info+0)->wave_length_default == (WAVE_LENGTH_DEFAULT_FLAG+210))
			{
					// Horizontal
					
	 		STI_info.no_wave_length_floating_point = 1;
	  
			i = 0;
			STI_info.H_wave_length_location[i]=31;
			STI_info.H_wave_length_value[i++]=0.7072;

			STI_info.H_wave_length_location[i]=65;
			STI_info.H_wave_length_value[i++]=1.0068;

			STI_info.H_wave_length_location[i]=110;
			STI_info.H_wave_length_value[i++]=1.4018;

			STI_info.H_wave_length_location[i]=151;
			STI_info.H_wave_length_value[i++]=1.8068;

			STI_info.H_wave_length_location[i]=185;
			STI_info.H_wave_length_value[i++]=2.2087;

			STI_info.no_H_wave_length_location = i;

					//	Vertical
					
			STI_info.no_wave_length_floating_point = 1;

			i = 0;
			STI_info.V_wave_length_location[i]=11;
			STI_info.V_wave_length_value[i++]=0.5091;

			STI_info.V_wave_length_location[i]=31;
			STI_info.V_wave_length_value[i++]=0.7072;

			STI_info.V_wave_length_location[i]=65;
			STI_info.V_wave_length_value[i++]=1.0068;

			STI_info.V_wave_length_location[i]=86;
			STI_info.V_wave_length_value[i++]=1.2092;

			STI_info.V_wave_length_location[i]=110;
			STI_info.V_wave_length_value[i++]=1.4018;

			STI_info.V_wave_length_location[i]=131;
			STI_info.V_wave_length_value[i++]=1.6095;

			STI_info.V_wave_length_location[i]=151;
			STI_info.V_wave_length_value[i++]=1.8068;

			STI_info.V_wave_length_location[i]=165;
			STI_info.V_wave_length_value[i++]=2.0099;

			STI_info.V_wave_length_location[i]=185;
			STI_info.V_wave_length_value[i++]=2.2087;

			STI_info.V_wave_length_location[i]=205;
			STI_info.V_wave_length_value[i++]=2.4059;

			STI_info.no_V_wave_length_location=i;
			
			}	// end "if ((class_info+0)->wave_length_default == ..."
			
				// Default wave length for FLC1 data
				
		if ((class_info+0)->wave_length_default == (WAVE_LENGTH_DEFAULT_FLAG+12))
			{
					//	Horizontal
					
			STI_info.no_wave_length_floating_point=2;

			i = 0;
			STI_info.H_wave_length_location[i]=1;
			STI_info.H_wave_length_value[i++]=0.45;

			STI_info.H_wave_length_location[i]=4;
			STI_info.H_wave_length_value[i++]=0.51;

			STI_info.H_wave_length_location[i]=7;
			STI_info.H_wave_length_value[i++]=0.60;

			STI_info.H_wave_length_location[i]=10;
			STI_info.H_wave_length_value[i++]=0.76;

			STI_info.no_H_wave_length_location=i;

					//	Vertical
					
			STI_info.no_wave_length_floating_point=2;

			i = 0;
			STI_info.V_wave_length_location[i]=1;
			STI_info.V_wave_length_value[i++]=0.45;

			STI_info.V_wave_length_location[i]=3;
			STI_info.V_wave_length_value[i++]=0.49;

			STI_info.V_wave_length_location[i]=5;
			STI_info.V_wave_length_value[i++]=0.525;

			STI_info.V_wave_length_location[i]=7;
			STI_info.V_wave_length_value[i++]=0.60;

			STI_info.V_wave_length_location[i]=9;
			STI_info.V_wave_length_value[i++]=0.69;

			STI_info.V_wave_length_location[i]=11;
			STI_info.V_wave_length_value[i++]=0.90;

			STI_info.no_V_wave_length_location=i;
			
			}	// end "if ((class_info+0)->wave_length_default == ..."
		*/	
		FS_gen_make_stat_image_same_scale (class_info,
														classPtr,
		 												no_class,
														featurePtr,
		 												numberFeatures,
		 												STI_info,
		 												newFileInfoPtr, 
		 												ERROR_FLAG);
	 												
		}	// end "if (*ERROR_FLAG == 0)"
	
			// Free memory
	
	STI_info.H_wave_length_value = CheckAndDisposePtr (STI_info.H_wave_length_value);
	STI_info.V_wave_length_value = CheckAndDisposePtr (STI_info.V_wave_length_value);

	STI_info.H_wave_length_location = 
										CheckAndDisposePtr (STI_info.H_wave_length_location);
	STI_info.V_wave_length_location = 
										CheckAndDisposePtr (STI_info.V_wave_length_location);

	STI_info.stdmin = CheckAndDisposePtr (STI_info.stdmin);
	STI_info.stdmax = CheckAndDisposePtr (STI_info.stdmax);
	
	STI_info.meanmin = CheckAndDisposePtr (STI_info.meanmin);
	STI_info.meanmax = CheckAndDisposePtr (STI_info.meanmax);
	
	STI_info.no_col_grid_indicator_value = 
								CheckAndDisposePtr (STI_info.no_col_grid_indicator_value);
	STI_info.no_col_grid_indicator_location = 
								CheckAndDisposePtr (STI_info.no_col_grid_indicator_location);

}	// end "FS_make_stat_image_same_scale"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void gen_put_number
//
//	Software purpose:	 
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Chulhee Lee				Date: ??/??/19??
//	Revised By:			Larry L. Biehl			Date: 01/15/1998

void gen_put_number (
				unsigned char* 					final_cor,
				SInt32								final_size_col,
				SInt32								row_pos,
				SInt32								col_pos,
				SInt32								number,
				SInt32								sign,
				SInt32								enlargement)
							
{
	SInt32								a,
											b,
											color[2],
											i,
											j;
										
	SInt16* 								no_bit_map;
	
	
	color[0] = iw;
	color[1] = ib;
	
			// If number is minus the put the number in red.
			
	if (number<0)
		number *= -1;
		
	if (sign<0)
		color[1]=ired;
	
	no_bit_map = n0;
	
	switch (number)
		{
		case	0:
			no_bit_map = n0;
			break;
			
		case	1:
			no_bit_map = n1;
			break;
			
		case	2:
			no_bit_map = n2;
			break;
			
		case	3:
			no_bit_map = n3;
			break;
			
		case	4:
			no_bit_map = n4;
			break;
			
		case	5:
			no_bit_map = n5;
			break;
			
		case	6:
			no_bit_map = n6;
			break;
			
		case	7:
			no_bit_map = n7;
			break;
			
		case	8:
			no_bit_map = n8;
			break;
			
		case	9:
			no_bit_map = n9;
			break;
         
      case	48:
			no_bit_map = n0;
			break;   
         
      case	49:
			no_bit_map = n1;
			break;
			
		case	50:
			no_bit_map = n2;
			break;
			
		case	51:
			no_bit_map = n3;
			break;
			
		case	52:
			no_bit_map = n4;
			break;
			
		case	53:
			no_bit_map = n5;
			break;
			
		case	54:
			no_bit_map = n6;
			break;
			
		case	55:
			no_bit_map = n7;
			break;
			
		case	56:
			no_bit_map = n8;
			break;
			
		case	57:
			no_bit_map = n9;
			break;
         
      case 65:
         no_bit_map = nA;
         break;
         
      case 66:
         no_bit_map = nB;
         break;   
      
      case 67:
         no_bit_map = nC;
         break;
      
      case 68:
         no_bit_map = nD;
         break; 
         
      case 69:
         no_bit_map = nE;
         break;
         
      case 70:
         no_bit_map = nF;
         break;   
      
      case 71:
         no_bit_map = nG;
         break;
      
      case 72:
         no_bit_map = nH;
         break; 
         
      case 73:
         no_bit_map = nI;
         break;
         
      case 74:
         no_bit_map = nJ;
         break;   
      
      case 75:
         no_bit_map = nK;
         break;
      
      case 76:
         no_bit_map = nL;
         break; 
         
      case 77:
         no_bit_map = nM;
         break;
         
      case 78:
         no_bit_map = nN;
         break;   
      
      case 79:
         no_bit_map = nO;
         break;
      
      case 80:
         no_bit_map = nP;
         break; 
         
      case 81:
         no_bit_map = nQ;
         break;
         
      case 82:
         no_bit_map = nR;
         break;   
      
      case 83:
         no_bit_map = nS;
         break;
      
      case 84:
         no_bit_map = nT;
         break; 
         
      case 85:
         no_bit_map = nU;
         break;
         
      case 86:
         no_bit_map = nV;
         break;   
      
      case 87:
         no_bit_map = nW;
         break;
      
      case 88:
         no_bit_map = nX;
         break; 
         
      case 89:
         no_bit_map = nY;
         break;
         
      case 90:
         no_bit_map = nZ;
         break; 
         
      case 97:
         no_bit_map = nA;
         break;
         
      case 98:
         no_bit_map = nB;
         break;   
      
      case 99:
         no_bit_map = nC;
         break;
      
      case 100:
         no_bit_map = nD;
         break; 
         
      case 101:
         no_bit_map = nE;
         break;
         
      case 102:
         no_bit_map = nF;
         break;   
      
      case 103:
         no_bit_map = nG;
         break;
      
      case 104:
         no_bit_map = nH;
         break; 
         
      case 105:
         no_bit_map = nI;
         break;
         
      case 106:
         no_bit_map = nJ;
         break;   
      
      case 107:
         no_bit_map = nK;
         break;
      
      case 108:
         no_bit_map = nL;
         break; 
         
      case 109:
         no_bit_map = nM;
         break;
         
      case 110:
         no_bit_map = nN;
         break;   
      
      case 111:
         no_bit_map = nO;
         break;
      
      case 112:
         no_bit_map = nP;
         break; 
         
      case 113:
         no_bit_map = nQ;
         break;
         
      case 114:
         no_bit_map = nR;
         break;   
      
      case 115:
         no_bit_map = nS;
         break;
      
      case 116:
         no_bit_map = nT;
         break; 
         
      case 117:
         no_bit_map = nU;
         break;
         
      case 118:
         no_bit_map = nV;
         break;   
      
      case 119:
         no_bit_map = nW;
         break;
      
      case 120:
         no_bit_map = nX;
         break; 
         
      case 121:
         no_bit_map = nY;
         break;
         
      case 122:
         no_bit_map = nZ;
         break; 
         
      case 32:
         no_bit_map = nSpace;
         break;
         
      case 45:
         no_bit_map = nMinus;
         break;
         
      case 46:
         no_bit_map = nDot;
         break;
         
      case 95:
         no_bit_map = nUnderScore;
         break;
      
		}	// end "switch (number)"
	 
	for (i=0; i<5; i++)
		for (j=0; j<3; j++)
			for (a=0; a<enlargement; a++)
				for (b=0; b<enlargement; b++)
					final_cor[(row_pos+i*enlargement+a)*final_size_col+
															j*enlargement+b+col_pos] =
	     													(unsigned char)color[no_bit_map[i*3+j]];
	     													
}	// end "gen_put_number"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void make_even
//
//	Software purpose:	 
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Chulhee Lee				Date: ??/??/19??
//	Revised By:			Larry L. Biehl			Date: 01/15/1998

void make_even (
				SInt32*								numberPtr)
								
{
	if (L_number_enlargement > 1 && *numberPtr != *numberPtr/2*2)
	  	*numberPtr -=1;
	   
}	// end "make_even"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void put_integer
//
//	Software purpose:	 
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Chulhee Lee				Date: ??/??/19??
//	Revised By:			Larry L. Biehl			Date: 01/15/1998

void put_integer (
				SInt32								start_zero_flag,
				unsigned char* 					final_cor,
				SInt32								final_size_col,
				SInt32								row_pos,
				double								largest_poss_no,
				double								xnumber,
				SInt32								start_col_pos,
				SInt32								max_digit,
				SInt32								enlargement)
				
{
	double 								max_aval_no,
											new_x,
											scale_back,
											y;
	
	SInt32								col_pos,
											flag,
											i,
											j,
											max_digit_to_be_used,
											max_sign,
											number_width;
									
										
	make_even (&row_pos);
	make_even (&start_col_pos);
	number_width = 4;
	
	max_aval_no = 1;
	for (i=0; i<max_digit; i++)
		max_aval_no *= 10;
	 
			// If number is larger than the maximum number, reduce
			
	scale_back=1;
	for (i=0; i<10; i++)
		{
		if (fabs (largest_poss_no)<max_aval_no*scale_back-1)
			break;
		
		scale_back *=10;
		
		}	// end "for (i=0; i<10; i++)"
		
	new_x = xnumber/scale_back;
	largest_poss_no /= scale_back;
	 
			// Find the max. digit of grid number
			
	max_digit_to_be_used = 1;
	if (largest_poss_no>1)
		max_digit_to_be_used = (SInt32)(log10 (fabs (largest_poss_no)) + 1);
		
	if (max_digit_to_be_used>max_digit)
		max_digit_to_be_used=max_digit;
	  
			// Put number
		
	y=1;
	for (i=0; i<max_digit_to_be_used-1; i++)
		y *=10;
	 
	max_sign=1;
	if (new_x<0)
		max_sign= -1;
	  
	for (flag=0,i=0; i<max_digit_to_be_used; i++)
		{
		col_pos = start_col_pos+number_width*i*enlargement;
		j = (SInt32)(new_x/y);
		if (j!=0 || flag==1 || start_zero_flag==1)
			{
			gen_put_number (final_cor,
 	   						final_size_col,
 	   						row_pos,
 	   						col_pos,
 	   						j,
 	   						max_sign,
 	   						enlargement);
			
			flag = 1;
			new_x -= j*y;
			
			}
			
		new_x *=10;
		
		}	// end "for (flag=0,i=0; i<max_digit_to_be_used; i++)"

	max_sign = 1;
	j = (SInt32)(xnumber);
	col_pos = start_col_pos+enlargement*number_width*(max_digit_to_be_used-1);
	
	if (j==0)	// if zero, put zero
		gen_put_number (final_cor,
								final_size_col,
								row_pos,
								col_pos,
								j,
								max_sign,
								enlargement);
	  
}	// end "put_integer"


		
//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void put_real
//
//	Software purpose:	put xnumber at (row_pos,start_col_pos)
//
//	Parameters in:		
//
//	Parameters out:	
//
// Value Returned:	None	
// 
// Called By:	
//
//	Coded By:			Chulhee Lee				Date: ??/??/19??
//	Revised By:			Larry L. Biehl			Date: 01/15/1998

 
void put_real (
				unsigned char* 					final_cor,
				SInt32								final_size_col,
				SInt32								row_pos,
				double 								largest_poss_no,
				double 								xnumber,
				SInt32								start_col_pos,
				SInt32								max_digit,
				SInt32								enlargement,
				SInt32								number_floating_point)				
				
{
	double 								tmp;
	
	SInt32								i,
											int_digit_used,
											k,
											a,
											b,
											start_zero_flag = 0,
											xsign = 1;
											
	
	make_even (&row_pos);
	make_even (&start_col_pos);

	if (xnumber<0)
		xsign= -1;
		
			// Integer ?
			
	tmp = fabs (largest_poss_no);
	i = (SInt32)tmp;
	int_digit_used = 1;
	if (i > 1)
		int_digit_used = (SInt32)(log10 (tmp) + .999);
	 
	if (number_floating_point == 0 || int_digit_used >= max_digit)
		{
		put_integer (start_zero_flag,
							final_cor,
							final_size_col,
							row_pos,
							largest_poss_no,
							xnumber,
							start_col_pos,
							max_digit,
							enlargement);
     
																									return;
		}	// end "if (number_floating_point == 0 || ..."
	
			// check space availablity
			
	if (number_floating_point > max_digit-int_digit_used)
		number_floating_point = max_digit - int_digit_used;
	
	k = 1;
	for (i=0; i<number_floating_point; i++)
		k *= 10;
	   
			// Put integer part
			
	put_integer (start_zero_flag,
						final_cor,
						final_size_col,
						row_pos,
						largest_poss_no,
						xnumber,
						start_col_pos,
						max_digit,
						enlargement);

			// Put floating part
			
	i = (SInt32)xnumber;
	xnumber -= i;
	xnumber *= k;
	start_col_pos += int_digit_used*4*enlargement + 2*enlargement;
	if (enlargement==0)
		start_col_pos++;
	  
	tmp = k - 1;
	start_zero_flag = 1;
	put_integer (start_zero_flag,
						final_cor,
						final_size_col,
						row_pos,
						tmp,
						xnumber,
						start_col_pos,
    					max_digit,
    					enlargement);

			// Put floating point
			
	for (a=0; a<enlargement; a++)
		{
		for (b=0; b<enlargement; b++)
			{
	   	if (xsign > 0)
	   		final_cor[(row_pos+4*enlargement+a)*final_size_col +
										start_col_pos-2*enlargement+b] = (unsigned char)ib;
	   	else
	  			final_cor[(row_pos+4*enlargement+a)*final_size_col +
										start_col_pos-2*enlargement+b] = (unsigned char)ired;
	  									
	  		}	// end "for (b=0; b<enlargement; b++)"
	  		
	  	}	// end "for (a=0; a<enlargement; a++)"
	    
}	// end "put_real"