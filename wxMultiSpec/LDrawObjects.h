//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LDrawObjects.h
//	Implementation:		LDrawObjects.cpp
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
//
#ifndef __LDRAWOBJ_H__
#define __LDRAWOBJ_H__

class CMImageView;
class CMImageDoc;

// CMDrawObject - base class for all 'drawable objects'

class CMDrawObject : public wxObject
	{
	protected:
		 CMDrawObject();

	public:
		 CMDrawObject(
					const wxRect& position);

		 virtual ~CMDrawObject();

		 virtual void Draw(
					CDC* pDC,
					CMImageView* imageViewCPtr);

		 void Invalidate(void);

		 virtual void OnOpen(
					CMImageView* pView);

		 virtual void Remove();

		 CMImageDoc* m_document;

		 Boolean m_drawFlag;

	protected:
		 wxRect m_position;

		 LOGPEN m_logpen;

		 LOGBRUSH m_logbrush;

		 static Rect s_viewRect;

	}; // end "class CMDrawObject"


//------------------------------------------------------------------------------------
//
// CMOutlineArea - class for outlining areas

class CMOutlineArea : public CMDrawObject
	{
	public:
		 CMOutlineArea ();

		 CMOutlineArea (const wxRect& position);

		 virtual ~CMOutlineArea ();

		 void Clear (CMImageView* imageViewCPtr);

		 virtual void Draw (CDC* pDC, CMImageView* imageViewCPtr);

		 void Invalidate (CMImageView* imageViewCPtr);
	protected:
	
	}; // end "class CMOutlineArea"

#endif // __LDRAWOBJ_H__
