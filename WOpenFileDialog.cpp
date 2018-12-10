// WOpenFileDialog.cpp : implementation file
//      
// Revised by Larry Biehl on 12/21/2017
//
 
#include "SMultiSpec.h"

#include "CImageWindow.h"

#include "WImageView.h"
#include "WDialog.h"
#include "WOpenFileDialog.h"

//#include	"SExtGlob.h"

#include "dlgs.h"
#include "afxcmn.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMFileDialog2 dialog


CMOpenFileDialog::CMOpenFileDialog(CWnd* pParent, TBYTE* filterString /*=NULL*/)
	: CFileDialog (TRUE,									// bOpenFileDialog
						NULL,									// lpszDefExt
						NULL,									// lpszFileName
//new
//						#ifdef _WIN32
							OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,	// dwFlags
//						#else
//							OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLETEMPLATE, 
//						#endif
//end new
//						"All File (*.*) | *.* |Multispectral (*.lan;*.img;*.) | *.lan;*.img;*. |Thematic (*.gis;*.) | *.gis *. |Project (*.prj) | *.prj |Trailer (*.trl) | *.trl ||", 
						(LPCTSTR)filterString,			// lpszFilter
						pParent,								// pParentWnd
						0,										// dwSize
						FALSE)								// bVistaStyle
{
	//{{AFX_DATA_INIT(CMOpemFileDialog)
	m_imageType = 0;
	m_linkOption = 0;
		
	//}}AFX_DATA_INIT   
	
	m_dialogFromPtr = NULL;
	m_dialogToPtr = NULL;
	m_filenamesBuffer = NULL;
	m_initializedFlag = FALSE;
	m_initialLinkSelectedFilesFlag = FALSE;
	m_showLinkPopupMenuFlag = TRUE;	
	m_selectedFileCount = 0;
	m_linkOptionSelectionDataCode = 1;
	m_userSetImageType = 0;

//	m_bVistaStyle = FALSE;	// Added on 3/24/2010 to get around Vista/Windows7 problem
			
	TRY
		{                          
		m_dialogFromPtr = new CDataExchange(this, TRUE);
	
		m_dialogToPtr = new CDataExchange(this, FALSE); 
		
		m_initializedFlag = TRUE;
		
		}
		
	CATCH_ALL(e)
		{                         
		MemoryMessage(0, kObjectMessage);
		
		}
		
	END_CATCH_ALL   
	
	if (m_initializedFlag)
		{ 
//		#ifdef _WIN32
//			lstructsize = OPENFILENAME_SIZE_VERSION_400;
			m_lpDialogTemplate = (DLGTEMPLATE*)MAKEINTRESOURCE(1537);
//		#else
//			m_lpDialogTemplate = MAKEINTRESOURCE(1536);
//		#endif                                           
//		HRSRC hrsrc = ::FindResource (AfxGetInstanceHandle(), m_lpDialogTemplate, RT_DIALOG);
//		m_ofn.hInstance = ::LoadResource (AfxGetInstanceHandle(), hrsrc); 

//new
//		#ifdef _WIN32
			m_ofn.Flags = OFN_ENABLETEMPLATE | OFN_EXPLORER | OFN_ENABLEHOOK;
//			m_ofn.Flags = OFN_ENABLETEMPLATE | OFN_EXPLORER;
			if (gFileFilterIndex == gImageFileFilterIndex)
				{
				m_ofn.Flags |= OFN_ALLOWMULTISELECT;
//				m_ofn.Flags |= OFN_ENABLEHOOK;
//				m_ofn.Flags &= ~OFN_EXPLORER;

				}
			m_ofn.nFilterIndex = gFileFilterIndex;
//		#else       
//			m_ofn.hInstance = AfxGetInstanceHandle();
//		#endif  
		                                  
		m_ofn.lpTemplateName = (LPTSTR)m_lpDialogTemplate;

//		#ifdef _WIN32
			m_nIDHelp = 1537;
//		#else
//			m_nIDHelp = 1536;
//		#endif
//end new
		m_initializedFlag = (m_ofn.hInstance != NULL);
		
		}		// end "if (m_initializedFlag)"
	
	if (m_initializedFlag)
		{
		if (gFileFilterIndex == gImageFileFilterIndex)
			{
					// Create buffer for multiple file names.
					
			const DWORD numberOfFileNames = 500;
			const DWORD fileNameMaxLength = MAX_PATH + 1;
			const DWORD bufferSize = (numberOfFileNames + 1) * fileNameMaxLength + 1;
			m_filenamesBuffer = new TBYTE[bufferSize];
			
					// Initialize beginning and end of buffer.
					
			m_filenamesBuffer[0] = 0;
			m_filenamesBuffer[bufferSize-2] = 0;
			m_filenamesBuffer[bufferSize-1] = 0;
 
					// Attach buffer to OPENFILENAME member.
 
			#if defined _UNICODE
				m_ofn.lpstrFile = (LPWSTR)m_filenamesBuffer;
			#endif
			#if !defined _UNICODE
				m_ofn.lpstrFile = (LPSTR)m_filenamesBuffer;
			#endif
			m_ofn.nMaxFile = bufferSize;

//			m_ofn.lpfnHook = NULL; // HookProcedure;
			
			}		// end "if (gFileFilterIndex == gImageFileFilterIndex)"
			
		else		// gFileFilterIndex != gImageFileFilterIndex
			{
			m_ofn.lpstrFile = m_fileName.GetBuffer(_MAX_PATH);
			
			}		// end "else gFileFilterIndex != gImageFileFilterIndex"
		
		m_initializedFlag = (m_ofn.lpstrFile != NULL);
		
		}		// end "if (m_initializedFlag)" 
	
}		// end "CMOpenFileDialog"



