// LStatisticsView.cpp : implementation file
//      
// Revised by Larry Biehl on 11/15/2018
//
#include "SMultiSpec.h"

#include "LImageFrame.h"
#include "LMainFrame.h"
#include "LMultiSpec.h" 
#include "LStatisticsView.h"
#include "LStatisticsDoc.h"
#include "LStatisticsFrame.h"
	
#include "wx/wx.h"
#include "wx/docview.h"

typedef wxString CString;

extern void WriteProjectName(void);

extern SInt16 GetCovarianceStatsFromMenuItem(
   SInt16 menuItem);

extern SInt16 GetProjectStatisticsTypeMenuItem(void);

extern void GetProjectStatisticsTypeText(
   char* textStringPtr,
   SInt16 menuItem);

extern void FieldListStatMode(
   SInt16 classNumber);

extern SInt16 GetCurrentField(
   SInt16 classNumber,
   SInt16 classFieldNumber);

extern void PolygonListStatMode(
   SInt16 classFieldNumber);

/////////////////////////////////////////////////////////////////////////////
// CMStatisticsView

IMPLEMENT_DYNAMIC_CLASS(CMStatisticsView, wxView)

CMStatisticsView::CMStatisticsView()//: wxView(CMStatisticsView::IDD) 
{
   m_classList = 0;
   m_histogramStatsCode = 0;
   m_listStatsCode = 0;
   m_statsTypeCode = 0;
   m_polygonFlag = FALSE;

   m_initializedFlag = FALSE;
   m_optionKeyFlag = FALSE;
	m_initializedFlag = TRUE;

}	// end "CMStatisticsView"



CMStatisticsView::~CMStatisticsView ()

{
   gProjectWindow = NULL;

}


BEGIN_EVENT_TABLE(CMStatisticsView, wxView)
END_EVENT_TABLE()
/////////////////////////////////////////////////////////////////////////////
// CMStatisticsView message handlers  


bool CMStatisticsView::OnCreate (wxDocument *doc, long WXUNUSED(flags) )
{
   int	clientWidth,
			clientHeight,
			width = 190,
			height = 500,
			textWindowXPosition,
			textWindowYPosition;
			
	
	GetMainFrame()->GetClientSize (&clientWidth, &clientHeight);
	width = MIN (width, clientWidth);
	height = MIN (height, clientHeight);

	
	textWindowXPosition = clientWidth - width - 12;
	
	#ifdef NetBeansProject
		textWindowYPosition = 80;
	#else	// mygeohub
		textWindowYPosition = 80;
	#endif
   
   m_frame = new CMStatisticsFrame (
   							doc,
								this,
								GetMainFrame (),
								wxID_ANY,
								wxT("Project"),
								wxPoint (textWindowXPosition, textWindowYPosition),
								wxSize (width,height),
								#if defined multispec_wxmac
									(wxDEFAULT_FRAME_STYLE | wxFRAME_FLOAT_ON_PARENT) & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));
								#else
									(wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP) & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX));
								#endif
	
   SetFrame (m_frame);
	
   m_frame->SetSizeHints (width, height, width, height, width, height);//make non resizable
   m_frame->SetClientSize (width, height);
   m_frame->SetTitle (wxT("Project"));
   m_frame->SetSize (width, height);
   m_frame->SetSizeConstraint (-1,-1,width,height);
   
   ((CMStatisticsDoc*)doc)->SetStatisticsFrame ((CMStatisticsFrame*)m_frame);
	gTheActiveWindow = (WindowPtr)this;   
   m_frame->Show (true);
	Activate(true);
   
   OnInitialUpdate ();
   
   return true;
	
}	// end "OnCreate"



bool CMStatisticsView::OnClose (bool deleteWindow)
{
	if (!GetDocument()->Close())
		{
 		return false;
		}
	
	if ((CMStatisticsView*)gTheActiveWindow == this)
		gTheActiveWindow = NULL;
	
	if (gActiveWindowType == kProjectWindowType)
		gActiveWindowType = -1;
		
	gProjectWindow = NULL;
	Activate(false);
	SetFrame(NULL);
	return true;
	
	/*
   Boolean returnFlag = CloseTheProject();
	if(returnFlag == TRUE){
      m_frame->Destroy();
      return;
		}      
   else{
      event.Veto();
      return;
		}
	*/
	
}		// end "OnClose"


