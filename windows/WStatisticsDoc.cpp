//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2020)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WStatisticsDoc.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/04/2018
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMStatisticsDoc class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
                     
#include "WStatisticsDoc.h"
#include "WStatisticsFrame.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNCREATE (CMStatisticsDoc, CDocument)

BEGIN_MESSAGE_MAP (CMStatisticsDoc, CDocument)
	//{{AFX_MSG_MAP (CMStatisticsDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMStatisticsDoc::CMStatisticsDoc ()

{                           
	gProjectWindow = NULL;
	m_statisticsFrameCPtr = NULL;

}	// end "CMStatisticsDoc"



CMStatisticsDoc::~CMStatisticsDoc (void)

{

}	// end "~CMStatisticsDoc"   



CMStatisticsFrame* CMStatisticsDoc::GetStatisticsFrame (void)

{
	return (m_statisticsFrameCPtr);

}	// end "GetStatisticsFrame"



BOOL CMStatisticsDoc::OnNewDocument (void)

{
	if (!CDocument::OnNewDocument ())
																							return FALSE;
	
	SetTitle (_T("Select Field"));
		
	return TRUE;
	
}	// end "OnNewDocument"



void CMStatisticsDoc::SetStatisticsFrame (
				CMStatisticsFrame*				statisticsFrameCPtr)

{
	m_statisticsFrameCPtr = statisticsFrameCPtr;

}	// end "SetStatisticsFrame"
