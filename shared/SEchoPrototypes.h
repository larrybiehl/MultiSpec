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
//	File:						SEchoPrototypes.h
//
//	Authors:					Larry L. Biehl
//
//	Language:				C
//
//	System:					Linux, Macintosh & Windows Operating Systems	
//
//	Brief description:	Header file for MultiSpec which contain the function 
//								prototype for the ECHO processor.
//	
//	Revised By:				Larry L. Biehl			Date: 12/20/2017 
//	
//------------------------------------------------------------------------------------

#pragma once

#if defined multispec_wx
	#include <stdio.h>
	#include "SDefines.h"
#endif         

		// In SClassifyEchoUtilities.cpp
		
void free_epix (
				EchoClassifierVar 				*echo_info);
		
double log_lik_ratio (
				HFldLikPtr							field_likPtr,
				HFldLikPtr							cell_likPtr,
				HFldLikPtr							auxlikPtr,
				UInt16								fieldClass,
				UInt16								cellClass,
				UInt16*								auxClassPtr,
				UInt32								numberClasses);
								
Boolean malloc_epix (
				EchoClassifierVar* 				echo_info);
								
SInt16 setup_basic_echo_information (
				common_classifier_information *c_info,
				EchoClassifierVar 				*echo_info);
		
void setup_echo_image_selection (
				common_classifier_information *c_info,
				EchoClassifierVar 				*echo_info);
					
void stuffing (
				HSInt32Ptr							epix_ptr, 
				SInt32								value, 
				UInt16								cell_length, 
				UInt16								cell_width, 
				SInt32								line_epix);
			
UInt16 subtract_log_lik (
				HFldLikPtr 							fldlik, 
				HFldLikPtr 							cell_lik, 
				UInt32	 							ncls);


SInt16 read_lines_of_data1 (
				FILE 									*fp,
				SInt32								lines);
							
void symtrix (
				HDoublePtr 							x,
				HDoublePtr 							sym,
				double 								*q,
				SInt32								n);
							
void threshold (
				SInt32								ncls,
				SInt32								degree_of_freedom,
				double 								*thres,
				double 								alpha,
				double 								coeff);
						
double translate_threshold (
				double 								det,
				double 								std_thd,
				double 								coeff);
								
		// in SClassifyEcho.cpp

SInt16 SaveProbabilityInformation (
				SInt32								numberLines, 
				double*								thresholdTablePtr, 
				statistics*							class_stat, 
				HUCharPtr							probabilityBufferPtr, 
				HFldLikPtr							cellLikePtr, 
				HSInt32Ptr							epix_ptr, 
				HSInt16Ptr							fieldClassNumberPtr, 
				HUInt32Ptr							field_number_table,
				AreaDescriptionPtr				areaDescriptionPtr);
			
		// in SClassifyEchoMemory.cpp
									
SInt16 free_class_stat (
				stat_information					*stat_info,
				statistics							**class_stat);
									
void initialize_common_info (
				common_classifier_information *c_info_ptr);
									
void initialize_parameter_info (
				parameter_information			*parameter_info);
									
void initialize_pixel_info (
				pixel_information					*pixel_info);
									
void initialize_stat_info (
				stat_information					*stat_info);
									
Boolean malloc_class_stat (
				statistics**						class_stat,
				UInt32								ncls, 
				SInt16								nband,
				Boolean*								already_read_stat);
		
		// in SClassifyEchoPhase.cpp
		
void loglik_echo (
				statistics							*class_stat,
				SInt16								nband,
				UInt32								ncls,
				HDoublePtr							c_ptr0,
				HFldLikPtr							fldlik_ptr,
				SInt32								line_x,
				HDoublePtr							ex2,
				double								*ex,
				double								*xmax,
				SInt16*								iclass,
				SInt16								cell_width);

SInt16 phase1 (
				FileIOInstructionsPtr			fileIOInstructionsPtr,
				statistics 							*class_stat,
				EchoClassifierVar 				*echo_info,
				double								*thresholdTablePtr, 
				HUCharPtr							probabilityBufferPtr,
				AreaDescriptionPtr 				areaDescriptionPtr);