void CMStatisticsView::OnInitialUpdate (void)

{
   CMStatisticsDoc* documentPtr = GetDocument();
   documentPtr->SetStatisticsFrame((CMStatisticsFrame*)gProjectWindow);
   
	gProjectWindow = (CMImageView*)this;
	gStatisticsListHandle = (wxListBox*)m_frame->FindWindow(IDC_ListBox);

			// Add current list of classes to the combo box.

   CreateClassNameComboBoxList((wxComboBox*) m_frame->FindWindow(IDC_ClassList));

   MHiliteControl(gProjectWindow, gProjectInfoPtr->addToControlH, 1);

			// This control is no longer used.

   wxComboBox* classList = (wxComboBox*) m_frame->FindWindow(IDC_ClassList);
   classList->SetSelection(m_classList);

   wxComboBox* comboBoxPtr = (wxComboBox*) m_frame->FindWindow(IDC_StatsCombo);

   comboBoxPtr->SetClientData(0, (void*)kOriginalStats);
   comboBoxPtr->SetClientData(1, (void*)kLeaveOneOutStats);
   comboBoxPtr->SetClientData(2, (void*)kEnhancedStats);
   comboBoxPtr->SetClientData(3, (void*)kMixedStats);

   WriteProjectName();
	
}		// end "OnInitialUpdate"


void CMStatisticsView::OnDraw (
				CDC* pDC)
{
   CString tempCString;

   SInt16 classStorage;


//   wxView::OnDraw(pDC);

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

         m_frame->UpdateStatsTypeCombo(gProjectInfoPtr->statsWindowMode);
         break;

      case 3: // "Field List" mode 
         m_frame->UpdateStatsTypeCombo(gProjectInfoPtr->statsWindowMode);

      case 4: // "Polygon Point List" mode

         classStorage = gProjectInfoPtr->storageClass[gProjectInfoPtr->currentClass];

         tempCString =
            CString((char*) &gProjectInfoPtr->classNamesPtr[classStorage].name[1]);
         
         wxWindow* projWin = this->GetFrame();
         wxStaticText* classname = (wxStaticText*)projWin->FindWindow(IDC_ClassName);
         classname->SetLabel(tempCString);
         
         if (gProjectInfoPtr->statsWindowMode == 4) 
				{
            GetFieldNameWithType (gProjectInfoPtr->currentField, gTextString);
            tempCString = CString((char*) &gTextString[1]);
				wxStaticText* fieldname = (wxStaticText*)projWin->FindWindow(IDC_FieldName);
				fieldname->SetLabel(tempCString);

				} // end "if (gProjectInfoPtr->statsWindowMode == 4)"

         break;

   } // end "switch (gProjectInfoPtr->statsWindowMode)"  
 
} // end "OnDraw"



//void CMStatisticsView::OnDestroy() 
//{
//   CloseTheProject();

//}


CMStatisticsDoc* CMStatisticsView::GetDocument() 
{
   return (CMStatisticsDoc*) m_viewDocument;
}


// This function is called when we call wxView::Activate()
// Inherited from wxView
void CMStatisticsView::OnActivateView(
        bool				bActivate,
        wxView*			pActivateView,
        wxView*			pDeactiveView)
{
	if (m_frame != NULL)
		{
		/*
		int numberChars = sprintf ((char*)&gTextString3,
												" LStatView::OnActivateView: (gActiveImageWindowInfoH, bActivate, pActivateView, pDeactiveView): %ld, %ld, %ld, %ld%s", 
												gActiveImageWindowInfoH,
												bActivate,
												pActivateView,
												pDeactiveView,
												gEndOfLine);
		ListString ((char*)&gTextString3, numberChars, gOutputTextH);
		*/
		if (bActivate)
			wxASSERT(pActivateView == (wxView*)this);
		  
				// If the window is being activated, outside of a processing operating, make sure the global
				// active image information is up to date.
				
		if (gProcessorCode == 0 && bActivate) 
			{
			gActiveWindowType = kProjectWindowType;
			gTheActiveWindow = (WindowPtr)gProjectWindow;
			
         if(gProjectInfoPtr != NULL)
            m_frame->ActivateStatisticsWindowItems (gProjectInfoPtr->statsWindowMode);
			
			}		// end "if (gProcessorCode == 0 && bActivate)"	
		
		}		// end "if (m_frame != NULL)"

} // end "OnActivateView" 


