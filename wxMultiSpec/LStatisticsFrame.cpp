// LStatisticsFrame.cpp : implementation file
//
// Revised by Larry Biehl on 01/16/2019
//
/*  Template for writing something to text window for debugging.             
	int numberChars = sprintf ((char*)&gTextString3,
												" LStatFrm::UpdateStatsTypeCombo 1: (count): %d%s", 
												statsTypeComboBoxPtr->GetCount(),
												gEndOfLine);
	ListString ((char*)&gTextString3, numberChars, gOutputTextH);	
*/   
                       
#include "SMultiSpec.h"
                      
#include "LImageView.h"
#include "LImageDoc.h"
#include "LStatisticsFrame.h"
#include "LStatisticsDoc.h"
#include "LTools.h"

#include "wx/display.h"
	
extern SInt16 GetCurrentField(
   SInt16 classNumber,
   SInt16 classFieldNumber);


IMPLEMENT_DYNAMIC_CLASS(CMStatisticsFrame, wxDocChildFrame)
//IMPLEMENT_DYNCREATE(CMStatisticsFrame, CFrameWnd)

CMStatisticsFrame::CMStatisticsFrame (void)

{
	//gProjectWindow = (CMImageView*)this;

}	// end "CMStatisticsFrame"



CMStatisticsFrame::CMStatisticsFrame(wxDocument* doc, wxView* view, wxDocParentFrame* parent, 
            wxWindowID id, const wxString& title, 
				const wxPoint& pos, const wxSize& size, long style) : 
		wxDocChildFrame (doc, view, parent, id, title, pos, size, style)

{
	//gProjectWindow = (CMImageView*)this;
   m_classList = 0;
   m_histogramStatsCode = 0;
   m_listStatsCode = 0;
   m_statsTypeCode = 0;
   m_polygonFlag = FALSE;

   m_initializedFlag = FALSE;
   m_optionKeyFlag = FALSE;
   
   CreateControls2();
	
   		// Change statistic dialog window to the top-right screen -- Tsung Tai 12/10/18
	
   int			menuHeight = 0,
   				menuWidth = 0,
   				windowHeight = 0,
   				windowWidth = 0,
   				xPosition,
   				yPosition;
	
   GetSize (&windowWidth, &windowHeight);
   xPosition = wxDisplay().GetGeometry().GetWidth() - windowWidth - 3;
   xPosition = MAX (3, xPosition);
   GetMainFrame()->m_menubar1->GetSize (&menuWidth, &menuHeight);
   yPosition = menuHeight + 3;
   Move (xPosition, yPosition);
	
			// Disable the polygon enter checkbox for now until implemented.
			
	wxAcceleratorEntry entries[32];
	entries[0].Set(wxACCEL_CTRL, (int) 'O', ID_IMAGE_OPEN); 
	entries[1].Set(wxACCEL_CTRL, (int) ';', ID_FILE_OPEN_PROJECT);
	entries[2].Set(wxACCEL_CTRL, (int) 'W', ID_FILE_CLOSE_WINDOW);
	entries[3].Set(wxACCEL_CTRL, (int) 'S', ID_FILE_SAVE_AS);
	entries[4].Set(wxACCEL_CTRL, (int) 'P', ID_FILE_PRINT);
	entries[5].Set(wxACCEL_CTRL, (int) 'Q', wxID_EXIT);
	
	entries[6].Set(wxACCEL_CTRL, (int) 'Z', wxID_UNDO);
	entries[7].Set(wxACCEL_CTRL, (int) 'X', wxID_CUT);
	entries[8].Set(wxACCEL_CTRL, (int) 'C', wxID_COPY);
	entries[9].Set(wxACCEL_CTRL, (int) 'V', wxID_PASTE);
	entries[10].Set(wxACCEL_NORMAL, (int) WXK_DELETE, wxID_CLEAR);
	entries[11].Set(wxACCEL_CTRL, (int) 'A', ID_EDIT_SELECT_ALL);
	entries[12].Set(wxACCEL_CTRL, (int) ',', ID_EDIT_IMAGE_DESCRIPTION);
	entries[13].Set(wxACCEL_CTRL, (int) '`', ID_EDIT_IMAGE_MAP_PARAMETERS);
	
	entries[14].Set(wxACCEL_CTRL, (int) 'D', ID_PROC_DISPLAY_IMAGE);
	entries[15].Set(wxACCEL_CTRL, (int) 'H', ID_PROC_HISTOGRAM_IMAGE);
	entries[16].Set(wxACCEL_CTRL, (int) 'J', ID_PROC_LISTDATA);
	entries[17].Set(wxACCEL_CTRL, (int) 'R', ID_PROC_REFORMAT_CHANGE_IMAGE);
	entries[18].Set(wxACCEL_CTRL, (int) 'L', ID_PROC_CLUSTER);
	entries[19].Set(wxACCEL_CTRL, (int) 'T', ID_PROC_STATISTICS);
	entries[20].Set(wxACCEL_CTRL, (int) 'E', ID_PROC_ENHANCE_STATISTICS);	
	entries[21].Set(wxACCEL_CTRL, (int) 'F', ID_PROC_FEATURE_EXTRACTION);	
	entries[22].Set(wxACCEL_CTRL, (int) 'B', ID_PROC_FEATURE_SELECTION);	
	entries[23].Set(wxACCEL_CTRL, (int) 'M', ID_PROC_CLASSIFY);	
	entries[24].Set(wxACCEL_CTRL, (int) 'Y', ID_PROC_LISTRESULTS);	
	entries[25].Set(wxACCEL_CTRL, (int) 'E', ID_PROC_ENHANCE_STATISTICS);	
	
	entries[26].Set(wxACCEL_CTRL, (int) 'K', ID_PROC_UTIL_PRIN_COMP_ANALYSIS);
	entries[27].Set(wxACCEL_CTRL, (int) 'I', ID_PROC_UTIL_CREATE_STAT_IMAGE);
	entries[28].Set(wxACCEL_CTRL, (int) 'G', ID_PROC_UTIL_BIPLOTS_OF_DATA);
	entries[29].Set(wxACCEL_CTRL, (int) 'U', ID_PROC_UTIL_LIST_IMAGE_DESC);
	entries[30].Set(wxACCEL_CTRL, (int) '[', ID_PROC_UTIL_CHECK_COVARIANCES);
	entries[31].Set(wxACCEL_CTRL, (int) ']', ID_PROC_UTIL_CHECKTRANS_MATRIX);	
	wxAcceleratorTable accel(32, entries);
	SetAcceleratorTable (accel);

}


