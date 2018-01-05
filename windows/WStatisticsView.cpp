// WStatisticsView.cpp : implementation file
//                   
                             
#include "SMultiSpec.h"
                     
#include "WStatisticsView.h"
#include "WStatisticsDoc.h"
#include "WImageFrame.h"

//#include "SExtGlob.h" 	

extern void 		WriteProjectName (void);

extern SInt16 		GetCovarianceStatsFromMenuItem (
							SInt16								menuItem);
							
extern SInt16 		GetProjectStatisticsTypeMenuItem (void);

extern void 		GetProjectStatisticsTypeText (
							char*									textStringPtr,
							SInt16								menuItem);

extern void 		FieldListStatMode (
							SInt16								classNumber); 

extern SInt16	 	GetCurrentField (
							SInt16								classNumber, 
							SInt16								classFieldNumber);

extern void 		PolygonListStatMode (
							SInt16								classFieldNumber); 

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMStatisticsForm

IMPLEMENT_DYNCREATE(CMStatisticsForm, CFormView)

CMStatisticsForm::CMStatisticsForm()
	: CFormView(CMStatisticsForm::IDD)
{
	//{{AFX_DATA_INIT(CMStatisticsForm)
	m_classList = 0;  
	m_histogramStatsCode = 0;
	m_listStatsCode = 0;
	m_statsTypeCode = 0;
	m_polygonFlag = FALSE;
	//}}AFX_DATA_INIT
	
	m_initializedFlag = FALSE;
	m_dialogFromPtr = NULL;
	m_dialogToPtr = NULL;

	m_optionKeyFlag = FALSE;
			
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
	
}		// end "CMStatisticsForm"


CMStatisticsForm::~CMStatisticsForm()
{  
	gProjectWindow = NULL; 
	
	if (m_dialogFromPtr != NULL)
		delete m_dialogFromPtr; 
			
	if (m_dialogToPtr != NULL)
		delete m_dialogToPtr;
	
			// Reset some project parameters.
				
	CloseProjectWindow();
	
}         



void CMStatisticsForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMStatisticsForm)
	DDX_CBIndex(pDX, IDC_ClassList, m_classList);
	DDX_CBIndex(pDX, IDC_HistogramStatsCombo, m_histogramStatsCode);
	DDX_CBIndex(pDX, IDC_ListStatsCombo, m_listStatsCode);
	DDX_CBIndex(pDX, IDC_StatsCombo, m_statsTypeCode);
//new
	DDX_Check(pDX, IDC_Polygon, m_polygonFlag);
//end new
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMStatisticsForm, CFormView)
	//{{AFX_MSG_MAP(CMStatisticsForm)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_AddToList, OnAddToList)
	ON_CBN_SELENDOK(IDC_ClassList, OnSelendokClassList)
	ON_BN_CLICKED(IDC_Project, OnProject)
	ON_BN_CLICKED(IDC_Class, OnClass)
	ON_BN_CLICKED(IDC_Field, OnField)
	ON_BN_CLICKED(IDC_Update, OnUpdate)
	ON_BN_CLICKED(IDC_Polygon, OnPolygon)
	ON_BN_CLICKED(IDC_Select, OnSelect) 
	ON_BN_CLICKED(IDC_Histogram, OnHistogram)
	ON_LBN_SELCHANGE(IDC_ListBox, OnSelchangeListBox)
	ON_LBN_DBLCLK(IDC_ListBox, OnDblclkListBox)
	ON_BN_CLICKED(IDC_EditName, OnEditName)
	ON_CBN_SELENDOK(IDC_ListStatsCombo, OnSelendokListStatsCombo)
	ON_CBN_SELENDOK(IDC_HistogramStatsCombo, OnSelendokHistogramStatsCombo)
	ON_CBN_SELENDOK(IDC_StatsCombo, OnSelendokStatsCombo)
	ON_WM_CHAR()
	ON_CBN_DROPDOWN(IDC_StatsCombo, OnDropdownStatsTypeCombo)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMStatisticsForm message handlers  



void 
CMStatisticsForm::OnInitialUpdate(void)

