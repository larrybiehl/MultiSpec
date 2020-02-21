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