CMStatisticsFrame::~CMStatisticsFrame(void)
{
//   gProjectWindow = NULL;

}		// end "~CMStatisticsFrame" 


BEGIN_EVENT_TABLE(CMStatisticsFrame, wxDocChildFrame)
	EVT_UPDATE_UI(ID_EDIT_CLEAR_SELECT_RECTANGLE, CMStatisticsFrame::OnUpdateEditClearSelectRectangle)
   EVT_UPDATE_UI(ID_EDIT_SELECTION_RECTANGLE, CMStatisticsFrame::OnUpdateEditSelectionRectangle)
   EVT_UPDATE_UI(ID_EDIT_SELECT_ALL, CMStatisticsFrame::OnUpdateEditSelectAll)
	
	EVT_MENU(ID_EDIT_CLEAR_SELECT_RECTANGLE, CMStatisticsFrame::OnEditClearSelectRectangle)	
	EVT_MENU(ID_EDIT_SELECTION_RECTANGLE, CMStatisticsFrame::OnEditSelectionRectangle)	
	EVT_MENU(ID_EDIT_SELECT_ALL, CMStatisticsFrame::OnEditSelectAll)	

   EVT_BUTTON(IDC_Project, CMStatisticsFrame::OnProject)
   EVT_BUTTON(IDC_Class, CMStatisticsFrame::OnClass)
   EVT_BUTTON(IDC_Field, CMStatisticsFrame::OnField)
   EVT_BUTTON(IDC_Update, CMStatisticsFrame::OnUpdate)
   EVT_CHECKBOX(IDC_Polygon, CMStatisticsFrame::OnPolygon)
   EVT_BUTTON(IDC_Select, CMStatisticsFrame::OnSelect)
   EVT_BUTTON(IDC_AddToList, CMStatisticsFrame::OnAddToList)
   EVT_BUTTON(IDC_EditName, CMStatisticsFrame::OnEditName)
	EVT_CHAR_HOOK (CMStatisticsFrame::OnCharHook)
   EVT_COMBOBOX(IDC_ClassList, CMStatisticsFrame::OnSelendokClassList)
   EVT_COMBOBOX(IDC_ListStatsCombo, CMStatisticsFrame::OnSelendokListStatsCombo)
   EVT_COMBOBOX_DROPDOWN(IDC_ListStatsCombo, CMStatisticsFrame::OnListStatsComboDropDown)
   EVT_COMBOBOX(IDC_HistogramStatsCombo, CMStatisticsFrame::OnSelendokHistogramStatsCombo)
   EVT_COMBOBOX(IDC_StatsCombo, CMStatisticsFrame::OnSelendokStatsCombo)
   EVT_COMBOBOX_DROPDOWN(IDC_HistogramStatsCombo, CMStatisticsFrame::OnHistogramStatsComboDropDown)
   EVT_COMBOBOX_DROPDOWN(IDC_StatsCombo, CMStatisticsFrame::OnDropdownStatsTypeCombo)
   EVT_LISTBOX(IDC_ListBox, CMStatisticsFrame::OnSelchangeListBox)
   EVT_LISTBOX_DCLICK(IDC_ListBox, CMStatisticsFrame::OnDblclkListBox)
END_EVENT_TABLE()


void CMStatisticsFrame::ActivateStatisticsWindowItems (
				SInt16										statsWindowMode)
{			
	switch (statsWindowMode)
		{
		case kSelectFieldMode:
			m_comboBox40->SetFocus();
			break;
			
		case kClassListMode:
			m_comboBox42->SetFocus();
			break;
			
		case kFieldListMode:
			m_comboBox42->SetFocus();
			break;
			
		case kCoordinateListMode:
			m_comboBox42->SetFocus();
			break;
			
		}		// end "switch (statsWindowMode)"

}		// end "ActivateStatisticsWindowItems"


void CMStatisticsFrame::CreateControls2()

