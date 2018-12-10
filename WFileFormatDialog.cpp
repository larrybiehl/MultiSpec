// WFileFormatDialog.cpp : implementation file
//
//	Revised by Larry Biehl on 07/30/2018
//

#include "SMultiSpec.h"

#include "CImageWindow.h"
#include "CProcessor.h" 

#include "WFileFormatDialog.h"
#include "WImageView.h"

//#include	"SExtGlob.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif        

extern void 			FileSpecificationDialogInitialize (
								DialogPtr							dialogPtr,
								Handle								fileInfoHandle,
								Handle								windowInfoHandle,
								FileInfoPtr*						fileInfoPtrPtr,
								WindowInfoPtr*						windowInfoPtrPtr,
								UInt32*								numberLinesPtr,
								UInt32*								numberColumnsPtr,
								UInt32*								numberChannelsPtr,
								SInt32*								startLinePtr,
								SInt32*								startColumnPtr,
								UInt32*								numberHeaderBytesPtr,
								UInt32*								numberTrailerBytesPtr,
								UInt32*								numberPreLineBytesPtr,
								UInt32*								numberPostLineBytesPtr,
								UInt32*								numberPreChannelBytesPtr,
								UInt32*								numberPostChannelBytesPtr,
								UInt32*								blockHeightPtr, 
								UInt32*								blockWidthPtr,
								SInt16*								bandInterleaveSelectionPtr,
								SInt16*								dataValueTypeSelectionPtr,
								SInt16*								eightBitsPerDataSelectionPtr,
								Boolean*								swapBytesFlagPtr,
								Boolean*								signedDataFlagPtr,
								Boolean*								linesBottomToTopFlagPtr,
								Boolean*								fillDataValueExistsFlagPtr,
								double*								fillDataValuePtr,
								SInt16*								hdfDataSetPtr,
								SInt16*								collapseClassesSelecctionPtr,
								Boolean*								callGetHDFLineFlagPtr);

extern Boolean 		FileSpecificationDialogOK (
								DialogPtr							dialogPtr,
								FileInfoPtr							fileInfoPtr,
								Handle								fileInfoHandle,
								Handle*								newMapInformationHandlePtr,
								Handle*								newHfaHandlePtr,
								Handle*								newChannelToHdfDataSetHandlePtr,
								WindowInfoPtr						windowInfoPtr,
								Handle								windowInfoHandle,
								UInt32								numberLines,
								UInt32								numberColumns,
								UInt32								numberChannels,
								SInt32								startLine,
								SInt32								startColumn,
								UInt32								numberHeaderBytes,
								UInt32								numberTrailerBytes,
								UInt32								numberPreLineBytes,
								UInt32								numberPostLineBytes,
								UInt32								numberPreChannelBytes,
								UInt32								numberPostChannelBytes,
								UInt32								blockHeight, 
								UInt32								blockWidth,
								Boolean								forceGroupTableUpdateFlag,
								SInt16								bandInterleaveSelection,
								SInt16								dataValueTypeSelection,
								Boolean								swapBytesFlag,
								Boolean								signedDataFlag,
								Boolean								computeNumberClassesFlag,
								Boolean								linesBottomToTopFlag,
								Boolean								fillDataValueExistsFlag,
								double								fillDataValue,
								SInt16								hdfDataSetSelection,
								UInt32								hdfDataSetIndex,
								SInt16								collapseClassesSelecction,
								UInt16								dataCompressionCode,
								SInt16								gdalDataTypeCode,
								Boolean								callGetHDFLineFlag);

extern void 			FileSpecificationDialogSetInterleaveItems (
								DialogPtr							dialogPtr,
								SInt16								bandInterleaveSelection,
								Boolean								blockedFlag);