CMOpenFileDialog::~CMOpenFileDialog(void)
{
	if (m_dialogFromPtr != NULL)
		delete m_dialogFromPtr;
		
	if (m_dialogToPtr != NULL)
		delete m_dialogToPtr;   
	
	if (m_filenamesBuffer != NULL)		
		delete[] m_filenamesBuffer;	

}		// end "~CMOpenFileDialog"



void CMOpenFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMOpenFileDialog)
	DDX_CBIndex(pDX, 13, m_imageType);
	DDX_CBIndex(pDX, 14, m_linkOption);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMOpenFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CMOpenFileDialog)                           
	ON_CBN_CLOSEUP(13, OnSelendokImageType)                   
	ON_CBN_CLOSEUP(14, OnSelendokLinkOption)
//	afxMsgLBSELCHANGE
	//}}AFX_MSG_MAP
//	ON_CBN_DROPDOWN(14, &CMOpenFileDialog::OnCbnDropdown14)
END_MESSAGE_MAP() 



//-----------------------------------------------------------------------------
//								 Copyright (1988-1998)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the open
//							file dialog box to the user and allow the user to select
//							the image file and specify how it is to be treated.
// 7/18/2006  Note that there is a problem with open file dialog when selected
// a file on the desktop after a file has already been opened.  Have not 
// determined what is causing it.  There was another message on the web dated
// 1/2006 from someone having a problem with this with his application.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			
//
//	Coded By:			Larry L. Biehl			Date: 11/20/1995
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean 
CMOpenFileDialog::DoDialog(
				SInt16								stringIndex)

{                                  
	Boolean				continueFlag = FALSE;
							
	INT_PTR				returnCode;
	
	
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																			return(FALSE);

	m_stringIndex = stringIndex;
	
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{                 
		if (m_imageType == 0)
			gGetFileImageType = 0;
			
		else if (m_imageType == 1) 
			gGetFileImageType = kMultispectralImageType;
			
		else if (m_imageType == 2) 
			gGetFileImageType = kThematicImageType;
			
		else		// (m_imageType > 2) 
			gGetFileImageType = 0;
			
		if (m_linkOptionSelectionDataCode == 1)
			gMultipleImageFileCode = 0;
			
		else if (m_linkOptionSelectionDataCode == 2)       
			gMultipleImageFileCode = 2;
			
		else if (m_linkOptionSelectionDataCode == 3)       
			gMultipleImageFileCode = 3;
		
//		#ifdef _WIN32
			if (stringIndex == IDS_SelectImage)
				gImageFileFilterIndex = (SInt16)m_ofn.nFilterIndex;
//		#endif  
			
		continueFlag = TRUE;
		
		}		// end "if (returnCode == IDOK)"
	
	return (continueFlag); 
	
}		// end "DoDialog" 


/////////////////////////////////////////////////////////////////////////////
// CMOpenFileDialog message handlers 

