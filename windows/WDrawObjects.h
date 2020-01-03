//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WDrawObjects.h
//	Implementation:		WDrawObjects.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Header file for the CMDrawObject and CMOutlineArea classes.
//
//	Written By:				Larry L. Biehl			Date: ??/??/1995?
//	Revised By:				Larry L. Biehl			Date: 12/05/2019
//
//------------------------------------------------------------------------------------

#pragma once

class CMImageView;
class CMImageDoc;


//----------------------------------------------------------------------------
// CMDrawObject - base class for all 'drawable objects'

class CMDrawObject : public CObject
{
	public:
		CMDrawObject (
				const CRect& 						position);
	                          
		virtual ~CMDrawObject ();
				
		virtual void Draw (
				CDC* 									pDC,
				CMImageView*						imageViewCPtr);
		
		void Invalidate (void);
											
		virtual void OnOpen (
				CMImageView* 						pView);
		                                        		
		virtual void Remove ();
		
		#ifdef _DEBUG
			void AssertValid ();
		#endif       
	
	
		CMImageDoc* 						m_document;
		
		Boolean								m_drawFlag;
		
	protected:
		CMDrawObject ();
	
	
		static Rect							s_viewRect;
	
		CRect									m_position;
	
		LOGBRUSH 							m_logbrush;
		                       
		LOGPEN 								m_logpen;

};	// end class CMDrawObject


//----------------------------------------------------------------------------
// CMOutlineArea - class for outlining areas

class CMOutlineArea : public CMDrawObject
{
	public:                                 
		CMOutlineArea ();
										
		CMOutlineArea (
				const CRect& 						position);
											
		virtual ~CMOutlineArea ();
	
		void Clear (
				CMImageView*						imageViewCPtr);
				
		virtual void Draw (
				CDC* 									pDC,
				CMImageView*						imageViewCPtr);
	
		void Invalidate (
				CMImageView*						imageViewCPtr);

		#ifdef _DEBUG
			void AssertValid ();
		#endif
	
};	// end class CMOutlineArea
