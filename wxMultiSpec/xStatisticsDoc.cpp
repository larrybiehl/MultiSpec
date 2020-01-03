//                               MultiSpec
//
//               Laboratory for Applications of Remote Sensing
//                         Purdue University
//                        West Lafayette, IN 47907
//                         Copyright (2009-2020)
//                     (c) Purdue Research Foundation
//                           All rights reserved.
//
//   File:                 xStatisticsDoc.cpp : class implementation file
//   Class Definition:     xStatisticsDoc.h
//
//   Authors:              Abdur Rahman Maud, Larry L. Biehl
//
//   Revision date:        06/20/2017
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
	
	SetTitle ("Select Field");
		
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