void 
CMOpenFileDialog::OnSelendokImageType(void)

{   
	DDX_CBIndex(m_dialogFromPtr, 13, m_imageType);
	m_userSetImageType = m_imageType;

}		// end "OnSelendokImageType"

  

void CMOpenFileDialog::OnSelendokLinkOption(void)

{ 
	DDX_CBIndex(m_dialogFromPtr, 14, m_linkOption);
	m_linkOptionSelectionDataCode = 
						(UInt32)((CComboBox*)GetDlgItem(14))->GetItemData(m_linkOption);
	
	if (m_linkOptionSelectionDataCode == 1)
		SetImageLinkToFalse();

	else if (m_linkOptionSelectionDataCode == 2)
		SetImageLinkToTrue();

	else if (m_linkOptionSelectionDataCode == 3)
		SetImageLinkToTrue();
	
}		// end "OnSelendokLinkOption"  

                                    

BOOL 
CMOpenFileDialog::OnInitDialog(void)

{
//	OSVERSIONINFOEX		osvi;
//	Boolean					vistaFlag = FALSE;

//	sprintf ((char*)&gTextString, "Start%s", gEndOfLine);
//	ListString ((char*)&gTextString, strlen((char*)&gTextString), gOutputTextH);
//	ZeroMemory (&osvi, sizeof(OSVERSIONINFOEX));
//	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
//	if (GetVersionEx ((OSVERSIONINFO*)&osvi))
//		{
//		if (osvi.dwMajorVersion == 6)
//			{
//			vistaFlag = TRUE;
//
//			}

//		}		// end "if (GetVersionEX ((OSVERSIONINFO*)&osvi))"

//	if (!vistaFlag)

	CString				string;


	CFileDialog::OnInitDialog(); 

	CWnd*	pWnd = GetParent();
		
 	if (GetItem11String (m_stringIndex, &string))
//	if (MGetString (gTextString, 0, m_stringIndex))
		{                                                   
//		CString string = CString((char*)&gTextString[1]);
		DDX_Text (m_dialogToPtr, 11, string);
	
		}		// end "if (MGetString (..."
	
	if (gGetFileStatus == -1)
		{                            
		GetDlgItem(12)->ShowWindow(SW_HIDE);
		GetDlgItem(13)->ShowWindow(SW_HIDE);
		GetDlgItem(14)->ShowWindow(SW_HIDE);
		GetDlgItem(15)->ShowWindow(SW_HIDE); 
		GetDlgItem(16)->ShowWindow(SW_HIDE);
		
		}		// end "if (gGetFileStatus == -1)"
	
	if (gGetFileStatus == 0)
		{                                   
		Handle windowInfoHandle = GetActiveImageWindowInfoHandle();
		WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
																windowInfoHandle, kNoLock, kNoMoveHi);
			
		if (windowInfoPtr == NULL ||
				windowInfoPtr->imageType != kMultispectralImageType ||
					windowInfoPtr->projectBaseImageFlag ||
						windowInfoPtr->bandInterleave == kBIS)
			m_showLinkPopupMenuFlag = FALSE;

		if (!m_showLinkPopupMenuFlag)
			GetDlgItem(14)->ShowWindow(SW_HIDE);
			
		GetDlgItem(15)->ShowWindow(SW_HIDE);
		GetDlgItem(16)->ShowWindow(SW_HIDE);

		((CComboBox*)GetDlgItem(13))->AddString((LPCTSTR)_T("default"));
		((CComboBox*)GetDlgItem(13))->AddString((LPCTSTR)_T("Multispectral type"));
		((CComboBox*)GetDlgItem(13))->AddString((LPCTSTR)_T("Thematic type"));
		
		((CComboBox*)GetDlgItem(14))->AddString((LPCTSTR)_T("Do not link"));
		((CComboBox*)GetDlgItem(14))->SetItemData(0, 1);
		if (m_showLinkPopupMenuFlag)
			{
			((CComboBox*)GetDlgItem(14))->AddString((LPCTSTR)_T("Link selected file(s) to active image window"));
			((CComboBox*)GetDlgItem(14))->SetItemData(1, 2);

			}		// end "if (m_showLinkPopupMenuFlag)"
		
		}		// end "if (gGetFileStatus == 0)"
	
	if (gMultipleImageFileCode == 2)
		{
		m_linkOption = 1;
		m_linkOptionSelectionDataCode = 2;

		SetImageLinkToTrue();                
		                                        
		if (gGetFileStatus == 2)	
			GetDlgItem(14)->EnableWindow(FALSE); 
		
		}		// end "if (gMultipleImageFileCode == 2)"
		
	if (gGetFileStatus == 1)
		gGetFileStatus = 3; 
		
	if (gGetFileStatus <= 0)
		gGetFileStatus = 1; 
	
	UpdateData(FALSE); 
	
   ::PositionDialogWindow ((CDialog*)this); 
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog"                       

                                    

