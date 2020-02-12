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
//	File:						WDisplayThematicDialog.cpp : implementation file
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
//								CMDisplayThematicDlg class.
//
//------------------------------------------------------------------------------------

#include	"SMultiSpec.h"

#include "SDisplay_class.h"
#include "SImageWindow_class.h"

#include "WClassGroupDialog.h"   
#include "WDisplayThematicDialog.h"
#include "WImageView.h"
#include "WLegendView.h"  

#ifdef _DEBUG
	#undef THIS_FILE
	static char BASED_CODE THIS_FILE[] = __FILE__;
#endif



BEGIN_MESSAGE_MAP (CMDisplayThematicDlg, CMDialog)
	//{{AFX_MSG_MAP (CMDisplayThematicDlg)
	ON_BN_CLICKED (IDC_BackgroundColor, OnBackgroundColor)
	ON_BN_CLICKED (IDC_CHECK1, OnBnClickedCheck1)
	ON_BN_CLICKED (IDEntireImage, ToEntireImage)
	ON_BN_CLICKED (IDSelectedImage, ToSelectedImage)

	ON_CBN_DROPDOWN (IDC_PaletteCombo, OnDropdownPaletteCombo)

	ON_CBN_SELENDOK (IDC_ClassesGroupsCombo, OnSelendokClassesGroupsCombo)
	ON_CBN_SELENDOK (IDC_PaletteCombo, OnSelendokPaletteCombo)

	ON_EN_CHANGE (IDC_ColumnEnd, CheckColumnEnd)
	ON_EN_CHANGE (IDC_ColumnStart, CheckColumnStart)
	ON_EN_CHANGE (IDC_LineEnd, CheckLineEnd)
	ON_EN_CHANGE (IDC_LineStart, CheckLineStart)

	ON_WM_PAINT ()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()



CMDisplayThematicDlg::CMDisplayThematicDlg (
				CWnd* 								pParent /*=NULL*/)
		: CMDialog (CMDisplayThematicDlg::IDD, pParent)

{                                
	//{{AFX_DATA_INIT (CMDisplayThematicDlg)
	m_classGroupSelection = 0;
	m_classSelection = 0;
	m_showLegendFlag = FALSE;
	m_paletteSelection = 0;
	m_numberClasses = "";
	m_numberGroups = "";
	m_fileName = "";
	m_magnification = 1.;
	m_useThresholdFileFlag = FALSE;
	m_includeVectorOverlaysFlag = FALSE;
	//}}AFX_DATA_INIT
	
	m_localClassGroupsHandle = NULL;
	m_localClassToGroupPtr = NULL;
	m_localClassGroupsPtr = NULL;
	
	m_initializedFlag = CMDialog::m_initializedFlag; 
		
	if (m_initializedFlag)                                                              
		{
		TRY
			{																			
					//	Set the image file name.
	
			WideFileStringPtr fileNamePtr = (WideFileStringPtr)(gActiveImageViewCPtr->
							GetImageWindowCPtr())->GetImageFileNamePointer (kReturnUnicode);
			if (fileNamePtr != NULL)
				m_fileName = CString	(fileNamePtr);
				
			}
			
		CATCH_ALL (e)
			{
			m_initializedFlag = FALSE;
			
			}
		
		END_CATCH_ALL
		
		}	// end "if (m_initializedFlag)"
	                                 
	if (m_initializedFlag)                                                              
		{
				// Get pointer to local class to group list.									
		
		m_localClassGroupsHandle = MNewHandle (
						(SInt32)gImageFileInfoPtr->numberClasses * 2 * sizeof	(SInt16));
						
		m_initializedFlag = (m_localClassGroupsHandle != NULL);
		
		}	// end "if (m_initializedFlag)"
	                                 
	if (m_initializedFlag)
		{
		m_localClassGroupsPtr = (SInt16*)GetHandlePointer (
																m_localClassGroupsHandle, kLock);
		
		}	// end "if (m_initializedFlag)"                   
	
}	// end "CMDisplayThematicDlg"  



