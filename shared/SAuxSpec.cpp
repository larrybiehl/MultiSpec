//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						SAuxSpec.cpp
//
//	Authors:					Byeungwoo Jeon
//								Larry L. Biehl
//
//	Revision number:		2.8
//
//	Revision date:			05/26/2017
//
//	Language:				C
//
//	System:					Macintosh and Windows Operating Systems
//
//	Brief description:	
//
//	Functions in file:	int 						read_lines_of_data1
//								void						symtrix
//								void						threshold
//								double 					translate_threshold

#include	"SMulSpec.h"
  
#if defined multispec_win | defined multispec_lin
	//#include	"SMulSpec.h"  
	#include "SExtGlob.h"
#endif	// defined multispec_win    

#include		"SEcho.h"

double 		translate_threshold(
				double								det,
				double								std_thd);



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		SInt16 read_lines_of_data1
//
//	Software purpose:	Read data from image_file
//  						 Prerequisite : Set up followings
//								pixel_info->num_row;
//								pixel_info->num_col;
//								pixel_info->nrow0;
//								pixel_info->ncol0;
//								pixel_info->nrow1;
//								pixel_info->ncol1;
//
//	Parameters in:				
//
//	Parameters out:	Data Read in from disk file
//
// Value Returned:	error_code
// 
// Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 04/10/92
//	Revised By:			Larry L. Biehl			Date: 10/26/99

SInt16 read_lines_of_data1(
				FILE*									fp,			// Starting point	
				SInt32								lines)
							
{
	FileIOInstructionsPtr			fileIOInstructionsPtr;
	
	SInt16								returnCode;

	
	fileIOInstructionsPtr = (FileIOInstructionsPtr)fp;

	fileIOInstructionsPtr->lineStart = fileIOInstructionsPtr->nextLineToRead;
	fileIOInstructionsPtr->lineEnd = fileIOInstructionsPtr->lineStart +
								(lines - 1) * fileIOInstructionsPtr->lineInterval;
	
	returnCode = GetLinesOfData (fileIOInstructionsPtr);

	return (returnCode);

}		// End "read_lines_of_data1"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void symtrix
//
//	Software purpose:	Compute Q = (transpose X) SYM (X)
//
//	 							Matrix multiplication  q = Transpose(X) SYM X
//	  							X    : n by 1
//	 							SYM  : n by n (Symmetric)
//	  							Q    : Scalar
//
//	Parameters in:				
//
//	Parameters out:	
//
// Value Returned:	None
// 
// Called By:			classify_cell_using_ML in phase1.c
//							loglik_pix in mxlik.c
//
//	Coded By:			Byeungwoo Jeon			Date: 01/01/88
//	Revised By:			Larry L. Biehl			Date: 05/07/96

void symtrix(
				HDoublePtr 							x,
				HDoublePtr 							sym_ptr,
				double* 								q,
				SInt32								nb)
				
{
   double								ans,
   										tmp;
   
   HDoublePtr							x_ptr;
   
  	SInt32								kx,
   										ky,
   										matrixOffset;
    

   ans 		= 0.;
   matrixOffset = nb;
   for (kx=0; kx<nb; kx++) 
   	{
		tmp = 0.;
   	x_ptr 	= x;
   	for(ky=0; ky<kx; ky++) 
			{
	   	tmp +=  *sym_ptr * *x_ptr;
	   	sym_ptr++;
	   	x_ptr++;
	   	
			}		// end "for(ky=0; ky<kx; ky++)"
		
				// Following code represents.
				// ans += (2.0 * xi * tmp + xi * sij * xi);
	
		tmp += tmp;	
		ans += *x_ptr * (tmp + *sym_ptr * *x_ptr);

		sym_ptr += matrixOffset;
		matrixOffset--;

   	}		// end "for (kx=0; kx<nb; kx++)"

   *q = ans;

}	// End "symtrix"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void threshold
//
//	Software purpose: Compute:
//
//								THD = -0.5*COEFF*LOG(|Cov|) - 0.5*(INV_CHI(prob_level))
//
//								Where	DF is a degree of freedom
//  							Others	: inv_chi_square
//
//	Parameters in:				
//
//	Parameters out:	
//
// Value Returned:	None
// 
// Called By:			
//
//	Coded By:			Byeungwoo Jeon			Date: ??/??/??
//	Revised By:			Byeungwoo Jeon			Date: 
//	Revised By:			Larry L. Biehl			Date: 04/03/98

void threshold(
				SInt32								ncls, 					// (I) Number of classes
				SInt32								degree_of_freedom, 	// (I) Number of features
				double*								thresholds,		// (I) Log Determinant of Covariance
		     																//  of each class, (O) Threshold Value
		     																//  of each Class 
				double				   			significance_level,
				double				   			coeff)

{
   double								chiSquaredValue,
   										Log_det;
   
   double*								d_ptr;
   
  	SInt32								classIndex;
   
   
	//thd_standard = inv_chi_square(degree_of_freedom, significance_level);
   
   chiSquaredValue = GetChiSquaredValue (degree_of_freedom, significance_level);
   
	d_ptr = thresholds;
   if (chiSquaredValue != DBL_MAX)
   	{
	  	chiSquaredValue /= 2.;
	   
	   for(classIndex=0; classIndex<ncls; classIndex++) 
	   	{
	   	Log_det = *d_ptr;  // Log Determinant of Covariance Matrix
			*d_ptr++ = translate_threshold(Log_det, chiSquaredValue);
		
	   	}	// "for (classIndex=0;...)"
	   	
	   }		// end "if (chiSquaredValue != DBL_MAX)"
	   
	else		// chiSquaredValue == DBL_MAX
		{
				// No thresholding to occur.
				
	   for(classIndex=0; classIndex<ncls; classIndex++) 
			*d_ptr++ = -chiSquaredValue;
	   	
	   }		// end "else chiSquaredValue == DBL_MAX"

}		// end "threshold"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2017)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		double translate_threshold
//
//	Software purpose: Compute:
//
//								THD = -0.5*COEFF*|Cov| - 0.5*(INV_CHI(prob_level))
//   								Don't forget to declare this function as 
//   								returning double in the calling routine.
//
//	Parameters in:				
//
//	Parameters out:	
//
// Value Returned:	None
// 
// Called By:			make_threshold_table in call_imsl.c
//
//	Coded By:			Byeungwoo Jeon			Date: ??/??/??
//	Revised By:			Byeungwoo Jeon			Date: 11/01/91

double translate_threshold(
				double								det,
				double   							std_thd)

{
   double							 	answer;
   
   
	//answer = -(coeff * det + std_thd)/2;

   answer = det - std_thd;
   return (answer);
   
}		// end "translate_threshold"

