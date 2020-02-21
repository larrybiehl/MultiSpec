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
//	File:						WGraphFrame.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			03/27/2018
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	This file contains functions that relate to the
//								CMGraphFrame class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include <afxext.h>

#include "WGraphDoc.h" 
#include "WGraphFrame.h"
#include "WGraphView.h"
#include "WMultiSpec.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



IMPLEMENT_DYNCREATE (CMGraphFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP (CMGraphFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP (CMGraphFrame)
	ON_COMMAND (ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND (ID_FILE_PRINT_PREVIEW, OnFilePrintPreview)

	ON_UPDATE_COMMAND_UI (ID_FILE_CLOSE, OnUpdateFileGraphClose)
	ON_UPDATE_COMMAND_UI (ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI (ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)

	ON_WM_CLOSE ()

	ON_WM_SYSCOMMAND ()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMGraphFrame::CMGraphFrame ()

{
	m_graphViewCPtr = NULL;
	
}	// end "CMGraphFrame"



CMGraphFrame::~CMGraphFrame ()

{

}	// end "~CMGraphFrame"



void CMGraphFrame::OnClose ()

{  
	CMDIChildWnd::OnClose ();
	
}	// end "OnClose"



BOOL CMGraphFrame::OnCreateClient (
				LPCREATESTRUCT 					lpcs,
				CCreateContext* 					pContext)
				
{  
	Boolean returnFlag = FALSE;
	
	                                                
	if (CMDIChildWnd::OnCreateClient (lpcs, pContext))
		{
		((CMGraphDoc*)pContext->m_pCurrentDoc)->SetGraphFrameCPtr (this);
		returnFlag = TRUE;
		
		}	// end "if (CMDIChildWnd::OnCreateClient (lpcs, pContext))"
		
	return (returnFlag);
	
}	// end "OnCreateClient"



void CMGraphFrame::OnFilePrint ()

{
	((CMultiSpecApp*)AfxGetApp())->SetPrintOrientation (GetActiveWindowInfoHandle ());

	m_graphViewCPtr->DoFilePrint ();
	
}	// end "OnFilePrint"



void CMGraphFrame::OnFilePrintPreview ()

{
	((CMultiSpecApp*)AfxGetApp())->SetPrintOrientation (GetActiveWindowInfoHandle ());

	m_graphViewCPtr->DoFilePrintPreview ();
	
}	// end "OnFilePrintPreview"



afx_msg void CMGraphFrame::OnSysCommand (
				UINT 									nID,
				LPARAM								lParam)
                      
{                     
	if (gProcessorCode != kListDataProcessor || (nID != SC_CLOSE && nID != SC_MAXIMIZE))
		CFrameWnd::OnSysCommand (nID, lParam);

	else   // gProcessorCode == kListDataProcessor && ...
		{
		if (nID == SC_CLOSE)
			m_graphViewCPtr->m_closeGraphSelectedFlag = TRUE;

		}	// end "else gProcessorCode == kListDataProcessor && ..."

}	// end "OnSysCommand"



void CMGraphFrame::OnUpdateFileGraphClose (
				CCmdUI* 								pCmdUI)

{
	UpdateFileGraphClose (pCmdUI);

	pCmdUI->Enable (TRUE);
	
}	// end "OnUpdateFileGraphClose"

 

void CMGraphFrame::OnUpdateFilePrint (
				CCmdUI* 								pCmdUI)

{                                                        
	Boolean enableFlag = UpdateFileGraphPrint (pCmdUI);
	                                                                                     
	pCmdUI->Enable (enableFlag);
	
}	// end "OnUpdateFilePrint"



void CMGraphFrame::OnUpdateFilePrintPreview (
				CCmdUI* 								pCmdUI)

{
	pCmdUI->Enable (TRUE);
	
}	// end "OnUpdateFilePrintPreview"



BOOL CMGraphFrame::PreCreateWindow (
				CREATESTRUCT& 						cs)

{
	SInt16								xSize = 280,
											ySize = 190;
	
	
	if (cs.cx != 0)
		{
		GetGraphWindowLocation (&xSize, &ySize);
		
				// This sets the size of the window
		
		cs.cx = xSize;
		cs.cy = ySize;
		
				// This sets the location of the window
		
		cs.x = gNextGraphicsWindowStart.h;
		cs.y = gNextGraphicsWindowStart.v;
		
		}	// end "if (cs.cx != 0)"
	
	return (CMDIChildWnd::PreCreateWindow (cs));

}	// end "PreCreateWindow"



void CMGraphFrame::SetGraphViewCPtr (
				CMGraphView*						graphViewCPtr)

{
	m_graphViewCPtr = graphViewCPtr;
	
}	// end "SetGraphViewCPtr"