extern SInt16 			FileSpecificationDialogSetHDFValues (
								DialogPtr							dialogPtr,
								FileInfoPtr							fileInfoPtr,
								SInt16								hdfDataSetSelection,
								Handle								okHandle,
								Boolean								shiftKeyDownFlag,
								Handle*								mapInformationHandlePtr,
								Handle*								hfaHandlePtr,
								Handle*								newChannelToHdfDataSetHandlePtr,
								SInt16*								bandInterleaveSelectionPtr,
								SInt16*								dataValueTypeSelectionPtr,
								UInt16*								dataCompressionCodePtr,
								SInt16*								gdalDataTypeCodePtr,
								Boolean*								callGetHDFLineFlagPtr);

extern SInt16 			FileSpecificationDialogGetNumberBytes (
								SInt16								dataTypeSelection);                            

// === Static Member Variable ===                      

// CMImageWindow*		CMFileFormatSpecsDlg::sImageWindowCPtr = NULL;							
// FileInfoPtr			CMFileFormatSpecsDlg::sFileInfoPtr = NULL;
// WindowInfoPtr		CMFileFormatSpecsDlg::sWindowInfoPtr = NULL;

/////////////////////////////////////////////////////////////////////////////
// CMFileFormatSpecsDlg dialog


CMFileFormatSpecsDlg::CMFileFormatSpecsDlg(CWnd* pParent /*=NULL*/)
	: CMDialog(CMFileFormatSpecsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMFileFormatSpecsDlg)
	m_numberLines = 512;
	m_numberColumns = 614;
	m_postChannelBytes = 0;
	m_headerBytes = 128;
	m_postLineBytes = 0;
	m_preChannelBytes = 0;
	m_preLineBytes = 0;
	m_numberChannels = 0;
	m_startColumnNumber = 1;
	m_startLineNumber = 1;
	m_swapBytesFlag = FALSE;
	m_linesBottomToTopFlag = FALSE;
	m_trailerBytes = 0;
	m_imageName = "";
	m_computeNumClasses = FALSE;
	m_dataValueType = 0;
	m_bandInterleave = 0;
	m_hdfDataSetSelection = -1;
	m_collapseClassSelection = -1;
	//}}AFX_DATA_INIT

	m_blockHeight = 0;
	m_blockWidth = 0;
	
	m_fileInfoHandle = NULL;
	m_windowInfoHandle = NULL;
	m_newChannelToHdfDataSetHandle = NULL;
	
	m_fileInfoPtr = NULL;
	m_windowInfoPtr = NULL;

	m_dataCompressionCode = 0;

	m_maxNumberChannelsClasses = 0;

	m_dataSetIndex = 1;
	
	m_eightBitsPerDataSelection = 0;
	m_forceGroupTableUpdateFlag = FALSE;
	m_initializedFlag = CMDialog::m_initializedFlag;
	m_showMessagesFlag = TRUE;
	
}

void CMFileFormatSpecsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMFileFormatSpecsDlg)
	DDX_Text(pDX, IDC_BlockHeight, m_blockHeight);
	DDV_MinMaxLong(pDX, m_blockHeight, 0, SInt32_MAX);
	DDX_Text(pDX, IDC_BlockWidth, m_blockWidth);
	DDV_MinMaxLong(pDX, m_blockWidth, 0, SInt32_MAX);
	DDX_Text(pDX, IDC_NumberLines, m_numberLines);
	DDV_MinMaxLong(pDX, m_numberLines, 1, kMaxNumberLines);
	DDX_Text(pDX, IDC_NumberColumns, m_numberColumns);
	DDV_MinMaxLong(pDX, m_numberColumns, 1, kMaxNumberColumns);
	DDX_Text(pDX, IDC_PostChannelBytes, m_postChannelBytes);
	DDV_MinMaxLong(pDX, m_postChannelBytes, 0, SInt32_MAX);
	DDX_Text(pDX, IDC_HeaderBytes, m_headerBytes);
	DDV_MinMaxLong(pDX, m_headerBytes, 0, SInt32_MAX);
	DDX_Text(pDX, IDC_PostLineBytes, m_postLineBytes);
	DDV_MinMaxLong(pDX, m_postLineBytes, 0, SInt32_MAX);
	DDX_Text(pDX, IDC_PreChannelBytes, m_preChannelBytes);
	DDV_MinMaxLong(pDX, m_preChannelBytes, 0, SInt32_MAX);
	DDX_Text(pDX, IDC_PreLineBytes, m_preLineBytes);
	DDV_MinMaxLong(pDX, m_preLineBytes, 0, SInt32_MAX);
	DDX_Text(pDX, IDC_NumberChannels, m_numberChannels);
	DDV_MinMaxLong(pDX, m_numberChannels, 0, m_maxNumberChannelsClasses);
	DDX_Text(pDX, IDC_StartColumnNumber, m_startColumnNumber);
	DDV_MinMaxLong(pDX, m_startColumnNumber, 0, SInt32_MAX); // SInt32_MIN
	DDX_Text(pDX, IDC_StartLineNumber, m_startLineNumber);
	DDV_MinMaxLong(pDX, m_startLineNumber, 0, SInt32_MAX); // SInt32_MIN
	DDX_Check(pDX, IDC_SwapBytes, m_swapBytesFlag);
	DDX_Check(pDX, IDC_LinesBottomToTop, m_linesBottomToTopFlag);
	DDX_Check(pDX, IDC_FillDataValueExists, m_fillDataValueExistsFlag);
	DDX_Text2(pDX, IDC_FillDataValue, m_fillDataValue);
	DDX_Text(pDX, IDC_TrailerBytes, m_trailerBytes);
	DDV_MinMaxLong(pDX, m_trailerBytes, 0, SInt32_MAX);
	DDX_Text(pDX, IDC_ImageName, m_imageName);
	DDV_MaxChars(pDX, m_imageName, 255);
	DDX_Check(pDX, IDC_ComputeNumClasses, m_computeNumClasses);
	DDX_CBIndex(pDX, IDC_DataValueTypePopUp, m_dataValueType);
	DDX_CBIndex(pDX, IDC_BandInterleavePopUp, m_bandInterleave);
	DDX_CBIndex(pDX, IDC_HDFDataSet, m_hdfDataSetSelection);
	DDX_CBIndex(pDX, IDC_CollapseClass, m_collapseClassSelection);
	//}}AFX_DATA_MAP
	
	if (pDX->m_bSaveAndValidate)
		{
		double		sizeDifference;
		UInt32		theNum;
		SInt16		numberBytes;

				// Check that the parameters make sense. In the case for windows
				// version we will just be checking the data type to verify that
				// it can be handled if this is a thematic image window. All of
				// the other
			
			if (m_fileInfoPtr->thematicType && 
						(m_dataValueType < k4BitUnsignedIntegerMenuItem || 
							m_dataValueType > k2ByteUnsignedIntegerMenuItem))
				{						
				DisplayAlert (kErrorAlertID, 
									kStopAlert, 
									kAlertStrID, 
									IDS_Alert135, 
									0, 
									NULL);
				
				pDX->Fail();

				}		// end "if (m_fileInfoPtr->thematicType &&"

				// Check the file parameters against the size of the file.
						
					// Do not check HDF files with more than one data set
					// for now. A more complex check will need to be made to
					// get the name of external files.			
				         
		sizeDifference = 0;
		if ((m_fileInfoPtr->format != kHDF4Type && m_fileInfoPtr->format != kNETCDFType &&
					m_fileInfoPtr->format != kHDF5Type && m_fileInfoPtr->format != kNETCDF2Type &&
							m_fileInfoPtr->format != kNITFType) || 
											m_fileInfoPtr->numberHdfDataSets <= 1)
			{
			theNum = 1;
			if (!m_fileInfoPtr->thematicType)
				theNum = m_numberChannels;

			numberBytes = FileSpecificationDialogGetNumberBytes (m_dataValueType);

			sizeDifference = CompareFileInfoAndFileSize(
						m_fileInfoPtr,
						m_numberLines,
						m_numberColumns,
						theNum,
						numberBytes,
						m_preLineBytes,
						m_postLineBytes,
						m_preChannelBytes,
						m_postChannelBytes,
						m_headerBytes,
						m_trailerBytes,
						m_bandInterleave+1,
						(m_dataValueType == 0) );

			}		// end "if (fileInfoPtr->format != kHDFType || ..."
										
		if (sizeDifference > 0)
			{  			
			SInt16 returnCode = DisplayAlert (kErrorAlertID,
											kCautionAlert, 
											kAlertStrID, 
											IDS_FileSizeLimit,
											0,
											NULL);
											
			pDX->PrepareEditCtrl(IDC_NumberLines);
			pDX->Fail();
			
			}		// end "if (sizeDifference > 0)"	

		else		// sizeDifference <= 0
			{
			if (m_fileInfoPtr->hdfHandle != NULL)
				{
						// Get the hdf data set selection in case it is needed.

				CComboBox*		comboBoxPtr;

				comboBoxPtr = (CComboBox*)GetDlgItem(IDC_HDFDataSet);
				m_dataSetIndex = (SInt16)comboBoxPtr->GetItemData (m_hdfDataSetSelection);

				}		// end "if (m_fileInfoPtr->hdfHandle != NULL)"

			}		// end "else sizeDifference <= 0"
		
		}		// end "if (pDX->m_bSaveAndValidate)"
		
}		// end "DoDataExchange"



