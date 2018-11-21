//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
// 								Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LClusterIsodataDialog.cpp : class implementation file
//	Class Definition:		LClusterIsodataDialog.h
//
//	Authors:					Abdur Rahman Maud, Larry L. Biehl
//
//	Revision date:			10/19/2018
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	This file contains functions that relate to the 
//								CMISODATAClusterDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "LClusterIsodataDialog.h"
#include "wx/valgen.h"
#include "wx/valnum.h"

#include "LImage_dialog.cpp"



IMPLEMENT_DYNAMIC_CLASS (CMISODATAClusterDialog, CMDialog)


CMISODATAClusterDialog::CMISODATAClusterDialog ()

{

}



CMISODATAClusterDialog::CMISODATAClusterDialog (
				wxWindow*							parent,
				wxWindowID							id,
				const wxString&					title)
		: CMDialog (CMISODATAClusterDialog::IDD, parent, title)

{
   m_initializationOption = -1;
   m_criticalDistance1 = 1;
   m_criticalDistance2 = 1;
   m_numberClusters = 1;
   m_minClusterSize = 1;
   m_convergence = 0;
   m_classSelection = -1;
   m_projectClassMeansFlag = FALSE;
	m_classListPtr = NULL;
   //}}AFX_DATA_INIT

   m_initializedFlag = CMDialog::m_initializedFlag;

   if (m_initializedFlag) {
      // Get memory for the local class list vector if it may be needed.	

      if (gProjectInfoPtr->numberStatisticsClasses > 0) {
         m_classListPtr = (UInt16*)MNewPointer (
                 (UInt32) gProjectInfoPtr->numberStatisticsClasses * sizeof (UInt16));

         m_initializedFlag = (m_classListPtr != NULL);

      } // end "if (...->numberStatisticsClasses > 0)"

   } // end "if (m_initializedFlag)"  
   
   CreateControls ();

}	// end "CMISODATAClusterDialog"



CMISODATAClusterDialog::~CMISODATAClusterDialog ()

{
   m_classListPtr = CheckAndDisposePtr (m_classListPtr);

}	// end "~CMISODATAClusterDialog"



BEGIN_EVENT_TABLE (CMISODATAClusterDialog, CMDialog)

EVT_INIT_DIALOG (CMISODATAClusterDialog::OnInitDialog)
	EVT_COMBOBOX (IDC_ClassCombo, CMISODATAClusterDialog::OnSelendokClassCombo)
	EVT_COMBOBOX_DROPDOWN (IDC_ClassCombo, CMISODATAClusterDialog::OnSelendokClassComboDropDown)
	EVT_RADIOBUTTON (IDC_ClusterTrainingAreas, CMISODATAClusterDialog::OnClusterTrainingAreas)
	EVT_RADIOBUTTON (IDC_ClusterImageArea, CMISODATAClusterDialog::OnClusterImageArea)
	EVT_RADIOBUTTON (IDC_1stCovEigenvector, CMISODATAClusterDialog::On1stCovEigenvector)
	EVT_RADIOBUTTON (IDC_EigenvectorVolume, CMISODATAClusterDialog::OnEigenvectorVolume)
	EVT_RADIOBUTTON (IDC_OnePassCluster, CMISODATAClusterDialog::OnOnePassCluster)
	EVT_BUTTON (IDEntireImage, CMISODATAClusterDialog::ToEntireImage)
	EVT_BUTTON (IDSelectedImage, CMISODATAClusterDialog::ToSelectedImage)
	EVT_CHECKBOX (IDC_ProjectClassMeans, CMISODATAClusterDialog::OnProjectClassMeans)
	EVT_TEXT (IDC_ColumnEnd, CMISODATAClusterDialog::CheckColumnEnd)
	EVT_TEXT (IDC_ColumnStart, CMISODATAClusterDialog::CheckColumnStart)
	EVT_TEXT (IDC_LineEnd, CMISODATAClusterDialog::CheckLineEnd)
	EVT_TEXT (IDC_LineStart, CMISODATAClusterDialog::CheckLineStart)
	EVT_TEXT (IDC_LineInterval, CMISODATAClusterDialog::CheckLineInterval)
	EVT_TEXT (IDC_ColumnInterval, CMISODATAClusterDialog::CheckColumnInterval)
END_EVENT_TABLE ()
 


void CMISODATAClusterDialog::ClustersFromSetting (
				bool									from,
				int									nIDC,
				int&									value)