{  
	CMStatisticsDoc* documentPtr = GetDocument();
	documentPtr->SetStatisticsFrame ( (CMStatisticsFrame*)gProjectWindow );
	
			// Force the window frame to be the same size as the statistics view.
	
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit(FALSE);
	
			// Move the statistics window to the right portion of the client area
			// of the main frame rect.
	                            
	CRect		mainFrameRect,
				statisticsRect;
	SInt16	xLocation,
				xSize,
				yLocation,
				ySize;		
	
	CFrameWnd* pMainFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	pMainFrame->GetClientRect (&mainFrameRect);
	GetParentFrame()->GetWindowRect (&statisticsRect);
	
	xSize = (SInt16)(statisticsRect.right - statisticsRect.left);
	ySize = (SInt16)(statisticsRect.bottom - statisticsRect.top);
	xLocation = (SInt16)(mainFrameRect.right - xSize - 4);
	yLocation = (SInt16)(mainFrameRect.top + 22);
	GetParentFrame()->MoveWindow (xLocation, yLocation, xSize, ySize, TRUE); 
	
	if ( m_listBox.SubclassDlgItem(IDC_ListBox, this) )
		{                             
		m_listBox.SetTabStops (30);                
		gProjectWindow = (CMImageView*)this;
		gStatisticsListHandle = &m_listBox;
		
		}		// end "if ( m_listBox.SubclassDlgItem(IDC_ListBox, this) )"
	
			// Add current list of classes to the combo box.
			
	CreateClassNameComboBoxList( (CComboBox*)GetDlgItem(IDC_ClassList) );

	MHiliteControl (gProjectWindow, gProjectInfoPtr->addToControlH, 1);

			// This control is no longer used.

	GetDlgItem(IDC_Histogram)->ShowWindow(SW_HIDE);


	//	UpdateData (false); 
	DDX_CBIndex(m_dialogToPtr, IDC_ClassList, m_classList); 
	 
	CComboBox* comboBoxPtr = (CComboBox*)GetDlgItem(IDC_StatsCombo);
	
	comboBoxPtr->SetItemData(0, kOriginalStats);
	comboBoxPtr->SetItemData(1, kLeaveOneOutStats);
	comboBoxPtr->SetItemData(2, kEnhancedStats);
	comboBoxPtr->SetItemData(3, kMixedStats);

	WriteProjectName ();
	
}		// end "OnInitialUpdate"

 
               
void CMStatisticsForm::OnDraw(
				CDC* 				pDC)
				
{  
	CString 						tempCString;
	
	SInt16						classStorage;
	
	
	CFormView::OnDraw(pDC);
	
	if (gProjectInfoPtr == NULL)
																					return;
																				
	switch (gProjectInfoPtr->statsWindowMode)
		{
		case 2:
//			menuItem = GetProjectStatisticsTypeMenuItem (); 
//			GetProjectStatisticsTypeText( (char*)gTextString, menuItem ); 
//			tempCString = CString((char*)&gTextString[1]); 
					
//			DDX_Text(m_dialogToPtr, 
//						IDC_StatsType, 
//						tempCString); 
						
			UpdateStatsTypeCombo (gProjectInfoPtr->statsWindowMode);  
			
			break;
																	
		case 3:		// "Field List" mode 
			UpdateStatsTypeCombo (gProjectInfoPtr->statsWindowMode);  
			
		case 4:		// "Polygon Point List" mode
	                                   
			classStorage = gProjectInfoPtr->storageClass[
															gProjectInfoPtr->currentClass];
															
			tempCString = 
					CString((char*)&gProjectInfoPtr->classNamesPtr[classStorage].name[1]);
					
			DDX_Text(m_dialogToPtr, 
						IDC_ClassName, 
						tempCString); 
			
			if (gProjectInfoPtr->statsWindowMode == 4)
				{
				GetFieldNameWithType (gProjectInfoPtr->currentField, gTextString);
				tempCString = CString((char*)&gTextString[1]);
				DDX_Text(m_dialogToPtr, IDC_FieldName, tempCString);
				
				}		// end "if (gProjectInfoPtr->statsWindowMode == 4)"
			
			break;
			
		}		// end "switch (gProjectInfoPtr->statsWindowMode)"  
																					
}		// end "OnDraw"