#if 0
void
CMStatisticsView::OnAddToList(wxCommandEvent& event)
 {
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent(
      gProjectInfoPtr->addToControlH,
      FALSE,
      location,
      kAddToListControl);

} // end "OnAddToList"


void CMStatisticsView::OnSelendokClassList(wxCommandEvent& event)
{
   wxWindow* projWin = this->GetFrame();
   wxComboBox* classlist = (wxComboBox*)projWin->FindWindow(IDC_ClassList);
   m_classList = classlist->GetSelection();
   gProjectInfoPtr->lastClassForAddFieldSelection = m_classList + 1;

} // end "OnSelendokClassList"


void CMStatisticsView::OnProject(wxCommandEvent& event) {
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent(
      gProjectInfoPtr->toProjectControlH,
      FALSE,
      location,
      kToProjectControl);

   
} // end "OnProject"


void CMStatisticsView::OnClass(wxCommandEvent& event) 
{
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent(
      gProjectInfoPtr->toClassControlH,
      FALSE,
      location,
      kToClassControl);

   m_frame->UpdateStatsTypeCombo(kFieldListMode);

   m_frame->UpdateHistogramStatsCombo();

   m_frame->UpdateListStatsCombo();

  
   
} // end "OnClass"

void CMStatisticsView::OnField(wxCommandEvent& event) {
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent(
      gProjectInfoPtr->toFieldControlH,
      FALSE,
      location,
      kToFieldControl);

   m_frame->UpdateHistogramStatsCombo();

	m_frame->UpdateListStatsCombo();
   
} // end "OnField"
  
/*
void CMStatisticsView::OnUpdate(wxCommandEvent& event) {
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent(
      gProjectInfoPtr->updateControlH,
      FALSE,
      location,
      kUpdateStatsControl);
   
   
} // end "OnUpdate"
*/
//new

void CMStatisticsView::OnPolygon(wxCommandEvent& event)
 {
   wxWindow* projWin = this->GetFrame();
   wxCheckBox* polygonFlag = (wxCheckBox*)projWin->FindWindow(IDC_Polygon);
   m_polygonFlag = polygonFlag->GetValue();
   
   Point location;


   location.h = 0;
   location.v = 0;
          
   //   ((CMImageFrame*)AfxGetApp())->OnClearSelectionRectangle(); 
	//
    //  if (m_polygonFlag)
    //     gStatisticsMode = kStatNewFieldPolyMode;
    //  else
    //     gStatisticsMode = kStatNewFieldRectMode; 

   StatisticsWControlEvent(
      gProjectInfoPtr->polygonTypeControlH,
      FALSE,
      location,
      kPolygonEnterControl);
   
} // end "OnPolygon"


void CMStatisticsView::OnSelect(wxCommandEvent& event)
 {
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent(
      gProjectInfoPtr->newFieldControlH,
      FALSE,
      location,
      kNewFieldControl);

} // end "OnSelect"

void CMStatisticsView::OnList() {
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent(
      gProjectInfoPtr->listControlH,
      FALSE,
      location,
      kStatPrintControl);
   
} // end "OnList"


void CMStatisticsView::OnHistogram(wxCommandEvent& event) {
   // TODO: Add your control notification handler code here

} // end "OnHistogram"


