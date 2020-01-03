//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						xDrawObjects.h
//	Implementation:		xDrawObjects.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMDrawObject and CMOutlineArea classes
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 06/20/2017
//
//------------------------------------------------------------------------------------

#pragma once

class CMImageView;
class CMImageDoc;

// CMDrawObject - base class for all 'drawable objects'

class CMDrawObject : public wxObject
{
	protected:
		CMDrawObject ();

	public:
		CMDrawObject (
				const wxRect& 						position);

		virtual ~CMDrawObject ();

		virtual void Draw (
				CDC* 									pDC,
				CMImageView* 						imageViewCPtr);

		void Invalidate (void);

		virtual void OnOpen (
				CMImageView* 						pView);

		virtual void Remove ();

		CMImageDoc* 						m_document;

		Boolean 								m_drawFlag;

	protected:
		LOGBRUSH 							m_logbrush;

		LOGPEN 								m_logpen;

		wxRect 								m_position;

		static Rect 						s_viewRect;

}; // end "class CMDrawObject"



//------------------------------------------------------------------------------------
//
// CMOutlineArea - class for outlining areas

class CMOutlineArea : public CMDrawObject
{
	public:
		CMOutlineArea ();

		CMOutlineArea (
		 		const wxRect& 						position);

		virtual ~CMOutlineArea ();

		void Clear (
				CMImageView* 						imageViewCPtr);

		virtual void Draw (
				CDC* 									pDC,
				CMImageView* 						imageViewCPtr);

		void Invalidate (
				CMImageView* 						imageViewCPtr);
	
};	// end "class CMOutlineArea"