BEGIN_MESSAGE_MAP(CMFileFormatSpecsDlg, CMDialog)
	//{{AFX_MSG_MAP(CMFileFormatSpecsDlg)
	ON_CBN_SELENDOK(IDC_BandInterleavePopUp, OnSelendokBandInterleave)
	ON_CBN_SELENDOK(IDC_DataValueTypePopUp, OnSelendokDataValueType)
	ON_BN_CLICKED(IDC_DetermineNumClasses, OnDetermineNumClasses)
	ON_CBN_SELENDOK(IDC_HDFDataSet, OnSelendokHDFDataSet)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_HDFDataSetHelp, OnBnClickedHdfdatasethelp)
	ON_BN_CLICKED(IDC_LinesBottomToTop, &CMFileFormatSpecsDlg::OnBnClickedLinebottomtotop)
	ON_BN_CLICKED(IDC_FillDataValueExists, &CMFileFormatSpecsDlg::OnBnClickedFillDataValueExists)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMFileFormatSpecsDlg message handlers

BOOL CMFileFormatSpecsDlg::OnInitDialog()
					
{  
	WideFileStringPtr				fileNamePtr;
	
	SInt16							bandInterleave,
										collapseClassesSelection,
										dataSetSelection,
										dataValueType;
										 
	Boolean							continueFlag,
										signedDataFlag;                    
	
	                                                  
	CDialog::OnInitDialog();
	
			// Initialize dialog variables.
			
	FileSpecificationDialogInitialize (this,
														m_fileInfoHandle,
														m_windowInfoHandle,
														&m_fileInfoPtr,
														&m_windowInfoPtr,
														&m_numberLines,
														&m_numberColumns,
														&m_numberChannels,
														&m_startLineNumber,
														&m_startColumnNumber,
														&m_headerBytes,
														&m_trailerBytes,
														&m_preLineBytes,
														&m_postLineBytes,
														&m_preChannelBytes,
														&m_postChannelBytes,
														&m_blockHeight, 
														&m_blockWidth,
														&bandInterleave,
														&dataValueType,
														(Boolean*)&m_eightBitsPerDataSelection,
														(Boolean*)&m_swapBytesFlag,
														&signedDataFlag,
														(Boolean*)&m_linesBottomToTopFlag,
														(Boolean*)&m_fillDataValueExistsFlag,
														&m_fillDataValue,
														&dataSetSelection,
														&collapseClassesSelection,
														&m_callGetHDFLineFlag);	
	
	m_bandInterleave = bandInterleave - 1;
	m_dataValueType = dataValueType;
	m_hdfDataSetSelection = dataSetSelection - 1;
	m_collapseClassSelection = collapseClassesSelection - 1;

	m_maxNumberChannelsClasses = kMaxNumberChannels;
	if (m_fileInfoPtr->thematicType)
		m_maxNumberChannelsClasses = kMaxNumberClasses;
	
			//	Set the image file name.										 

	fileNamePtr = (WideFileStringPtr)GetFileNameCPointerFromFileInfo (m_fileInfoPtr, kReturnUnicode);
	if (fileNamePtr != NULL)
		m_imageName = CString(fileNamePtr);
		
	continueFlag = UpdateData(FALSE); 
	
	PositionDialogWindow (); 
	
	return TRUE;  // return TRUE  unless you set the focus to a control
	
}		// end "OnInitDialog" 



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
//	Revised By:			Larry L. Biehl			Date: 05/26/2017	