{
	int	buttonWidth = 210;
	
	
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
	wxFont  font (gFontSize, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxBoxSizer* bSizer233;
	bSizer233 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer234;
	bSizer234 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText223 = new wxStaticText( this, IDC_ProjectName, wxT("Project Name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText223->Wrap( -1 );
	bSizer234->Add( m_staticText223, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText224 = new wxStaticText( this, IDC_ProjectWindowMode, wxT("SELECT FIELD"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText224->Wrap( -1 );
	bSizer234->Add( m_staticText224, 0, wxALIGN_CENTER, 5 );
	
	
	bSizer233->Add( bSizer234, 0, wxEXPAND, 3);
	
	wxFlexGridSizer* fgSizer8;
	fgSizer8 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer8->SetFlexibleDirection( wxBOTH );
	fgSizer8->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* bSizer235 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText225 = new wxStaticText (this,
													IDC_STATIC_Class,
													wxT("Class:"),
													wxDefaultPosition,
													wxDefaultSize,
													0 );
	m_staticText225->Wrap( -1 );
	//bSizer235->Add( m_staticText225, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxBOTTOM|wxTOP|wxLEFT, 5 );
   bSizer235->Add (m_staticText225,
   							wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxBOTTOM|wxTOP|wxLEFT, 5));
	
	m_staticText228 = new wxStaticText( this, IDC_ClassName, wxT("Class 1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText228->Wrap( -1 );
	//bSizer235->Add( m_staticText228, 0, wxALIGN_CENTER|wxBOTTOM|wxTOP, 5 );
   bSizer235->Add (m_staticText228, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxBOTTOM|wxTOP, 5));
	
	m_comboBox40 = new wxComboBox( this, IDC_ClassList, wxT("Combo!"), wxDefaultPosition, wxSize(125,25), 0, NULL, 0 );
   SetUpToolTip(m_comboBox40, IDS_ToolTip1);
	//bSizer235->Add( m_comboBox40, 0, wxALIGN_CENTER|wxALIGN_RIGHT|wxEXPAND, 5 );
	bSizer235->Add( m_comboBox40, wxSizerFlags(0).Align(wxALIGN_CENTER));
	
	m_staticText260 = new wxStaticText (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText260->Wrap( -1 );
	//bSizer235->Add( m_staticText260, 0, wxALIGN_CENTER|wxBOTTOM|wxTOP, 5 );
   bSizer235->Add( m_staticText260, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxBOTTOM|wxTOP, 5));
	
	fgSizer8->Add( bSizer235, 0, wxEXPAND|wxLEFT|wxRIGHT, 6);
	
	wxBoxSizer* bSizer243 = new wxBoxSizer (wxHORIZONTAL);
	
	m_staticText230 = new wxStaticText( this, IDC_STATIC_Field, wxT("Field:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText230->Wrap( -1 );
	//bSizer243->Add( m_staticText230, 0, wxALIGN_CENTER|wxBOTTOM|wxTOP|wxLEFT, 5 );
   bSizer243->Add (m_staticText230, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxBOTTOM|wxTOP|wxLEFT, 5));
	
	m_staticText231 = new wxStaticText( this, IDC_FieldName, wxT("Field 1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText231->Wrap( -1 );
	//bSizer243->Add( m_staticText231, 0, wxALIGN_CENTER|wxBOTTOM|wxTOP, 5 );
	bSizer243->Add (m_staticText231, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxBOTTOM|wxTOP, 5));
	
	m_staticText259 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText259->Wrap( -1 );
	//bSizer243->Add( m_staticText259, 0, wxALIGN_CENTER|wxBOTTOM|wxTOP, 5 );
	bSizer243->Add (m_staticText259, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxBOTTOM|wxTOP, 5));
	
	fgSizer8->Add (bSizer243, 0, wxEXPAND|wxLEFT|wxRIGHT, 6);
	
	bSizer233->Add (fgSizer8, 0, wxEXPAND, 6);
	
	wxFlexGridSizer* fgSizer12;
	fgSizer12 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer12->SetFlexibleDirection( wxBOTH );
	fgSizer12->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED);
	
	wxBoxSizer* bSizer281;
	bSizer281 = new wxBoxSizer( wxHORIZONTAL);
	
	m_checkBox53 = new wxCheckBox( this, IDC_Polygon, wxT("Polygon Enter"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox53, IDS_ToolTip2);
	bSizer281->Add( m_checkBox53, 0, wxALIGN_CENTER|wxLEFT, 5 );
	
	m_staticText261 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText261->Wrap( -1 );
	bSizer281->Add( m_staticText261, 0, wxALIGN_CENTER|wxALL, 5 );
	
	
	fgSizer12->Add( bSizer281, 0, wxEXPAND|wxLEFT|wxRIGHT, 6);
	
	m_button56 = new wxButton (this,
										IDC_EditName,
										wxT("Edit Class Name ..."),
										wxDefaultPosition,
										wxSize (buttonWidth, 25), 0);
   SetUpToolTip(m_button56, IDS_ToolTip3);
   /*
	m_button56->SetFont (wxFont (gFontSize,
											wxFONTFAMILY_DEFAULT,
											wxFONTSTYLE_NORMAL,
											wxFONTWEIGHT_NORMAL,
											false,
											wxEmptyString) );
	*/
	//fgSizer12->Add( m_button56, 0, wxALIGN_CENTER|wxEXPAND|wxLEFT|wxRIGHT, 5 );
	//fgSizer12->Add (m_button56, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxLEFT|wxRIGHT, 5));
	fgSizer12->Add (m_button56, wxSizerFlags(0).Expand().Border(wxLEFT|wxRIGHT, 5));
	
	//bSizer233->Add( fgSizer12, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 3);
	bSizer233->Add (fgSizer12, wxSizerFlags(0).Border(wxBOTTOM|wxLEFT|wxRIGHT, 3));
	
	wxFlexGridSizer* fgSizer13;
	fgSizer13 = new wxFlexGridSizer( 1, 1, 0, 0 );
	fgSizer13->SetFlexibleDirection( wxBOTH );
	fgSizer13->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxBoxSizer* bSizer236;
	bSizer236 = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText226 = new wxStaticText( this, IDC_StatsPrompt, wxT("Stats:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText226->Wrap( -1 );
	bSizer236->Add( m_staticText226, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_staticText227 = new wxStaticText( this, IDC_StatsType, wxT("Original"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText227->Wrap( -1 );
	bSizer236->Add( m_staticText227, 0, wxALIGN_CENTER|wxALL, 5 );
	
	m_comboBox41 = new wxComboBox( this, IDC_StatsCombo, wxT("Combo!"), wxDefaultPosition, wxSize(-1,25), 0, NULL, wxCB_READONLY );
	m_comboBox41->Append( wxT("Original") );
	m_comboBox41->Append( wxT("LOOC") );
	m_comboBox41->Append( wxT("Enhanced") );
	m_comboBox41->Append( wxT("Mixed") );
   SetUpToolTip(m_comboBox41, IDS_ToolTip4);
	//bSizer236->Add( m_comboBox41, 0, wxALIGN_CENTER|wxEXPAND, 5 );
   //bSizer236->Add( m_comboBox41, 0, wxEXPAND);
   bSizer236->Add (m_comboBox41, 0, wxALIGN_CENTER);
	
	m_staticText262 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText262->Wrap( -1 );
	bSizer236->Add( m_staticText262, 0, wxALIGN_CENTER|wxALL, 5 );
	
	fgSizer13->Add( bSizer236, 0, wxEXPAND|wxLEFT|wxRIGHT, 5 );
	
	bSizer233->Add (fgSizer13, 0, wxEXPAND);
	
	wxFlexGridSizer* fgSizer14;
	fgSizer14 = new wxFlexGridSizer (3, 1, 0, 0);
	fgSizer14->SetFlexibleDirection (wxBOTH);
	fgSizer14->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	wxBoxSizer* bSizer282 = new wxBoxSizer (wxHORIZONTAL);
	m_comboBox42 = new wxComboBox (this,
												IDC_HistogramStatsCombo,
												wxT ("Combo!"),
												wxDefaultPosition,
												wxSize (buttonWidth, 25),
												0,
												NULL,
												wxCB_READONLY);
	m_comboBox42->Append (wxT("Histogram Classes"));
	m_comboBox42->Append (wxT("Histrgram Fields"));
	m_comboBox42->Append (wxT("Histogram..."));
   SetUpToolTip (m_comboBox42, IDS_ToolTip5);
   bSizer282->Add (
   		m_comboBox42,
			wxSizerFlags(0).ReserveSpaceEvenIfHidden().Expand().Border(wxBOTTOM, 5));
	
	//m_staticText263 = new wxStaticText (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	//m_staticText263->Wrap (-1);
   //bSizer282->Add (m_staticText263, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));
	
   //fgSizer14->Add (bSizer282, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxTOP|wxLEFT|wxRIGHT, 5));
   fgSizer14->Add (bSizer282, wxSizerFlags(0).Expand().Border(wxTOP|wxLEFT|wxRIGHT, 5));
	
	wxBoxSizer* bSizer283 = new wxBoxSizer (wxHORIZONTAL);
	m_comboBox43 = new wxComboBox (this,
												IDC_ListStatsCombo,
												wxT("Combo!"),
												wxDefaultPosition,
												wxSize (buttonWidth, 25),
												0,
												NULL,
												wxCB_READONLY);
	m_comboBox43->Append( wxT("List Classes Stats") );
	m_comboBox43->Append( wxT("List Classes & Fields Stats") );
	m_comboBox43->Append( wxT("List Fields Stats") );
	m_comboBox43->Append( wxT("List Stats...") );
   SetUpToolTip(m_comboBox43, IDS_ToolTip6);
   bSizer283->Add (m_comboBox43, wxSizerFlags(0).ReserveSpaceEvenIfHidden().Expand());
	
	//m_staticText264 = new wxStaticText (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	//m_staticText264->Wrap (-1);
   //bSizer283->Add (m_staticText264, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxTOP|wxBOTTOM, 5));
	
   //fgSizer14->Add (bSizer283, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxTOP|wxLEFT|wxRIGHT, 5));
   fgSizer14->Add (bSizer283, wxSizerFlags(0).Expand().Border(wxTOP|wxLEFT|wxRIGHT, 5));
	
	wxBoxSizer* bSizer238;
	bSizer238 = new wxBoxSizer( wxHORIZONTAL );
	
	m_button61 = new wxButton (this,
										IDC_AddToList,
										wxT("Add To List..."),
										wxDefaultPosition,
										wxSize (buttonWidth, 25),
										0);
   SetUpToolTip(m_button61, IDS_ToolTip7);
	//bSizer238->Add( m_button61, 0, wxALIGN_CENTER|wxEXPAND|wxALL, 5 );
	bSizer238->Add (m_button61, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));
	
	m_button60 = new wxButton (this,
										IDC_Update,
										wxT("Update Project Stats"),
										wxDefaultPosition,
										wxSize (buttonWidth, 25),
										0);
   SetUpToolTip(m_button60, IDS_ToolTip8);
	//bSizer238->Add( m_button60, 0, wxALIGN_CENTER|wxEXPAND|wxALL, 5 );
   bSizer238->Add (m_button60, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxALL, 5));
	
	fgSizer14->Add( bSizer238, 0, wxEXPAND, 5 );
	
	//bSizer233->Add( fgSizer14, 0, wxALIGN_CENTER|wxEXPAND|wxLEFT|wxRIGHT, 3);
	bSizer233->Add (fgSizer14, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxLEFT|wxRIGHT, 3));
	
	wxFlexGridSizer* fgSizer15 = new wxFlexGridSizer (2, 2, 0, 0 );
	fgSizer15->SetFlexibleDirection (wxBOTH);
	fgSizer15->SetNonFlexibleGrowMode (wxFLEX_GROWMODE_SPECIFIED);
	
	m_button62 = new wxButton (this,
										IDC_Project,
										wxT(">Classes"),
										wxDefaultPosition,
										wxSize (90, -1),
										0 )
										;
   SetUpToolTip(m_button62, IDS_ToolTip9);
	//fgSizer15->Add( m_button62, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5 );
   fgSizer15->Add (m_button62, wxSizerFlags(0).Border(wxLEFT|wxRIGHT, 5));
	
	m_button63 = new wxButton (this,
										IDC_Class,
										wxT(">Fields"),
										wxDefaultPosition,
										wxSize (105, -1),
										0);
   SetUpToolTip(m_button63, IDS_ToolTip10);
	//fgSizer15->Add( m_button63, 0, wxALIGN_RIGHT|wxLEFT|wxRIGHT, 5 );
   fgSizer15->Add (m_button63, wxSizerFlags(0).Border(wxLEFT|wxRIGHT, 5));
	
	m_button64 = new wxButton (this,
										IDC_Select,
										wxT(">Select"),
										wxDefaultPosition,
										wxSize (90,-1),
										0);
   SetUpToolTip(m_button64, IDS_ToolTip11);
	//fgSizer15->Add( m_button64, 0, wxALIGN_LEFT|wxRIGHT|wxTOP|wxLEFT, 5 );
   fgSizer15->Add (m_button64, wxSizerFlags(0).Border(wxRIGHT|wxTOP|wxLEFT, 5));
	
	m_button65 = new wxButton (this,
										IDC_Field,
										wxT(">Coordinates"),
										wxDefaultPosition,
										wxSize (105, -1),
										0);
   SetUpToolTip(m_button65, IDS_ToolTip12);
	//fgSizer15->Add( m_button65, 0, wxALIGN_RIGHT|wxLEFT|wxTOP|wxRIGHT, 5 );
	fgSizer15->Add (m_button65, wxSizerFlags(0).Border(wxLEFT|wxTOP|wxRIGHT, 5));
	
	//bSizer233->Add( fgSizer15, 0, wxALIGN_CENTER|wxEXPAND|wxLEFT|wxRIGHT, 3);
	bSizer233->Add (fgSizer15, wxSizerFlags(0).Border(wxLEFT|wxRIGHT, 3));
	
	wxBoxSizer* bSizer242 = new wxBoxSizer (wxVERTICAL);
	
	m_staticText229 = new wxStaticText( this, IDC_ListTitle, wxT("Classes"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText229->Wrap( -1 );
	//bSizer242->Add( m_staticText229, 0, wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL|wxTOP, 5 );
	bSizer242->Add (m_staticText229, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxTOP, 5));
	
	m_listBox4 = new wxListBox (this,
											IDC_ListBox,
											wxDefaultPosition,
											wxSize (buttonWidth, 156),
											0,
											NULL,
											0);
	//bSizer242->Add( m_listBox4, 0, wxALIGN_BOTTOM|wxALIGN_CENTER_HORIZONTAL|wxALL|wxEXPAND, 5 );
	bSizer242->Add (m_listBox4, wxSizerFlags(0).Align(wxALIGN_CENTER_HORIZONTAL).Border(wxTOP, 5));
	
	//bSizer233->Add( bSizer242, 0, wxALIGN_BOTTOM|wxALIGN_CENTER|wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 3);
	bSizer233->Add (bSizer242, wxSizerFlags(0).Align(wxALIGN_CENTER).Border(wxBOTTOM|wxLEFT|wxRIGHT, 8));
	
	m_listBox = m_listBox4;
	
    m_staticText223->SetFont(font);
    m_staticText224->SetFont(font);
    m_staticText225->SetFont(font);
    m_staticText226->SetFont(font);
    m_staticText227->SetFont(font);
    m_staticText228->SetFont(font);
    m_staticText229->SetFont(font);
    m_staticText230->SetFont(font);
    m_staticText231->SetFont(font);
    m_staticText259->SetFont(font);
    m_staticText260->SetFont(font);
    m_staticText261->SetFont(font);
    m_staticText262->SetFont(font);
    //m_staticText263->SetFont(font);
    //m_staticText264->SetFont(font);
    m_comboBox40->SetFont(font);
    m_comboBox41->SetFont(font);
    m_comboBox42->SetFont(font);
    m_comboBox43->SetFont(font);
    m_checkBox53->SetFont(font);
    m_button56->SetFont(font);
    m_button60->SetFont(font);
    m_button61->SetFont(font);
    m_button62->SetFont(font);
    m_button63->SetFont(font);
    m_button64->SetFont(font);
    m_button65->SetFont(font);
    m_listBox->SetFont(font);
	
   bSizer234->SetMinSize (bSizer234->GetSize());
   fgSizer8->SetMinSize (fgSizer8->GetSize());
   fgSizer12->SetMinSize (fgSizer12->GetSize());
   fgSizer13->SetMinSize (fgSizer13->GetSize());
   fgSizer14->SetMinSize (fgSizer14->GetSize());
   fgSizer15->SetMinSize (fgSizer15->GetSize());
	
	this->SetSizer (bSizer233);
	this->Layout ();
	//this->Centre( wxBOTH );
	
}	// end "CreateControls2"


void CMStatisticsFrame::OnAddToList (
				wxCommandEvent& 						event)

{
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent (gProjectInfoPtr->addToControlH,
										FALSE,
										location,
										kAddToListControl);

}	// end "OnAddToList"



void CMStatisticsFrame::OnCharHook (
				wxKeyEvent& 									event)

{
   CMTool* pTool = CMTool::FindTool(CMTool::c_toolType);
	if (pTool != NULL)
		{
		int keyCode = event.GetKeyCode ();
	
		if (keyCode == WXK_RETURN || keyCode == WXK_NUMPAD_ENTER)
			pTool->OnCharHook();
		
		}	// end "if (pTool != NULL)"

}	// end "OnCharHook"



void CMStatisticsFrame::OnClass(wxCommandEvent& event) 
{
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent(
      gProjectInfoPtr->toClassControlH,
      FALSE,
      location,
      kToClassControl);

   UpdateStatsTypeCombo(kFieldListMode);

   UpdateHistogramStatsCombo();

   UpdateListStatsCombo();
		
	ActivateStatisticsWindowItems (gProjectInfoPtr->statsWindowMode);
   
   SetUpToolTip(m_button56, IDS_ToolTip14);
   SetUpToolTip(m_button60, IDS_ToolTip15);
//   m_tooltipUpdate->SetTip(wxT("Update statistics for the class if training fields have been changed"));
} // end "OnClass"


void CMStatisticsFrame::OnDblclkListBox(wxCommandEvent& event) 
{
   SInt16 cellIndex = -1,
      savedStatsMode;


   savedStatsMode = gProjectInfoPtr->statsWindowMode;
   cellIndex = gStatisticsListHandle->GetSelection();

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


void CMStatisticsFrame::OnDropdownStatsTypeCombo(wxCommandEvent& event) 
{
   SInt16 savedStatsTypeCode;
   wxMouseState mousestate;
//   printf("triggered in EVT_DROPDOWN stats combo\n");

   savedStatsTypeCode = m_statsTypeCode;
   wxComboBox* comboBoxPtr = (wxComboBox*) FindWindow(IDC_StatsCombo);
   comboBoxPtr->Delete(1);
   if (mousestate.RightIsDown() || wxGetKeyState(WXK_SHIFT)) {      
      comboBoxPtr->Insert((char*) "LOOC...", 1);
      m_optionKeyFlag = TRUE;
   }// end "if (GetKeyState (VK_RBUTTON) < 0)"
   else // GetKeyState (VK_RBUTTON) >= 0
   { 
     comboBoxPtr->Insert((char*) "LOOC", 1);
     m_optionKeyFlag = FALSE;
   } // end "if (GetKeyState (VK_RBUTTON) < 0)"

   comboBoxPtr->SetClientData(1, (void*)kLeaveOneOutStats);

   // Make sure that the drown down selection is drawn properly.

//   DDX_CBIndex(m_dialogToPtr, IDC_StatsCombo, m_statsTypeCode); ??

} // end "OnDropdownStatsTypeCombo"


void CMStatisticsFrame::OnEditClearSelectRectangle(wxCommandEvent& event)
{                  
	if (gProjectSelectionWindow != NULL)
		{                                                                                  
		ClearSelectionArea (gProjectSelectionWindow);
		
		if (gActiveImageViewCPtr == gProjectSelectionWindow)
			ShowGraphSelection ();
		
		}		// end "if (gProjectSelectionWindow != NULL)"
	
}		// end "OnEditClearSelectRectangle"


void CMStatisticsFrame::OnEditName(wxCommandEvent& event) 
{
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent(
      gProjectInfoPtr->editNameControlH,
      FALSE,
      location,
      kEditNameControl);
   
} // end "OnEditName"


void CMStatisticsFrame::OnEditSelectionRectangle(wxCommandEvent& event)

{                                              
	EditSelectionDialog(kRectangleType, TRUE);
	
}		// end "OnEditSelectionRectangle" 


void CMStatisticsFrame::OnEditSelectAll(wxCommandEvent& event)
{
	DoEditSelectAllImage (gProjectSelectionWindow);
	
}		// end "OnEditSelectAll"


void CMStatisticsFrame::OnField(wxCommandEvent& event) 
{
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent (gProjectInfoPtr->toFieldControlH,
										FALSE,
										location,
										kToFieldControl);

   UpdateHistogramStatsCombo();

   UpdateListStatsCombo();
		
	ActivateStatisticsWindowItems (gProjectInfoPtr->statsWindowMode);
   
   SetUpToolTip(m_button56, IDS_ToolTip16);
   SetUpToolTip(m_button60, IDS_ToolTip17);
   
//   m_tooltipEdit->SetTip(wxT("Edit the coordinates for the field"));
//   m_tooltipUpdate->SetTip(wxT("Update statistics for the field if training fields have been changed"));
} // end "OnField"  


void CMStatisticsFrame::OnHistogram(wxCommandEvent& event) 
{
   // TODO: Add your control notification handler code here

} // end "OnHistogram"


void CMStatisticsFrame::OnHistogramStatsComboDropDown(wxCommandEvent& event) 
{
   wxComboBox* histogramStatsCode =  wxDynamicCast(this->FindWindow(IDC_HistogramStatsCombo), wxComboBox);
   histogramStatsCode->SetSelection(-1);
}


void CMStatisticsFrame::OnList() 
{
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent(
      gProjectInfoPtr->listControlH,
      FALSE,
      location,
      kStatPrintControl);
   
} // end "OnList"


void CMStatisticsFrame::OnListStatsComboDropDown(wxCommandEvent& event) 
{
   wxComboBox* listStatsCodePtr =  wxDynamicCast(((CMStatisticsView*) gProjectWindow)->GetFrame()->FindWindow(IDC_ListStatsCombo), wxComboBox);
   listStatsCodePtr->SetSelection(-1);
   
}		// end "OnListStatsComboDropDown"


void CMStatisticsFrame::OnPolygon(wxCommandEvent& event)
{
//   DDX_Check(m_dialogFromPtr,IDC_Polygon,m_polygonFlag);
//   wxWindow* projWin = this->GetFrame();
   wxCheckBox* polygonFlag = (wxCheckBox*)this->FindWindow(IDC_Polygon);
   m_polygonFlag = polygonFlag->GetValue();
   
   Point location;


   location.h = 0;
   location.v = 0;
        
//   ((CMImageFrame*)AfxGetApp())->OnClearSelectionRectangle(); 
   ClearSelectionArea(gProjectSelectionWindow); // previous line commented out in Windows


   StatisticsWControlEvent(
      gProjectInfoPtr->polygonTypeControlH,
      FALSE,
      location,
      kPolygonEnterControl);
   
} // end "OnPolygon"


void CMStatisticsFrame::OnProject(wxCommandEvent& event) 
{
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent (gProjectInfoPtr->toProjectControlH,
										FALSE,
										location,
										kToProjectControl);
		
	ActivateStatisticsWindowItems (gProjectInfoPtr->statsWindowMode);
   
   SetUpToolTip(m_button56, IDS_ToolTip3);
   SetUpToolTip(m_button60, IDS_ToolTip18);
   
//   m_tooltipEdit->SetTip(wxT("Edit the class name"));
//   m_tooltipUpdate->SetTip(wxT("Update project statistics if training fields have been changed"));
} // end "OnProject"


void CMStatisticsFrame::OnSelchangeListBox(wxCommandEvent& event) 
{
   SInt16 cellIndex = -1;


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


void CMStatisticsFrame::OnSelect(wxCommandEvent& event)
{
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent (gProjectInfoPtr->newFieldControlH,
										FALSE,
										location,
										kNewFieldControl);
		
	ActivateStatisticsWindowItems (gProjectInfoPtr->statsWindowMode);
   SetUpToolTip(m_button56, IDS_ToolTip13);
//   m_tooltipEdit->SetTip(wxT("Edit the line and column coordinates of the selected rectangular area"));
} // end "OnSelect"


void CMStatisticsFrame::OnSelendokClassList(wxCommandEvent& event)
{
   wxComboBox* classlist = (wxComboBox*)this->FindWindow(IDC_ClassList);
   m_classList = classlist->GetSelection();
   gProjectInfoPtr->lastClassForAddFieldSelection = m_classList + 1;

} // end "OnSelendokClassList"


void CMStatisticsFrame::OnSelendokHistogramStatsCombo(wxCommandEvent& event) 
{
   wxComboBox* histogramStatsCode =  wxDynamicCast(((CMStatisticsView*) gProjectWindow)->GetFrame()->FindWindow(IDC_HistogramStatsCombo), wxComboBox);
   m_histogramStatsCode = histogramStatsCode->GetSelection();
   
   if (m_histogramStatsCode >= 0) 
		{
				// Make 1 base.

      m_histogramStatsCode++;

      if (gProjectInfoPtr->statsWindowMode == 4)
         m_histogramStatsCode += 1;

      gProcessorCode = kHistogramStatsProcessor;
      HistogramStatsControl (gProjectInfoPtr->statsWindowMode, m_histogramStatsCode);
      gProcessorCode = 0;

      if (m_histogramStatsCode > 0) 
			{
         m_histogramStatsCode = 0;
         histogramStatsCode->SetSelection(m_histogramStatsCode);
			} // end "if (m_histogramStatsCode > 0)"

		} // end "if (m_histogramStatsCode >= 0)" 

} // end "OnSelendokHistogramStatsCombo"


void CMStatisticsFrame::OnSelendokListStatsCombo(wxCommandEvent& event) 
{
   SInt16 listStatsCode = 1;

   wxComboBox* listStatsCodePtr =  wxDynamicCast(((CMStatisticsView*) gProjectWindow)->GetFrame()->FindWindow(IDC_ListStatsCombo), wxComboBox);

   m_listStatsCode = listStatsCodePtr->GetSelection();
   
   if (m_listStatsCode >= 0) 
		{
      listStatsCode = m_listStatsCode + 1;

      if (gProjectInfoPtr->statsWindowMode == 4)
         listStatsCode += 2;

      gProcessorCode = kListStatsProcessor;
      ListStatsControl(gProjectInfoPtr->statsWindowMode, listStatsCode);
      gProcessorCode = 0;

      if (m_listStatsCode > 0) 
			{
         m_listStatsCode = 0;
         listStatsCodePtr->SetSelection(m_listStatsCode);
			} // end "if (m_listStatsCode > 0)"

		} // end "if (m_listStatsCode >= 0)" 

} // end "OnSelendokListStatsCombo"


void CMStatisticsFrame::OnSelendokStatsCombo(wxCommandEvent& event) 
{
   //	SInt32								menuItemCode;

   SInt16 covarianceStatsToUse,
      savedStatsTypeCode;

      
   savedStatsTypeCode = m_statsTypeCode;
   wxComboBox* statsTypeComboPtr=  wxDynamicCast(this->FindWindow(IDC_StatsCombo), wxComboBox);
   m_statsTypeCode = statsTypeComboPtr->GetSelection();
   
   if (m_statsTypeCode == 1 && m_optionKeyFlag) 
		{
      if (!LOOCOptionsDialog(gProjectInfoPtr->statsWindowMode))
         m_statsTypeCode = savedStatsTypeCode;

      else // LOOCOptionsDialog (...
         m_statsTypeCode = 1;

		} // end "if (m_statsTypeCode == 1 && m_optionKeyFlag)"

    SInt64 statstype64 = (SInt64)((int*)statsTypeComboPtr->GetClientData(m_statsTypeCode));
    covarianceStatsToUse = (SInt16) statstype64;
    
   if (covarianceStatsToUse > 0) 
		{
      //		covarianceStatsToUse = GetCovarianceStatsFromMenuItem ((SInt16)menuItemCode);

      if (gProjectInfoPtr->statsWindowMode == kClassListMode)
         SetProjectCovarianceStatsToUse(covarianceStatsToUse);

      else // gProjectInfoPtr->statsWindowMode == kFieldListMode
         SetClassCovarianceStatsToUse(covarianceStatsToUse);

		} // end "if (covarianceStatsToUse > 0)"

   if (m_optionKeyFlag && covarianceStatsToUse == 2) 
		{
      statsTypeComboPtr->Delete(1);
      statsTypeComboPtr->Insert((char*) "LOOC...",1);
      statsTypeComboPtr->SetClientData(1, (void*)kLeaveOneOutStats);

		} // end "if (m_optionKeyFlag && ..."

			// Make sure that the drown down selection is drawn properly.

   statsTypeComboPtr->SetSelection(m_statsTypeCode);
	
} // end "OnSelendokStatsCombo"


void CMStatisticsFrame::OnUpdate(wxCommandEvent& event) 
{
   Point location;


   location.h = 0;
   location.v = 0;

   StatisticsWControlEvent (
      gProjectInfoPtr->updateControlH,
      FALSE,
      location,
      kUpdateStatsControl);
   
   
} // end "OnUpdate"


void CMStatisticsFrame::OnUpdateEditClearSelectRectangle(wxUpdateUIEvent&  pCmdUI)
{                                                                                                                                                                                                                      
	SInt16 selectionTypeCode = GetSelectionTypeCode (gProjectSelectionWindow);
	
			// Update Menu item text.
			
	UpdateEditClearSelection (&pCmdUI, selectionTypeCode); 
		
	pCmdUI.Enable ( selectionTypeCode != 0 );
	
}		// end "OnUpdateEditClearSelectRectangle" 


void CMStatisticsFrame::OnUpdateEditSelectAll(wxUpdateUIEvent&  pCmdUI)
{  
	Boolean enableFlag = FALSE;
	                                                      
	if ( gProjectInfoPtr != NULL &&
						gProjectInfoPtr->selectionType == kRectangleType) 
		enableFlag = TRUE; 
						                                                                                            
	pCmdUI.Enable( enableFlag );
	
}		// end "OnUpdateEditSelectAll"


void CMStatisticsFrame::OnUpdateEditSelectionRectangle(wxUpdateUIEvent&  pCmdUI)

{  
	Boolean	enableFlag = FALSE;
	if ( gProjectInfoPtr != NULL && 
					gProjectInfoPtr->selectionType == kRectangleType )
		enableFlag = TRUE;
   
	pCmdUI.Enable( enableFlag );
	
}		// end "OnUpdateEditSelectionRectangle"


void CMStatisticsFrame::UpdateHistogramStatsCombo() 
{
//   wxWindow* projWin = this->GetFrame();
//   histogramStatsComboBoxPtr = wxDynamicCast(projWin->FindWindow(IDC_HistogramStatsCombo), wxComboBox);
   wxComboBox* histogramStatsComboBoxPtr =  wxDynamicCast(((CMStatisticsView*) gProjectWindow)->GetFrame()->FindWindow(IDC_HistogramStatsCombo), wxComboBox);
   
   if (gProjectInfoPtr->statsWindowMode > 1) 
		{     
				// Clear the current items.
            
      histogramStatsComboBoxPtr->Clear();

				// Load the new items depending on the current statistics window mode.

      switch (gProjectInfoPtr->statsWindowMode) 
			{
         case 2:
            histogramStatsComboBoxPtr->Append("Histogram Classes");
            histogramStatsComboBoxPtr->Append("Histogram Fields");
            histogramStatsComboBoxPtr->Append("Histogram...");

            break;

         case 3:
            histogramStatsComboBoxPtr->Append("Histogram Class");
            histogramStatsComboBoxPtr->Append("Histogram Fields");
            histogramStatsComboBoxPtr->Append("Histogram...");

            break;

         case 4:
            histogramStatsComboBoxPtr->Append("Histogram Field");
            histogramStatsComboBoxPtr->Append("Histogram...");

            break;

			} // end "switch (gProjectInfoPtr->statsWindowMode)" 

      m_histogramStatsCode = 0;
      histogramStatsComboBoxPtr->SetSelection(m_histogramStatsCode);

		} // end "if (gProjectInfoPtr->statsWindowMode > 1)"

} // end "UpdateHistogramStatsCombo" 


void CMStatisticsFrame::UpdateListStatsCombo() 
{
   wxComboBox* listStatsComboBoxPtr;
   listStatsComboBoxPtr = (wxComboBox*) this->FindWindow(IDC_ListStatsCombo);

   if (gProjectInfoPtr->statsWindowMode > 1) 
		{
				// Clear the current items.

      listStatsComboBoxPtr->Clear();

				// Load the new items depending on the current statistics window
				// mode.
      
      switch (gProjectInfoPtr->statsWindowMode) 
			{
         case 2:
            listStatsComboBoxPtr->Append("List Classes Stats");
            listStatsComboBoxPtr->Append("List Classes & Fields Stats");
            listStatsComboBoxPtr->Append("List Fields Stats");
            listStatsComboBoxPtr->Append("List Stats...");

            break;

         case 3:
            listStatsComboBoxPtr->Append("List Class Stats");
            listStatsComboBoxPtr->Append("List Class & Fields Stats");
            listStatsComboBoxPtr->Append("List Fields Stats");
            listStatsComboBoxPtr->Append("List Stats...");

            break;

         case 4:
            listStatsComboBoxPtr->Append("List Field Stats");
            listStatsComboBoxPtr->Append("List Stats...");

            break;

      } // end "switch (gProjectInfoPtr->statsWindowMode)" 

      m_listStatsCode = 0;
      listStatsComboBoxPtr->SetSelection(m_listStatsCode);
		
		} // end "if (gProjectInfoPtr->statsWindowMode > 1)"

} // end "UpdateListStatsCombo"  

void CMStatisticsFrame::UpdateStatsTypeCombo(
   SInt16 statsWindowMode)
 {
   wxComboBox*		statsTypeComboBoxPtr;

   SInt16			numberItems,
						classStorage;


   // Draw the prompt string and current covariance stats to be used.
	 //wxWindow* projWin = this->GetFrame();
   statsTypeComboBoxPtr = (wxComboBox*) this->FindWindow(IDC_StatsCombo);		
	   
   if(statsTypeComboBoxPtr->GetCount() > 3)
      statsTypeComboBoxPtr->Delete(3);	
	if(statsTypeComboBoxPtr->GetCount() > 2)
			statsTypeComboBoxPtr->Delete(2);	
	
   if (statsWindowMode == kClassListMode) {
      m_statsTypeCode = GetProjectStatisticsTypeMenuItem() - 1;

      if (gProjectInfoPtr->enhancedStatsExistFlag)
			{
         statsTypeComboBoxPtr->Append("Enhanced");
         statsTypeComboBoxPtr->SetClientData(2, (void*)kEnhancedStats);

			} // end "if (gProjectInfoPtr->enhancedStatsExistFlag)"

      if (m_statsTypeCode == 3)
			{
         statsTypeComboBoxPtr->Append("Mixed");
         numberItems = statsTypeComboBoxPtr->GetCount();
         statsTypeComboBoxPtr->SetClientData(numberItems - 1, (void*)kMixedStats);
         m_statsTypeCode = numberItems - 1;

			} // end "if (gProjectInfoPtr->enhancedStatsExistFlag)"

		}// end "if (statsWindowMode == kClassListMode)"

   else // statsWindowMode == kFieldListMode
		{
      m_statsTypeCode = GetClassStatisticsTypeMenuItem() - 1;

      classStorage = gProjectInfoPtr->storageClass[gProjectInfoPtr->currentClass];
      if (gProjectInfoPtr->classNamesPtr[classStorage].modifiedStatsFlag) 
			{
         statsTypeComboBoxPtr->Append("Enhanced");
         statsTypeComboBoxPtr->SetClientData(2, (void*)kEnhancedStats);

			} // end "if (...modifiedStatsFlag)"

		} // end "else statsWindowMode == kFieldListMode"

   statsTypeComboBoxPtr->SetSelection(m_statsTypeCode);
	
} // end "UpdateStatsTypeCombo"
