//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//								(c) Purdue Research Foundation
//								   All rights reserved.
//
//	File:						SClassifyEchoMemory.cpp
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
//	Brief description:	This file contains functions that do echo classification
//								processes.
//
//	Functions in file:	SInt16				free_class_stat
//								void 					free_common_classifier_info
//								void					free_pixel_info
//								void					initialize_common_info
//								void					initialize_parameter_info
//								void					initialize_pixel_info
//								void					initialize_stat_info
//								void					initialize_summary_info
//								SInt16				malloc_common_classifier_info
//								SInt16				malloc_class_stat
//
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"

#if defined multispec_lin  
	#include	"SMultiSpec.h"
#endif

#include	"SEcho.h"

//#include "SExtGlob.h"



//=======================================================================
// FUNCTION : free_class_stat
// Purpose  : Free Memory allocated to "Class Statistics" and set it NULL
// Globals  : none
// Returns  : error_code
// Called by: EchoClsfierControl ()	in echo_classify.c (Multispec)
// 		xlik_to_siginicance_level ()	in aux_mtp.c
// Coded by : Byeungwoo Jeon	at	01/01/91
//=========================================================================

SInt16 free_class_stat (
				stat_information*					stat_info,
				statistics**						class_stat)
				
{
   SInt16								error_code;
   HDoublePtr							temp_ptr;
   

   error_code = 1;
   
   if (stat_info != NULL)
   	{
   	stat_info->already_read_stat = FALSE;
   
	   if (*class_stat != NULL)
	   	{
	   	temp_ptr = (*class_stat)[0].vmean;
			temp_ptr = CheckAndDisposePtr (temp_ptr);
			*class_stat = (statistics*)CheckAndDisposePtr ((Ptr)*class_stat);
			
	   	}	// "if (*class_stat != NULL)"	
   
   	error_code = 0;
   
   }	// end "if (stat_info != NULL)"
   
   return (error_code);
   
}	// end "free_class_stat"



//==============================================================
// FUNCTION : MALLOC_CLASS_STAT
// Purpose  : Memory allocation of stat variable.
// 	This function checks first whether stat_info.class_stat 
// 	is already assigned memory or not. New memory allocation 
// 	is performed only when it is not memory assigned.
// 	This is done by cheking the value of "class_stat".
// 	If it is NULL, then it is not memory assigned.
// Prerequsite : 
// 	c_info_ptr.class_stat
// 	stat_info.ncls, stat_info.stat_info, stat_info.already_read_stat
// 	pixel_info.nband
// 	
// Globals  : No global variables used in this function.
// Returns  : error_code
// 	0 : 	no error
// 	1 : if error occurred
// 	
// Called by	getstat ()			in getstat.c
// 		compute_sta_after_extrain ()	in compute_sta.c
//  		calstat ()			in calstat.c
// 
// Coded By   : Byeungwoo Jeon		Date : 01/01/89
// Revised by : Byeungwoo Jeon		Date : 03/20/96
//==============================================================

Boolean malloc_class_stat (
				statistics**						class_stat,
				UInt32								ncls,
				SInt16								nband,
				Boolean*								already_read_stat)

{  							
	HDoublePtr							temp_ptr;
   
   Boolean								continueFlag = TRUE;
   
   UInt32								bytes,
											classIndex,
											itmp,
											nband2;
   							
   statistics*							class_stat_ptr;
   

			// CHK Here																			
			
   nband2 = (UInt32)nband * nband;
   
   if (*class_stat == (statistics*)NULL)
   	{
				// Allocate memory only if it is not done yet.						
  	
  		*already_read_stat = FALSE;	
	
		bytes = (UInt32)ncls * sizeof (statistics);
		*class_stat = (statistics*)MNewPointer (bytes);
	   continueFlag = (*class_stat != NULL);
	
  				// Initialize the variables in class_stat structure 				

		class_stat_ptr = *class_stat;
		if (continueFlag)
			{
	 		for (classIndex=0; classIndex<ncls; classIndex++)
	 			{
			   class_stat_ptr->number_of_training_samples = 0;
			   class_stat_ptr->threshold 		= -DBL_MAX;
			   class_stat_ptr->log_prior 		= 1;
			   class_stat_ptr->vmean 		= NULL;
			   class_stat_ptr->vprd 		= NULL;
			   class_stat_ptr->covinv 	 	= NULL;
			   class_stat_ptr->sprd 	 	= 0;
			   class_stat_ptr->classConstant 	= 0;
			   class_stat_ptr->classConstantML 	= 0;
			   class_stat_ptr->Log_determinant 	= 0;
			   
			   class_stat_ptr++;
			   
		   	}	//	end "for (classIndex=0; classIndex<ncls; classIndex++)"
		   	
		   }	// end "if (continueFlag)"

		class_stat_ptr = *class_stat;
		if (continueFlag) 
			{
			//itmp = 2 * nband2 + nband * 2;
	   
	   	itmp = nband2 + nband * 2;
			
	   	itmp *= ncls;
	   
	  	 	bytes = (UInt32)itmp * sizeof (double);
	   	temp_ptr = (HDoublePtr)MNewPointer (bytes);
	   	continueFlag = (temp_ptr != NULL);
		
			}	//	"if (continueFlag)"
	
		if (continueFlag) 
			{
			class_stat_ptr->vmean = temp_ptr;
		
      	for (bytes=0; bytes<itmp; bytes++)
				*temp_ptr++ = 0.;

	   			// Assign starting pointer to Each Component 					
	
			temp_ptr = class_stat_ptr->vmean;
	   	for (classIndex=0; classIndex<ncls; classIndex++)
	   		{
 	      	class_stat_ptr->vmean = temp_ptr;
	      	temp_ptr += nband;
	   
	      	class_stat_ptr->vprd  = temp_ptr;
	      	temp_ptr += nband;
	   
	      	class_stat_ptr->covinv= temp_ptr; 
	      	temp_ptr += nband2;	
	 
	   		class_stat_ptr++;
	   		
				}	// end "for classIndex..." 
				
      	}	// end "if (continueFlag)"

   	}	// "if (class_stat_ptr == (statistics*)NULL)"
         
   return (continueFlag);
   
}	// end "malloc_class_stat"