void CMStatisticsView::OnSelchangeListBox(wxCommandEvent& event) {
   SInt16 cellIndex = -1;


//   cellIndex = gStatisticsListHandle->GetCurSel(); 
   cellIndex = gStatisticsListHandle->GetSelection();
   if (cellIndex >= 0) {
      if (gProjectInfoPtr->statsWindowMode == 2) {
         gProjectInfoPtr->currentClass = cellIndex;
         MHiliteControl(gProjectWindow, gProjectInfoPtr->toClassControlH, 0);
         MHiliteControl(gProjectWindow, gProjectInfoPtr->editNameControlH, 0);

      } // end "if (gProjectInfoPtr->statsWindowMode == 2)" 

      if (gProjectInfoPtr->statsWindowMode == 3) {
         gProjectInfoPtr->currentField =
            GetCurrentField(gProjectInfoPtr->currentClass, cellIndex);

         if (gProjectInfoPtr->fieldIdentPtr[
            gProjectInfoPtr->currentField].pointType < kClusterType)
            MHiliteControl(gProjectWindow, gProjectInfoPtr->toFieldControlH, 0);
         else // ...pointType >= kClusterType 
            MHiliteControl(gProjectWindow, gProjectInfoPtr->toFieldControlH, 255);

         MHiliteControl(gProjectWindow, gProjectInfoPtr->editNameControlH, 0);

      } // end "if (gProjectInfoPtr->statsWindowMode == 3)" 

      if (gProjectInfoPtr->statsWindowMode == 4) {
         gProjectInfoPtr->currentCoordinate = cellIndex;
         MHiliteControl(gProjectWindow, gProjectInfoPtr->editNameControlH, 0);

      } // end "if (gProjectInfoPtr->statsWindowMode == 4)"

   }// end "if (cellIndex != LB_ERR)" 

   else // cellIndex == LB_ERR  
   {
      if (gProjectInfoPtr->statsWindowMode == 2) {
         gProjectInfoPtr->currentClass = -1;
         MHiliteControl(gProjectWindow, gProjectInfoPtr->toClassControlH, 255);
         MHiliteControl(gProjectWindow, gProjectInfoPtr->editNameControlH, 255);

      } // end "if (gProjectInfoPtr->statsWindowMode == 2)" 

      if (gProjectInfoPtr->statsWindowMode == 3) {
         gProjectInfoPtr->currentField = -1;
         MHiliteControl(gProjectWindow, gProjectInfoPtr->toFieldControlH, 255);
         MHiliteControl(gProjectWindow, gProjectInfoPtr->editNameControlH, 255);

      } // end "if (gProjectInfoPtr->statsWindowMode == 3)" 

      if (gProjectInfoPtr->statsWindowMode == 4) {
         gProjectInfoPtr->currentCoordinate = -1;
         if (gProjectInfoPtr->fieldIdentPtr[
            gProjectInfoPtr->currentField].pointType == kPolygonType)
            MHiliteControl(gProjectWindow, gProjectInfoPtr->editNameControlH, 255);

      } // end "if (gProjectInfoPtr->statsWindowMode == 4)"

   } // end "if (cellIndex != LB_ERR)"

} // end "OnSelchangeListBox"

void CMStatisticsView::OnDblclkListBox(wxCommandEvent& event) {
   SInt16 cellIndex = -1,
      savedStatsMode;


   savedStatsMode = gProjectInfoPtr->statsWindowMode;
   cellIndex = gStatisticsListHandle->GetSelection();
//   cellIndex = gStatisticsListHandle->GetCurSel();

   if (cellIndex >= 0) { //temporary
      if (savedStatsMode == 2) {
         //			FieldListStatMode (cellIndex);
         gProjectInfoPtr->currentClass = cellIndex;
         OnClass(event);

      }// end "if (savedStatsMode == 2)"

      else if (savedStatsMode == 3)
         //			PolygonListStatMode (cellIndex);
         OnField(event);

      else if (savedStatsMode == 4)
         EditCoordinatesDialog(
         gProjectInfoPtr->currentClass,
         gProjectInfoPtr->currentField,
         gProjectInfoPtr->currentCoordinate);

   }// end "if (cellIndex != LB_ERR)"

   else // cellIndex == LB_ERR  
   {
      if (savedStatsMode == 2) {
         gProjectInfoPtr->currentClass = -1;
         MHiliteControl(gProjectWindow, gProjectInfoPtr->toClassControlH, 255);
         MHiliteControl(gProjectWindow, gProjectInfoPtr->editNameControlH, 255);

      } // end "if (savedStatsMode == 2)" 

      if (savedStatsMode == 3) {
         gProjectInfoPtr->currentField = -1;
         MHiliteControl(gProjectWindow, gProjectInfoPtr->toFieldControlH, 255);
         MHiliteControl(gProjectWindow, gProjectInfoPtr->editNameControlH, 255);

      } // end "if (savedStatsMode == 3)" 

      if (savedStatsMode == 4) {
         gProjectInfoPtr->currentCoordinate = -1;
         if (gProjectInfoPtr->fieldIdentPtr[
            gProjectInfoPtr->currentField].pointType == kPolygonType)
            MHiliteControl(gProjectWindow, gProjectInfoPtr->editNameControlH, 255);

      } // end "if (savedStatsMode == 4)"

   } // end "if (cellIndex != LB_ERR)"

} // end "OnDblclkListBox"


