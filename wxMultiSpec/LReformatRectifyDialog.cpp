//                               MultiSpec
//
//               Laboratory for Applications of Remote Sensing
//                         Purdue University
//                        West Lafayette, IN 47907
//                         Copyright (2009-2018)
//                     (c) Purdue Research Foundation
//                           All rights reserved.
//
//   File:                  LReformatRectifyDialog.cpp : class implementation file
//   Class Definition:      LReformatRectifyDialog.h
//
//   Authors:               Abdur Rahman Maud, Larry L. Biehl
//
//   Revision date:         11/16/2018
//
//   Language:            C++
//
//   System:               Linux Operating System
//
//   Brief description:   This file contains functions that relate to the
//                        CMReformatRectifyDlg class.
//
//------------------------------------------------------------------------------------
/*
 Template for debug code.
 int numberChars = sprintf ((char*)&gTextString3,
 " LReformatRectifyDlg:OnCbnSelendokTargetCombo (m_fileNamesSelection): %d%s",
 m_fileNamesSelection,
 gEndOfLine);
 ListString ((char*)&gTextString3, numberChars, gOutputTextH);
 */

#include   "SMultiSpec.h"
#include "LReformatRectifyDialog.h"
#include "wx/gbsizer.h"
#include "LChannelsDialog.h"
#include "LImageView.h"
#include "CImageWindow.h"
#include "LImage_dialog.cpp"

extern void RectifyImageDialogInitialize (
                                          DialogPtr                     dialogPtr,
                                          FileInfoPtr                     fileInfoPtr,
                                          DialogSelectArea*               dialogSelectAreaPtr,
                                          ReformatOptionsPtr            reformatOptionsPtr,
                                          SInt16*                        headerOptionsSelectionPtr,
                                          SInt16*                        channelSelectionPtr,
                                          Boolean*                        blankOutsideSelectedAreaFlagPtr,
                                          double*                        lastBackgroundValuePtr,
                                          SInt16*                        procedureCodePtr,
                                          SInt16*                        resampleCodePtr,
                                          SInt16*                        fileNamesSelectionPtr,
                                          Handle*                        referenceWindowInfoHandlePtr,
                                          SInt32*                        lastLineShiftPtr,
                                          SInt32*                        lastColumnShiftPtr,
                                          double*                        lastLineScaleFactorPtr,
                                          double*                        lastColumnScaleFactorPtr,
                                          double*                        lastRotationAnglePtr,
                                          double*                        mapOrientationAnglePtr);

extern void RectifyImageDialogOK (
                                  DialogPtr                     dialogPtr,
                                  FileInfoPtr                     outFileInfoPtr,
                                  FileInfoPtr                     fileInfoPtr,
                                  WindowInfoPtr                  windowInfoPtr,
                                  LayerInfoPtr                  layerInfoPtr,
                                  DialogSelectArea*               dialogSelectAreaPtr,
                                  ReformatOptionsPtr            reformatOptionsPtr,
                                  SInt16                        headerOptionsSelection,
                                  Boolean                        blankOutsideSelectedAreaFlag,
                                  SInt16                        channelSelection,
                                  double                        lastBackgroundValue,
                                  SInt16                        proced,// !defined __WREFORMATRECTIFYDLG_H__ureCode,
                                  SInt16                        resampleCode,
                                  Handle                        referenceWindowInfoHandle,
                                  SInt32                        lastLineShift,
                                  SInt32                        lastColumnShift,
                                  double                        lastLineScaleFactor,
                                  double                        lastColumnScaleFactor,
                                  double                        lastRotationAngle);

extern void RectifyImageDialogOnRectifyCode (
                                             DialogPtr                     dialogPtr,
                                             SInt16                        rectifyCode,
                                             Boolean                        blankOutsideSelectedAreaFlag,
                                             double                        mapOrientationAngle);

extern SInt16    RectifyImageDialogOnReferenceFile (
                                                    DialogPtr                     dialogPtr,
                                                    SInt16                        procedureCode,
                                                    SInt16                        fileNamesSelection,
                                                    Handle*                        targetWindowInfoHandlePtr,
                                                    DialogSelectArea*               dialogSelectAreaPtr);

/////////////////////////////////////////////////////////////////////////////
// CMReformatRectifyDlg dialog

CMReformatRectifyDlg::CMReformatRectifyDlg(wxWindow* pParent,
                                           wxWindowID id, const wxString& title/*=NULL*/)
:CMDialog(CMReformatRectifyDlg::IDD, pParent, title)
{
   m_kNoneMenuItem = kNoneMenuItem;
   m_kArcViewMenuItem = kArcViewMenuItem;
   m_kERDAS74MenuItem = kERDAS74MenuItem;
   m_kGAIAMenuItem = kGAIAMenuItem;
   m_kTIFFGeoTIFFMenuItem = kTIFFGeoTIFFMenuItem;
   m_kMatlabMenuItem = kMatlabMenuItem;
   
   m_backgroundValue = 0;
   m_blankOutsideSelectedAreaFlag = FALSE;
   m_channelSelection = -1;
   m_columnScaleFactor = 0.0;
   m_columnShift = 0;
   m_fileNamesSelection = -1;
   m_headerListSelection = -1;
   m_headerOptionsSelection = -1;
   m_lineScaleFactor = 0.0;
   m_lineShift = 0;
   m_procedureCode = 0;
   m_resampleSelection = 0;
   m_rotationAngle = 0.0;
   m_useMapOrientationAngleFlag = FALSE;
   
   m_referenceWindowInfoHandle = NULL;
   m_initializedFlag = CMDialog::m_initializedFlag;
   
   CreateControls();
   SetSizerAndFit(bSizer119);
}


CMReformatRectifyDlg::~CMReformatRectifyDlg()
{
   
}


BEGIN_EVENT_TABLE(CMReformatRectifyDlg, CMDialog)