CMDisplayThematicDlg::~CMDisplayThematicDlg (void)

{
	m_localClassGroupsHandle = UnlockAndDispose (m_localClassGroupsHandle);
	
	m_localClassToGroupPtr = CheckAndDisposePtr (m_localClassToGroupPtr);                                                              
	
}	// end "~CMDisplayThematicDlg"



void CMDisplayThematicDlg::DoDataExchange (
				CDataExchange* 					pDX)

{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP (CMDisplayThematicDlg)
	DDX_CBIndex (pDX, IDC_ClassesGroupsCombo, m_classSelection);
	DDX_CBIndex (pDX, IDC_DisplayCombo, m_classGroupSelection);
	DDX_Check (pDX, IDC_DisplayLegend, m_showLegendFlag);
	DDX_CBIndex (pDX, IDC_PaletteCombo, m_paletteSelection);
	DDX_Text (pDX, IDC_NumberClasses, m_numberClasses);
	DDV_MaxChars (pDX, m_numberClasses, 32);
	DDX_Text (pDX, IDC_NumberGroups, m_numberGroups);
	DDV_MaxChars (pDX, m_numberGroups, 32);
	DDX_Text (pDX, IDC_FileName, m_fileName);
	DDV_MaxChars (pDX, m_fileName, gFileNameLengthLimit);
	DDX_Text2 (pDX, IDC_Magnification, m_magnification);
	DDV_MinMaxDouble (pDX, m_magnification, 1.e-002, 99.);
	DDX_Text (pDX, IDC_LineEnd, m_LineEnd);
	DDV_MinMaxLong (pDX, m_LineEnd, 1, m_maxNumberLines);
	DDX_Text (pDX, IDC_LineInterval, m_LineInterval);
	DDV_MinMaxLong (pDX, m_LineInterval, 1, m_maxNumberLines);
	DDX_Text (pDX, IDC_LineStart, m_LineStart);
	DDV_MinMaxLong (pDX, m_LineStart, 1, m_maxNumberLines);
	DDX_Text (pDX, IDC_ColumnEnd, m_ColumnEnd);
	DDV_MinMaxLong (pDX, m_ColumnEnd, 1, m_maxNumberColumns);
	DDX_Text (pDX, IDC_ColumnInterval, m_ColumnInterval);
	DDV_MinMaxLong (pDX, m_ColumnInterval, 1, m_maxNumberColumns);
	DDX_Text (pDX, IDC_ColumnStart, m_ColumnStart);
	DDV_MinMaxLong (pDX, m_ColumnStart, 1, m_maxNumberColumns);
	DDX_Check (pDX, IDC_UseThresholdingFile, m_useThresholdFileFlag);
	DDX_Check (pDX, IDC_VectorOverlays, m_includeVectorOverlaysFlag);
	//}}AFX_DATA_MAP 
	
	if (pDX->m_bSaveAndValidate)
		{    
		SInt16 returnCode = VerifyLineColumnValues (pDX,
																	10,
																	13,
																	m_displaySpecsPtr->pixelSize,
																	2,
																	1); 										
			
		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;
		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;      
		
				// Get the data value associated with the selection palette item.
				
		gPaletteSelection = (SInt16)((CComboBox*)GetDlgItem (IDC_PaletteCombo))->
																	GetItemData (m_paletteSelection);
		
				// Adjust Class/Group selection to be 1 based.
				
		gClassGroupSelection = m_classGroupSelection + 1; 
		
				// Adjust All or Subset of Classes/Groups selection to be 1 based.
				
		gClassSelection = m_classSelection + 1;
		
		}	// end "if (pDX->m_bSaveAndValidate)"
		
}	// end "DoDataExchange"



//-----------------------------------------------------------------------------
//                   Copyright 1988-2020 Purdue Research Foundation
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the display
//							thematic specification dialog box to the user and copy the
//							revised back to the display specification structure if
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
//	Coded By:			Larry L. Biehl			Date: 03/28/1996
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean CMDisplayThematicDlg::DoDialog (
				DisplaySpecsPtr					displaySpecsPtr)

