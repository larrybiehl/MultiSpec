//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						mathConversionUtilities.c
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/16/1998
//	Revision date:			08/09/2011
//	Revision date:			11/22/2017
//
//	Language:				C
//
//	System:					Macintosh Operating System
//
//	Brief description:	The routines in this file are used to convert between 80 bit
//								SANE values and 96 bit 68881 values and PPC 64 bit doubles and
//								96 bit 68881 values.  The routines are needed when the double
//								for the processor is not the 96 bit extended values used by 
//								the 68881 coprocessor.
//
//	Diagram of MultiSpec procedure calls for the procedures in the file.
//
//	Functions in file:	void					nonx96tox96
//								void					x96tononx96
//								
//	Include files:			"MultiSpecHeaders"
//								"multiSpec.h"
//
//------------------------------------------------------------------------------------

//#include "SMultiSpec.h"
//#include "SExtGlob.h" 

#if TARGET_RT_MAC_MACHO	
	extern "C" void  	dtox80 (const double *x, extended80 *x80);

	extern "C" double  x80tod (const extended80 * x80);
#endif	// TARGET_RT_MAC_MACHO

//extern void  ldtox80 (const long double *x, extended80 *x80);
							
			

		// Prototypes for routines in this file that are only called by		
		// other routines in this file.												

void nonx96tox96 (
				double*								doublePtr, 
				extended96*							extended96Ptr)
				
{
	#ifndef _MC68881_ 
		//#ifdef powerpc	
		#if TARGET_CPU_PPC || TARGET_CPU_X86					
			extended80					saneValue;
				
			#if TARGET_RT_MAC_MACHO	
				double						doubleValue;
				
				
				doubleValue = *doublePtr;
			
				#ifndef __XCODE__ 
				dtox80 (&doubleValue, &saneValue);
				#endif	// !defined __XCODE__
			#else		// !TARGET_RT_MAC_MACHO
						// Note that longDoubleValue is given as an array of 2 because		
						// it does not appear that 16 bytes are really being allocated		
						// for longDoubleValue by itself.	(4/6/1994)	
			
				long double					longDoubleValue[2];
			
			
						// Convert powerpc double to powerpc long double.						
					
				longDoubleValue[1] = 0;
				longDoubleValue[0] = *doublePtr;
			
						// Convert powerpc long double to sane value.	
						
				ldtox80 (&longDoubleValue[0], &saneValue);
			#endif	// TARGET_RT_MAC_MACHO, else...	
			
					// Convert sane value to m68881 extended value.							
							
			extended96Ptr->exp[0] = saneValue.exp;
			extended96Ptr->exp[1] = 0;
			BlockMoveData (&saneValue.man[0], &extended96Ptr->man[0], 8);
		#else		// not TARGET_CPU_PPC 
			x80tox96 ((extended80*)doublePtr, extended96Ptr);
		#endif	// TARGET_CPU_PPC || TARGET_CPU_X86
	#endif	// !_MC68881_ 
	
}	// end "nonx96tox96" 


										
void x96tononx96 (
				extended96* 		extended96Ptr, 
				double* 				doublePtr )
				
{		
	#ifndef _MC68881_ 
		#if defined TARGET_CPU_PPC || defined TARGET_CPU_X86
	
			extended80					saneValue;
			
			
					// Convert m68881 extended value to sane value.								
										
			BlockMoveData (&extended96Ptr->man[0], &saneValue.man[0], 8);
			saneValue.exp = extended96Ptr->exp[0];
				
			#if TARGET_RT_MAC_MACHO	
				//#ifndef __XCODE__ 
					*doublePtr = x80tod ((const extended80 *)&saneValue);
				//#endif	// !defined __XCODE__
			#else	// !TARGET_RT_MAC_MACHO
				long double					longDoubleValue[2];
				
						// Convert sane value to powerpc long double value.						
						
				x80told (&saneValue, &longDoubleValue[0]);
				
						// Convert powerpc long double to powerpc double.							
						
				*doublePtr = longDoubleValue[0];
			#endif	// TARGET_RT_MAC_MACHO, else...	
		#else	// not TARGET_CPU_PPC && not TARGET_CPU_X86
			x96tox80 (extended96Ptr, (extended80*)doublePtr);
		#endif	// TARGET_CPU_PPC || TARGET_CPU_X86
	#endif	// !_MC68881_ 
	
}	// end "x96tononx96" 

/*
#if TARGET_RT_MAC_MACHO
	#if __cplusplus && TYPE_LONGDOUBLE_IS_DOUBLE
		#define x80told       __inline_x80told
		#define ldtox80        __inline_ldtox80
	#endif


	//  x80told ()
	//  
	//  Availability:
	//    Mac OS X:         in version 10.3 and later in CoreServices.framework
	//    CarbonLib:        in CarbonLib 1.0 and later
	//    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
	//
	extern void  x80told (const extended80 *x80, long double *x);
	#if TYPE_LONGDOUBLE_IS_DOUBLE
	  #ifdef __cplusplus
		 inline void x80told (const extended80 *x80, long double *x) {
																	*(x) = (long double)x80tod (x80);}
	  #else
		 #define x80told (x80, x) (*(x) = (long double)x80tod (x80))
	  #endif
	#endif



	//  ldtox80 ()
	//  
	//  Availability:
	//    Mac OS X:         in version 10.3 and later in CoreServices.framework
	//    CarbonLib:        in CarbonLib 1.0 and later
	//    Non-Carbon CFM:   in MathLib 1.0 and later or as macro/inline
	//
	extern void  ldtox80 (const long double *x, extended80 *x80);
	#if TYPE_LONGDOUBLE_IS_DOUBLE
	  #ifdef __cplusplus
		 inline void ldtox80 (const long double *x, extended80 *x80) {
													double d = (double) *(x); dtox80 (&d, (x80));}
	  #else
		 #define ldtox80 (x, x80) do {
								double d = (double)*(x); dtox80 (&d, (x80)); } while (false)
	  #endif
	#endif
#endif	// TARGET_RT_MAC_MACHO
*/