/*
//====================================================================
// FUNCTION : free_common_classifier_info (c_info_ptr)
// Purpose  : Release memory assigned to Gloval Arrays
// Returns  : none
// Globals  : none
// Called by: EchoClsfierControl ()	in echo_classifiy.c (Multispec)
// Coded by : Byeungwoo Jeon	Date : 10/01/1991
//=====================================================================

void free_common_classifier_info (
				common_classifier_information* 		c_info_ptr)

{
			// If "c_info_ptr" is NULL, there is no need to 
    		// free anything in it, return												

   if (c_info_ptr == (common_classifier_information *)NULL)
   																					return;

	//free_pixel_info (&(c_info_ptr->pixel_info));
	//free_stat_info (&(c_info_ptr->stat_info));

}	// end "free_common_classifier_info"
*/

/*
//===================================================================
// FUNCTION : free_pixel_info
// Purpose  : Memory allocation to pixel information variables
// Usage    : Before calling this routine, "nband" and "ncls_original"
// 		must be known.
// Returns  : none
// Globals  : none
// Called by:  free_common_classifier_info ()	in	memory.c
// Coded by : Byeungwoo Jeon	Date : 10/01/1991
//=====================================================================

void free_pixel_info (
				pixel_information*				p_info)
				
{ 
   if (p_info == (pixel_information*)NULL) 
   																					return;
   
			// Make sure the structure variables inside 
   		// "p_info->class_info" are freeed before "p_info->class_info"
   		// itself is freeed by calling free_class_info ().

	//free_class_info (p_info->class_info);
   
	//p_info->class_info = (class_information*)CheckAndDisposePtr (
	//   														(Ptr)p_info->class_info);
   
}	// end "free_pixel_info"
*/


// ================================================================
// FUNCTION : initialize_common_info
// Purpose  : Initialize the variables in common_classifier_info
// Last Revised by : Byeungwoo Jeon         Date : 09/28/1991
// ==================================================================

void initialize_common_info (
				common_classifier_information 	*c_info_ptr)
				
{
   
   initialize_pixel_info (&(c_info_ptr->pixel_info));
   initialize_stat_info (&(c_info_ptr->stat_info));
   initialize_parameter_info (&(c_info_ptr->parameter_info));
     
   c_info_ptr->class_stat 	= (statistics*)NULL;
	//c_info_ptr->pixel 		= (pix*)NULL;
	//c_info_ptr->loglik_storage 	= (double*)NULL;

}	// end "initialize_common_info"



// ================================================================
// FUNCTION : initialize_pixel_info
//  Purpose  : Initialize the variables in pixel_info
// Last Revised by : Byeungwoo Jeon         Date : 09/28/1991
// ==================================================================

void initialize_pixel_info (
				pixel_information*				pixel_info)

{  
   pixel_info->nrow 	 	= 0;
   pixel_info->ncol 	 	= 0;
   
   pixel_info->nband 	= 0;
	//pixel_info->class_info = (class_information*)NULL;
   
   pixel_info->nrow0 	 = 0;
   pixel_info->nrow1 	 = 0;
   pixel_info->ncol0	 	 = 0;
   pixel_info->ncol1 	 = 0;
   pixel_info->num_row 	 = 0;
   pixel_info->num_col 	 = 0;
   pixel_info->ipixels   = 0;

}	// end "initialize_pixel_info" 



// ================================================================
// FUNCTION : initialize_stat_info
// Purpose  : Initialize the variables in stat_info
// Last Revised by : Byeungwoo Jeon         Date : 09/28/1991
// ==================================================================

void initialize_stat_info (
				stat_information*					stat_info)

{ 
   stat_info->ncls 	  = 0;
  
   stat_info->already_read_stat = FALSE;

}	// end "initialize_stat_info"



// ================================================================
// FUNCTION : initialize_parameter_info
// Purpose  : Initialize the variables in parameter_info
// Last Revised by : Byeungwoo Jeon         Date : 09/28/1991
// ==================================================================

void initialize_parameter_info (
				parameter_information*			parameter_info)
				
{        
  parameter_info->fixed_threshold_option = TRUE;
  
  parameter_info->icelwth 	= 0;
  
  parameter_info->thd1 		= 0;
  parameter_info->thd2 		= 0;
    
}	// end "initialize_parameter_info"