void CMStatisticsForm::OnDestroy()
{
	CFormView::OnDestroy();
	
	// TODO: Add your message handler code here

	CloseTheProject ();
	
}  


CMStatisticsDoc* 
CMStatisticsForm::GetDocument()
{                                                           
	return (CMStatisticsDoc*)m_pDocument;
}



void 
CMStatisticsForm::OnAddToList(void)

{  
	Point			location;
	
	
	location.h = 0;
	location.v = 0;
	                          
	StatisticsWControlEvent (
				gProjectInfoPtr->addToControlH, 
				FALSE, 
				location,
				kAddToListControl); 
	
}		// end "OnAddToList"



void 
CMStatisticsForm::OnSelendokClassList(void)

{  
	DDX_CBIndex(m_dialogFromPtr, IDC_ClassList, m_classList);
	gProjectInfoPtr->lastClassForAddFieldSelection = m_classList + 1;
	
}		// end "OnSelendokClassList"



void CMStatisticsForm::OnProject()
{                                    	                                                         
	Point						location;
	
	
	location.h = 0;
	location.v = 0;
	                          
	StatisticsWControlEvent (
				gProjectInfoPtr->toProjectControlH, 
				FALSE, 
				location,
				kToProjectControl);
	
}		// end "OnProject"



void CMStatisticsForm::OnClass()
{                                                          
	Point			location;
	
	
	location.h = 0;
	location.v = 0;
	                          
	StatisticsWControlEvent (
				gProjectInfoPtr->toClassControlH, 
				FALSE, 
				location,
				kToClassControl);
				
	UpdateStatsTypeCombo (kFieldListMode);

	UpdateHistogramStatsCombo ();
				
	UpdateListStatsCombo ();
	
}		// end "OnClass"



void CMStatisticsForm::OnField()
{                                                           
	Point			location;
	
	
	location.h = 0;
	location.v = 0;
	                          
	StatisticsWControlEvent (
				gProjectInfoPtr->toFieldControlH, 
				FALSE, 
				location,
				kToFieldControl);

	UpdateHistogramStatsCombo ();
				
	UpdateListStatsCombo ();
	
}		// end "OnField"  



void CMStatisticsForm::OnUpdate()
{                                                          
	Point			location;
	
	
	location.h = 0;
	location.v = 0;
	                          
	StatisticsWControlEvent (
				gProjectInfoPtr->updateControlH, 
				FALSE, 
				location,
				kUpdateStatsControl);
	
}		// end "OnUpdate"

//new
void CMStatisticsForm::OnPolygon()

{                                                                
	DDX_Check(m_dialogFromPtr, 
					IDC_Polygon, 
					m_polygonFlag);
					
					                                 
	Point			location;
	
	
	location.h = 0;
	location.v = 0;
/*       
	((CMImageFrame*)AfxGetApp())->OnClearSelectionRectangle(); 

	if (m_polygonFlag)
		gStatisticsMode = kStatNewFieldPolyMode;
	else
		gStatisticsMode = kStatNewFieldRectMode; 
*/                
	                          
	StatisticsWControlEvent (
				gProjectInfoPtr->polygonTypeControlH, 
				FALSE, 
				location,
				kPolygonEnterControl);

}		// end "OnPolygon"

void CMStatisticsForm::OnSelect()

{                                                           
	Point			location;
	
	
	location.h = 0;
	location.v = 0;
	                          
	StatisticsWControlEvent (
				gProjectInfoPtr->newFieldControlH, 
				FALSE, 
				location,
				kNewFieldControl);
	
}		// end "OnSelect"



void CMStatisticsForm::OnList()
{                                                                                              
	Point			location;
	
	
	location.h = 0;
	location.v = 0;
	                          
	StatisticsWControlEvent (
				gProjectInfoPtr->listControlH, 
				FALSE, 
				location,
				kStatPrintControl);
	
}		// end "OnList"



void CMStatisticsForm::OnHistogram()
{
	// TODO: Add your control notification handler code here
	
}		// end "OnHistogram"