{
   wxRadioButton                 *imgarea,
                                 *trainingarea;
   
   bool                          settingFlag;
   

   trainingarea = (wxRadioButton*)FindWindow (IDC_ClusterTrainingAreas);
   imgarea = (wxRadioButton*)FindWindow (IDC_ClusterImageArea);

   if (from)
      {
            // Reading from radio group. Get index ignoring the disabled items.

      value = -1;

      settingFlag = trainingarea->GetValue ();
      if (settingFlag)
         value = 0;

      settingFlag = imgarea->GetValue ();
      if (settingFlag)
         value = 1;

      }  // end "if (from)"

   else // !from
      {
      int localValue = value;

      bool savedTrainingAreaEnabledFlag;


      savedTrainingAreaEnabledFlag = trainingarea->IsEnabled ();

            // Setting specific radio group item. Take into account any disabled items.

      if (!savedTrainingAreaEnabledFlag)
         trainingarea->Enable (true);

      if (localValue == 0)
         trainingarea->SetValue (true);
      else
         imgarea->SetValue (true);
      
      if (!savedTrainingAreaEnabledFlag)
         trainingarea->Enable (false);

      }	// end "else !pDX->m_bSaveAndValidate"

}	// end "ClustersFromSetting"



void CMISODATAClusterDialog::CreateControls ()

