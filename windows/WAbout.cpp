//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (1995-2018)
//								(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						WAbout.cpp : implementation file
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			01/03/2018
//
//	Language:				C
//
//	System:					Windows Operating Systems
//
//	Brief description:	This file contains functions that relate to the CAboutDlg
//								class.
//
//	Functions in file:	
//
//	Include files:			"SMultiSpec.h"
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "WAbout.h"
#include <direct.h> 
	                                                               
//#if !defined _WIN32
//	#include <dos.h>
//#endif	// if !defined _WIN32

//#include "SExtGlob.h"   
#include "wabout.h"

extern UInt16							gDisplayBitsPerPixel;


#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif 


BEGIN_MESSAGE_MAP (CAboutDlg, CMDialog)
	//{{AFX_MSG_MAP (CAboutDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED (IDC_SimplerVersion, &CAboutDlg::OnBnClickedSimplerversion)
END_MESSAGE_MAP ()



CAboutDlg::CAboutDlg (CWnd* pParent /*=NULL*/)
	: CMDialog (CAboutDlg::IDD, pParent)

{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_simplerVersionFlag = !gFullVersionFlag;
	//}}AFX_DATA_INIT 
	
	m_fromStartUpFlag = FALSE;
	m_FullVersionSettingFlag = FALSE;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}	// end "CAboutDlg"

  

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		BOOL Create
//
//	Software purpose:	The purpose of this routine is to create the dialog box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/23/1995
//	Revised By:			Larry L. Biehl			Date: 10/18/1995	

BOOL CAboutDlg::Create (
				CWnd*				pParent)

{ 
	m_fromStartUpFlag = TRUE;

	if ( !CDialog::Create (CAboutDlg::IDD, pParent))
																					return FALSE;
																			
	return TRUE;
	
}	// end "Create"
 
 

//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDataExchange
//
//	Software purpose:	The purpose of this routine is to handle exchanging
//							data in dialog items.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/23/1995
//	Revised By:			Larry L. Biehl			Date: 10/18/1995	

void CAboutDlg::DoDataExchange (
				CDataExchange*						pDX)

{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CAboutDlg)
	DDX_Check (pDX, IDC_SimplerVersion, m_simplerVersionFlag);
	//}}AFX_DATA_MAP
	
}	// end "DoDataExchange"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the file
//							specification dialog box to the user and copy the
//							revised back to the file information structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/23/1995
//	Revised By:			Larry L. Biehl			Date: 10/18/1995	

Boolean CAboutDlg::DoDialog (void)

{        
	SInt16			returnCode; 


			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																				return (FALSE);
																					
	returnCode = (SInt16)DoModal ();
	if (returnCode == IDOK)
		{
		if (m_FullVersionSettingFlag)
			gFullVersionFlag = !m_simplerVersionFlag;

		}	// end "if (returnCode == IDOK)"

	return (returnCode);
	
}	// end "DoDialog"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void OnBnClickedSimplerversion
//
//	Software purpose:	The purpose of this routine is to handle the check box
//							requesting that a simpler version of the menus be
//							displayed for the user.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/23/1995
//	Revised By:			Larry L. Biehl			Date: 10/18/1995	

void CAboutDlg::OnBnClickedSimplerversion ()

{
	m_FullVersionSettingFlag = TRUE;
	DDX_Check (m_dialogFromPtr, IDC_SimplerVersion, m_simplerVersionFlag); 

}	// end "OnBnClickedSimplerversion"
 


//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		BOOL OnInitDialog
//
//	Software purpose:	The purpose of this routine is to initialize the dialog
//							box.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 05/23/1995
//	Revised By:			Larry L. Biehl			Date: 10/18/1995	

BOOL CAboutDlg::OnInitDialog ()