void CMStatisticsForm::OnSelchangeListBox()
{                      
	SInt16		cellIndex;
					                
	                                                          
	cellIndex = gStatisticsListHandle->GetCurSel ();
	
	if (cellIndex != LB_ERR)
		{		                                     
		if (gProjectInfoPtr->statsWindowMode == 2)
			{
			gProjectInfoPtr->currentClass = cellIndex;
			MHiliteControl (gProjectWindow, gProjectInfoPtr->toClassControlH, 0);
			MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 0);
					
			}		// end "if (gProjectInfoPtr->statsWindowMode == 2)" 
					
		if (gProjectInfoPtr->statsWindowMode == 3)
			{
			gProjectInfoPtr->currentField =
						GetCurrentField (gProjectInfoPtr->currentClass, cellIndex);
								
			if (gProjectInfoPtr->fieldIdentPtr[
					gProjectInfoPtr->currentField].pointType < kClusterType)
				MHiliteControl (gProjectWindow, gProjectInfoPtr->toFieldControlH, 0);
			else		// ...pointType >= kClusterType 
				MHiliteControl (gProjectWindow, gProjectInfoPtr->toFieldControlH, 255);
						
			MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 0);  
					
			}		// end "if (gProjectInfoPtr->statsWindowMode == 3)" 
					
		if (gProjectInfoPtr->statsWindowMode == 4)
			{                     
			gProjectInfoPtr->currentCoordinate = cellIndex;
			MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 0); 
					
			}		// end "if (gProjectInfoPtr->statsWindowMode == 4)"
			
		}		// end "if (cellIndex != LB_ERR)" 
		
	else		// cellIndex == LB_ERR  
		{		                   
		if (gProjectInfoPtr->statsWindowMode == 2)
			{
			gProjectInfoPtr->currentClass = -1;
			MHiliteControl (gProjectWindow, gProjectInfoPtr->toClassControlH, 255);
			MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 255);
					
			}		// end "if (gProjectInfoPtr->statsWindowMode == 2)" 
					
		if (gProjectInfoPtr->statsWindowMode == 3)
			{
			gProjectInfoPtr->currentField = -1;
			MHiliteControl (gProjectWindow, gProjectInfoPtr->toFieldControlH, 255);
			MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 255);
					
			}		// end "if (gProjectInfoPtr->statsWindowMode == 3)" 
					
		if (gProjectInfoPtr->statsWindowMode == 4)
			{
			gProjectInfoPtr->currentCoordinate = -1;
			if (gProjectInfoPtr->fieldIdentPtr[
						gProjectInfoPtr->currentField].pointType == kPolygonType)
				MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 255);
					
			}		// end "if (gProjectInfoPtr->statsWindowMode == 4)"
			
		}		// end "if (cellIndex != LB_ERR)"
	
}		// end "OnSelchangeListBox"



void CMStatisticsForm::OnDblclkListBox()
{  
	SInt16		cellIndex,
					savedStatsMode;
					
	                       
	savedStatsMode = gProjectInfoPtr->statsWindowMode;
	                                                          
	cellIndex = gStatisticsListHandle->GetCurSel ();
	
	if (cellIndex != LB_ERR)
		{		
		if (savedStatsMode == 2)
			{ 
//			FieldListStatMode (cellIndex);
			gProjectInfoPtr->currentClass = cellIndex;
			OnClass ();
			
			}		// end "if (savedStatsMode == 2)"
					
		else if (savedStatsMode == 3)  
//			PolygonListStatMode (cellIndex);
			OnField ();
			
		else if (savedStatsMode == 4)
			EditCoordinatesDialog(
							gProjectInfoPtr->currentClass,
							gProjectInfoPtr->currentField,
							gProjectInfoPtr->currentCoordinate);
			
		}		// end "if (cellIndex != LB_ERR)"
		
	else		// cellIndex == LB_ERR  
		{		                   
		if (savedStatsMode == 2)
			{
			gProjectInfoPtr->currentClass = -1;
			MHiliteControl (gProjectWindow, gProjectInfoPtr->toClassControlH, 255);
			MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 255);
					
			}		// end "if (savedStatsMode == 2)" 
					
		if (savedStatsMode == 3)
			{
			gProjectInfoPtr->currentField = -1;
			MHiliteControl (gProjectWindow, gProjectInfoPtr->toFieldControlH, 255);
			MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 255);
					
			}		// end "if (savedStatsMode == 3)" 
					
		if (savedStatsMode == 4)
			{
			gProjectInfoPtr->currentCoordinate = -1;
			if (gProjectInfoPtr->fieldIdentPtr[
						gProjectInfoPtr->currentField].pointType == kPolygonType)
				MHiliteControl (gProjectWindow, gProjectInfoPtr->editNameControlH, 255);
					
			}		// end "if (savedStatsMode == 4)"
			
		}		// end "if (cellIndex != LB_ERR)"
	
}		// end "OnDblclkListBox"