void CMStatisticsView::OnEditName(wxCommandEvent& event) {
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent(
      gProjectInfoPtr->editNameControlH,
      FALSE,
      location,
      kEditNameControl);

} // end "OnEditName"

void CMStatisticsView::OnHistogramStatsComboDropDown(wxCommandEvent& event) {
   wxComboBox* histogramStatsCode =  wxDynamicCast(((CMStatisticsView*) gProjectWindow)->GetFrame()->FindWindow(IDC_HistogramStatsCombo), wxComboBox);
   histogramStatsCode->SetSelection(-1);
   
}

void CMStatisticsView::OnHistogramStatsComboCloseUp(wxCommandEvent& event) {
   wxComboBox* histogramStatsCode =  wxDynamicCast(((CMStatisticsView*) gProjectWindow)->GetFrame()->FindWindow(IDC_HistogramStatsCombo), wxComboBox);
   if(histogramStatsCode->GetSelection() < 0)
      histogramStatsCode->SetSelection(0);   
}

void CMStatisticsView::OnSelendokHistogramStatsCombo(wxCommandEvent& event) {
   
//    wxWindow* projWin = this->GetFrame();
//   wxComboBox* histogramStatsCode= (wxComboBox*)projWin->FindWindow(IDC_HistogramStatsCombo);
   wxComboBox* histogramStatsCode =  wxDynamicCast(((CMStatisticsView*) gProjectWindow)->GetFrame()->FindWindow(IDC_HistogramStatsCombo), wxComboBox);
   m_histogramStatsCode = histogramStatsCode->GetSelection();
   
   if (m_histogramStatsCode >= 0) {
      // Make 1 base.

      m_histogramStatsCode++;

      if (gProjectInfoPtr->statsWindowMode == 4)
         m_histogramStatsCode += 1;

      gProcessorCode = kHistogramStatsProcessor;
      HistogramStatsControl(gProjectInfoPtr->statsWindowMode,
         m_histogramStatsCode);
      gProcessorCode = 0;

      if (m_histogramStatsCode > 0) {
         m_histogramStatsCode = 0;
         histogramStatsCode->SetSelection(m_histogramStatsCode);
      } // end "if (m_histogramStatsCode > 0)"

   } // end "if (m_histogramStatsCode >= 0)" 

} // end "OnSelendokHistogramStatsCombo"

void CMStatisticsView::OnListStatsComboDropDown(wxCommandEvent& event) {
   wxComboBox* listStatsCodePtr =  wxDynamicCast(((CMStatisticsView*) gProjectWindow)->GetFrame()->FindWindow(IDC_ListStatsCombo), wxComboBox);
   listStatsCodePtr->SetSelection(-1);
   
}

void CMStatisticsView::OnListStatsComboCloseUp(wxCommandEvent& event) {
   wxComboBox* listStatsCodePtr =  wxDynamicCast(((CMStatisticsView*) gProjectWindow)->GetFrame()->FindWindow(IDC_ListStatsCombo), wxComboBox);
   if(listStatsCodePtr->GetSelection() < 0)
      listStatsCodePtr->SetSelection(0);   
}