Boolean 
CMFileFormatSpecsDlg::DoDialog(
				Handle								fileInfoHandle, 
				Handle								windowInfoHandle,
				Handle*								mapInformationHandlePtr,
				Handle*								hfaHandlePtr,
				Handle*								newChannelToHdfDataSetHandlePtr,
				Boolean*								parameterChangedFlagPtr)

{                                  
	INT_PTR				returnCode;
	
	Boolean				continueFlag = FALSE,
							changedFlag,
							classesChanged = FALSE,
							parameterChangedFlag = FALSE;
							
	
			// Make sure intialization has been completed.
							                         
	if ( !m_initializedFlag )
																			return(FALSE);
	
	m_fileInfoHandle = fileInfoHandle;
	m_windowInfoHandle = windowInfoHandle;

	m_mapInformationHandle = *mapInformationHandlePtr;
	m_hfaHandle = *hfaHandlePtr;
	m_newChannelToHdfDataSetHandle = *newChannelToHdfDataSetHandlePtr;
																					
	returnCode = DoModal ();
	
	if (returnCode == IDOK)
		{
		changedFlag = FALSE;
		continueFlag = TRUE; 
		
		parameterChangedFlag = FileSpecificationDialogOK (
												this,
												m_fileInfoPtr,
												fileInfoHandle,
												&m_mapInformationHandle,
												&m_hfaHandle,
												&m_newChannelToHdfDataSetHandle,
												m_windowInfoPtr,
												windowInfoHandle,
												m_numberLines,
												m_numberColumns,
												m_numberChannels,
												m_startLineNumber,
												m_startColumnNumber,
												m_headerBytes,
												m_trailerBytes,
												m_preLineBytes,
												m_postLineBytes,
												m_preChannelBytes,
												m_postChannelBytes,
												m_blockHeight,
												m_blockWidth,
												m_forceGroupTableUpdateFlag,
												m_bandInterleave+1,
												m_dataValueType,
												m_swapBytesFlag,
												FALSE,
												m_linesBottomToTopFlag,
												m_computeNumClasses,
												m_fillDataValueExistsFlag,
												m_fillDataValue,
												m_hdfDataSetSelection+1,
												m_dataSetIndex,
												m_collapseClassSelection+1,
												m_dataCompressionCode,
												m_gdalDataTypeCode,
												m_callGetHDFLineFlag);
													
		}		// end "if (returnCode == IDOK)"
		
	if (parameterChangedFlagPtr != NULL)
		*parameterChangedFlagPtr = parameterChangedFlag;

	*mapInformationHandlePtr = m_mapInformationHandle;
	*hfaHandlePtr = m_hfaHandle;
	*newChannelToHdfDataSetHandlePtr = m_newChannelToHdfDataSetHandle;
	
	return (continueFlag); 
	
}		// end "DoDialog" 



void 
CMFileFormatSpecsDlg::OnSelendokBandInterleave()