void 
CMOpenFileDialog::SetImageLinkToTrue(void)

{  
	if (m_linkOptionSelectionDataCode == 2)
		{
		gMultipleImageFileCode = 2;
		
		if (MGetString (gTextString, 0, IDS_SelectFileToLink))
			{                              
			Handle windowInfoHandle = GetActiveImageWindowInfoHandle();
			WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
																	windowInfoHandle, kNoLock, kNoMoveHi);
			
			sprintf ((char*)&gTextString[13],
						"%3d",
						windowInfoPtr->numberImageFiles+1);
			gTextString[16] = ' ';
						                   
			CString string = CString((char*)&gTextString[1]);
			DDX_Text(m_dialogToPtr, 11, string);
		
			//SetWindowText ((char*)&gTextString[1]);                    
			
			}		// end "if (MGetString (..."

		if (gGetFileStatus == 2)
			{
						// This is a re-entrent to the file dialog to allow the use to select more files
						// to be added to the active image window.

			GetDlgItem(14)->ShowWindow(SW_HIDE);
			GetDlgItem(16)->ShowWindow(SW_SHOW);

			}		// end "if (gGetFileStatus == 2)"

		}		// end "if (m_linkOption == 1)"

	else		// m_linkOptionSelectionDataCode == 3
		gMultipleImageFileCode = 3;
	
	GetDlgItem(13)->ShowWindow(SW_HIDE);
	GetDlgItem(15)->ShowWindow(SW_SHOW);

	m_initialLinkSelectedFilesFlag = TRUE;
	
//	m_linkToActiveImageFlag = TRUE;
	
	m_imageType = 1;
		
}		// end "SetImageLinkToTrue"                       

                                    

void 
CMOpenFileDialog::SetImageLinkToFalse(void)

{
	CString				string;


	gMultipleImageFileCode = 0;
	
	if (gGetFileImageType == 0)
		m_imageType = 0;
		
	else if (gGetFileImageType == kMultispectralImageType)
		m_imageType = 1;  
		
	else		// gGetFileImageType == kThematicImageType)
		m_imageType = 2;
		
	if (GetItem11String (IDS_SelectImage, &string))
		DDX_Text (m_dialogToPtr, 11, string);
	
	GetDlgItem(13)->ShowWindow(SW_SHOW);
	m_imageType = m_userSetImageType;
	DDX_CBIndex(m_dialogToPtr, 13, m_imageType);
	
	if (gMultipleImageFileCode > 0)
		{
		GetDlgItem(14)->ShowWindow(SW_SHOW);
		DDX_CBIndex(m_dialogToPtr, 14, m_linkOption);

		}		// end "if (gMultipleImageFileCode > 0)"
			
	GetDlgItem(15)->ShowWindow(SW_HIDE);
	GetDlgItem(16)->ShowWindow(SW_HIDE);
		
}		// end "SetImageLinkToFalse"                      

                                    
/*
BOOL 
CMOpenFileDialog::PreTranslateMessage(
				MSG*							pMsg)

{  
	int			message;


	message = pMsg->message;

	return CDialog::PreTranslateMessage (pMsg);
		
}		// end "PreTranslateMessage" 
*/

