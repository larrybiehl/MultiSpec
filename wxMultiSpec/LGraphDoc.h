//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2019)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LGraphDoc.h
//	Implementation:		LGraphDoc.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMGraphDoc class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 08/23/2019
//
//------------------------------------------------------------------------------------
//
#ifndef __LGRAFDOC_H__
	#define __LGRAFDOC_H__

#include "LGraphFrame.h"
#include "LGraphView.h"

#include "wx/docview.h"
#include "wx/cmdproc.h"


typedef bool BOOL;


class CMGraphDoc : public wxDocument
	{
	DECLARE_DYNAMIC_CLASS (CMGraphDoc)
    
	public:
		 CMGraphDoc ();   // protected constructor used by dynamic creation
		 ~CMGraphDoc ();
	
		 BOOL OnNewDocument ();
	
		 CMGraphFrame*   m_graphFrameCPtr;
		 CMGraphView*    m_graphViewCPtr;
	
	public:
		 CMGraphFrame* GetGraphFrameCPtr () { return m_graphFrameCPtr; }
		 CMGraphView* GetGraphViewCPtr (void);
	
		 void SetGraphFrameCPtr (CMGraphFrame* graphFrameCPtr);
		 void SetGraphViewCPtr (CMGraphView* graphViewCPtr);
	
	};
#endif
