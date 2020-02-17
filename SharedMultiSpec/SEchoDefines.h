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
//	File:						SEchoDefines.h
//
//	Authors:					Byeungwoo Jeon
//
//	Language:				C
//
//	System:					Linux, Macintosh and Windows Operating Systems
//
//	Brief description:	Various Definitions of Structure variables used in echo 
//								programs.
//
//	Written By:				Byeungwoo Jeon			Date: 01/01/1989
//	Revised By:				Byeungwoo Jeon			Date: 01/01/1990 
//	Revised By:				Byeungwoo Jeon			Date: 08/01/1990 
//	Revised By:				Byeungwoo Jeon			Date: 03/26/1992
//	Revised By:				Abdur Maud				Date: 06/24/2013
//	Revised By:				Larry L. Biehl			Date: 03/12/2018
//	
//------------------------------------------------------------------------------------

#pragma once

		// Structure for Pixel Information

typedef struct 	pixel_information_def 
	{	 
			// Similar to "FileInfo" in Multispec
	
	UInt32						nrow;
	UInt32						ncol;

	unsigned short int		nband;
	
   		// Selection of Image Data (Assigned in readpre())

	UInt32						nrow0, 
									nrow1;	
	UInt32						ncol0, 
									ncol1;
	UInt32						num_row,
									num_col;
	SInt64						ipixels;

	}	pixel_information, *pixel_infoPtr;


		// class_statistics Definition

typedef struct 	stat_def
	{
	UInt32						number_of_training_samples;
	
	double						threshold;
	double						log_prior;		/* log(prior) */
	double						sprd;
	double						classConstant;	
	double						classConstantML;	
	double						Log_determinant;

	HDoublePtr					vmean;
	HDoublePtr					vprd;
	HDoublePtr					covinv;
	
	} statistics;
	

		// Statistics Information

typedef struct 	stat_information_def 
	{		
			// ProjectInfo

	UInt32						ncls;
	UInt16						nfeatures;
	Boolean						already_read_stat;

	} stat_information;


		// Parameter_information (These are read in from pre_file)

typedef	struct	parameter_information_def
	{
			// Double Variables
	
	double						thd1;
	double						thd2;

			// Integer Variables
	
	short int					icelwth;

			// Boolean Variables
						
			// 1 : Use different Prior
   Boolean						fixed_threshold_option;

	} parameter_information;

	
		// Common_classifier_information

typedef	struct	common_classifier_information_def 
	{
			// Other structures
	
	pixel_information			pixel_info;
	stat_information			stat_info;
	parameter_information	parameter_info;

			// following three variables are not freed at 
			// free_common_classifier_info()
	
	statistics* 				class_stat;  // Memory Allocated at getstat=
	
	} common_classifier_information, *common_classifier_informationPtr;

