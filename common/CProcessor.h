//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//							 Copyright (1988-2017)
//						(c) Purdue Research Foundation
//								All rights reserved.
//
//	File:						CProcessor.h
//	Implementation:		CProcessor.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision number:		2.7
//
//	Revision date:			11/29/2017
//
//	Language:				C++
//
//	System:					Windows Operating Systems
//
//	Brief description:	This file is the definition for the CMProcessor class
//
//------------------------------------------------------------------------------------
               
#if !defined __MPROCESS_H__
	#define	__MPROCESS_H__                      

class	CMProcessor : public CCmdTarget
	{
	public:
										CMProcessor();		// constructor
		virtual 						~CMProcessor();	// destructor stub 

		static CMImageDoc*		GetActiveImageDocCPtr (void);
		
		static CMOutlineArea*	GetActiveSelectionAreaCPtr (void);
		
		static CMOutlineArea*	GetSelectionAreaCPtr (
											CMImageView*						imageViewCPtr);
											
		static CMOutlineArea*	GetSelectionAreaCPtr (
											WindowInfoPtr						imageWindowInfoPtr);
	
	protected:
		
	};  
#endif	// !defined __MPROCESS_H__