{
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);
   /*
   wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA (entireim);
   wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA (toentire);
   wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA (selected);
   wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA (bmp4);
	*/
   //wxBoxSizer* bVSizer1;
   //bVSizer1 = new wxBoxSizer (wxVERTICAL);
	/*
   m_dialogPanel = new wxPanel (this,
   										wxID_ANY,
   										wxDefaultPosition,
   										wxDefaultSize,
   										wxTAB_TRAVERSAL,
   										wxT("ClusterDialog"));
	*/
   wxBoxSizer* bSizer45;
   bSizer45 = new wxBoxSizer (wxVERTICAL);
	
   wxWindow* parent = this;
   //wxWindow* parent = m_dialogPanel;

   wxBoxSizer* bSizer46;
   bSizer46 = new wxBoxSizer (wxHORIZONTAL);

   wxStaticBoxSizer* sbSizer3;
   sbSizer3 = new wxStaticBoxSizer (new wxStaticBox (parent,
                                                     wxID_ANY,
                                                     wxT("Initialization Options")),
                                    wxVERTICAL);

   m_radioBtn5 = new wxRadioButton (sbSizer3->GetStaticBox (),
                                    IDC_1stCovEigenvector,
                                    wxT("Along first eigenvector"),
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    wxRB_GROUP);
   SetUpToolTip (m_radioBtn5, IDS_ToolTip97);
   sbSizer3->Add (m_radioBtn5, wxSizerFlags(0).Border(wxALL, 5));

   m_radioBtn6 = new wxRadioButton (sbSizer3->GetStaticBox (),
                                    IDC_EigenvectorVolume,
                                    wxT("Within eigenvector volume"),
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    0);
   SetUpToolTip (m_radioBtn6, IDS_ToolTip98);
   sbSizer3->Add (m_radioBtn6, wxSizerFlags(0).Border(wxALL, 5));

   m_radioBtn7 = new wxRadioButton (sbSizer3->GetStaticBox (),
                                    IDC_OnePassCluster,
                                    wxT("Use single-pass clusters"),
                                    wxDefaultPosition,
                                    wxDefaultSize,
                                    0);
   SetUpToolTip (m_radioBtn7, IDS_ToolTip99);
   sbSizer3->Add (m_radioBtn7, wxSizerFlags(0).Border(wxALL, 5));

   m_checkBox2 = new wxCheckBox (sbSizer3->GetStaticBox (),
                                 IDC_ProjectClassMeans,
                                 wxT("Include project class means"),
                                 wxDefaultPosition,
                                 wxDefaultSize,
                                 0);
   SetUpToolTip (m_checkBox2, IDS_ToolTip100);
   sbSizer3->Add (m_checkBox2,
   					wxSizerFlags(0).Align(wxALIGN_CENTER_HORIZONTAL).Border(wxALL, 5));

	bSizer46->Add (sbSizer3, wxSizerFlags(1).Expand().Border (wxRIGHT|wxBOTTOM, 5));

   wxStaticBoxSizer* sbSizer4;
   sbSizer4 = new wxStaticBoxSizer (new wxStaticBox (parent,
                                                     wxID_ANY,
                                                     wxT("Other Options"),
                                                     wxDefaultPosition,
                                                     wxDefaultSize,
                                                     wxTAB_TRAVERSAL),
                                    wxVERTICAL);

   wxBoxSizer* bSizer47;
   bSizer47 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText33 = new wxStaticText (sbSizer4->GetStaticBox (),
                                      IDC_NumberClustersPrompt,
                                      wxT("Number Clusters: "),
                                      wxDefaultPosition,
                                      wxSize (140, -1),
                                      0);
   m_staticText33->Wrap (-1);
   SetUpToolTip (m_staticText33, IDS_ToolTip101);
   m_staticText33->SetFont (wxFont (wxNORMAL_FONT->GetPointSize (),
												wxFONTFAMILY_MODERN,
												wxFONTSTYLE_NORMAL,
												wxFONTWEIGHT_NORMAL,
												false,
												wxEmptyString));
   bSizer47->Add (m_staticText33, wxSizerFlags(0).Border(wxALL, 5));

   m_textCtrl16 = new wxTextCtrl (sbSizer4->GetStaticBox (),
                                  IDC_NumberClusters,
                                  wxEmptyString,
                                  wxDefaultPosition,
                                  wxDefaultSize,
                                  0);
   m_textCtrl16->SetValidator (
								wxTextValidator (wxFILTER_DIGITS, &m_numberClustersString));
   SetUpToolTip (m_textCtrl16, IDS_ToolTip101);
   bSizer47->Add (m_textCtrl16, wxSizerFlags(0).Border(wxALL, 5));

   sbSizer4->Add (bSizer47, wxSizerFlags(1).Expand());

   wxBoxSizer* bSizer48;
   bSizer48 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText34 = new wxStaticText (sbSizer4->GetStaticBox (),
												  IDC_ConvergencePrompt,
												  wxT("Convergence (%): "),
												  wxDefaultPosition,
												  wxSize (140, -1),
												  0);
   m_staticText34->Wrap (-1);
   SetUpToolTip (m_staticText34, IDS_ToolTip102);
	//wxFloatingPointValidator<double> _val (1, &m_convergence);
	//_val.SetRange (0.0, 100.);
	//m_staticText34->SetValidator (_val);
   bSizer48->Add (m_staticText34, wxSizerFlags(0).Border(wxALL, 5));

   m_textCtrl17 = new wxTextCtrl (sbSizer4->GetStaticBox (),
											 IDC_Convergence,
											 wxEmptyString,
											 wxDefaultPosition,
											 wxDefaultSize,
											 0);
   m_textCtrl17->SetValidator (
									wxTextValidator (wxFILTER_NUMERIC, &m_convergenceString));
	wxFloatingPointValidator<double> _val (1, &m_convergence);
	_val.SetRange (0.0, 100.);
	m_textCtrl17->SetValidator (_val);
   SetUpToolTip (m_textCtrl17, IDS_ToolTip102);
   bSizer48->Add (m_textCtrl17, wxSizerFlags(0).Border(wxALL, 5));

   sbSizer4->Add (bSizer48, wxSizerFlags(1).Expand());

   wxBoxSizer* bSizer50;
   bSizer50 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText35 = new wxStaticText (sbSizer4->GetStaticBox (),
												  IDC_MinClusterSizePrompt,
												  wxT("Minimum Cluster size: "),
												  wxDefaultPosition,
												  wxSize (140, -1),
												  0);
   m_staticText35->Wrap (-1);
   SetUpToolTip (m_staticText35, IDS_ToolTip91);
   bSizer50->Add (m_staticText35, wxSizerFlags(0).Border(wxALL, 5));

   m_textCtrl18 = new wxTextCtrl (sbSizer4->GetStaticBox (),
											 IDC_MinClusterSize,
											 wxEmptyString,
											 wxDefaultPosition,
											 wxDefaultSize,
											 0);
   m_textCtrl18->SetValidator (
								wxTextValidator (wxFILTER_DIGITS, &m_minClusterSizeString));
   SetUpToolTip (m_textCtrl18, IDS_ToolTip91);
   bSizer50->Add (m_textCtrl18, wxSizerFlags(0).Border(wxALL, 5));

   sbSizer4->Add (bSizer50, wxSizerFlags(1).Expand());

   wxBoxSizer* bSizer51;
   bSizer51 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText36 = new wxStaticText (sbSizer4->GetStaticBox (),
												  IDC_Distance1Prompt,
												  wxT("Distance 1 (first line): "),
												  wxDefaultPosition,
												  wxSize (140, -1),
												  0);
   m_staticText36->Wrap (-1);
   SetUpToolTip (m_staticText36, IDS_ToolTip92);
   bSizer51->Add (m_staticText36, wxSizerFlags(0).Border(wxALL, 5));

   m_textCtrl19 = new wxTextCtrl (sbSizer4->GetStaticBox (),
											 IDC_Distance1,
											 wxEmptyString,
											 wxDefaultPosition,
											 wxDefaultSize,
											 0);
   m_textCtrl19->SetValidator (
									wxTextValidator (wxFILTER_NUMERIC, &m_distance1String));
   SetUpToolTip (m_textCtrl19, IDS_ToolTip92);
   bSizer51->Add (m_textCtrl19, wxSizerFlags(0).Border(wxALL, 5));

   sbSizer4->Add (bSizer51, wxSizerFlags(1).Expand());

   wxBoxSizer* bSizer52;
   bSizer52 = new wxBoxSizer (wxHORIZONTAL);

   m_staticText37 = new wxStaticText (sbSizer4->GetStaticBox (),
												  IDC_Distance2Prompt,
												  wxT("Distance 2 (other line): "),
												  wxDefaultPosition,
												  wxSize (140, -1),
												  0);
   m_staticText37->Wrap (-1);
   SetUpToolTip (m_staticText37, IDS_ToolTip93);
   bSizer52->Add (m_staticText37, wxSizerFlags(0).Border(wxALL, 5));

   m_textCtrl20 = new wxTextCtrl (sbSizer4->GetStaticBox (),
											 IDC_Distance2,
											 wxEmptyString,
											 wxDefaultPosition,
											 wxDefaultSize,
											 0);
   m_textCtrl20->SetValidator (
									wxTextValidator (wxFILTER_NUMERIC, &m_distance2String));
   SetUpToolTip (m_textCtrl20, IDS_ToolTip93);
   bSizer52->Add (m_textCtrl20, wxSizerFlags(0).Border(wxALL, 5));

   sbSizer4->Add (bSizer52, wxSizerFlags(1).Expand());

	bSizer46->Add (sbSizer4, wxSizerFlags(1).Expand().Border(wxLEFT|wxBOTTOM, 5));

	bSizer45->Add (bSizer46, wxSizerFlags(0).Expand().Border(wxLEFT|wxTOP|wxRIGHT, 12));

   wxBoxSizer* bSizer53;
   bSizer53 = new wxBoxSizer (wxHORIZONTAL);

   wxStaticBoxSizer* sbSizer5;
   sbSizer5 = new wxStaticBoxSizer (new wxStaticBox (parent,
																	  wxID_ANY,
																	  wxT("Determine Clusters from"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
												wxVERTICAL);

   wxBoxSizer* bSizer66;
   bSizer66 = new wxBoxSizer (wxHORIZONTAL);

   m_radioBtn8 = new wxRadioButton (sbSizer5->GetStaticBox (),
												IDC_ClusterTrainingAreas,
												wxT("Training Area(s)"),
												wxDefaultPosition,
												wxDefaultSize,
												wxRB_GROUP);
   SetUpToolTip (m_radioBtn8, IDS_ToolTip94);
	bSizer66->Add (m_radioBtn8,
						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   bSizer66->Add (0, 0, 0, wxEXPAND);

   m_staticText50 = new wxStaticText (sbSizer5->GetStaticBox (),
												  IDC_ClassPrompt, wxT("Classes:"),
												  wxDefaultPosition,
												  wxDefaultSize,
												  0);
   m_staticText50->Wrap (-1);
   SetUpToolTip (m_staticText50, IDS_ToolTip103);
   bSizer66->Add (m_staticText50,
   					wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   m_comboBox4 = new wxComboBox (sbSizer5->GetStaticBox (),
											IDC_ClassCombo,
											wxT("Combo!"),
											wxDefaultPosition,
											wxDefaultSize,
											0,
											NULL,
											wxCB_READONLY);
   m_comboBox4->Append ("All");
	m_comboBox4->Append ("Subset...");
   m_comboBox4->SetSelection (0);
   SetUpToolTip (m_comboBox4, IDS_ToolTip103);
	bSizer66->Add (m_comboBox4, wxSizerFlags(1).Border(wxALL, 5));

   sbSizer5->Add (bSizer66, wxSizerFlags(0).Expand());

   m_radioBtn9 = new wxRadioButton (sbSizer5->GetStaticBox (),
												IDC_ClusterImageArea,
												wxT("Image Area"),
												wxDefaultPosition,
												wxDefaultSize,
												0);
   SetUpToolTip (m_radioBtn9, IDS_ToolTip95);
   sbSizer5->Add (m_radioBtn9, wxSizerFlags(0).Border(wxALL, 5));

   wxStaticBoxSizer* sbSizer8;
   sbSizer8 = new wxStaticBoxSizer (new wxStaticBox (sbSizer5->GetStaticBox (),
																		IDC_LineColFrame,
																		wxT("Area to Cluster"),
																		wxDefaultPosition,
																		wxDefaultSize,
																		wxTAB_TRAVERSAL),
												wxHORIZONTAL);
	/*
   wxBoxSizer* bSizer791;
   bSizer791 = new wxBoxSizer (wxHORIZONTAL);

   m_bpButton4 = new wxBitmapButton (sbSizer8->GetStaticBox (),
												 IDEntireImage,
												 entireimi,
												 wxDefaultPosition,
												 wxDefaultSize,
												 wxBU_AUTODRAW);
   SetUpToolTip (m_bpButton4, IDS_ToolTip40);
   m_bpButton4->SetBitmapDisabled (toentirei);
   bSizer791->Add (m_bpButton4,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   m_bpButton5 = new wxBitmapButton (sbSizer8->GetStaticBox (),
												 IDSelectedImage,
												 selectedi,
												 wxDefaultPosition,
												 wxDefaultSize,
												 wxBU_AUTODRAW);
   m_bpButton5->Hide ();
   m_bpButton5->SetBitmapDisabled (bmp4i);
   bSizer791->Add (m_bpButton5,
   						wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

   sbSizer8->Add (bSizer791, wxSizerFlags(0).Expand());
	*/
	CreateLineColumnControls (sbSizer8);
	
	sbSizer5->Add (sbSizer8, wxSizerFlags(1).Border(wxLEFT|wxTOP|wxRIGHT, 6));

	bSizer53->Add (sbSizer5, wxSizerFlags(0).Border(wxTOP|wxRIGHT, 6));
	/*
   wxBoxSizer* bSizer54;
   bSizer54 = new wxBoxSizer (wxVERTICAL);

   bSizer54->Add (0, 0, 1, wxEXPAND);

   m_button17 = new wxButton (this,
										wxID_CANCEL,
										wxT("Cancel"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
	bSizer54->Add (m_button17, wxSizerFlags(0).Right().Border(wxLEFT|wxBOTTOM, 6));

   m_button18 = new wxButton (this,
										wxID_OK,
										wxT("OK"),
										wxDefaultPosition,
										wxDefaultSize,
										0);
	bSizer54->Add (m_button18, wxSizerFlags(0).Right().Border(wxLEFT, 6));

	bSizer53->Add (bSizer54, wxSizerFlags(0).Expand());
	*/
	bSizer45->Add (bSizer53,
						wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 12));
	
	//m_dialogPanel->SetSizer (bSizer45);
	
	//bVSizer1->Add (m_dialogPanel, wxSizerFlags(0));
	
	//wxSizer* standardButtonSizer = CreateButtonSizer (wxOK | wxCANCEL);
	//bSizer45->Add (standardButtonSizer, wxSizerFlags(0).Right());
	//bVSizer1->Add (standardButtonSizer, wxSizerFlags(0).Right());
	
	CreateStandardButtons (bSizer45);

   SetSizerAndFit (bSizer45);
   //SetSizerAndFit (bVSizer1);
   //this->Layout ();
	
			// Set focus and tab order
	
   SelectDialogItemText (this, IDC_NumberClusters, 0, SInt16_MAX);
	
	//m_textCtrl17->MoveAfterInTabOrder (m_textCtrl16);
	//m_textCtrl18->MoveAfterInTabOrder (m_textCtrl17);
	
   //m_linestartctrl->MoveAfterInTabOrder (m_textCtrl18);
	
}	// end "CreateControls"



//------------------------------------------------------------------------------------
//								 Copyright (1988-2018)
//								c Purdue Research Foundation
//									All rights reserved.
//
//	Function name:		void DoDialog
//
//	Software purpose:	The purpose of this routine is to present the cluster
//							specification dialog box to the user and copy the
//							revised back to the display specification structure if
//							the user selected OK.
//
//	Parameters in:		None
//
//	Parameters out:	None
//
//	Value Returned:	None		
// 
//	Called By:			Dialog in MDisMult.cpp
//
//	Coded By:			Larry L. Biehl			Date: 08/02/2009
//	Revised By:			Larry L. Biehl			Date: 08/02/1996	

SInt16 CMISODATAClusterDialog::DoDialog (void)

{
	Boolean								continueFlag = FALSE;

   SInt16								clustersFrom,
											returnCode;


			// Make sure intialization has been completed.

	if (!m_initializedFlag)
																							return (FALSE);

   returnCode = ShowModal ();

   if (returnCode == wxID_OK)
		{
      continueFlag = TRUE;

      clustersFrom = kTrainingType;
      if (m_clustersFrom == 1)
         clustersFrom = kAreaType;

				// Cluster area

      m_dialogSelectArea.lineStart = m_LineStart;
      m_dialogSelectArea.lineEnd = m_LineEnd;
      m_dialogSelectArea.lineInterval = m_LineInterval;

      m_dialogSelectArea.columnStart = m_ColumnStart;
      m_dialogSelectArea.columnEnd = m_ColumnEnd;
      m_dialogSelectArea.columnInterval = m_ColumnInterval;

      ISODATAClusterDialogOK ((SInt16)(m_initializationOption + 1),
										  FALSE,
										  m_projectClassMeansFlag,
										  clustersFrom,
										  (SInt16) m_classSelection,
										  m_classListPtr,
										  m_localNumberClasses,
										  &m_dialogSelectArea,
										  (SInt16) m_numberClusters,
										  m_convergence,
										  (SInt16) m_minClusterSize,
										  m_criticalDistance1,
										  m_criticalDistance2);

		}	// end "if (returnCode == IDOK)"

   return (continueFlag);

}	// end "DoDialog"



int CMISODATAClusterDialog::GetRadioSelection ()

{
   int retval = 0;
   wxRadioButton* along1st = (wxRadioButton*)FindWindow (IDC_1stCovEigenvector);
   wxRadioButton* eigvol = (wxRadioButton*)FindWindow (IDC_EigenvectorVolume);
   wxRadioButton* sngpass = (wxRadioButton*)FindWindow (IDC_OnePassCluster);
   
   if (along1st->GetValue ())
      retval = 0;
	
   else if (eigvol->GetValue ())
      retval = 1;
	
   else
      retval = 2;
	
   return retval;
      
}	// end "GetRadioSelection"



void CMISODATAClusterDialog::On1stCovEigenvector (
				wxCommandEvent&					event)

{
   UpdateOptionSettings ();

}	// end "On1stCovEigenvector"


/*
void CMISODATAClusterDialog::OnButtonPress (
				wxKeyEvent&							event)

{
	if (event.GetKeyCode () == WXK_RETURN)
		{
		if (Validate () && TransferDataFromWindow ())
			EndModal (wxID_OK);
		} 
		
	else 
		{
		event.Skip ();
		}
 
}	// end "OnButtonPress"
*/


void CMISODATAClusterDialog::OnClusterTrainingAreas (
				wxCommandEvent&					event)

{
   ISODATAClusterDialogOnTrainingAreas (this);

}	// end "OnClusterTrainingAreas"



void CMISODATAClusterDialog::OnClusterImageArea (
				wxCommandEvent&					event)

{
   ISODATAClusterDialogOnImageArea (this);

   SetEntireImageButtons (
           NULL,
           m_LineStart,
           m_LineEnd,
           m_ColumnStart,
           m_ColumnEnd);

}	// end "OnClusterImageArea"



void CMISODATAClusterDialog::OnEigenvectorVolume (
				wxCommandEvent&					event)

{
   UpdateOptionSettings ();

}	// end "OnEigenvectorVolume"



void CMISODATAClusterDialog::OnInitDialog (
				wxInitDialogEvent&				event)

{
   SInt16 classSelection,
           clustersFrom,
           initializationOption,
           projectClassMeansCode;
   UInt16  decimaldigits;

   Boolean savedUseCorrelationMatrixFlag;


   ISODATAClusterDialogInitialize (this,
											  m_classListPtr,
											  &initializationOption,
											  &savedUseCorrelationMatrixFlag,
											  &projectClassMeansCode,
											  &clustersFrom,
											  &classSelection,
											  &m_localNumberClasses,
											  &m_dialogSelectArea,
											  &m_numberClusters,
											  &m_convergence,
											  &m_minClusterSize,
											  &m_criticalDistance1,
											  &m_criticalDistance2,
											  &decimaldigits);

			// Adjust those options that need to be.
			// Initialization option.

   m_initializationOption = initializationOption - 1;

			// User project class means flag.

   m_projectClassMeansFlag = FALSE;
   if (projectClassMeansCode == 1)
		m_projectClassMeansFlag = TRUE;

			// Cluster areas from training areas or a selected area.

   m_clustersFrom = 0;
   if (clustersFrom == kAreaType)
      m_clustersFrom = 1;

			// All class training areas or a subset.

   m_classSelection = classSelection;

			// Selected area to cluster.

   m_LineStart = m_dialogSelectArea.lineStart;
   m_LineEnd = m_dialogSelectArea.lineEnd;
   m_LineInterval = m_dialogSelectArea.lineInterval;
   m_ColumnStart = m_dialogSelectArea.columnStart;
   m_ColumnEnd = m_dialogSelectArea.columnEnd;
   m_ColumnInterval = m_dialogSelectArea.columnInterval;

   if (TransferDataToWindow ())
      PositionDialogWindow ();

			// Set default text selection to first edit text item

   UInt16 defaultTextIndex;
   if (gClusterSpecsPtr->initializationOption != 4)
      defaultTextIndex = IDC_NumberClusters;

   else	// gClusterSpecsPtr->initializationOption == 4
      defaultTextIndex = IDC_Convergence;

   SelectDialogItemText (this, defaultTextIndex, 0, SInt16_MAX);
   this->Fit ();

}	// end "OnInitDialog"



void CMISODATAClusterDialog::OnOnePassCluster (
				wxCommandEvent&					event)

{
   UpdateOptionSettings ();

}	// end "OnOnePassCluster"



void CMISODATAClusterDialog::OnProjectClassMeans (
				wxCommandEvent&					event)

{
   wxCheckBox* projmean = (wxCheckBox*)FindWindow (IDC_ProjectClassMeans);
   m_projectClassMeansFlag = projmean->GetValue ();

}	// end "OnProjectClassMeans"



void CMISODATAClusterDialog::OnSelendokClassCombo (
				wxCommandEvent&					event)

{
   HandleClassesMenu (&m_localNumberClasses,
           (SInt16*)m_classListPtr,
           1,
           (SInt16) gProjectInfoPtr->numberStatisticsClasses,
           IDC_ClassCombo,
           &m_classSelection);

}	// end "OnSelendokClassCombo"



void CMISODATAClusterDialog::SetRadioSelection (
				int									setval)

{
   wxRadioButton* along1st = (wxRadioButton*)FindWindow (IDC_1stCovEigenvector);
   wxRadioButton* eigvol = (wxRadioButton*)FindWindow (IDC_EigenvectorVolume);
   wxRadioButton* sngpass = (wxRadioButton*)FindWindow (IDC_OnePassCluster);
   along1st->SetValue (false);
   eigvol->SetValue (false);
   sngpass->SetValue (false);
   
   if (setval == 0)
      along1st->SetValue (true);

   else if (setval == 1)
      eigvol->SetValue (true);

   else
      sngpass->SetValue (true);
	
}	// end "SetRadioSelection"



bool CMISODATAClusterDialog::TransferDataFromWindow ()

{
	SInt16								continueCode,
											returnCode;
	
	
	returnCode = 0;
	
   wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
   wxTextCtrl* c_inter = (wxTextCtrl*)FindWindow (IDC_ColumnInterval);
   wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
   wxTextCtrl* l_inter = (wxTextCtrl*)FindWindow (IDC_LineInterval);
   wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
   wxTextCtrl* dist1 = (wxTextCtrl*)FindWindow (IDC_Distance1);
   wxTextCtrl* dist2 = (wxTextCtrl*)FindWindow (IDC_Distance2);
   wxTextCtrl* nclusters = (wxTextCtrl*)FindWindow (IDC_NumberClusters);
   wxTextCtrl* minclustersz = (wxTextCtrl*)FindWindow (IDC_MinClusterSize);
   wxTextCtrl* convergence = (wxTextCtrl*)FindWindow (IDC_Convergence);
   wxCheckBox* projmean = (wxCheckBox*)FindWindow (IDC_ProjectClassMeans);

   m_LineStartString = l_start->GetValue ();
   m_LineEndString = l_end->GetValue ();
   m_LineIntervalString = l_inter->GetValue ();
   m_ColumnStartString = c_start->GetValue ();
   m_ColumnEndString = c_end->GetValue ();
   m_ColumnIntervalString = c_inter->GetValue ();
   
   m_LineInterval = wxAtoi (m_LineIntervalString);
   m_ColumnInterval = wxAtoi (m_ColumnIntervalString);
   wxString crdist1 = (dist1->GetValue ());
   crdist1.ToDouble (&m_criticalDistance1);
   wxString crdist2 = (dist2->GetValue ());
   crdist2.ToDouble (&m_criticalDistance2);
   //m_criticalDistance2 = dist2->GetValue ();
	
	m_numberClustersString = nclusters->GetValue ();
   m_numberClusters = wxAtoi (m_numberClustersString);
	
   m_minClusterSizeString = minclustersz->GetValue ();
   m_minClusterSize = wxAtoi (m_minClusterSizeString);
	
   wxString convg = (convergence->GetValue ());
   convg.ToDouble (&m_convergence);
   //m_convergence = convergence->GetValue ();
	
   m_projectClassMeansFlag = projmean->GetValue ();
   
   m_initializationOption = GetRadioSelection ();
   
   ClustersFromSetting (true, IDC_ClusterTrainingAreas, m_clustersFrom);
	
	if (m_initializationOption != 2)
		{
		continueCode = CheckMaxValue (this,
													IDC_NumberClusters,
													1,
													kMaxNumberStatClasses,
													kDisplayRangeAlert);
		if (continueCode != 1)
			returnCode = IDC_NumberClusters;
		
		}	// end "if (m_initializationOption != 2)"
								
	if (returnCode == 0)							
		returnCode = CheckDialogRealValue (this,
														IDC_Convergence,
														0,
														kMaxThreshold,
														1,
														false,	// minValue not allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);
	
	if (returnCode == 0)
		{
		continueCode = CheckMaxValue (this,
													IDC_MinClusterSize,
													1,
													SInt32_MAX,
													kDisplayRangeAlert);
		if (continueCode != 1)
			returnCode = IDC_MinClusterSize;
		
		}	// end "if (returnCode == 0)"
						
	if (returnCode == 0 && m_initializationOption == 2)						
		returnCode = CheckDialogRealValue (this,
														IDC_Distance1,
														0,
														UInt32_MAX,
														0,
														false,	// minValue not allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);
						
	if (returnCode == 0 && m_initializationOption == 2)						
		returnCode = CheckDialogRealValue (this,
														IDC_Distance2,
														0,
														UInt32_MAX,
														0,
														false,	// minValue not allowed.
														true,		// maxValue is allowed,
														kDisplayRangeAlert);

	if (returnCode == 0 && m_clustersFrom == 1)
		returnCode = VerifyLineColumnValues (
							  IDC_LineStart,
							  IDC_ColumnStart,
							  true);
   
   return (returnCode == 0);
	
}	// end "TransferDataFroomWindow"



bool CMISODATAClusterDialog::TransferDataToWindow ()

{
   wxTextCtrl* c_end = (wxTextCtrl*)FindWindow (IDC_ColumnEnd);
   wxTextCtrl* c_inter = (wxTextCtrl*)FindWindow (IDC_ColumnInterval);
   wxTextCtrl* c_start = (wxTextCtrl*)FindWindow (IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*)FindWindow (IDC_LineEnd);
   wxTextCtrl* l_inter = (wxTextCtrl*)FindWindow (IDC_LineInterval);
   wxTextCtrl* l_start = (wxTextCtrl*)FindWindow (IDC_LineStart);
   wxTextCtrl* dist1 = (wxTextCtrl*)FindWindow (IDC_Distance1);
   wxTextCtrl* dist2 = (wxTextCtrl*)FindWindow (IDC_Distance2);
   wxTextCtrl* nclusters = (wxTextCtrl*)FindWindow (IDC_NumberClusters);
   wxTextCtrl* minclustersz = (wxTextCtrl*)FindWindow (IDC_MinClusterSize);
   wxTextCtrl* convergence = (wxTextCtrl*)FindWindow (IDC_Convergence);
   wxCheckBox* projmean = (wxCheckBox*)FindWindow (IDC_ProjectClassMeans);

   c_end->ChangeValue (wxString::Format (wxT("%i"), m_ColumnEnd));
   c_inter->ChangeValue (wxString::Format (wxT("%i"), m_ColumnInterval));
   c_start->ChangeValue (wxString::Format (wxT("%i"), m_ColumnStart));
   l_end->ChangeValue (wxString::Format (wxT("%i"), m_LineEnd));
   l_inter->ChangeValue (wxString::Format (wxT("%i"), m_LineInterval));
   l_start->ChangeValue (wxString::Format (wxT("%i"), m_LineStart));
   dist1->ChangeValue (wxString::Format (wxT("%.1f"), m_criticalDistance1));
   dist2->ChangeValue (wxString::Format (wxT("%.1f"), m_criticalDistance2));
   nclusters->ChangeValue (wxString::Format (wxT("%i"), m_numberClusters));
   minclustersz->ChangeValue (wxString::Format (wxT("%i"), m_minClusterSize));
   convergence->ChangeValue (wxString::Format (wxT("%.1f"), m_convergence));
   projmean->SetValue (m_projectClassMeansFlag);
   
   SetRadioSelection (m_initializationOption);
   ClustersFromSetting (false, IDC_ClusterTrainingAreas, m_clustersFrom);
   UpdateOptionSettings ();
   if	(m_clustersFrom == 1)
		SetEntireImageButtons (NULL,
										m_LineStart,
										m_LineEnd,
										m_ColumnStart,
										m_ColumnEnd);
	
   return true;
	
}	// end "TransferDataToWindow"
        


void CMISODATAClusterDialog::UpdateOptionSettings (void)

{
   wxCheckBox* projmean = (wxCheckBox*)FindWindow (IDC_ProjectClassMeans);
   m_initializationOption = GetRadioSelection ();

   if (m_initializationOption != 2)
		{
      MShowDialogItem (this, IDC_NumberClustersPrompt);
      MShowDialogItem (this, IDC_NumberClusters);

      MHideDialogItem (this, IDC_Distance1Prompt);
      MHideDialogItem (this, IDC_Distance1);
      MHideDialogItem (this, IDC_Distance2Prompt);
      MHideDialogItem (this, IDC_Distance2);

      projmean->SetValue (m_projectClassMeansFlag);
      SetDLogControlHilite (this, IDC_ProjectClassMeans, 0);
		
   	SelectDialogItemText (this, IDC_NumberClusters, 0, SInt16_MAX);

		}	// end "if (m_initializationOption != 2)"

   else // m_initializationOption == 2)
		{
      MHideDialogItem (this, IDC_NumberClustersPrompt);
      MHideDialogItem (this, IDC_NumberClusters);

      MShowDialogItem (this, IDC_Distance1Prompt);
      MShowDialogItem (this, IDC_Distance1);
      MShowDialogItem (this, IDC_Distance2Prompt);
      MShowDialogItem (this, IDC_Distance2);

      projmean->SetValue (m_projectClassMeansFlag);
      SetDLogControlHilite (this, IDC_ProjectClassMeans, 255);
		
   	SelectDialogItemText (this, IDC_Convergence, 0, SInt16_MAX);

		}	// end "else m_initializationOption == 2"
	
   this->Fit ();

}	// end "UpdateOptionSettings"