EVT_INIT_DIALOG(CMReformatRectifyDlg::OnInitDialog)
EVT_BUTTON(IDEntireImage, CMReformatRectifyDlg::ToEntireImage)
EVT_BUTTON(IDSelectedImage, CMReformatRectifyDlg::ToSelectedImage)
EVT_TEXT(IDC_LineStart, CMReformatRectifyDlg::CheckLineStart)
EVT_TEXT(IDC_LineEnd, CMReformatRectifyDlg::CheckLineEnd)
EVT_TEXT(IDC_ColumnStart, CMReformatRectifyDlg::CheckColumnStart)
EVT_TEXT(IDC_ColumnEnd, CMReformatRectifyDlg::CheckColumnEnd)
EVT_COMBOBOX(IDC_Channels, CMReformatRectifyDlg::OnSelendokOutChannels)
EVT_COMBOBOX_DROPDOWN(IDC_Channels, CMReformatRectifyDlg::OnSelendokChannelComboDropDown)
EVT_TEXT(IDC_RotationClockwise, CMReformatRectifyDlg::OnEnChangeRotationClockwise)
EVT_CHECKBOX(IDC_UseMapOrientationAngle, CMReformatRectifyDlg::OnBnClickedUseMapOrientationAngle)
EVT_RADIOBUTTON(IDC_TranslateScaleRotateRadio, CMReformatRectifyDlg::OnBnClickedTranslateScaleRotateRadio)
EVT_RADIOBUTTON(IDC_ReprojectToRadio, CMReformatRectifyDlg::OnBnClickedReprojectToRadio)
EVT_COMBOBOX(IDC_ReferenceFileList, CMReformatRectifyDlg::OnCbnSelendokTargetCombo)
EVT_COMBOBOX(IDC_ResampleMethod, CMReformatRectifyDlg::OnCbnSelendokResampleMethod)
//EVT_CHAR_HOOK(CMReformatRectifyDlg::OnButtonPress)

END_EVENT_TABLE()


void CMReformatRectifyDlg::CreateControls ()