void CMStatisticsForm::OnEditName()
{                                                                                             
	Point			location;
	
	
	location.h = 0;
	location.v = 0;
	                          
	StatisticsWControlEvent (
				gProjectInfoPtr->editNameControlH, 
				FALSE, 
				location,
				kEditNameControl);
	
}		// end "OnEditName"
 


void CMStatisticsForm::OnSelendokHistogramStatsCombo()
{  
	DDX_CBIndex(m_dialogFromPtr, IDC_HistogramStatsCombo, m_histogramStatsCode);
	
	if (m_histogramStatsCode >= 0)
		{
				// Make 1 base.

		m_histogramStatsCode++;

		if (gProjectInfoPtr->statsWindowMode == 4)
			m_histogramStatsCode += 1;

		gProcessorCode = kHistogramStatsProcessor;
		HistogramStatsControl (gProjectInfoPtr->statsWindowMode, 
										m_histogramStatsCode); 
		gProcessorCode = 0;
		
		if (m_histogramStatsCode > 0)
			{
			m_histogramStatsCode = 0; 
			DDX_CBIndex(m_dialogToPtr, IDC_HistogramStatsCombo, m_histogramStatsCode);
			
			}		// end "if (m_histogramStatsCode > 0)"
		
		}		// end "if (m_histogramStatsCode >= 0)" 
	
}		// end "OnSelendokHistogramStatsCombo"
 


void CMStatisticsForm::OnSelendokListStatsCombo()
{  
	SInt16			listStatsCode = 1;
	
	                                                         
	DDX_CBIndex(m_dialogFromPtr, IDC_ListStatsCombo, m_listStatsCode);
	
	if (m_listStatsCode >= 0)
		{
		listStatsCode = m_listStatsCode + 1;
		
		if (gProjectInfoPtr->statsWindowMode == 4)
			listStatsCode += 2;
			
		gProcessorCode = kListStatsProcessor;
		ListStatsControl (gProjectInfoPtr->statsWindowMode, listStatsCode); 
		gProcessorCode = 0;
		
		if (m_listStatsCode > 0)
			{
			m_listStatsCode = 0; 
			DDX_CBIndex(m_dialogToPtr, IDC_ListStatsCombo, m_listStatsCode);
			
			}		// end "if (m_listStatsCode > 0)"
		
		}		// end "if (m_listStatsCode >= 0)" 
	
}		// end "OnSelendokListStatsCombo"



void CMStatisticsForm::UpdateHistogramStatsCombo()
{  
	CComboBox*				histogramStatsComboBoxPtr;
	
	
	if (gProjectInfoPtr->statsWindowMode > 1)
		{
		histogramStatsComboBoxPtr = (CComboBox*)GetDlgItem(IDC_HistogramStatsCombo);
		
				// Clear the current items.
				
		histogramStatsComboBoxPtr->ResetContent(); 
		
				// Load the new items depending on the current statistics window
				// mode.
			
		switch (gProjectInfoPtr->statsWindowMode)
			{                                                                           
			case 2:  
				histogramStatsComboBoxPtr->AddString(_T("Histogram Classes"));
				histogramStatsComboBoxPtr->AddString(_T("Histogram Fields"));
				histogramStatsComboBoxPtr->AddString(_T("Histogram..."));                                        
														
				break; 
				
			case 3:                                                   
				histogramStatsComboBoxPtr->AddString(_T("Histogram Class"));
				histogramStatsComboBoxPtr->AddString(_T("Histogram Fields"));
				histogramStatsComboBoxPtr->AddString(_T("Histogram..."));     
														
				break; 
				
			case 4:                                                
				histogramStatsComboBoxPtr->AddString(_T("Histogram Field"));
				histogramStatsComboBoxPtr->AddString(_T("Histogram..."));   
														
				break;
														
			}		// end "switch (gProjectInfoPtr->statsWindowMode)" 
			
		m_histogramStatsCode = 0; 
		DDX_CBIndex(m_dialogToPtr, IDC_HistogramStatsCombo, m_histogramStatsCode); 
		
		}		// end "if (gProjectInfoPtr->statsWindowMode > 1)"
	
}		// end "UpdateHistogramStatsCombo"  