{  
	Boolean								continueFlag = FALSE;
	
	INT_PTR								returnCode;

	                          
			// Make sure intialization has been completed.
							                         
	if (!m_initializedFlag)
																						return (FALSE);
																			
	m_displaySpecsPtr = displaySpecsPtr;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{		 
				// Display area													
				
		m_dialogSelectArea.lineStart = m_LineStart;
		m_dialogSelectArea.lineEnd = m_LineEnd;
		m_dialogSelectArea.lineInterval = m_LineInterval;
																
		m_dialogSelectArea.columnStart = m_ColumnStart;
		m_dialogSelectArea.columnEnd = m_ColumnEnd;
		m_dialogSelectArea.columnInterval = m_ColumnInterval;

		gCurrentSelectedColor = m_oldRGB;

		DisplayThematicDialogOK (m_magnification,
											displaySpecsPtr,
											&m_dialogSelectArea,
											m_localClassGroupsPtr,
											m_localClassGroupsHandle,
											m_localDisplayClasses,
											m_localDisplayGroups,
											m_localAllSubsetClass,
											m_localAllSubsetGroup,
											m_localPaletteUpToDateFlag,
											m_showLegendFlag,
											m_includeVectorOverlaysFlag); 
			
		      // Get maximum magnification value to use.
		      
		displaySpecsPtr->maxMagnification = CMDisplay::GetMaxZoomValue (displaySpecsPtr);
	
				// Magnification to use										
						
		displaySpecsPtr->magnification = 
									MIN (m_magnification, displaySpecsPtr->maxMagnification);
		displaySpecsPtr->magnification = 
									MAX (gMinMagnification, displaySpecsPtr->magnification);
		
				// Update the combo list in the legend. 
				
		CMLegendView* legendViewCPtr = gActiveImageViewCPtr->GetImageLegendViewCPtr ();
		legendViewCPtr->UpdateClassGroupComboList (gClassGroupSelection);
		 
		continueFlag = TRUE;
		
		}	// end "if (returnCode == IDOK)"
		
	return (continueFlag);
		
}	// end "DoDialog"



void CMDisplayThematicDlg::OnBackgroundColor ()

{
	RGBColor								newRGB;


	if (SelectColor (3, &m_oldRGB, &newRGB))
		{
		m_oldRGB = newRGB;

		InvalidateRect (NULL, FALSE);

		}	// end "if (SelectColor (3, ..."
	
}	// end "OnBackgroundColor"



void CMDisplayThematicDlg::OnBnClickedCheck1 ()

{
	// Add your control notification handler code here
	
}	// end "OnBnClickedCheck1"



void CMDisplayThematicDlg::OnDropdownPaletteCombo ()

{
	SInt16								paletteCode;


	if (m_displaySpecsPtr->classGroupCode == kClassDisplay)
		paletteCode = m_displaySpecsPtr->thematicClassPaletteType;

	else    // m_displaySpecsPtr->classGroupCode != kClassDisplay
		paletteCode = m_displaySpecsPtr->thematicGroupPaletteType;
	
	SetUpPalettePopUpMenu (this,
									NULL,
									gImageFileInfoPtr->format,
									gImageFileInfoPtr->ancillaryInfoformat,
									gImageFileInfoPtr->numberClasses,
									gImageFileInfoPtr->classDescriptionH,
									gImageFileInfoPtr->colorTableOffset,
									m_displaySpecsPtr,
									abs (m_classGroupSelection) + 1,
									paletteCode);
	
	DDX_CBIndex (m_dialogToPtr, IDC_PaletteCombo, m_paletteSelection);
	
}	// end "OnDropdownPaletteCombo"



BOOL CMDisplayThematicDlg::OnInitDialog ()

