// WAbout.cpp : Defines the class behaviors for the application.
//
//	Revised by Larry Biehl on 03/26/2017
                  
#include "SMulSpec.h"

#include "WAbout.h"
#include <direct.h> 
	                                                               
//#if !defined _WIN32
//	#include <dos.h>
//#endif	// if !defined _WIN32

#include "SExtGlob.h"   
#include "wabout.h"

extern UInt16							gDisplayBitsPerPixel;


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif 


BEGIN_MESSAGE_MAP(CAboutDlg, CMDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SimplerVersion, &CAboutDlg::OnBnClickedSimplerversion)
END_MESSAGE_MAP() 



CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_simplerVersionFlag = !gFullVersionFlag;
	//}}AFX_DATA_INIT 
	
	m_fromStartUpFlag = FALSE;
	m_FullVersionSettingFlag = FALSE;
	
	m_initializedFlag = CMDialog::m_initializedFlag;
	
}		// end "CAboutDlg"

  

BOOL CAboutDlg::Create(
				CWnd*				pParent)

{ 
	m_fromStartUpFlag = TRUE;

	if ( !CDialog::Create(CAboutDlg::IDD, pParent) )
																			return FALSE;
																			
	return TRUE;
	
}		// end "Create"
 
 

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Check(pDX, IDC_SimplerVersion, m_simplerVersionFlag);
	//}}AFX_DATA_MAP
} 


//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
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

Boolean CAboutDlg::DoDialog(void)

{        
	SInt16			returnCode; 


			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																			return(FALSE);
																					
	returnCode = (SInt16)DoModal();
	if (returnCode == IDOK)
		{
		if (m_FullVersionSettingFlag)
			gFullVersionFlag = !m_simplerVersionFlag;

		}		// end "if (returnCode == IDOK)"

	return (returnCode);
	
}		// end "DoDialog"
 


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg commands

BOOL CAboutDlg::OnInitDialog()
{    
	CString				str, 
							strFmt;
	
	  
	CDialog::OnInitDialog();
	
	if (m_fromStartUpFlag)
		MHideDialogItem (NULL, IDOK);  

#	if defined _WIN64
		LoadDItemString (IDC_BitVersion, "\0(64-bit version)");
#	else
		LoadDItemString (IDC_BitVersion, "\0(32-bit version)");
#	endif 
	
	if (gProjectInfoPtr == NULL && (GetKeyState(VK_CONTROL) & 0x8000))
		MShowDialogItem (NULL, IDC_SimplerVersion);
	else	// gProjectInfoPtr != NULL || !(GetKeyState(VK_CONTROL) & 0x8000)
		MHideDialogItem (NULL, IDC_SimplerVersion);

			// fill available memory
	                                             		               
	strFmt.LoadString(IDS_AboutMemory);

#	if defined _WIN64
		wsprintf(str.GetBuffer(80), strFmt, GetFreeSpace(0) / 1024L);
#	else
		MEMORYSTATUS lpBuffer; 
		lpBuffer.dwLength=sizeof(MEMORYSTATUS);
		GlobalMemoryStatus(&lpBuffer); 	// pointer to the memory status structure  
		wsprintf(str.GetBuffer(80), strFmt, lpBuffer.dwAvailPageFile/1024L);
#	endif
	
	str.ReleaseBuffer();
	SetDlgItemText(IDC_AboutMemory, str);    

			// fill largest contiguous block of memory parameter
			               
	strFmt.LoadString(IDS_AboutContiguousMemory);
#	if defined _WIN64
		wsprintf(str.GetBuffer(80), strFmt, ::GlobalCompact(0) / 1024L);
#	else
		wsprintf(str.GetBuffer(80), strFmt, lpBuffer.dwAvailPhys/1024L);
#	endif
	str.ReleaseBuffer();
	SetDlgItemText (IDC_AboutContiguousMemory, str);

			// fill math coprocessor information
/*			                                     
	#if !defined WIN32           
		if (GetWinFlags() & WF_80x87)
			str.LoadString(IDS_AboutCoprocessor);
		else
			str.LoadString(IDS_AboutNoCoprocessor);
		SetDlgItemText(IDC_AboutCoprocessor, str);
	#endif	// !defined WIN32
*/
			// fill disk free information
			
	struct _diskfree_t diskfree;                                      
//	#if !defined WIN32
//		if (_dos_getdiskfree(_getdrive(), &diskfree) == 0)
//	#else
		if (_getdiskfree(_getdrive(), &diskfree) == 0)
//	#endif	// !defined WIN32
		{
		strFmt.LoadString(IDS_AboutDiskSpace);

		SInt64 diskBytesAvailable = (SInt64)diskfree.avail_clusters * 
					diskfree.sectors_per_cluster *
							diskfree.bytes_per_sector / 1024L;

		#if defined _UNICODE
			wsprintf(str.GetBuffer(80), strFmt, diskBytesAvailable);
		#endif
		#if !defined _UNICODE
			sprintf (str.GetBuffer(80), strFmt, diskBytesAvailable);
		#endif

//		wsprintf(str.GetBuffer(80), strFmt,
//			(DWORD)diskfree.avail_clusters *
//			(DWORD)diskfree.sectors_per_cluster *
//			(DWORD)diskfree.bytes_per_sector / (DWORD)1024L);
		str.ReleaseBuffer();
		
		}
		
	else
		str.LoadString(IDS_AboutNoDiskSpace);
		
	SetDlgItemText(IDC_AboutDisk, str);              
	
			// Set color information about the display 
/*			
	CDC				pDC;                                             
	
	if ( pDC.CreateIC("DISPLAY", NULL, NULL, NULL) )
		{                        
		
		SInt16 bitsPerPixel = pDC.GetDeviceCaps(BITSPIXEL);
		SetDlgItemInt(IDC_AboutBitsColor, bitsPerPixel);
		gDisplayBitsPerPixel = bitsPerPixel;     
		
		SInt16 colorPlanes = pDC.GetDeviceCaps(PLANES);
		SetDlgItemInt(IDC_AboutColorPlanes, colorPlanes);     
		
		SInt16 rasterCaps = pDC.GetDeviceCaps(RASTERCAPS);
		gStretchDIBitsFlag = rasterCaps & RC_STRETCHDIB;
		
		}		// end "if ( pDC->CreateIC("Display", NULL, NULL, NULL) )"
*/	
	PositionDialogWindow (); 
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog" 


void CAboutDlg::OnBnClickedSimplerversion()
{
			// TODO: Add your control notification handler code here
   
	m_FullVersionSettingFlag = TRUE;
	DDX_Check (m_dialogFromPtr, IDC_SimplerVersion, m_simplerVersionFlag); 

}