void CMStatisticsForm::UpdateListStatsCombo()
{  
	CComboBox*				listStatsComboBoxPtr;
	
	
	if (gProjectInfoPtr->statsWindowMode > 1)
		{
		listStatsComboBoxPtr = (CComboBox*)GetDlgItem(IDC_ListStatsCombo);
		
				// Clear the current items.
				
		listStatsComboBoxPtr->ResetContent(); 
		
				// Load the new items depending on the current statistics window
				// mode.
			
		switch (gProjectInfoPtr->statsWindowMode)
			{                                                                           
			case 2:  
				listStatsComboBoxPtr->AddString(_T("List Classes Stats"));
				listStatsComboBoxPtr->AddString(_T("List Classes & Fields Stats"));
				listStatsComboBoxPtr->AddString(_T("List Fields Stats"));
				listStatsComboBoxPtr->AddString(_T("List Stats..."));                                        
														
				break; 
				
			case 3:                                                   
				listStatsComboBoxPtr->AddString(_T("List Class Stats"));
				listStatsComboBoxPtr->AddString(_T("List Class & Fields Stats"));
				listStatsComboBoxPtr->AddString(_T("List Fields Stats"));
				listStatsComboBoxPtr->AddString(_T("List Stats..."));     
														
				break; 
				
			case 4:                                                
				listStatsComboBoxPtr->AddString(_T("List Field Stats"));
				listStatsComboBoxPtr->AddString(_T("List Stats..."));   
														
				break;
														
			}		// end "switch (gProjectInfoPtr->statsWindowMode)" 
			
		m_listStatsCode = 0; 
		DDX_CBIndex(m_dialogToPtr, IDC_ListStatsCombo, m_listStatsCode); 
		
		}		// end "if (gProjectInfoPtr->statsWindowMode > 1)"
	
}		// end "UpdateListStatsCombo"  



void CMStatisticsForm::UpdateStatsTypeCombo(
				SInt16								statsWindowMode)

{  
	CComboBox*				statsTypeComboBoxPtr;
	
	SInt16					numberItems,
								classStorage;
								
	
			// Draw the prompt string and current covariance stats to be used.
	                                         
	statsTypeComboBoxPtr = (CComboBox*)GetDlgItem(IDC_StatsCombo);

	statsTypeComboBoxPtr->DeleteString(3);
	statsTypeComboBoxPtr->DeleteString(2);
	
	if (statsWindowMode == kClassListMode)
		{
		m_statsTypeCode = GetProjectStatisticsTypeMenuItem() - 1; 

		if (gProjectInfoPtr->enhancedStatsExistFlag)
			{
			statsTypeComboBoxPtr->AddString(_T("Enhanced"));
			statsTypeComboBoxPtr->SetItemData(2, kEnhancedStats);

			}		// end "if (gProjectInfoPtr->enhancedStatsExistFlag)" 

		if (m_statsTypeCode == 3)
			{
			statsTypeComboBoxPtr->AddString(_T("Mixed"));
			numberItems = statsTypeComboBoxPtr->GetCount();
			statsTypeComboBoxPtr->SetItemData(numberItems-1, kMixedStats);
			m_statsTypeCode = numberItems - 1;

			}		// end "if (gProjectInfoPtr->enhancedStatsExistFlag)"

		}		// end "if (statsWindowMode == kClassListMode)"

	else		// statsWindowMode == kFieldListMode
		{
		m_statsTypeCode = GetClassStatisticsTypeMenuItem() - 1;
	                                
		classStorage = gProjectInfoPtr->storageClass[gProjectInfoPtr->currentClass];
		if (gProjectInfoPtr->classNamesPtr[classStorage].modifiedStatsFlag)
			{
			statsTypeComboBoxPtr->AddString(_T("Enhanced"));
			statsTypeComboBoxPtr->SetItemData(2, kEnhancedStats); 
				
			}		// end "if (...modifiedStatsFlag)"

		}		// end "else statsWindowMode == kFieldListMode"
			               
	DDX_CBIndex(m_dialogToPtr, IDC_StatsCombo, m_statsTypeCode); 
	
}		// end "UpdateStatsTypeCombo"