{                         
	CDataExchange dx(this, TRUE);

			
			// prevent control notifications from being dispatched during 
			// UpdateData
			
	DDX_CBIndex(&dx, IDC_BandInterleavePopUp, m_bandInterleave);

	FileSpecificationDialogSetInterleaveItems (this, 
																m_bandInterleave+1,
																m_fileInfoPtr->blockedFlag);
	
}		// end "OnSelendokBandInterleave"



void 
CMFileFormatSpecsDlg::OnSelendokDataValueType()

{                                                           
	DDX_CBIndex(m_dialogFromPtr, IDC_DataValueTypePopUp, m_dataValueType); 
	
//	if (m_bitsPerDataValue > m_eightBitsPerDataSelection)
	if (m_dataValueType >= k2ByteSignedIntegerMenuItem)
		{
		GetDlgItem(IDC_SwapBytes)->EnableWindow(TRUE);
							
		}		// end "if (m_BitsPerDataValue > m_EightBitsPerDataSelection)"

	else		// m_bitsPerDataValue < k2ByteSignedIntegerMenuItem
		{
		GetDlgItem(IDC_SwapBytes)->EnableWindow(FALSE);
							
		}		// end "if (m_BitsPerDataValue > m_EightBitsPerDataSelection)"
	
}		// end "OnSelendokDataValueType"


 		
void 
CMFileFormatSpecsDlg::OnDetermineNumClasses(void)

{
	if ( GetClassesFromHistogram (m_fileInfoPtr, m_collapseClassSelection+1) )
		{
		m_numberChannels = m_fileInfoPtr->numberClasses;      
		DDX_Text(m_dialogToPtr, IDC_NumberChannels, m_numberChannels);
		m_forceGroupTableUpdateFlag = TRUE;

		}		// end "if ( GetClassesFromHistogram (gFileInfoPtr) )"
		
	if (m_fileInfoPtr->classesComputedFlag)
		GetDlgItem(IDC_DetermineNumClasses)->EnableWindow(FALSE);
	
}		// end "OnDetermineNumClasses"



void CMFileFormatSpecsDlg::OnSelendokHDFDataSet() 
{
	int					newDataSetSelection;

	SInt16				bandInterleaveSelection,
							dataValueTypeSelection;

	Boolean				shiftKeyDownFlag;
							

	DDX_CBIndex(m_dialogFromPtr, IDC_HDFDataSet, newDataSetSelection); 
	
	shiftKeyDownFlag = FALSE;
	if (GetKeyState(VK_SHIFT) & 0x8000)
		shiftKeyDownFlag = TRUE;
	
	if (newDataSetSelection != m_hdfDataSetSelection || shiftKeyDownFlag)
		{
		if (FileSpecificationDialogSetHDFValues (
										this,
										m_fileInfoPtr,
										newDataSetSelection,
										NULL,
										shiftKeyDownFlag,
										&m_mapInformationHandle,
										&m_hfaHandle,
										&m_newChannelToHdfDataSetHandle,
										&bandInterleaveSelection,
										&dataValueTypeSelection,
										&m_dataCompressionCode,
										&m_gdalDataTypeCode,
										&m_callGetHDFLineFlag) == noErr)
			{
			m_hdfDataSetSelection = newDataSetSelection;
		
			m_bandInterleave = bandInterleaveSelection - 1;
			DDX_CBIndex(m_dialogToPtr, 
							IDC_BandInterleavePopUp, 
							m_bandInterleave);
		
			m_dataValueType = dataValueTypeSelection;
			DDX_CBIndex(m_dialogToPtr, 
							IDC_DataValueTypePopUp, 
							m_dataValueType);

			}		// end "if (FileSpecificationDialogSetHDFValues (..."

		else		// FileSpecificationDialogSetHDFValues != noErr
			DDX_CBIndex(m_dialogToPtr, IDC_HDFDataSet, m_hdfDataSetSelection); 
							
		}		// end "if (savedDataSetSelection != m_hdfDataSetSelection)"
	
}		// end "OnSelendokHDFDataSet"



void CMFileFormatSpecsDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	SInt16				bandInterleaveSelection,
							dataValueTypeSelection;

	
	if (m_showMessagesFlag)
		{
		m_showMessagesFlag = FALSE;
	
				// Do not allow specifications to be changed if the active				
				// window represents the base image for an open project.					
		
		if (gProcessorCode == kChangeImageDescriptionProcessor &&
															m_windowInfoPtr->projectWindowFlag)
			{
			DisplayAlert (kErrorAlertID, kCautionAlert, kAlertStrID, IDS_Alert25, 0, NULL);
			
			}		// end "if (gProcessorCode == kChangeImageDescriptionProcessor && ..."

		if (gProcessorCode == kOpenImageFileProcessor && 
					(m_fileInfoPtr->format == kHDF4Type || m_fileInfoPtr->format == kNETCDFType ||
						m_fileInfoPtr->format == kHDF5Type || m_fileInfoPtr->format == kNETCDF2Type ||
							m_fileInfoPtr->format == kNITFType) &&
											m_fileInfoPtr->numberHdfDataSets > 1 &&
																!gHDFDataSetSelectionAlertDisplayedFlag)
			{
			DisplayAlert (kErrorAlertID, 
							kNoteAlert, 
							kAlertStrID, 
							IDS_Alert103,
							IDS_Alert104, 
							NULL);

			gHDFDataSetSelectionAlertDisplayedFlag = TRUE;
			
			}		// end "if (gProcessorCode == kOpenImageFileProcessor && fileInfoPtr->...)"
	
				// Force any alert messages concerning the default hdf data set to
				// be shown.
				
		if (gProcessorCode == kOpenImageFileProcessor && 
				(m_fileInfoPtr->format == kHDF4Type || m_fileInfoPtr->format == kNETCDFType ||
						m_fileInfoPtr->format == kHDF5Type || m_fileInfoPtr->format == kNETCDF2Type ||
								m_fileInfoPtr->format == kNITFType))	
			{
			FileSpecificationDialogSetHDFValues (this,
																m_fileInfoPtr,
																m_hdfDataSetSelection,
																NULL,
																FALSE,
																&m_mapInformationHandle,
																&m_hfaHandle,
																&m_newChannelToHdfDataSetHandle,
																&bandInterleaveSelection,
																&dataValueTypeSelection,
																&m_dataCompressionCode,
																&m_gdalDataTypeCode,
																&m_callGetHDFLineFlag);
		
			m_bandInterleave = bandInterleaveSelection - 1;
			DDX_CBIndex(m_dialogToPtr, 
							IDC_BandInterleavePopUp, 
							m_bandInterleave);
		
			m_dataValueType = dataValueTypeSelection;
			DDX_CBIndex(m_dialogToPtr, 
							IDC_DataValueTypePopUp, 
							m_dataValueType);

			}		// end "if (gProcessorCode == kOpenImageFileProcessor && ..."

		}		// end "if (m_showMessagesFlag)"
	
	// Do not call CMDialog::OnPaint() for painting messages
}



void CMFileFormatSpecsDlg::OnBnClickedHdfdatasethelp()
{
	// TODO: Add your control notification handler code here

	DisplayAlert (kErrorAlertID, 
							kNoteAlert, 
							kAlertStrID, 
							IDS_Alert103,
							IDS_Alert104, 
							NULL);
}

void CMFileFormatSpecsDlg::OnBnClickedLinebottomtotop()
{
	// TODO: Add your control notification handler code here
}



void CMFileFormatSpecsDlg::OnBnClickedFillDataValueExists()
{						
	DDX_Check(m_dialogFromPtr, IDC_FillDataValueExists, m_fillDataValueExistsFlag);
	ShowHideDialogItem (this, IDC_FillDataValue, m_fillDataValueExistsFlag);

}		// end "OnBnClickedFillDataValueExists"