void CMStatisticsView::OnSelendokListStatsCombo(wxCommandEvent& event) {
   SInt16 listStatsCode = 1;
//   printf("triggered in EVT list stats combo\n");

   wxComboBox* listStatsCodePtr =  wxDynamicCast(((CMStatisticsView*) gProjectWindow)->GetFrame()->FindWindow(IDC_ListStatsCombo), wxComboBox);

//   wxComboBox* listStatsCodePtr= (wxComboBox*)projWin->FindWindow(IDC_ListStatsCombo);
   m_listStatsCode = listStatsCodePtr->GetSelection();
   
   if (m_listStatsCode >= 0) {
      listStatsCode = m_listStatsCode + 1;

      if (gProjectInfoPtr->statsWindowMode == 4)
         listStatsCode += 2;

      gProcessorCode = kListStatsProcessor;
      ListStatsControl(gProjectInfoPtr->statsWindowMode, listStatsCode);
      gProcessorCode = 0;

      if (m_listStatsCode > 0) {
         m_listStatsCode = 0;
         listStatsCodePtr->SetSelection(m_listStatsCode);
      } // end "if (m_listStatsCode > 0)"

   } // end "if (m_listStatsCode >= 0)" 

} // end "OnSelendokListStatsCombo"




void CMStatisticsView::OnSelendokStatsCombo(wxCommandEvent& event) {
   //	SInt32								menuItemCode;

   SInt16 covarianceStatsToUse,
      savedStatsTypeCode;


   savedStatsTypeCode = m_statsTypeCode;
//   wxWindow* projWin = this->GetFrame();
   wxComboBox* statsTypeComboPtr=  wxDynamicCast(((CMStatisticsView*) gProjectWindow)->GetFrame()->FindWindow(IDC_StatsCombo), wxComboBox);
//   wxComboBox* statsTypeCode= (wxComboBox*)projWin->FindWindow(IDC_StatsCombo);
   m_statsTypeCode = statsTypeComboPtr->GetSelection();
   
   if (m_statsTypeCode == 1 && m_optionKeyFlag) {
      if (!LOOCOptionsDialog(gProjectInfoPtr->statsWindowMode))
         m_statsTypeCode = savedStatsTypeCode;

      else // LOOCOptionsDialog (...
         m_statsTypeCode = 1;

   } // end "if (m_statsTypeCode == 1 && m_optionKeyFlag)"

    SInt64 statstype64 = (SInt64)((int*)statsTypeComboPtr->GetClientData(m_statsTypeCode));
    covarianceStatsToUse = (SInt16) statstype64;
    
   if (covarianceStatsToUse > 0) {
      //		covarianceStatsToUse = GetCovarianceStatsFromMenuItem ((SInt16)menuItemCode);

      if (gProjectInfoPtr->statsWindowMode == kClassListMode)
         SetProjectCovarianceStatsToUse(covarianceStatsToUse);

      else // gProjectInfoPtr->statsWindowMode == kFieldListMode
         SetClassCovarianceStatsToUse(covarianceStatsToUse);

   } // end "if (covarianceStatsToUse > 0)"

   if (m_optionKeyFlag && covarianceStatsToUse == 2)
		{

      statsTypeComboPtr->Delete(1);
      statsTypeComboPtr->Insert((char*) "LOOC",1);
      statsTypeComboPtr->SetClientData(1, (void*)kLeaveOneOutStats);

   } // end "if (m_optionKeyFlag && ..."

   // Make sure that the drown down selection is drawn properly.

   statsTypeComboPtr->SetSelection(m_statsTypeCode);
} // end "OnSelendokStatsCombo"



void CMStatisticsView::OnDropdownStatsTypeCombo(wxCommandEvent& event)

{
   SInt16 savedStatsTypeCode;

   savedStatsTypeCode = m_statsTypeCode;

} // end "OnDropdownStatsTypeCombo"


bool CMStatisticsView::TransferDataToWindow()
{
   return true;
}


bool CMStatisticsView::TransferDataFromWindow()
{
   return true;
}

#endif