{
   this->SetSizeHints( wxDefaultSize, wxDefaultSize );
   
   wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA(entireim);
   wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA(toentire);
   wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA(selected);
   wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA(bmp4);
   
   //wxBoxSizer* bSizer119;
   bSizer119 = new wxBoxSizer( wxVERTICAL );
   
   wxGridSizer* gSizer16;
   gSizer16 = new wxGridSizer( 0, 2, 0, 0 );
   
   wxBoxSizer* bSizer195;
   bSizer195 = new wxBoxSizer( wxVERTICAL );
   
   wxBoxSizer* bSizer198;
   bSizer198 = new wxBoxSizer( wxVERTICAL );
   
   wxStaticBoxSizer* inputImageSettingsSizer;
   wxStaticBox* inputImageSettingsBox = new wxStaticBox( this, IDC_LineColFrame, wxT("Input Image Settings") );
   inputImageSettingsSizer = new wxStaticBoxSizer (inputImageSettingsBox, wxHORIZONTAL);
   
   
   wxBoxSizer* bSizer791;
   bSizer791 = new wxBoxSizer (wxHORIZONTAL);
   
   m_bpButton4 = new wxBitmapButton(inputImageSettingsBox, IDEntireImage, entireimi, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
   m_bpButton4->SetBitmapDisabled(toentirei);
   //bSizer791->Add (m_bpButton4, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
   //bSizer791->Add (m_bpButton4, 0, wxALL, 5);
   bSizer791->Add (m_bpButton4,
                   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   m_bpButton5 = new wxBitmapButton(inputImageSettingsBox, IDSelectedImage, selectedi, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
   m_bpButton5->SetBitmapDisabled(bmp4i);
   //bSizer791->Add (m_bpButton5, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
   //bSizer791->Add (m_bpButton5, 0, wxALL, 5);
   bSizer791->Add (m_bpButton5,
                   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   inputImageSettingsSizer->Add( bSizer791, 0, wxALL|wxEXPAND, 5 );
   m_bpButton4->Hide();
   
   wxGridSizer* gSizer1;
   gSizer1 = new wxGridSizer(3, 3, 0, 0);
   
   wxStaticText* staticText63 = new wxStaticText(inputImageSettingsBox, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
   staticText63->Wrap(-1);
   gSizer1->Add(staticText63, 0, wxALL, 5);
   
   wxStaticText* staticText64 = new wxStaticText(inputImageSettingsBox, wxID_ANY, wxT("Start"), wxDefaultPosition, wxSize(70, -1), 0);
   staticText64->Wrap(-1);
   gSizer1->Add(staticText64, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxStaticText* staticText65 = new wxStaticText(inputImageSettingsBox, wxID_ANY, wxT("End"), wxDefaultPosition, wxSize(70, -1), 0);
   staticText65->Wrap(-1);
   gSizer1->Add(staticText65, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxStaticText* staticText60 = new wxStaticText(inputImageSettingsBox, wxID_ANY, wxT("Lines"), wxDefaultPosition, wxSize(70, -1), 0);
   staticText60->Wrap(-1);
   gSizer1->Add(staticText60, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxTextCtrl* linestartctrl = new wxTextCtrl(inputImageSettingsBox, IDC_LineStart, wxEmptyString, wxDefaultPosition, wxSize(70, -1), 0);
   linestartctrl->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_LineStartString));
   gSizer1->Add(linestartctrl, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxTextCtrl* lineendctrl = new wxTextCtrl(inputImageSettingsBox, IDC_LineEnd, wxEmptyString, wxDefaultPosition, wxSize(70, -1), 0);
   lineendctrl->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_LineEndString));
   gSizer1->Add(lineendctrl, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxStaticText* staticText62 = new wxStaticText(inputImageSettingsBox, wxID_ANY, wxT("Columns"), wxDefaultPosition, wxSize(70, -1), 0);
   staticText62->Wrap(-1);
   gSizer1->Add(staticText62, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxTextCtrl* colstartctrl = new wxTextCtrl(inputImageSettingsBox, IDC_ColumnStart, wxEmptyString, wxDefaultPosition, wxSize(70, -1), 0);
   colstartctrl->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_ColumnStartString));
   gSizer1->Add(colstartctrl, 0, wxALIGN_CENTER | wxALL, 5);
   
   wxTextCtrl* colendctrl = new wxTextCtrl(inputImageSettingsBox, IDC_ColumnEnd, wxEmptyString, wxDefaultPosition, wxSize(70, -1), 0);
   colendctrl->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_ColumnEndString));
   
   SetUpToolTip(m_bpButton4, IDS_ToolTip40);
   SetUpToolTip(linestartctrl, IDS_ToolTip19);
   SetUpToolTip(lineendctrl, IDS_ToolTip20);
   
   SetUpToolTip(colstartctrl, IDS_ToolTip22);
   SetUpToolTip(colendctrl, IDS_ToolTip23);
   
   gSizer1->Add(colendctrl, 0, wxALIGN_CENTER | wxALL, 5);
   
   inputImageSettingsSizer->Add(gSizer1, 1, wxEXPAND|wxALL, 5);
   
   
   //bSizer198->Add( inputImageSettingsSizer, 1, wxEXPAND, 5 );
   //bSizer198->Add (inputImageSettingsSizer, 1, wxEXPAND|wxALL, 12);
   bSizer198->Add (inputImageSettingsSizer, 1, wxEXPAND | wxBOTTOM|wxRIGHT, 6);
   
   
   bSizer195->Add (bSizer198, 1, wxEXPAND, 5);
   
   wxBoxSizer* bSizer200;
   bSizer200 = new wxBoxSizer( wxVERTICAL );
   
   wxStaticBoxSizer* outputOptionsSizer;
   wxStaticBox* outputOptionsBox = new wxStaticBox( this, wxID_ANY, wxT("Output Image Options") );
   outputOptionsSizer = new wxStaticBoxSizer (outputOptionsBox, wxVERTICAL );
   
   wxBoxSizer* bSizer278;
   bSizer278 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText402 = new wxStaticText (outputOptionsBox, wxID_ANY, wxT("Header:"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText402->Wrap( -1 );
   SetUpToolTip(m_staticText402, IDS_ToolTip203);
   bSizer278->Add( m_staticText402, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   
   m_comboBox27 = new wxComboBox( outputOptionsBox, IDC_Header, wxT("GeoTIFF format"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
   m_comboBox27->Append("'None'");
   m_comboBox27->Append("ArcView format");
   m_comboBox27->Append("Erdas 74 format");
   SetUpToolTip(m_comboBox27, IDS_ToolTip203);
   bSizer278->Add( m_comboBox27, 0, wxALL, 5 );
   
   
   outputOptionsSizer->Add( bSizer278, 0, wxALL|wxEXPAND, 5 );
   
   wxBoxSizer* bSizer279;
   bSizer279 = new wxBoxSizer( wxHORIZONTAL );
   
   m_checkBox20 = new wxCheckBox( outputOptionsBox, IDC_NonSelectedPixels, wxT("Set Nonselected Pixels to Background Value"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox20, IDS_ToolTip204);
   bSizer279->Add( m_checkBox20, 0, wxALL, 5 );
   
   
   outputOptionsSizer->Add( bSizer279, 0, wxEXPAND, 5 );
   
   wxBoxSizer* bSizer280;
   bSizer280 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText403 = new wxStaticText( outputOptionsBox, wxID_ANY, wxT("Background value:"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText403->Wrap( -1 );
   SetUpToolTip(m_staticText403, IDS_ToolTip205);
   bSizer280->Add( m_staticText403, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   
   m_textCtrl154 = new wxTextCtrl( outputOptionsBox, IDC_BackgroundValue, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_textCtrl154, IDS_ToolTip205);
   m_textCtrl154->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_backgroundValueString));
   bSizer280->Add( m_textCtrl154, 0, wxALL, 5 );
   
   
   outputOptionsSizer->Add( bSizer280, 0, wxEXPAND, 5 );
   
   wxBoxSizer* bSizer281;
   bSizer281 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText404 = new wxStaticText( outputOptionsBox, IDC_ChannelsPrompt, wxT("Channels:"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText404->Wrap( -1 );
   SetUpToolTip(m_staticText404, IDS_ToolTip206);
   bSizer281->Add( m_staticText404, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   
   m_comboBox28 = new wxComboBox( outputOptionsBox, IDC_Channels, wxT("All"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
   m_comboBox28->Append("All");
   m_comboBox28->Append("Subset...");
   SetUpToolTip(m_comboBox28, IDS_ToolTip206);
   bSizer281->Add( m_comboBox28, 0, wxALL, 5 );
   
   outputOptionsSizer->Add( bSizer281, 0, wxEXPAND, 5 );
   
   //bSizer200->Add (outputOptionsSizer, 1, wxEXPAND, 5);
   //bSizer200->Add (outputOptionsSizer, 1, wxEXPAND|wxALL, 12);
   bSizer200->Add (outputOptionsSizer, 1, wxEXPAND | wxTOP|wxRIGHT, 6);
   
   bSizer195->Add (bSizer200, 1, wxEXPAND, 5);
   
   
   gSizer16->Add( bSizer195, 1, wxEXPAND, 5 );
   
   wxBoxSizer* bSizer197;
   bSizer197 = new wxBoxSizer( wxVERTICAL );
   
   wxStaticBoxSizer* rectifySettingsSizer;
   wxStaticBox* rectifySettingsBox = new wxStaticBox( this, wxID_ANY, wxT("Rectification Settings") );
   rectifySettingsSizer = new wxStaticBoxSizer (rectifySettingsBox, wxVERTICAL );
   
   wxTextValidator negativeIntegerValidator = wxTextValidator(wxFILTER_ASCII|wxFILTER_INCLUDE_CHAR_LIST);
   wxString valid_str = "'-','0','1','2','3','4','5','6','7','8','9'";
   negativeIntegerValidator.SetCharIncludes(valid_str);
   
   m_radioBtn18 = new wxRadioButton( rectifySettingsBox, IDC_TranslateScaleRotateRadio, wxT("Translate, Scale and/or Rotate"), wxDefaultPosition, wxDefaultSize, 0 );
   rectifySettingsSizer->Add( m_radioBtn18, 0, wxALL, 5 );
   
   wxBoxSizer* bSizer247;
   bSizer247 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText361 = new wxStaticText( rectifySettingsBox, IDC_TranslateScaleTitle, wxT("Linear"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText361->Wrap( -1 );
   bSizer247->Add( m_staticText361, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5 );
   
   
   bSizer247->Add( 40, 0, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 5 );
   
   m_staticText362 = new wxStaticText( rectifySettingsBox, IDC_TranslateScaleTitle2, wxT("Offset"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText362->Wrap( -1 );
   bSizer247->Add( m_staticText362, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5 );
   
   
   bSizer247->Add( 60, 0, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 5 );
   
   m_staticText363 = new wxStaticText( rectifySettingsBox, IDC_TranslateScaleTitle3, wxT("Scale"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText363->Wrap( -1 );
   bSizer247->Add( m_staticText363, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5 );
   
   
   rectifySettingsSizer->Add( bSizer247, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 5 );
   
   wxBoxSizer* bSizer262;
   bSizer262 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText364 = new wxStaticText( rectifySettingsBox, IDC_LineTranslateScalePrompt, wxT("Lines: "), wxDefaultPosition, wxDefaultSize, 0);
   m_staticText364->Wrap( -1 );
   bSizer262->Add( m_staticText364, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   
   
   bSizer262->Add( 20, 0, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 5 );
   
   m_textCtrl147 = new wxTextCtrl( rectifySettingsBox, IDC_LineOffset, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_textCtrl147, IDS_ToolTip208);
   //m_textCtrl147->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_lineOffsetString));
   m_textCtrl147->SetValidator(negativeIntegerValidator);
   
   bSizer262->Add( m_textCtrl147, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5 );
   
   m_textCtrl144 = new wxTextCtrl( rectifySettingsBox, IDC_LineScale, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_textCtrl144, IDS_ToolTip210);
   m_textCtrl144->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_lineScaleString));
   bSizer262->Add( m_textCtrl144, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5 );
   
   
   rectifySettingsSizer->Add( bSizer262, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 5 );
   
   wxBoxSizer* bSizer263;
   bSizer263 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText366 = new wxStaticText( rectifySettingsBox, IDC_ColumnTranslateScalePrompt, wxT("Columns:"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText366->Wrap( -1 );
   bSizer263->Add( m_staticText366, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   
   m_textCtrl145 = new wxTextCtrl( rectifySettingsBox, IDC_ColumnOffset, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_textCtrl145, IDS_ToolTip209);
   //m_textCtrl145->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_columnOffsetString));
   m_textCtrl145->SetValidator(negativeIntegerValidator);
   bSizer263->Add( m_textCtrl145, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5 );
   
   m_textCtrl146 = new wxTextCtrl( rectifySettingsBox, IDC_ColumnScale, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_textCtrl146, IDS_ToolTip211);
   m_textCtrl146->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_columnScaleString));
   bSizer263->Add( m_textCtrl146, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5 );
   
   
   rectifySettingsSizer->Add( bSizer263, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 5 );
   
   wxBoxSizer* bSizer266;
   bSizer266 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText378 = new wxStaticText( rectifySettingsBox, IDC_RotationTitle, wxT("Rotation  (clockwise)"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText378->Wrap( -1 );
   bSizer266->Add( m_staticText378, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5 );
   
   
   rectifySettingsSizer->Add( bSizer266, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 5 );
   
   wxBoxSizer* bSizer268;
   bSizer268 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText380 = new wxStaticText( rectifySettingsBox, IDC_RotationClockwisePrompt, wxT("Degrees:"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText380->Wrap( -1 );
   SetUpToolTip(m_staticText380, IDS_ToolTip207);
   bSizer268->Add( m_staticText380, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   
   m_textCtrl149 = new wxTextCtrl( rectifySettingsBox, IDC_RotationClockwise, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_textCtrl149, IDS_ToolTip207);
   m_textCtrl149->SetValidator(wxTextValidator(wxFILTER_NUMERIC, &m_rotationAngleString));
   bSizer268->Add( m_textCtrl149, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5 );
   
   
   rectifySettingsSizer->Add( bSizer268, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 5 );
   
   wxBoxSizer* bSizer269;
   bSizer269 = new wxBoxSizer( wxHORIZONTAL );
   
   m_checkBox18 = new wxCheckBox( rectifySettingsBox, IDC_UseMapOrientationAngle, wxT("Use image orientation angle"), wxDefaultPosition, wxDefaultSize, 0 );
   bSizer269->Add( m_checkBox18, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5 );
   
   
   rectifySettingsSizer->Add( bSizer269, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 5 );
   
   wxBoxSizer* bSizer270;
   bSizer270 = new wxBoxSizer( wxHORIZONTAL );
   
   m_radioBtn19 = new wxRadioButton( rectifySettingsBox, IDC_ReprojectToRadio, wxT("Reproject"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_radioBtn19, IDS_ToolTip214);
   bSizer270->Add( m_radioBtn19, 0, wxALL, 5 );
   
   
   rectifySettingsSizer->Add( bSizer270, 0, wxEXPAND, 5 );
   
   wxBoxSizer* bSizer271;
   bSizer271 = new wxBoxSizer( wxHORIZONTAL );
   
   
   bSizer271->Add( 20, 0, 0, wxEXPAND, 5 );
   
   m_staticText381 = new wxStaticText( rectifySettingsBox, IDC_ReferenceFileListPrompt, wxT("Reference:"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText381->Wrap( -1 );
   SetUpToolTip(m_staticText381, IDS_ToolTip212);
   bSizer271->Add( m_staticText381, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   //m_staticText381->Hide();
   m_comboBox29 = new wxComboBox( rectifySettingsBox, IDC_ReferenceFileList, wxT("Combo!"), wxDefaultPosition, wxSize(200, -1), 0, NULL, 0 );
   SetUpToolTip(m_comboBox29, IDS_ToolTip212);
   m_comboBox29->SetClientSize(wxSize(260, -1));
   
   bSizer271->Add( m_comboBox29, 0, wxALL, 5 );
   
   rectifySettingsSizer->Add( bSizer271, 0, wxEXPAND, 5 );
   
   wxBoxSizer* bSizer272;
   bSizer272 = new wxBoxSizer( wxHORIZONTAL );
   
   bSizer272->Add( 20, 0, 0, wxEXPAND, 5 );
   
   m_staticText382 = new wxStaticText( rectifySettingsBox, IDC_ResampleMethodPrompt, wxT("Algorithm:"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText382->Wrap( -1 );
   SetUpToolTip(m_staticText382, IDS_ToolTip213);
   bSizer272->Add( m_staticText382, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   //m_staticText382->Hide();
   m_comboBox30 = new wxComboBox( rectifySettingsBox, IDC_ResampleMethod, wxT("Nearest Neighbor"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
   SetUpToolTip(m_comboBox30, IDS_ToolTip213);
   //m_comboBox30->Append("Nearest Neighbor");
   m_comboBox30->SetMinSize(wxSize(150, -1));
   bSizer272->Add( m_comboBox30, 0, wxALL, 5 );
   
   rectifySettingsSizer->Add( bSizer272, 0, wxEXPAND, 5 );
   
   //bSizer197->Add (rectifySettingsSizer, 1, wxEXPAND, 5);
   //bSizer197->Add (rectifySettingsSizer, 1, wxEXPAND|wxALL, 12);
   bSizer197->Add (rectifySettingsSizer, 1, wxEXPAND | wxLEFT, 6);
   
   gSizer16->Add (bSizer197, 1, wxEXPAND, 5);
   
   //bSizer119->Add (gSizer16, 1, wxEXPAND, 5);
   bSizer119->Add (gSizer16, 1, wxEXPAND|wxLEFT|wxTOP|wxRIGHT, 12);
   /*
   m_sdbSizer5 = new wxStdDialogButtonSizer();
   m_sdbSizer5OK = new wxButton( this, wxID_OK );
   m_sdbSizer5->AddButton( m_sdbSizer5OK );
   m_sdbSizer5Cancel = new wxButton( this, wxID_CANCEL );
   m_sdbSizer5->AddButton( m_sdbSizer5Cancel );
   m_sdbSizer5->Realize();
   
   bSizer119->Add (m_sdbSizer5, 0, wxALIGN_RIGHT | wxLEFT|wxBOTTOM, 12);
	*/
	CreateStandardButtons (bSizer119);
   
   //this->SetSizer( bSizer119 );
   this->Layout();
   
   // This forces the positions to be used when the toEntire and toSelected buttons are switched.
   m_bpButton4->Show();
   m_bpButton5->Hide();
   this->Layout();
   
   //this->Centre( wxBOTH );
   
}      // end "CreateControls"


Boolean CMReformatRectifyDlg::DoDialog(
                                       FileInfoPtr                     outFileInfoPtr,
                                       FileInfoPtr                     fileInfoPtr,
                                       WindowInfoPtr                  imageWindowInfoPtr,
                                       LayerInfoPtr                  imageLayerInfoPtr,
                                       ReformatOptionsPtr            reformatOptionsPtr,
                                       double                        minBackgroundValue,
                                       double                        maxBackgroundValue)
{
   SInt16                  returnCode;
   Boolean                  continueFlag = FALSE;
   
   // Make sure intialization has been complete
   
   if (!m_initializedFlag)
      return(FALSE);
   
   m_outputFileInfoPtr = outFileInfoPtr;
   m_fileInfoPtr = fileInfoPtr;
   m_imageWindowInfoPtr = imageWindowInfoPtr;
   m_imageLayerInfoPtr = imageLayerInfoPtr;
   m_reformatOptionsPtr = reformatOptionsPtr;
   m_rectifyImageOptionsPtr = reformatOptionsPtr->rectifyImageOptionsPtr;
   m_minBackgroundValue = minBackgroundValue;
   m_maxBackgroundValue = maxBackgroundValue;
   
   // Selected area for output file.
   m_dialogSelectArea.lineStart = m_LineStart;
   m_dialogSelectArea.lineEnd = m_LineEnd;
   m_dialogSelectArea.lineInterval = m_LineInterval;
   
   m_dialogSelectArea.columnStart = m_ColumnStart;
   m_dialogSelectArea.columnEnd = m_ColumnEnd;
   m_dialogSelectArea.columnInterval = m_ColumnInterval;
   
   returnCode = ShowModal();
   
   if (returnCode == wxID_OK)
   {
      continueFlag = TRUE;
      
      m_dialogSelectArea.lineStart = m_LineStart;
      m_dialogSelectArea.lineEnd = m_LineEnd;
      m_dialogSelectArea.lineInterval = 1;
      m_dialogSelectArea.columnStart = m_ColumnStart;
      m_dialogSelectArea.columnEnd = m_ColumnEnd;
      m_dialogSelectArea.columnInterval = 1;
      
      RectifyImageDialogOK (this,
                            m_outputFileInfoPtr,
                            m_fileInfoPtr,
                            m_imageWindowInfoPtr,
                            m_imageLayerInfoPtr,
                            &m_dialogSelectArea,
                            m_reformatOptionsPtr,
                            (SInt16)m_headerOptionsSelection,
                            (Boolean)m_blankOutsideSelectedAreaFlag,
                            (SInt16)m_channelSelection,
                            m_backgroundValue,
                            (SInt16)(m_procedureCode+1),
                            (SInt16)m_resampleMethodCode,
                            m_referenceWindowInfoHandle,
                            m_lineShift,
                            m_columnShift,
                            m_lineScaleFactor,
                            m_columnScaleFactor,
                            m_rotationAngle);
      
   }      // end "if (returnCode == IDOK)"
   
   return (continueFlag);
   
}      // end "DoDialog"


void CMReformatRectifyDlg::OnBnClickedUseMapOrientationAngle(wxCommandEvent& event)
{
   SInt16         numberDecimalPlaces;
   
   wxCheckBox* mapOrientationAngle = (wxCheckBox*) FindWindow(IDC_UseMapOrientationAngle);
   m_useMapOrientationAngleFlag = mapOrientationAngle->GetValue();
   
   m_rotationAngle = 0.;
   numberDecimalPlaces = 1;
   if (m_useMapOrientationAngleFlag)
   {
      m_rotationAngle = m_mapOrientationAngle;
      numberDecimalPlaces = 6;
      
   }      // end "if (m_useMapOrientationAngleFlag)"
   
   wxTextCtrl* rotationAngle = (wxTextCtrl*) FindWindow(IDC_RotationClockwise);
   rotationAngle->ChangeValue(wxString::Format(wxT("%.*f"), numberDecimalPlaces, m_rotationAngle));
   
}      // end "OnBnClickedUseMapOrientationAngle"


void CMReformatRectifyDlg::OnBnClickedTranslateScaleRotateRadio(wxCommandEvent& event)
{
   m_procedureCode = kTranslateScaleRotate - 1;
   UpdateProcedureItems (IDC_LineOffset, m_blankOutsideSelectedAreaFlag);
   
}      // end "OnBnClickedTranslateScaleRotateRadio"


void CMReformatRectifyDlg::OnBnClickedReprojectToRadio(wxCommandEvent& event)
{
   m_procedureCode = kReprojectToReferenceImage - 1;
   UpdateProcedureItems (IDC_LineStart, TRUE);
   
}      // end "OnBnClickedReprojectToRadio"

/*
 //Press keyboard "Enter"  -> wxID_OK
 void CMReformatRectifyDlg::OnButtonPress(wxKeyEvent& event)
 {
 if (event.GetKeyCode() == WXK_RETURN) {
 if(Validate() && TransferDataFromWindow())
 this->EndModal(wxID_OK);
 } else {
 event.Skip();
 }
 }
 */

void CMReformatRectifyDlg::OnCbnSelendokTargetCombo(wxCommandEvent& event)
{
   SInt16   savedFileNamesSelection;
   
   
   // Popup box to select the reference image to register against
   
   if (m_fileNamesSelection >= 0)
   {
      savedFileNamesSelection = m_fileNamesSelection;
      
      m_fileNamesSelection = m_comboBox29->GetSelection();
      
      if (savedFileNamesSelection != m_fileNamesSelection)
         RectifyImageDialogOnReferenceFile (this,
                                            m_procedureCode+1,
                                            m_fileNamesSelection+1,
                                            &m_referenceWindowInfoHandle,
                                            &m_dialogSelectArea);
      
   }      // end "if (m_fileNamesSelection >= 0)"
   
}      // end "OnCbnSelendokTargetCombo"


void CMReformatRectifyDlg::OnCbnSelendokResampleMethod(wxCommandEvent& event)
{
   // Select resampling method popup box
   
   if (m_resampleSelection >= 0)
   {
      m_resampleSelection = m_comboBox30->GetSelection();
      
      SInt64 windowIndex64 = (SInt64)((int*)m_comboBox30->GetClientData(m_resampleSelection));
      m_resampleMethodCode = (UInt32)windowIndex64;
      
   }   // end "if (m_resampleSelection >= 0)"
   
}      // end "OnCbnSelendokResamplemethod"


void CMReformatRectifyDlg::OnEnChangeRotationClockwise(wxCommandEvent& event)
{
   if (m_mapOrientationAngle != 0)
   {
      wxTextCtrl* rotationAngle = (wxTextCtrl*) FindWindow(IDC_RotationClockwise);
      rotationAngle->ChangeValue(wxString::Format(wxT("%.1f"), m_rotationAngle ));
      
      if (m_rotationAngle == m_mapOrientationAngle)
         m_useMapOrientationAngleFlag = TRUE;
      else      // m_rotationAngle != m_mapOrientationAngle
         m_useMapOrientationAngleFlag = FALSE;
      
      wxCheckBox* mapOrientationAngle = (wxCheckBox*) FindWindow(IDC_UseMapOrientationAngle);
      mapOrientationAngle->SetValue(m_useMapOrientationAngleFlag);
      
   } // end "if (m_mapOrientationAngle != 0)"
   
} // end "OnEnChangeRotationClockwise"


void CMReformatRectifyDlg::OnInitDialog(wxInitDialogEvent& event)
{
   CMDialog*                     dialogPtr = this;
   RectifyImageOptionsPtr         rectifyImageOptionsPtr;
   wxComboBox*                     comboBoxPtr;
   
   SInt16         channelSelection,
   fileNamesSelection,
   procedureCode,
   resampleMethodCode,
   headerOptionsSelection;
   
   Boolean         blankOutsideSelectedAreaFlag,
   mapInfoExistsFlag;
   
   CMDialog::OnInitDialog(event);
   
   RectifyImageDialogInitialize (
                                 this,
                                 m_fileInfoPtr,
                                 &m_dialogSelectArea,
                                 m_reformatOptionsPtr,
                                 &headerOptionsSelection,
                                 &channelSelection,
                                 &blankOutsideSelectedAreaFlag,
                                 &m_backgroundValue,
                                 &procedureCode,
                                 &resampleMethodCode,
                                 &fileNamesSelection,
                                 &m_referenceWindowInfoHandle,
                                 (SInt32*)&m_lineShift,
                                 (SInt32*)&m_columnShift,
                                 &m_lineScaleFactor,
                                 &m_columnScaleFactor,
                                 &m_rotationAngle,
                                 &m_mapOrientationAngle);
   
   m_LineStart = m_reformatOptionsPtr->lineStart;
   m_LineEnd = m_reformatOptionsPtr->lineEnd;
   m_ColumnStart = m_reformatOptionsPtr->columnStart;
   m_ColumnEnd = m_reformatOptionsPtr->columnEnd;
   
   m_blankOutsideSelectedAreaFlag = blankOutsideSelectedAreaFlag;
   m_procedureCode = procedureCode - 1;
   m_resampleMethodCode = resampleMethodCode;
   
   // Get the resample method list selection that matches the input
   // resample method code.
   
   m_resampleSelection = GetComboListSelection(IDC_ResampleMethod,
                                               m_resampleMethodCode);
   
   if (m_resampleSelection == -1)
      m_resampleSelection = 0;
   
   m_fileNamesSelection = fileNamesSelection - 1;
   //printf("m_fileNamesSelection:%d\n", m_fileNamesSelection);
   
   // Set text indicating whether the output file format could be
   // GeoTIFF or TIFF.
   mapInfoExistsFlag = FindIfMapInformationExists (gImageWindowInfoPtr);
   
   comboBoxPtr = (wxComboBox*) FindWindow(IDC_Header);
   
   if (mapInfoExistsFlag)
      comboBoxPtr->Append(wxT("GeoTIFF format"));
   else
      comboBoxPtr->Append(wxT("TIFF format"));
   
   comboBoxPtr->SetClientData(0, (void*) m_kNoneMenuItem);
   comboBoxPtr->SetClientData(1, (void*) m_kArcViewMenuItem);
   comboBoxPtr->SetClientData(2, (void*) m_kERDAS74MenuItem);
   comboBoxPtr->SetClientData(3, (void*) m_kTIFFGeoTIFFMenuItem);
   
   m_headerOptionsSelection = headerOptionsSelection;
   m_headerListSelection = GetComboListSelection (IDC_Header,
                                                  m_headerOptionsSelection);
   
   m_localActiveFeaturesPtr = (UInt16*)m_rectifyImageOptionsPtr->rectifyChannelPtr;
   m_channelSelection = channelSelection;
   
   if (TransferDataToWindow())
      PositionDialogWindow();
   
   // Set default text selection to first edit text item
   SelectDialogItemText (this, IDC_LineOffset, 0, SInt16_MAX);
   
}      // end "OnInitDialog"


void CMReformatRectifyDlg::OnSelendokOutChannels(wxCommandEvent& event) {
   m_localActiveNumberFeatures = m_rectifyImageOptionsPtr->numberChannelsToRectify;
   
   HandleChannelsMenu (IDC_Channels,
                       kNoTransformation,
                       (SInt16) gImageWindowInfoPtr->totalNumberChannels,
                       kImageChannelType,
                       TRUE);
   
   m_rectifyImageOptionsPtr->numberChannelsToRectify = m_localActiveNumberFeatures;
   
} // end "OnSelendokOutChannels"


void CMReformatRectifyDlg::UpdateProcedureItems(
                                                int   selectItemNumber,
                                                Boolean  blankOutsideSelectedAreaFlag)
{
   RectifyImageDialogOnRectifyCode (this,
                                    m_procedureCode+1,
                                    blankOutsideSelectedAreaFlag,
                                    m_mapOrientationAngle);
   
   RectifyImageDialogOnReferenceFile (this,
                                      m_procedureCode+1,
                                      m_fileNamesSelection+1,
                                      &m_referenceWindowInfoHandle,
                                      &m_dialogSelectArea);
   
   // Set default text selection to first edit text item
   
   SelectDialogItemText (this, selectItemNumber, 0, SInt16_MAX);
   
}      // end "UpdateProcedureItems"


bool CMReformatRectifyDlg::TransferDataFromWindow()
{
   SInt16      continueCode = 1,
   returnCode = 0;
   
   
   wxTextCtrl* c_end = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
   wxTextCtrl* c_start = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*) FindWindow(IDC_LineEnd);
   wxTextCtrl* l_start = (wxTextCtrl*) FindWindow(IDC_LineStart);
   
   wxTextCtrl* backgroundValue = (wxTextCtrl*) FindWindow(IDC_BackgroundValue);
   wxTextCtrl* lineShift = (wxTextCtrl*) FindWindow(IDC_LineOffset);
   wxTextCtrl* columnShift = (wxTextCtrl*) FindWindow(IDC_ColumnOffset);
   wxTextCtrl* lineScaleFactor = (wxTextCtrl*) FindWindow(IDC_LineScale);
   wxTextCtrl* columnScaleFactor = (wxTextCtrl*) FindWindow(IDC_ColumnScale);
   wxTextCtrl* rotationAngle = (wxTextCtrl*) FindWindow(IDC_RotationClockwise);
   wxCheckBox* mapOrientationAngle = (wxCheckBox*) FindWindow(IDC_UseMapOrientationAngle);
   wxComboBox* header = (wxComboBox*) FindWindow(IDC_Header);
   wxComboBox* outChannels = (wxComboBox*) FindWindow(IDC_Channels);
   //wxStaticText *linearText = (wxStaticText*) FindWindow(IDC_TranslateScaleTitle);
   //wxStaticText *offsetText = (wxStaticText*) FindWindow(IDC_TranslateScaleTitle2);
   //wxStaticText *scaleText = (wxStaticText*) FindWindow(IDC_TranslateScaleTitle3);
   //wxStaticText *lineText = (wxStaticText*) FindWindow(IDC_LineTranslateScalePrompt);
   //wxStaticText *columnText = (wxStaticText*) FindWindow(IDC_ColumnTranslateScalePrompt);
   //wxStaticText *rotationText = (wxStaticText*) FindWindow(IDC_RotationTitle);
   //wxStaticText *degreeText = (wxStaticText*) FindWindow(IDC_RotationClockwisePrompt);
   //wxStaticText *resampleMethodText = (wxStaticText*) FindWindow(IDC_ResampleMethodPrompt);
   //wxStaticText *referenceFileListText = (wxStaticText*) FindWindow(IDC_ReferenceFileListPrompt);
   
   m_LineStartString = l_start->GetValue();
   m_LineEndString = l_end->GetValue();
   m_ColumnStartString = c_start->GetValue();
   m_ColumnEndString = c_end->GetValue();
   
   m_LineStart = wxAtoi(m_LineStartString);
   m_LineEnd = wxAtoi(m_LineEndString);
   m_ColumnStart = wxAtoi(m_ColumnStartString);
   m_ColumnEnd = wxAtoi(m_ColumnEndString);
   
   m_backgroundValueString = backgroundValue->GetValue();
   m_lineOffsetString = lineShift->GetValue();
   m_columnOffsetString = columnShift->GetValue();
   m_lineScaleString = lineScaleFactor->GetValue();
   m_columnScaleString = columnScaleFactor->GetValue();
   m_rotationAngleString = rotationAngle->GetValue();
   
   m_backgroundValue = wxAtof(m_backgroundValueString);
   m_lineShift = wxAtoi(m_lineOffsetString);
   m_columnShift = wxAtoi(m_columnOffsetString);
   m_lineScaleFactor = wxAtof(m_lineScaleString);
   m_columnScaleFactor = wxAtof(m_columnScaleString);
   m_rotationAngle = wxAtof(m_rotationAngleString);
   
   m_headerListSelection = header->GetSelection();
   SInt64 headerList =  (SInt64)((int*)header->GetClientData(m_headerListSelection));
   m_headerOptionsSelection = (SInt16)headerList;
   
   m_channelSelection = outChannels->GetSelection();
   if(m_channelSelection < 0)
      m_channelSelection = m_channelSelection_Saved;
   
   returnCode = CheckDialogRealValue (this,
                                      IDC_BackgroundValue,
                                      m_minBackgroundValue,
                                      m_maxBackgroundValue,
                                      0,
                                      true,      // minValue is allowed.
                                      true,      // maxValue is allowed,
                                      kDisplayRangeAlert);
   
   if (m_procedureCode == kTranslateScaleRotate - 1) {
      if (returnCode == 0) {
         continueCode = CheckMaxValue (this,
                                       IDC_LineOffset,
                                       -kMaxNumberLines,
                                       kMaxNumberLines,
                                       kDisplayRangeAlert);
         if (continueCode != 1)
            returnCode = IDC_LineOffset;
      }
      
      if (returnCode == 0) {
         continueCode = CheckMaxValue (this,
                                       IDC_ColumnOffset,
                                       -kMaxNumberLines,
                                       kMaxNumberLines,
                                       kDisplayRangeAlert);
         if (continueCode != 1)
            returnCode = IDC_ColumnOffset;
      }
      
      
      if (returnCode == 0)
         returnCode = CheckDialogRealValue (this,
                                            IDC_LineScale,
                                            0,
                                            1000,
                                            0,
                                            false,   // minValue not allowed.
                                            true,      // maxValue is allowed,
                                            kDisplayRangeAlert);
      
      if (returnCode == 0)
         returnCode = CheckDialogRealValue (this,
                                            IDC_ColumnScale,
                                            0,
                                            1000,
                                            0,
                                            false,   // minValue not allowed.
                                            true,      // maxValue is allowed,
                                            kDisplayRangeAlert);
      
      if (returnCode == 0)
         returnCode = CheckDialogRealValue (this,
                                            IDC_RotationClockwise,
                                            -180,
                                            180,
                                            0,
                                            true,   // minValue is allowed.
                                            true,      // maxValue is allowed,
                                            kDisplayRangeAlert);
      
   }      // end "if (m_procedureCode == kTranslateScaleRotate - 1)"
   
   if (returnCode == 0)
      returnCode = VerifyLineColumnValues(
                                          IDC_LineStart,
                                          IDC_ColumnStart,
                                          false);
   
   return (returnCode == 0);
   
}      // end "TransferDataFromWindow"


bool CMReformatRectifyDlg::TransferDataToWindow() {
   wxTextCtrl* c_end = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
   wxTextCtrl* c_start = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
   wxTextCtrl* l_end = (wxTextCtrl*) FindWindow(IDC_LineEnd);
   wxTextCtrl* l_start = (wxTextCtrl*) FindWindow(IDC_LineStart);
   
   wxTextCtrl* backgroundValue = (wxTextCtrl*) FindWindow(IDC_BackgroundValue);
   wxTextCtrl* lineShift = (wxTextCtrl*) FindWindow(IDC_LineOffset);
   wxTextCtrl* columnShift = (wxTextCtrl*) FindWindow(IDC_ColumnOffset);
   wxTextCtrl* lineScaleFactor = (wxTextCtrl*) FindWindow(IDC_LineScale);
   wxTextCtrl* columnScaleFactor = (wxTextCtrl*) FindWindow(IDC_ColumnScale);
   wxTextCtrl* rotationAngle = (wxTextCtrl*) FindWindow(IDC_RotationClockwise);
   wxCheckBox* mapOrientationAngle = (wxCheckBox*) FindWindow(IDC_UseMapOrientationAngle);
   wxComboBox* header = (wxComboBox*) FindWindow(IDC_Header);
   wxComboBox* outChannels = (wxComboBox*) FindWindow(IDC_Channels);
   //wxStaticText *linearText = (wxStaticText*) FindWindow(IDC_TranslateScaleTitle);
   //wxStaticText *offsetText = (wxStaticText*) FindWindow(IDC_TranslateScaleTitle2);
   //wxStaticText *scaleText = (wxStaticText*) FindWindow(IDC_TranslateScaleTitle3);
   //wxStaticText *lineText = (wxStaticText*) FindWindow(IDC_LineTranslateScalePrompt);
   //wxStaticText *columnText = (wxStaticText*) FindWindow(IDC_ColumnTranslateScalePrompt);
   //wxStaticText *rotationText = (wxStaticText*) FindWindow(IDC_RotationTitle);
   //wxStaticText *degreeText = (wxStaticText*) FindWindow(IDC_RotationClockwisePrompt);
   //wxStaticText *resampleMethodText = (wxStaticText*) FindWindow(IDC_ResampleMethodPrompt);
   //wxStaticText *referenceFileListText = (wxStaticText*) FindWindow(IDC_ReferenceFileListPrompt);
   
   m_radioBtn18->SetValue(m_procedureCode == kTranslateScaleRotate-1);
   m_radioBtn19->SetValue(m_procedureCode == kReprojectToReferenceImage-1);
   
   l_start->ChangeValue(wxString::Format(wxT("%d"), m_LineStart ));
   l_end->ChangeValue(wxString::Format(wxT("%d"), m_LineEnd ));
   c_start->ChangeValue(wxString::Format(wxT("%d"), m_ColumnStart ));
   c_end->ChangeValue(wxString::Format(wxT("%d"), m_ColumnEnd ));
   
   backgroundValue->ChangeValue(wxString::Format(wxT("%.1f"), m_backgroundValue ));
   lineShift->ChangeValue(wxString::Format(wxT("%ld"), m_lineShift ));
   columnShift->ChangeValue(wxString::Format(wxT("%ld"), m_columnShift ));
   lineScaleFactor->ChangeValue(wxString::Format(wxT("%.1f"), m_lineScaleFactor ));
   columnScaleFactor->ChangeValue(wxString::Format(wxT("%.1f"), m_columnScaleFactor ));
   rotationAngle->ChangeValue(wxString::Format(wxT("%.1f"), m_rotationAngle ));
   mapOrientationAngle->SetValue(m_useMapOrientationAngleFlag);
   outChannels->SetSelection(m_channelSelection);
   header->SetSelection(m_headerListSelection);
   
   return true;
   
}      // end "TransferDataToWindow"