{
	SInt32								numberClasses,
											numberGroups;

	SInt16								classGroupSelection,
											classSelection;

	Boolean								includeVectorOverlaysFlag,
											showLegendFlag,
											useThresholdFileFlag;
	
	
	CMDialog::OnInitDialog ();

			// Get pointer to display specifications
			
	DisplayThematicDialogInitialize (this,
												m_displaySpecsPtr,
												gImageWindowInfoPtr,
												gImageFileInfoPtr,
												&m_dialogSelectArea,
												&m_magnification,
												&classGroupSelection,
												&m_paletteChangedFlag,
												&m_localPaletteUpToDateFlag,
												m_localClassGroupsPtr,
												&m_localDisplayClasses,
												&m_localDisplayGroups,
												&m_localAllSubsetClass,
												&m_localAllSubsetGroup,
												&classSelection,
												&m_oldRGB,
												&showLegendFlag,
												&useThresholdFileFlag,
												&numberClasses,
												&numberGroups,
												&includeVectorOverlaysFlag);

	m_classGroupSelection = classGroupSelection - 1;
	m_classSelection = classSelection - 1;

	m_showLegendFlag = showLegendFlag;
	m_useThresholdFileFlag = useThresholdFileFlag;
	m_includeVectorOverlaysFlag = includeVectorOverlaysFlag;
	
			// Load number of classes in c-string variable.										
	
	DDX_Text (m_dialogFromPtr, IDC_NumberClasses, m_numberClasses);
	
			// Load number of groups in c-string variable.										
	
	DDX_Text (m_dialogFromPtr, IDC_NumberGroups, m_numberGroups);

		// Make sure that we have the bitmaps for the entire image buttons.
		
	VERIFY (toEntireButton.AutoLoad (IDEntireImage, this));
	VERIFY (toSelectedButton.AutoLoad (IDSelectedImage, this));
		
			// Determine if this is the entire area and set the 
			// to entire image icon.
	
	SetEntireImageButtons (NULL,
									m_LineStart, 
									m_LineEnd, 
									m_ColumnStart, 
									m_ColumnEnd);
	
			// Update the default palette selection in the combo base on the
			// items actually in the list.                                    
	                                    
	m_paletteSelection = GetComboListSelection (IDC_PaletteCombo, gPaletteSelection);
												                 
	if (UpdateData (FALSE))
		PositionDialogWindow ();
		                                       
	SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
	
	return FALSE;  // return TRUE  unless you set the focus to a control
	
}	// end "OnInitDialog"



void CMDisplayThematicDlg::OnPaint ()

{
	CPaintDC 							dc (this); // device context for painting
	
	COLORREF								newColor;

	RECT									colorChipRect;

	CWnd*									cWndPtr;

	SInt32								eightBitColor;
	

	eightBitColor = m_oldRGB.blue/256;
	newColor = eightBitColor<<16;

	eightBitColor = m_oldRGB.green/256;
	newColor += eightBitColor<<8;

	eightBitColor = m_oldRGB.red/256;
	newColor += eightBitColor;

	cWndPtr = (CWnd*)GetDlgItem (IDC_BackgroundColorChip);
	cWndPtr->GetClientRect (&colorChipRect);
	cWndPtr->MapWindowPoints (this, &colorChipRect);

	dc.Rectangle (&colorChipRect);

	colorChipRect.top++;
	colorChipRect.bottom--;
	colorChipRect.left++;
	colorChipRect.right--;
	dc.FillSolidRect (&colorChipRect, newColor);
	
			// Do not call CMDialog::OnPaint () for painting messages
	
}	// end "OnPaint"



void CMDisplayThematicDlg::OnSelendokClassesGroupsCombo (void)