/*
void CMOpenFileDialog::OnCbnDropdown14()
{
	// TODO: Add your control notification handler code here
}
*/
BOOL 
CMOpenFileDialog::GetItem11String(
				UInt16							stringID,
				CString*							stringPtr)
{
	if (MGetString (gTextString, 0, stringID))
		{
		if (stringID == IDS_SelectFileToLink)
			{
			Handle windowInfoHandle = GetActiveImageWindowInfoHandle();
			WindowInfoPtr windowInfoPtr = (WindowInfoPtr)GetHandlePointer(
																	windowInfoHandle, kNoLock, kNoMoveHi);
			
			if (windowInfoPtr != NULL)
				{
				sprintf ((char*)&gTextString[13],
							"%3d",
							windowInfoPtr->numberImageFiles+1);
				gTextString[16] = ' ';

				}		// end "if (windowInfoPtr != NULL)"

			else		// windowInfoPtr != NULL
				gTextString[1] = 0;

			}		// end "if (stringID == IDS_SelectFileToLink)"
						                   
		*stringPtr = CString((char*)&gTextString[1]);

		return (TRUE);
		
		}		// end "if (MGetString (..."

	return (FALSE);

}		// end "GetItem11String"



void	CMOpenFileDialog::OnFileNameChange()
{
	CComboBox*			comboBoxPtr;

	CWnd					*pWnd = NULL,
							*pWnd2 = NULL;

	SInt16				menuItemCount;


	if (gFileFilterIndex == gImageFileFilterIndex)
		{																			
		pWnd = GetParent();

		if (pWnd != NULL)
			{
			pWnd2 = pWnd->GetDlgItem (lst2);

			if (pWnd2 != NULL)
				{
				CListCtrl* listControlPtr = (CListCtrl*)pWnd2->GetDlgItem(1);
				if (listControlPtr != NULL)
					m_selectedFileCount = listControlPtr->GetSelectedCount();

				}		// end "if (pWnd2 != NULL)"

			}		// end "if (pWnd != NULL)"

		comboBoxPtr = (CComboBox*)GetDlgItem(14);
		menuItemCount = comboBoxPtr->GetCount();

		if (m_selectedFileCount <= 1)
			{
			if ((m_showLinkPopupMenuFlag && menuItemCount == 3) ||
					(!m_showLinkPopupMenuFlag && menuItemCount == 2))
						// delete option to link to new window
				comboBoxPtr->DeleteString(menuItemCount-1);

			if (m_selectedFileCount == 0 || !m_showLinkPopupMenuFlag)
				comboBoxPtr->ShowWindow(SW_HIDE);

			else		// m_selectedFileCount == 1 && m_showLinkPopupMenuFlag
				comboBoxPtr->ShowWindow(SW_SHOW);
			
			if (m_linkOptionSelectionDataCode == 3)
				{
				m_linkOption = 0;
				m_linkOptionSelectionDataCode = 1;
				m_initialLinkSelectedFilesFlag = FALSE;
				SetImageLinkToFalse ();

				}		// end "if (m_linkOptionSelectionDataCode == 3)"

			if (m_selectedFileCount == 0)
				pWnd->GetDlgItem(1)->EnableWindow(FALSE);

			else		// m_selectedFileCount != 0
				pWnd->GetDlgItem(IDOK)->EnableWindow(TRUE);

			}		// end "if (m_selectedFileCount <= 1)"

		else		// m_selectedFileCount > 1
			{
			if ((m_showLinkPopupMenuFlag && menuItemCount == 2) ||
					(!m_showLinkPopupMenuFlag && menuItemCount == 1))
				{
						// Add link to new window option
				comboBoxPtr->AddString ((LPCTSTR)_T("Link selected files to new image window"));
				comboBoxPtr->SetItemData (menuItemCount, 3);

				}		// end "if ((m_showLinkPopupMenuFlag && ..."
			
			if (gGetFileStatus != 2)
				{
				comboBoxPtr->ShowWindow(SW_SHOW);

				if (!m_initialLinkSelectedFilesFlag)
					{
					m_linkOption = menuItemCount;
					m_linkOptionSelectionDataCode = 3;
					GetDlgItem(13)->ShowWindow(SW_HIDE);
					GetDlgItem(15)->ShowWindow(SW_SHOW);
					m_imageType = 1;
					DDX_CBIndex(m_dialogToPtr, 13, m_imageType);
					m_initialLinkSelectedFilesFlag = TRUE;

					}		// end "if (!m_initialLinkSelectedFilesFlag)"

				}		// end "if (gGetFileStatus != 2)"

			}		// end "if (m_selectedFileCount > 1)"
		
		if (gGetFileStatus != 2)
			DDX_CBIndex (m_dialogToPtr, 14, m_linkOption);

		}		// end "if (gFileFilterIndex == gImageFileFilterIndex)"

}		// end "OnFileNameChange"