{    
	CString								str,
											strFmt;

	char									string[256];

	MEMORYSTATUSEX						memoryStatusEx;

	int									numberChars;

	USES_CONVERSION;
	
	  
	CDialog::OnInitDialog ();
	
	if (m_fromStartUpFlag)
		MHideDialogItem (NULL, IDOK);  

	#if defined _WIN64
		LoadDItemString (IDC_BitVersion, "(64-bit version)");
	#else
		LoadDItemString (IDC_BitVersion, "(32-bit version)");
	#endif
	
	if (gProjectInfoPtr == NULL && (GetKeyState (VK_CONTROL) & 0x8000))
		MShowDialogItem (NULL, IDC_SimplerVersion);
	else	// gProjectInfoPtr != NULL || !(GetKeyState (VK_CONTROL) & 0x8000)
		MHideDialogItem (NULL, IDC_SimplerVersion);

	memoryStatusEx.dwLength = sizeof (memoryStatusEx);
	GlobalMemoryStatusEx (&memoryStatusEx);

			// fill available memory
	                                             		               
	strFmt.LoadString (IDS_AboutMemory);
	#if defined _WIN64
		//wsprintf (str.GetBuffer (80), strFmt, GetFreeSpace (0) / 1024L);
		numberChars = sprintf (string, "%lld", memoryStatusEx.ullAvailPageFile / 1024L);
		InsertCommasInNumberString (string, numberChars, -1, numberChars);
		wsprintf (str.GetBuffer (80), strFmt, A2T(string));
	#else
		MEMORYSTATUS lpBuffer; 
		lpBuffer.dwLength=sizeof (MEMORYSTATUS);
		GlobalMemoryStatus (&lpBuffer); 	// pointer to the memory status structure  
		wsprintf (str.GetBuffer (80), strFmt, lpBuffer.dwAvailPageFile/1024L);
	#endif
	str.ReleaseBuffer ();
	SetDlgItemText (IDC_AboutMemory, str);

			// fill largest contiguous block of memory parameter
			               
	strFmt.LoadString (IDS_AboutContiguousMemory);
	#if defined _WIN64
		//wsprintf (str.GetBuffer (80), strFmt, memoryStatusEx.ullAvailPhys / 1024L);
		numberChars = sprintf (string, "%lld", memoryStatusEx.ullAvailPhys / 1024L);
		InsertCommasInNumberString (string, numberChars, -1, numberChars);
		wsprintf (str.GetBuffer (80), strFmt, A2T(string));
	#else
		wsprintf (str.GetBuffer (80), strFmt, lpBuffer.dwAvailPhys/1024L);
	#endif
	str.ReleaseBuffer ();
	SetDlgItemText (IDC_AboutContiguousMemory, str);

			// fill math coprocessor information
	/*
	#if !defined WIN32           
		if (GetWinFlags () & WF_80x87)
			str.LoadString (IDS_AboutCoprocessor);
		else
			str.LoadString (IDS_AboutNoCoprocessor);
		SetDlgItemText (IDC_AboutCoprocessor, str);
	#endif	// !defined WIN32
	*/
			// fill disk free information
			
	struct _diskfree_t diskfree;                                      
	if (_getdiskfree (_getdrive (), &diskfree) == 0)
		{
		strFmt.LoadString (IDS_AboutDiskSpace);

		SInt64 diskBytesAvailable = (SInt64)diskfree.avail_clusters * 
					diskfree.sectors_per_cluster *
							diskfree.bytes_per_sector / 1024L;

		numberChars = sprintf (string, "%lld", diskBytesAvailable);
		InsertCommasInNumberString (string, numberChars, -1, numberChars);
		wsprintf (str.GetBuffer (80), strFmt, A2T(string));
		/*
		wsprintf (str.GetBuffer (80), 
					strFmt,
					(DWORD)diskfree.avail_clusters *
							(DWORD)diskfree.sectors_per_cluster *
								(DWORD)diskfree.bytes_per_sector / (DWORD)1024L);
		*/
		str.ReleaseBuffer ();
		
		}
		
	else
		str.LoadString (IDS_AboutNoDiskSpace);
		
	SetDlgItemText (IDC_AboutDisk, str);              
	
			// Set color information about the display 
	/*
	CDC				pDC;                                             
	
	if ( pDC.CreateIC ("DISPLAY", NULL, NULL, NULL))
		{                        
		
		SInt16 bitsPerPixel = pDC.GetDeviceCaps (BITSPIXEL);
		SetDlgItemInt (IDC_AboutBitsColor, bitsPerPixel);
		gDisplayBitsPerPixel = bitsPerPixel;     
		
		SInt16 colorPlanes = pDC.GetDeviceCaps (PLANES);
		SetDlgItemInt (IDC_AboutColorPlanes, colorPlanes);     
		
		SInt16 rasterCaps = pDC.GetDeviceCaps (RASTERCAPS);
		gStretchDIBitsFlag = rasterCaps & RC_STRETCHDIB;
		
		}	// end "if ( pDC->CreateIC ("Display", NULL, NULL, NULL) )"
	*/
	PositionDialogWindow (); 
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"