{
	Boolean								returnFlag = FALSE;


			// Determine whether all or subset of classes/groups are
			// was selected.
	
	DDX_CBIndex (m_dialogFromPtr, IDC_ClassesGroupsCombo, m_classSelection);
	
			// Determine whether classes or groups are being used.
	
	DDX_CBIndex (m_dialogFromPtr, IDC_DisplayCombo, m_classGroupSelection);
	
			// Adjust for an offset of 1 not 0
	
	m_classGroupSelection++;
	
	if (m_classSelection == kSubsetMenuItem)
		{
				// Subset of classes or groups to be used.
		
		SetDLogControlHilite (NULL, IDOK, 255);
		
		CMClassGroupDlg*		classesGroupsDialogPtr = NULL;
		
		TRY
			{
			classesGroupsDialogPtr = new CMClassGroupDlg ();
			
			if (m_classGroupSelection == kClassDisplay)
				{
				returnFlag = classesGroupsDialogPtr->DoDialog (
														&m_localDisplayClasses,
														m_localClassGroupsPtr,
														1,
														(SInt16)gImageFileInfoPtr->numberClasses,
														kClassDisplay);
					
				if (m_localDisplayClasses == gImageFileInfoPtr->numberClasses)
					{
					m_classSelection = kAllMenuItem;
					DDX_CBIndex (m_dialogToPtr, IDC_ClassesGroupsCombo, m_classSelection);
					
					}	// end "if (m_localDisplayClasses == ..."
					
				}	// end "if (m_classGroupSelection == kClassDisplay)"
			
			if (m_classGroupSelection == kGroupDisplay)
				{
				returnFlag = classesGroupsDialogPtr->DoDialog (
										&m_localDisplayGroups,
										&m_localClassGroupsPtr[gImageFileInfoPtr->numberClasses],
										1,
										m_localNumberGroups,
										kGroupDisplay);
				
				if (m_localDisplayGroups == (UInt16)m_localNumberGroups)
					{
					m_classSelection = kAllMenuItem;
					DDX_CBIndex (m_dialogToPtr, IDC_ClassesGroupsCombo, m_classSelection);
					
					}	// end "if (m_localDisplayGroups == ..."
					
				}	// end "if (m_classGroupSelection == kGroupDisplay)"
			
		   if (classesGroupsDialogPtr != NULL)
				delete classesGroupsDialogPtr;
				
			}	// end "TRY"
		
		CATCH_ALL (e)
			{
			MemoryMessage (0, kObjectMessage);
			}
		
		END_CATCH_ALL
		
		SetDLogControlHilite (NULL, IDOK, 0);
		
		}	// end "if (m_classSelection == kSubsetMenuItem)"

	if (returnFlag)
		m_localPaletteUpToDateFlag  = FALSE;
	
	if (m_classGroupSelection == kClassDisplay)
		m_localAllSubsetClass = m_classSelection + 1;
	
	else	// m_classGroupSelection == kGroupDisplay
		m_localAllSubsetGroup = m_classSelection + 1;
	
}	// end "OnSelendokClassesGroupsCombo"



void CMDisplayThematicDlg::OnSelendokPaletteCombo ()

{  
	int									lastPaletteSelection;
	
	
	lastPaletteSelection = m_paletteSelection;                                                        
	DDX_CBIndex (m_dialogFromPtr, IDC_PaletteCombo, m_paletteSelection);
	
	CComboBox* comboBoxPtr = (CComboBox*)GetDlgItem (IDC_PaletteCombo);
	gPaletteSelection = (SInt16)comboBoxPtr->GetItemData (m_paletteSelection);
	
	if (gPaletteSelection == kFalseColors)
		{
		if (FalseColorPaletteDialog ())
			m_localPaletteUpToDateFlag = FALSE;
			
		else	// !FalseColorPaletteDialog ()
			{                 
			if (lastPaletteSelection != m_paletteSelection)
				{
				m_paletteSelection = lastPaletteSelection;                                            
				DDX_CBIndex (m_dialogToPtr, IDC_PaletteCombo, m_paletteSelection);
				
				}	// end "if (lastPaletteSelection != m_paletteSelection)"
				
			m_paletteSelection = lastPaletteSelection;
			
			}	// end "else !FalseColorPaletteDialog ()"
			
		}	// end "if (gPaletteSelection == kFalseColors)"
	
	if (lastPaletteSelection != m_paletteSelection)
		m_paletteChangedFlag = TRUE;
	
}	// end "OnSelendokPaletteCombo"
