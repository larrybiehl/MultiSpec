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
//   File:                 xStatisticsDoc.cpp : class implementation file
//   Class Definition:     xStatisticsDoc.h
//
//   Authors:              Abdur Rahman Maud, Larry L. Biehl
//
//   Revision date:        04/21/2020
//
//   Language:					C++
//
//   System:               Linux and MacOS Operating Systems
//
//   Brief description:  	This file contains functions that relate to the
//                       	CMStatisticsDoc class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
                     
#include "xStatisticsDoc.h"
#include "xStatisticsFrame.h"


IMPLEMENT_DYNAMIC_CLASS (CMStatisticsDoc, wxDocument)

BEGIN_EVENT_TABLE (CMStatisticsDoc, wxDocument)
END_EVENT_TABLE ()



CMStatisticsDoc::CMStatisticsDoc (void)

{                           
	gProjectWindow = NULL;
	m_statisticsFrameCPtr = NULL;

}	// end "CMStatisticsDoc"



CMStatisticsDoc::~CMStatisticsDoc (void)

{

}	// end "~CMStatisticsDoc"



bool CMStatisticsDoc::Close (void)

{	
			// The user was allowed a chance to save the project before the text
			// window was closed. Allow closing of project window to continue.
	
	if (CloseTheProject ())
		return true;
		
	else
		return false;
	
}	// end "Close"



bool CMStatisticsDoc::OnNewDocument (void)

{
	if (!wxDocument::OnNewDocument ())
																							return FALSE;
	
	SetTitle (wxT("Select Field"));
		
	return TRUE;
	
}	// end "OnNewDocument"



CMStatisticsFrame* CMStatisticsDoc::GetStatisticsFrame (void)

{
	return (m_statisticsFrameCPtr);

}	// end "GetStatisticsFrame"



void CMStatisticsDoc::SetStatisticsFrame(
				CMStatisticsFrame*				statisticsFrameCPtr)

{
	m_statisticsFrameCPtr = statisticsFrameCPtr;

}	// end "SetStatisticsFrame"