void CMStatisticsForm::OnSelendokStatsCombo()
{  
//	SInt32								menuItemCode;

	SInt16								covarianceStatsToUse,
											savedStatsTypeCode;                           
	
	
	savedStatsTypeCode = m_statsTypeCode;
	DDX_CBIndex(m_dialogFromPtr, IDC_StatsCombo, m_statsTypeCode);

	if (m_statsTypeCode == 1 && m_optionKeyFlag)
		{
		if ( !LOOCOptionsDialog (gProjectInfoPtr->statsWindowMode) )
			m_statsTypeCode = savedStatsTypeCode;

		else		// LOOCOptionsDialog (...
			m_statsTypeCode = 1;

		}		// end "if (m_statsTypeCode == 1 && m_optionKeyFlag)"

	CComboBox* comboBoxPtr = (CComboBox*)GetDlgItem(IDC_StatsCombo);
	covarianceStatsToUse = (SInt16)comboBoxPtr->GetItemData(m_statsTypeCode);
	
	if (covarianceStatsToUse > 0)
		{
//		covarianceStatsToUse = GetCovarianceStatsFromMenuItem ((SInt16)menuItemCode);

		if (gProjectInfoPtr->statsWindowMode == kClassListMode)
			SetProjectCovarianceStatsToUse (covarianceStatsToUse);

		else		// gProjectInfoPtr->statsWindowMode == kFieldListMode
			SetClassCovarianceStatsToUse (covarianceStatsToUse);

		}		// end "if (covarianceStatsToUse > 0)"

	if (m_optionKeyFlag && covarianceStatsToUse == 2)
		{
		CComboBox* comboBoxPtr = (CComboBox*)GetDlgItem(IDC_StatsCombo);

		comboBoxPtr->DeleteString (1);
		comboBoxPtr->InsertString (1, _T("LOOC"));
		comboBoxPtr->SetItemData(1, kLeaveOneOutStats);

		}		// end "if (m_optionKeyFlag && ..."
	
			// Make sure that the drown down selection is drawn properly.

	DDX_CBIndex(m_dialogToPtr, IDC_StatsCombo, m_statsTypeCode);
	
}		// end "OnSelendokStatsCombo"



void CMStatisticsForm::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{                                                                  
	Boolean		charHandledFlag = FALSE;
	
	if (nChar == 0x08) 
		charHandledFlag = gProjectSelectionWindow->DoDeleteKey (nChar);
	
	CFormView::OnChar(nChar, nRepCnt, nFlags);
}

   

void CMStatisticsForm::OnDropdownStatsTypeCombo()
{
	SInt16			savedStatsTypeCode;


	savedStatsTypeCode = m_statsTypeCode;

	CComboBox* comboBoxPtr = (CComboBox*)GetDlgItem(IDC_StatsCombo);
	comboBoxPtr->DeleteString (1);
	
	if (GetKeyState (VK_RBUTTON) < 0)
		{  		
		comboBoxPtr->InsertString (1, _T("LOOC..."));
								
		m_optionKeyFlag = TRUE;
		
		}		// end "if (GetKeyState (VK_RBUTTON) < 0)"

	else		// GetKeyState (VK_RBUTTON) >= 0
		{  		
		comboBoxPtr->InsertString (1, _T("LOOC"));
								
		m_optionKeyFlag = FALSE;
		
		}		// end "if (GetKeyState (VK_RBUTTON) < 0)"

	comboBoxPtr->SetItemData(1, kLeaveOneOutStats);
	
			// Make sure that the drown down selection is drawn properly.

	DDX_CBIndex(m_dialogToPtr, IDC_StatsCombo, m_statsTypeCode);
	
}		// end "OnDropdownStatsTypeCombo"
