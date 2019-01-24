// LStatisticsView.cpp : implementation file
//      
// Revised by Larry Biehl on 01/16/2019
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
   //m_histogramStatsCode = 0;
   //m_listStatsCode = 0;
   //m_statsTypeCode = 0;
   //m_polygonFlag = FALSE;

   m_initializedFlag = FALSE;
   //m_optionKeyFlag = FALSE;
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
			width = 225,
			height = 505,
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
								wxSize (width, height),
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
