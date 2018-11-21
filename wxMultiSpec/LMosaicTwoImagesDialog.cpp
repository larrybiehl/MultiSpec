//                               MultiSpec
//
//               Laboratory for Applications of Remote Sensing
//                         Purdue University
//                        West Lafayette, IN 47907
//                         Copyright (2009-2018)
//                     (c) Purdue Research Foundation
//                           All rights reserved.
//
//   File:                  LMosaicTwoImagesDialog.cpp : class implementation file
//   Class Definition:      LMosaicTwoImagesDialog.h
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
//                        CMMosaicTwoImagesDialog class.
//
//------------------------------------------------------------------------------------

#include   "SMultiSpec.h"

#include "CImageWindow.h"

#include "LChannelsDialog.h"
#include "LImageView.h"
#include "LImage_dialog.cpp"
#include "LMosaicTwoImagesDialog.h"

extern void         MosaicTwoImagesDialogInitialize (
                                                     DialogPtr                     dialogPtr,
                                                     ReformatOptionsPtr            reformatOptionsPtr,
                                                     FileInfoPtr                     leftTopFileInfoPtr,
                                                     FileInfoPtr                     outFileInfoPtr,
                                                     SInt16*                        mosaicDirectionCodePtr,
                                                     DialogSelectArea*               leftTopDialogSelectAreaPtr,
                                                     DialogSelectArea*               rightBottomDialogSelectAreaPtr,
                                                     SInt16*                        fileNamesSelectionPtr,
                                                     Handle*                        rightBottomWindowInfoHandlePtr,
                                                     Boolean*                        ignoreBackgroundFlagPtr,
                                                     SInt32*                        maxDataValuePtr,
                                                     SInt32*                        newBackgroundValuePtr,
                                                     SInt16*                        headerOptionsSelectionPtr);

extern void         MosaicTwoImagesDialogOK (
                                             ReformatOptionsPtr            reformatOptionsPtr,
                                             FileInfoPtr                     outFileInfoPtr,
                                             SInt16                        mosaicDirectionCode,
                                             Handle                        rightBottomWindowInfoHandle,
                                             DialogSelectArea*               leftTopDialogSelectAreaPtr,
                                             DialogSelectArea*               rightBottomDialogSelectAreaPtr,
                                             Boolean                        ignoreBackgroundFlag,
                                             SInt32                        newBackgroundValue,
                                             Boolean                        channelDescriptionsFlag,
                                             SInt16                        headerFormat);

extern void         MosaicTwoImagesDialogOnSelectRightBottomImage (
                                                                   DialogPtr                     dialogPtr,
                                                                   DialogSelectArea*               leftTopDialogSelectAreaPtr,
                                                                   DialogSelectArea*               rightBottomDialogSelectAreaPtr,
                                                                   SInt16                        fileNamesSelection,
                                                                   Handle*                        rightOrBottomMosaicWindowInfoHandlePtr,
                                                                   SInt16                        mosaicDirectionCode);

extern void         MosaicTwoImagesDialogUpdateBackGroundValuesItem (
                                                                     DialogPtr                     dialogPtr,
                                                                     Boolean                        ignoreBackgroundFlag);

extern void         MosaicTwoImagesDialogUpdateChannelDescriptionItem (
                                                                       DialogPtr                     dialogPtr,
                                                                       SInt16                        headerFormat,
                                                                       Boolean                        thematicTypeFlag);

extern void         MosaicTwoImagesDialogUpdateDirectionItems (
                                                               DialogPtr                     dialogPtr,
                                                               DialogSelectArea*               leftTopDialogSelectAreaPtr,
                                                               DialogSelectArea*               rightBottomDialogSelectAreaPtr,
                                                               Handle                        rightBottomWindowInfoHandle,
                                                               SInt16                        mosaicDirectionCode,
                                                               Boolean                        ignoreBackgroundFlag);

extern void         MosaicTwoImagesDialogUpdateLineColumnEnds (
                                                               DialogPtr                     dialogPtr,
                                                               DialogSelectArea*               topLeftDialogSelectAreaPtr,
                                                               DialogSelectArea*               dialogSelectAreaPtr,
                                                               SInt16                        mosaicDirectionCode,
                                                               Handle                        rightOrBottomMosaicWindowInfoHandle,
                                                               UInt16                        itemSelected);

extern SInt16      MosaicTwoImagesDialogVerifyLineColumnSettings (
                                                                  DialogSelectArea*               leftTopDialogSelectAreaPtr,
                                                                  DialogSelectArea*               rightBottomDialogSelectAreaPtr,
                                                                  SInt16                        mosaicDirectionCode);

// CMMosaicTwoImagesDialog dialog
CMMosaicTwoImagesDialog::CMMosaicTwoImagesDialog(wxWindow* pParent,
                                                 wxWindowID id, const wxString& title/*=NULL*/)
:CMDialog(CMMosaicTwoImagesDialog::IDD, pParent, title)
{
   m_mosaicDirectionCode = -1;
   m_rightFileNameSelection = -1;
   m_bottomFileNameSelection = -1;
   m_ignoreBackgroundValueFlag = FALSE;
   m_backgroundValue = 0;
   m_writeChannelDescriptionsFlag = FALSE;
   m_headerListSelection = -1;
   
   m_initializedFlag = CMDialog::m_initializedFlag;
   
   if (m_dialogSelectArea.imageWindowInfoPtr != NULL) {
      m_maxNumberLines = m_dialogSelectArea.imageWindowInfoPtr->maxNumberLines;
      m_maxNumberColumns = m_dialogSelectArea.imageWindowInfoPtr->maxNumberColumns;
      
   }// end "m_dialogSelectArea.imageWindowInfoPtr != NULL"
   
   else // m_dialogSelectArea.imageWindowInfoPtr == NULL
   {
      m_maxNumberLines = 1;
      m_maxNumberColumns = 1;
      
   } // end "else m_dialogSelectArea.imageWindowInfoPtr == NULL"
   
   CreateControls();
   SetSizerAndFit(bSizer119);
}

CMMosaicTwoImagesDialog::~CMMosaicTwoImagesDialog()
{
}

BEGIN_EVENT_TABLE(CMMosaicTwoImagesDialog, CMDialog)
EVT_INIT_DIALOG(CMMosaicTwoImagesDialog::OnInitDialog)
EVT_COMBOBOX(IDC_MosaicDirection, CMMosaicTwoImagesDialog::OnCbnSelChangeMosaicDirection)
EVT_COMBOBOX(IDC_HeaderFormatList, CMMosaicTwoImagesDialog::OnCbnSelChangeHeaderFormatList)
EVT_COMBOBOX(IDC_RightImageFileList, CMMosaicTwoImagesDialog::OnCbnSelChangeRightImageFileList)
EVT_COMBOBOX(IDC_BottomImageFileList, CMMosaicTwoImagesDialog::OnCbnSelChangeBottomImageFileList)
EVT_TEXT(IDC_LineStart, CMMosaicTwoImagesDialog::OnEnChangeLeftTopLineStart)
EVT_TEXT(IDC_LineEnd, CMMosaicTwoImagesDialog::OnEnChangeLeftTopLineEnd)
EVT_TEXT(IDC_ColumnStart, CMMosaicTwoImagesDialog::OnEnChangeLeftTopColumnStart)
EVT_TEXT(IDC_ColumnEnd, CMMosaicTwoImagesDialog::OnEnChangeLeftTopColumnEnd)
EVT_TEXT(IDC_LineStart2, CMMosaicTwoImagesDialog::OnEnChangeRightLineStart2)
EVT_TEXT(IDC_ColumnEnd3, CMMosaicTwoImagesDialog::OnEnChangeBottomColumnStart3)
EVT_CHECKBOX(IDC_IgnoreBackgroundValue, CMMosaicTwoImagesDialog::OnBnClickedIgnoreBackgroundValue)
EVT_BUTTON(IDEntireImage, CMMosaicTwoImagesDialog::ToEntireImage)
EVT_BUTTON(IDSelectedImage, CMMosaicTwoImagesDialog::ToSelectedImage)
//EVT_CHAR_HOOK(CMMosaicTwoImagesDialog::OnButtonPress)
END_EVENT_TABLE()


void CMMosaicTwoImagesDialog::CheckBottomImageColumns(
                                                      UInt16                     itemSelected)
{
   if ((m_dialogSelectArea.columnEnd -
        m_dialogSelectArea.columnStart) !=
       (m_rightBottomDialogSelectArea.columnEnd -
        m_rightBottomDialogSelectArea.columnStart))
      MosaicTwoImagesDialogUpdateLineColumnEnds (
                                                 this,
                                                 &m_dialogSelectArea,
                                                 &m_rightBottomDialogSelectArea,
                                                 m_mosaicDirectionCode+1,
                                                 m_rightBottomWindowInfoHandle,
                                                 itemSelected);
   
}      // end "CheckBottomImageColumns"


void CMMosaicTwoImagesDialog::CheckRightImageLines(
                                                   UInt16                     itemSelected)
{
   /*
    int numberChars = sprintf ((char*)&gTextString3,
    " CheckRightImageLines (lines start end): %d, %d, %d, %d%s",
    m_dialogSelectArea.lineStart,
    m_dialogSelectArea.lineEnd,
    m_rightBottomDialogSelectArea.lineStart,
    m_rightBottomDialogSelectArea.lineEnd,
    gEndOfLine);
    ListString ((char*)&gTextString3, numberChars, gOutputTextH);
    */
   if ((m_dialogSelectArea.lineEnd -
        m_dialogSelectArea.lineStart) !=
       (m_rightBottomDialogSelectArea.lineEnd -
        m_rightBottomDialogSelectArea.lineStart))
      MosaicTwoImagesDialogUpdateLineColumnEnds (
                                                 this,
                                                 &m_dialogSelectArea,
                                                 &m_rightBottomDialogSelectArea,
                                                 m_mosaicDirectionCode+1,
                                                 m_rightBottomWindowInfoHandle,
                                                 itemSelected);
   
}      // end "CheckRightImageLines"


void CMMosaicTwoImagesDialog::CreateControls ()

{
   this->SetSizeHints( wxDefaultSize, wxDefaultSize );
   
   wxBitmap entireimi = wxBITMAP_PNG_FROM_DATA(entireim);
   wxBitmap toentirei = wxBITMAP_PNG_FROM_DATA(toentire);
   wxBitmap selectedi = wxBITMAP_PNG_FROM_DATA(selected);
   wxBitmap bmp4i = wxBITMAP_PNG_FROM_DATA(bmp4);
   
   
   bSizer119 = new wxBoxSizer (wxVERTICAL);
   
   wxBoxSizer* bSizer245;
   bSizer245 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText248 = new wxStaticText( this, wxID_ANY, wxT("Mosaic:"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText248->Wrap( -1 );
   bSizer245->Add( m_staticText248, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   
   m_comboBox45 = new wxComboBox( this, IDC_MosaicDirection, wxT("Left-Right"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
   m_comboBox45->Append("Left-Right");
   m_comboBox45->Append("Top-Bottom");
   SetUpToolTip(m_comboBox45, IDS_ToolTip198);
   bSizer245->Add( m_comboBox45, 0, wxALL, 5 );
   
   
   //bSizer119->Add (bSizer245, 0, wxEXPAND, 5);
   bSizer119->Add (bSizer245, 0, wxEXPAND | wxTOP|wxLEFT|wxRIGHT, 12);
   
   wxBoxSizer* bSizer247;
   bSizer247 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText249 = new wxStaticText( this, IDC_LeftTopPrompt, wxT("Left"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText249->Wrap( -1 );
   bSizer247->Add( m_staticText249, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   
   m_staticText251 = new wxStaticText( this, IDC_LeftTopImage, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText251->Wrap( -1 );
   SetUpToolTip(m_staticText251, IDS_ToolTip200);
   bSizer247->Add( m_staticText251, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   
   
   bSizer247->Add( 0, 0, 1, wxEXPAND, 5 );
   
   m_staticText250 = new wxStaticText( this, IDC_RightPrompt, wxT("Right:"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText250->Wrap( -1 );
   bSizer247->Add( m_staticText250, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   
   m_comboBox46 = new wxComboBox( this, IDC_RightImageFileList, wxT(""), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
   SetUpToolTip(m_comboBox46, IDS_ToolTip199);
   bSizer247->Add( m_comboBox46, 0, wxALL, 5 );
   m_comboBox46->SetMinSize(wxSize(250, -1));
   
   
   //bSizer119->Add (bSizer247, 0, wxEXPAND, 5);
   bSizer119->Add (bSizer247, 0, wxEXPAND | wxLEFT|wxRIGHT, 12);
   
   wxGridSizer* gSizer13;
   gSizer13 = new wxGridSizer( 0, 2, 0, 0 );
   
   wxStaticBoxSizer* leftImageSettingsSizer;
   wxStaticBox* leftImageSettingsBox = new wxStaticBox (this, IDC_LeftImageSettings, wxT("Left Image Settings"));
   leftImageSettingsSizer = new wxStaticBoxSizer (leftImageSettingsBox, wxVERTICAL);
   
   wxBoxSizer* bSizer791;
   bSizer791 = new wxBoxSizer( wxHORIZONTAL );
   
   m_bpButton4 = new wxBitmapButton(leftImageSettingsBox, IDSelectedImage, selectedi, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
   m_bpButton4->SetBitmapDisabled(selectedi);
   m_bpButton4->Hide();
   //bSizer791->Add (m_bpButton4, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
   bSizer791->Add (m_bpButton4,
                   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   m_bpButton5 = new wxBitmapButton(leftImageSettingsBox, IDEntireImage, entireimi, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
   m_bpButton5->SetBitmapDisabled(entireimi);
   
   //bSizer791->Add (m_bpButton5, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
   bSizer791->Add (m_bpButton5,
                   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   wxBoxSizer* bSizer299;
   bSizer299 = new wxBoxSizer( wxVERTICAL );
   
   wxBoxSizer* bSizer300;
   bSizer300 = new wxBoxSizer( wxHORIZONTAL );
   
   
   bSizer300->Add( 100, 0, 0, wxEXPAND, 5 );
   
   m_staticText307 = new wxStaticText( leftImageSettingsBox, IDC_StartTitleMosaic, wxT("Start"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText307->Wrap( -1 );
   bSizer300->Add( m_staticText307, 0, wxALL, 5 );
   
   
   bSizer300->Add( 50, 0, 0, wxEXPAND, 5 );
   
   m_staticText308 = new wxStaticText( leftImageSettingsBox, IDC_EndTitleMosaic, wxT("End"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText308->Wrap( -1 );
   bSizer300->Add( m_staticText308, 0, wxALL, 5 );
   
   
   bSizer299->Add( bSizer300, 0, wxEXPAND, 5 );
   
   wxBoxSizer* bSizer301;
   bSizer301 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText309 = new wxStaticText( leftImageSettingsBox, wxID_ANY, wxT("Lines"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText309->Wrap( -1 );
   bSizer301->Add( m_staticText309, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   
   
   bSizer301->Add( 15, 0, 0, wxEXPAND, 5 );
   
   m_textCtrl141 = new wxTextCtrl( leftImageSettingsBox, IDC_LineStart, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   m_textCtrl141->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_LineStartString));
   bSizer301->Add( m_textCtrl141, 0, wxALL, 5 );
   
   m_textCtrl142 = new wxTextCtrl( leftImageSettingsBox, IDC_LineEnd, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   m_textCtrl142->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_LineEndString));
   bSizer301->Add( m_textCtrl142, 0, wxALL, 5 );
   
   m_textCtrl143 = new wxTextCtrl( leftImageSettingsBox, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   m_textCtrl143->Hide();
   bSizer301->Add( m_textCtrl143, 0, wxALL, 5 );
   
   
   bSizer299->Add( bSizer301, 0, wxEXPAND, 5 );
   
   wxBoxSizer* bSizer303;
   bSizer303 = new wxBoxSizer (wxHORIZONTAL);
   
   m_staticText310 = new wxStaticText (leftImageSettingsBox, wxID_ANY, wxT("Column"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText310->Wrap (-1);
   //bSizer303->Add (m_staticText310, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   bSizer303->Add (m_staticText310, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   m_textCtrl144 = new wxTextCtrl( leftImageSettingsBox, IDC_ColumnStart, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   m_textCtrl144->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_ColumnStartString));
   bSizer303->Add( m_textCtrl144, 0, wxALL, 5 );
   
   m_textCtrl145 = new wxTextCtrl( leftImageSettingsBox, IDC_ColumnEnd, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   m_textCtrl145->SetValidator(wxTextValidator(wxFILTER_DIGITS, &m_ColumnEndString));
   bSizer303->Add( m_textCtrl145, 0, wxALL, 5 );
   
   m_textCtrl146 = new wxTextCtrl( leftImageSettingsBox, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   m_textCtrl146->Hide();
   bSizer303->Add( m_textCtrl146, 0, wxALL, 5 );
   
   
   bSizer299->Add( bSizer303, 0, wxEXPAND, 5 );
   
   
   bSizer791->Add( bSizer299, 1, wxEXPAND, 5 );
   
   SetUpToolTip(m_bpButton5, IDS_ToolTip40);
   SetUpToolTip(m_textCtrl141, IDS_ToolTip19);
   SetUpToolTip(m_textCtrl142, IDS_ToolTip20);
   
   SetUpToolTip(m_textCtrl144, IDS_ToolTip22);
   SetUpToolTip(m_textCtrl145, IDS_ToolTip23);
   
   leftImageSettingsSizer->Add( bSizer791, 1, wxEXPAND, 5 );
   
   //gSizer13->Add (leftImageSettingsSizer, 1, wxEXPAND, 5);
   gSizer13->Add (leftImageSettingsSizer, 1, wxEXPAND | wxRIGHT|wxBOTTOM, 6);
   
   wxStaticBoxSizer* rightImageSettingsSizer;
   wxStaticBox* rightImageSettingsBox = new wxStaticBox (this, IDC_RightImageSettings, wxT("Right Image Settings"));
   rightImageSettingsSizer = new wxStaticBoxSizer (rightImageSettingsBox, wxVERTICAL );
   
   wxBoxSizer* bSizer304;
   
   bSizer304 = new wxBoxSizer( wxHORIZONTAL );
   
   m_bpButton52 = new wxBitmapButton(rightImageSettingsBox, IDEntireImage, entireimi, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
   
   m_bpButton52->SetBitmapDisabled(entireimi);
   m_bpButton52->Hide();
   bSizer304->Add( m_bpButton52, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   
   wxBoxSizer* bSizer305;
   bSizer305 = new wxBoxSizer( wxVERTICAL );
   
   wxBoxSizer* bSizer306;
   bSizer306 = new wxBoxSizer( wxHORIZONTAL );
   
   
   bSizer306->Add( 100, 0, 0, wxEXPAND, 5 );
   
   m_staticText311 = new wxStaticText( rightImageSettingsBox, IDC_StartTitleMosaic2, wxT("Start"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText311->Wrap( -1 );
   bSizer306->Add( m_staticText311, 0, wxALL, 5 );
   
   
   bSizer306->Add( 50, 0, 0, wxEXPAND, 5 );
   
   m_staticText312 = new wxStaticText( rightImageSettingsBox, IDC_EndTitleMosaic2, wxT("End"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText312->Wrap( -1 );
   bSizer306->Add( m_staticText312, 0, wxALL, 5 );
   
   
   bSizer305->Add( bSizer306, 0, wxEXPAND, 5 );
   
   wxBoxSizer* bSizer307;
   bSizer307 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText313 = new wxStaticText( rightImageSettingsBox, IDC_LinePrompt2, wxT("Lines"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText313->Wrap (-1);
   //bSizer307->Add (m_staticText313, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
   bSizer307->Add (m_staticText313,
                   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   
   bSizer307->Add( 15, 0, 0, wxEXPAND, 5 );
   
   m_textCtrl147 = new wxTextCtrl( rightImageSettingsBox, IDC_LineStart2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   bSizer307->Add( m_textCtrl147, 0, wxALL, 5 );
   
   m_staticText148 = new wxStaticText( rightImageSettingsBox, IDC_LineEnd2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   bSizer307->Add( m_staticText148, 0, wxALL, 5 );
   
   
   bSizer305->Add( bSizer307, 0, wxEXPAND, 5 );
   
   wxBoxSizer* bSizer308;
   bSizer308 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText314 = new wxStaticText( rightImageSettingsBox, IDC_ColumnPrompt2, wxT("Column"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText314->Wrap( -1 );
   //bSizer308->Add (m_staticText314, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
   bSizer308->Add (m_staticText314,
                   wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   m_textCtrl149 = new wxTextCtrl( rightImageSettingsBox, IDC_ColumnStart2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   bSizer308->Add( m_textCtrl149, 0, wxALL, 5 );
   
   m_textCtrl150 = new wxTextCtrl( rightImageSettingsBox, IDC_ColumnEnd2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   bSizer308->Add( m_textCtrl150, 0, wxALL, 5 );
   
   
   bSizer305->Add( bSizer308, 0, wxEXPAND, 5 );
   
   
   bSizer304->Add( bSizer305, 1, wxEXPAND, 5 );
   
   SetUpToolTip(m_bpButton52, IDS_ToolTip40);
   SetUpToolTip(m_textCtrl147, IDS_ToolTip19);
   SetUpToolTip(m_staticText148, IDS_ToolTip20);
   
   SetUpToolTip(m_textCtrl149, IDS_ToolTip22);
   SetUpToolTip(m_textCtrl150, IDS_ToolTip23);
   
   rightImageSettingsSizer->Add( bSizer304, 0, wxEXPAND, 5 );
   
   
   //gSizer13->Add (rightImageSettingsSizer, 1, wxEXPAND, 5);
   gSizer13->Add (rightImageSettingsSizer, 1, wxEXPAND | wxLEFT|wxBOTTOM, 6);
   
   wxStaticBoxSizer* bottomImageSettingsSizer;
   wxStaticBox* bottomImageSettingsBox = new wxStaticBox (this, IDC_BottomImageSettings, wxT("Bottom Image Settings"));
   bottomImageSettingsSizer = new wxStaticBoxSizer (bottomImageSettingsBox, wxVERTICAL);
   
   wxBoxSizer* bSizer287;
   bSizer287 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText291 = new wxStaticText (bottomImageSettingsBox,
                                       IDC_BottomPrompt,
                                       wxT("Bottom:"),
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       0 );
   m_staticText291->Wrap (-1);
   //bSizer287->Add (m_staticText291, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
   bSizer287->Add (m_staticText291, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   m_comboBox48 = new wxComboBox( bottomImageSettingsBox, IDC_BottomImageFileList, wxT("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
   SetUpToolTip(m_comboBox48, IDS_ToolTip201);
   bSizer287->Add( m_comboBox48, 0, wxALL, 5 );
   m_comboBox48->SetMinSize(wxSize(250, -1));
   
   
   bottomImageSettingsSizer->Add( bSizer287, 0, wxEXPAND, 5 );
   
   wxBoxSizer* bSizer289;
   bSizer289 = new wxBoxSizer( wxHORIZONTAL );
   
   m_bpButton51 = new wxBitmapButton(bottomImageSettingsBox, IDEntireImage, entireimi, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
   
   m_bpButton51->SetBitmapDisabled(toentirei);
   m_bpButton51->Hide();
   //bSizer289->Add ( m_bpButton51, 0, wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT|wxALL, 5);
   bSizer289->Add (m_bpButton51, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   wxBoxSizer* bSizer295;
   bSizer295 = new wxBoxSizer( wxVERTICAL );
   
   wxBoxSizer* bSizer296;
   bSizer296 = new wxBoxSizer( wxHORIZONTAL );
   
   
   bSizer296->Add( 100, 0, 0, wxEXPAND, 5 );
   
   m_staticText303 = new wxStaticText( bottomImageSettingsBox, IDC_StartTitleMosaic3, wxT("Start"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText303->Wrap( -1 );
   bSizer296->Add( m_staticText303, 0, wxALL, 5 );
   
   
   bSizer296->Add( 50, 0, 0, wxEXPAND, 5 );
   
   m_staticText304 = new wxStaticText( bottomImageSettingsBox, IDC_EndTitleMosaic3, wxT("End"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText304->Wrap( -1 );
   bSizer296->Add( m_staticText304, 0, wxALL, 5 );
   
   
   bSizer295->Add( bSizer296, 0, wxEXPAND, 5 );
   
   wxBoxSizer* bSizer297;
   bSizer297 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText305 = new wxStaticText (bottomImageSettingsBox, IDC_LinePrompt3, wxT("Lines"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText305->Wrap (-1);
   //bSizer297->Add (m_staticText305, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   bSizer297->Add (m_staticText305, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   bSizer297->Add( 15, 0, 0, wxEXPAND, 5 );
   
   m_textCtrl137 = new wxTextCtrl( bottomImageSettingsBox, IDC_LineStart3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   bSizer297->Add( m_textCtrl137, 0, wxALL, 5 );
   
   m_textCtrl138 = new wxTextCtrl( bottomImageSettingsBox, IDC_LineEnd3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   bSizer297->Add( m_textCtrl138, 0, wxALL, 5 );
   
   
   bSizer295->Add( bSizer297, 0, wxEXPAND, 5 );
   
   wxBoxSizer* bSizer298;
   bSizer298 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText306 = new wxStaticText( bottomImageSettingsBox, IDC_ColumnPrompt3, wxT("Column"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText306->Wrap( -1 );
   //bSizer298->Add (m_staticText306, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   bSizer298->Add (m_staticText306, wxSizerFlags(0).Align(wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
   
   m_textCtrl139 = new wxTextCtrl( bottomImageSettingsBox, IDC_ColumnStart3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   bSizer298->Add( m_textCtrl139, 0, wxALL, 5 );
   
   m_textCtrl140 = new wxStaticText( bottomImageSettingsBox, IDC_ColumnEnd3, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   bSizer298->Add( m_textCtrl140, 0, wxALL, 5 );
   
   
   bSizer295->Add( bSizer298, 0, wxEXPAND, 5 );
   
   
   bSizer289->Add( bSizer295, 1, wxEXPAND, 5 );
   
   SetUpToolTip(m_bpButton51, IDS_ToolTip40);
   SetUpToolTip(m_textCtrl137, IDS_ToolTip19);
   SetUpToolTip(m_textCtrl138, IDS_ToolTip20);
   
   SetUpToolTip(m_textCtrl139, IDS_ToolTip22);
   SetUpToolTip(m_textCtrl140, IDS_ToolTip23);
   
   
   bottomImageSettingsSizer->Add( bSizer289, 0, wxEXPAND, 5 );
   
   
   //gSizer13->Add (bottomImageSettingsSizer, 1, wxEXPAND, 5);
   gSizer13->Add (bottomImageSettingsSizer, 1, wxEXPAND | wxRIGHT|wxTOP, 6);
   
   wxStaticBoxSizer* optionsSizer;
   wxStaticBox* optionsBox = new wxStaticBox(this, wxID_ANY, wxT("Options"));
   optionsSizer = new wxStaticBoxSizer (optionsBox, wxVERTICAL);
   
   wxBoxSizer* bSizer283;
   bSizer283 = new wxBoxSizer( wxHORIZONTAL );
   
   m_checkBox21 = new wxCheckBox( optionsBox, IDC_IgnoreBackgroundValue, wxT("Ignore Background Value:"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox21, IDS_ToolTip194);
   bSizer283->Add( m_checkBox21, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5 );
   
   m_textCtrl136 = new wxTextCtrl( optionsBox, IDC_BackgroundValue, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_textCtrl136, IDS_ToolTip195);
   bSizer283->Add( m_textCtrl136, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5 );
   
   
   optionsSizer->Add( bSizer283, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxEXPAND, 5 );
   
   wxBoxSizer* bSizer286;
   bSizer286 = new wxBoxSizer( wxHORIZONTAL );
   
   m_checkBox22 = new wxCheckBox( optionsBox, IDC_WriteChannelDescriptions, wxT("Write channel descriptions"), wxDefaultPosition, wxDefaultSize, 0 );
   SetUpToolTip(m_checkBox22, IDS_ToolTip196);
   bSizer286->Add( m_checkBox22, 0, wxRESERVE_SPACE_EVEN_IF_HIDDEN | wxALL, 5 );
   
   
   optionsSizer->Add( bSizer286, 0, wxEXPAND, 5 );
   
   wxBoxSizer* bSizer285;
   bSizer285 = new wxBoxSizer( wxHORIZONTAL );
   
   m_staticText290 = new wxStaticText( optionsBox, wxID_ANY, wxT("Header Format:"), wxDefaultPosition, wxDefaultSize, 0 );
   m_staticText290->Wrap( -1 );
   SetUpToolTip(m_staticText290, IDS_ToolTip197);
   bSizer285->Add( m_staticText290, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );
   
   m_comboBox47 = new wxComboBox( optionsBox, IDC_HeaderFormatList, wxT("GeoTIFF"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
   m_comboBox47->Append("'None'");
   m_comboBox47->Append("ArcView");
   m_comboBox47->Append("ERDAS 74");
   m_comboBox47->Append("GeoTIFF");
   SetUpToolTip(m_comboBox47, IDS_ToolTip197);
   bSizer285->Add( m_comboBox47, 0, wxALL, 5 );
   
   optionsSizer->Add( bSizer285, 0, wxEXPAND, 5 );
	
   //gSizer13->Add (optionsSizer, 1, wxEXPAND, 5);
   gSizer13->Add (optionsSizer, 1, wxEXPAND | wxLEFT|wxTOP, 6);
   
   //bSizer119->Add (gSizer13, 1, wxEXPAND, 5);
   //bSizer119->Add (gSizer13, 1, wxEXPAND | wxALL, 12);
   bSizer119->Add (gSizer13, 1, wxEXPAND | wxLEFT|wxTOP|wxRIGHT, 12);
   /*
   m_sdbSizer6 = new wxStdDialogButtonSizer();
   m_sdbSizer6OK = new wxButton( this, wxID_OK );
   m_sdbSizer6->AddButton( m_sdbSizer6OK );
   m_sdbSizer6Cancel = new wxButton( this, wxID_CANCEL );
   m_sdbSizer6->AddButton( m_sdbSizer6Cancel );
   m_sdbSizer6->Realize();
   
   //bSizer119->Add (m_sdbSizer6, 0, wxEXPAND, 5);
   bSizer119->Add (m_sdbSizer6, 0, wxEXPAND | wxLEFT|wxTOP|wxBOTTOM, 12);
	*/
	CreateStandardButtons (bSizer119);
   
   //this->SetSizer (bSizer119);
   this->Layout();
   //this->Centre (wxBOTH);
   
   // This forces the positions to be used when the toEntire and toSelected buttons are switched.
   //m_bpButton4->Hide();
   //m_bpButton5->Show();
   //this->Layout();
   
}      // end "CreateControls"


Boolean CMMosaicTwoImagesDialog::DoDialog(
                                          FileInfoPtr                     fileInfoPtr,
                                          FileInfoPtr                     outFileInfoPtr,
                                          ReformatOptionsPtr            reformatOptionsPtr)

{
   SInt16                  returnCode;
   Boolean                  continueFlag = FALSE;
   
   
   // Make sure intialization has been completed.
   
   if (!m_initializedFlag)
      return(FALSE);
   
   m_outputFileInfoPtr = outFileInfoPtr;
   m_fileInfoPtr = fileInfoPtr;
   m_reformatOptionsPtr = reformatOptionsPtr;
   
   // Selected area for output file.
   
   m_dialogSelectArea.lineStart = m_LineStart;
   m_dialogSelectArea.lineEnd = m_LineEnd;
   m_dialogSelectArea.lineInterval = m_LineInterval;
   
   m_dialogSelectArea.columnStart = m_ColumnStart;
   m_dialogSelectArea.columnEnd = m_ColumnEnd;
   m_dialogSelectArea.columnInterval = m_ColumnInterval;
   
   returnCode = ShowModal ();
   
   if (returnCode == wxID_OK)
   {
      continueFlag = TRUE;
      
      m_dialogSelectArea.lineStart = m_LineStart;
      m_dialogSelectArea.lineEnd = m_LineEnd;
      m_dialogSelectArea.lineInterval = 1;
      m_dialogSelectArea.columnStart = m_ColumnStart;
      m_dialogSelectArea.columnEnd = m_ColumnEnd;
      m_dialogSelectArea.columnInterval = 1;
      
      MosaicTwoImagesDialogOK (m_reformatOptionsPtr,
                               m_outputFileInfoPtr,
                               m_mosaicDirectionCode+1,
                               m_rightBottomWindowInfoHandle,
                               &m_dialogSelectArea,
                               &m_rightBottomDialogSelectArea,
                               m_ignoreBackgroundValueFlag,
                               m_backgroundValue,
                               m_writeChannelDescriptionsFlag,
                               (SInt16)m_headerOptionsSelection);
      
   }      // end "if (returnCode == IDOK)"
   
   return (continueFlag);
   
}      // end "DoDialog"


void CMMosaicTwoImagesDialog::OnBnClickedIgnoreBackgroundValue(wxCommandEvent& event)
{
   //DDX_Check(m_dialogFromPtr, IDC_IgnoreBackgroundValue, m_ignoreBackgroundValueFlag);
   wxCheckBox* m_dialogFromPtr = (wxCheckBox*)wxWindow::FindWindow(IDC_IgnoreBackgroundValue);
   m_ignoreBackgroundValueFlag = m_dialogFromPtr->GetValue();
   MosaicTwoImagesDialogUpdateBackGroundValuesItem (
                                                    this,
                                                    m_ignoreBackgroundValueFlag);
   
}      // end "OnBnClickedIgnoreBackgroundValue"

/*
 //Press keyboard "Enter"  -> wxID_OK
 void CMMosaicTwoImagesDialog::OnButtonPress(wxKeyEvent& event)
 {
 if (event.GetKeyCode() == WXK_RETURN) {
 if(Validate() && TransferDataFromWindow())
 this->EndModal(wxID_OK);
 } else {
 event.Skip();
 }
 }
 */

void CMMosaicTwoImagesDialog::OnCbnSelChangeRightImageFileList(wxCommandEvent& event)
{
   SInt16                     savedFileNameSelection;
   
   
   savedFileNameSelection = m_rightFileNameSelection;
   //DDX_CBIndex(m_dialogFromPtr, IDC_RightImageFileList, m_rightFileNameSelection);
   wxComboBox* m_dialogFromPtr = (wxComboBox*)wxWindow::FindWindow(IDC_RightImageFileList);
   m_rightFileNameSelection = m_dialogFromPtr->GetSelection();
   
   if (m_rightFileNameSelection != savedFileNameSelection)
   {
      MosaicTwoImagesDialogOnSelectRightBottomImage (
                                                     this,
                                                     &m_dialogSelectArea,
                                                     &m_rightBottomDialogSelectArea,
                                                     m_rightFileNameSelection+1,
                                                     &m_rightBottomWindowInfoHandle,
                                                     m_mosaicDirectionCode+1);
      
      // Keep the bottom file list in sync with the right file list.
      
      m_bottomFileNameSelection = m_rightFileNameSelection;
      //DDX_CBIndex(m_dialogToPtr, IDC_BottomImageFileList, m_bottomFileNameSelection);
      wxComboBox* m_dialogFromPtr = (wxComboBox*)wxWindow::FindWindow(IDC_BottomImageFileList);
      m_bottomFileNameSelection = m_dialogFromPtr->GetSelection();
      
   }      // end "if (m_rightFileNameSelection != savedFileNameSelection)"
   
}      // end "OnCbnSelChangeRightImageFileList"


void CMMosaicTwoImagesDialog::OnCbnSelChangeHeaderFormatList(wxCommandEvent& event)
{
   SInt16                     savedHeaderListSelection;
   
   
   int m_kNoneMenuItem = kNoneMenuItem;
   int m_kArcViewMenuItem = kArcViewMenuItem;
   int m_kERDAS74MenuItem = kERDAS74MenuItem;
   int m_kTIFFGeoTIFFMenuItem = kTIFFGeoTIFFMenuItem;
   
   savedHeaderListSelection = m_headerListSelection;
   wxComboBox* m_dialogFromPtr = (wxComboBox*)wxWindow::FindWindow(IDC_HeaderFormatList);
   
   m_dialogFromPtr->SetClientData(0, (void *)m_kNoneMenuItem);
   m_dialogFromPtr->SetClientData(1, (void *) m_kArcViewMenuItem);
   m_dialogFromPtr->SetClientData(2, (void *)m_kERDAS74MenuItem);
   m_dialogFromPtr->SetClientData(3, (void *)m_kTIFFGeoTIFFMenuItem);
   
   m_headerListSelection = m_dialogFromPtr->GetSelection();
   
   
   if (m_headerListSelection != savedHeaderListSelection)
   {
      wxComboBox*         comboBoxPtr;
      
      comboBoxPtr = (wxComboBox*) FindWindow(IDC_HeaderFormatList);
      
      m_headerOptionsSelection = (SInt64) ((int*)comboBoxPtr->GetClientData(m_headerListSelection));
      printf("m_headerOptionsSelection:%d\n", m_headerListSelection);
      MosaicTwoImagesDialogUpdateChannelDescriptionItem (
                                                         this,
                                                         GetHeaderFormatFromPopUpSelection (m_headerOptionsSelection),
                                                         m_outputFileInfoPtr->thematicType);
      
   }      // end "if (m_headerListSelection != savedHeaderListSelection)"
   
}      // end "OnCbnSelChangeHeaderFormatList"


void CMMosaicTwoImagesDialog::OnCbnSelChangeMosaicDirection(wxCommandEvent& event)
{
   SInt16         savedMosaicDirectionCode;
   
   
   savedMosaicDirectionCode = m_mosaicDirectionCode;
   wxComboBox* m_dialogFromPtr = (wxComboBox*)wxWindow::FindWindow(IDC_MosaicDirection);
   m_mosaicDirectionCode = m_dialogFromPtr->GetSelection();
   
   if (m_mosaicDirectionCode != savedMosaicDirectionCode)
   {
      MosaicTwoImagesDialogUpdateDirectionItems (
                                                 this,
                                                 &m_dialogSelectArea,
                                                 &m_rightBottomDialogSelectArea,
                                                 m_rightBottomWindowInfoHandle,
                                                 m_mosaicDirectionCode + 1,
                                                 m_ignoreBackgroundValueFlag);
      
   }      // end "if (m_mosaicDirectionCode != savedMosaicDirectionCode)"
   
}      // end "OnCbnSelChangeMosaicDirection"


void CMMosaicTwoImagesDialog::OnCbnSelChangeBottomImageFileList(wxCommandEvent& event)
{
   SInt16                     savedFileNameSelection;
   
   
   savedFileNameSelection = m_bottomFileNameSelection;
   //DDX_CBIndex(m_dialogFromPtr, IDC_BottomImageFileList, m_bottomFileNameSelection);
   wxComboBox* m_dialogFromPtr = (wxComboBox*)wxWindow::FindWindow(IDC_BottomImageFileList);
   m_bottomFileNameSelection = m_dialogFromPtr->GetSelection();
   
   if (m_bottomFileNameSelection != savedFileNameSelection)
   {
      MosaicTwoImagesDialogOnSelectRightBottomImage (
                                                     this,
                                                     &m_dialogSelectArea,
                                                     &m_rightBottomDialogSelectArea,
                                                     m_rightFileNameSelection+1,
                                                     &m_rightBottomWindowInfoHandle,
                                                     m_mosaicDirectionCode+1);
      
      // Keep the right file list in sync with the bottom file list.
      
      m_rightFileNameSelection = m_bottomFileNameSelection;
      //DDX_CBIndex(m_dialogToPtr, IDC_RightImageFileList, m_rightFileNameSelection);
      wxComboBox* m_dialogFromPtr = (wxComboBox*)wxWindow::FindWindow(IDC_RightImageFileList);
      m_rightFileNameSelection = m_dialogFromPtr->GetSelection();
      
   }      // end "if (m_bottomFileNameSelection != savedFileNameSelection)"
   
}      // end "OnCbnSelChangeBottomImageFileList"


void CMMosaicTwoImagesDialog::OnEnChangeBottomColumnStart3(wxCommandEvent& event)
{
   SInt32            columnStart3;
   
   //columnStart3 = GetDialogItemValue(IDC_ColumnStart3);
   wxTextCtrl* columnStart3Ptr = (wxTextCtrl*) FindWindow(IDC_ColumnEnd3);
   columnStart3 = wxAtoi(columnStart3Ptr->GetValue());
   if (columnStart3 > 0)
   {
      m_rightBottomDialogSelectArea.columnStart = columnStart3;
      
      MosaicTwoImagesDialogUpdateLineColumnEnds (
                                                 this,
                                                 &m_dialogSelectArea,
                                                 &m_rightBottomDialogSelectArea,
                                                 m_mosaicDirectionCode+1,
                                                 m_rightBottomWindowInfoHandle,
                                                 IDC_ColumnEnd3);
      
   }      // end "if (columnStart3 > 0)"
   
}      // end "OnEnChangeBottomColumnStart3"


void CMMosaicTwoImagesDialog::OnEnChangeLeftTopLineStart(wxCommandEvent& event)
{
   CMDialog::CheckLineStart(event);
   m_dialogSelectArea.lineStart = m_LineStart;
   /*
    int numberChars = sprintf ((char*)&gTextString3,
    " OnEnChangeLeftTopLineStart (m_mosaicDirectionCode, m_LineStart): %d, %d%s",
    m_mosaicDirectionCode,
    m_LineStart,
    gEndOfLine);
    ListString ((char*)&gTextString3, numberChars, gOutputTextH);
    */
   if (m_mosaicDirectionCode == 0)
      CheckRightImageLines(IDC_LineStart);
   
}      // end "OnEnChangeLeftTopLineStart"


void CMMosaicTwoImagesDialog::OnEnChangeLeftTopLineEnd(wxCommandEvent& event)
{
   CMDialog::CheckLineEnd(event);
   m_dialogSelectArea.lineEnd = m_LineEnd;
   
   if (m_mosaicDirectionCode == 0)
      CheckRightImageLines(IDC_LineEnd);
   
}   // end "OnEnChangeLeftTopLineEnd"


void CMMosaicTwoImagesDialog::OnEnChangeLeftTopColumnStart(wxCommandEvent& event)
{
   CMDialog::CheckColumnStart(event);
   m_dialogSelectArea.columnStart = m_ColumnStart;
   
   if (m_mosaicDirectionCode == 1)
      CheckBottomImageColumns(IDC_ColumnStart);
   
}      // end "OnEnChangeLeftTopColumnStart"



void CMMosaicTwoImagesDialog::OnEnChangeLeftTopColumnEnd(wxCommandEvent& event)
{
   CMDialog::CheckColumnEnd(event);
   m_dialogSelectArea.columnEnd = m_ColumnEnd;
   
   if (m_mosaicDirectionCode == 1)
      CheckBottomImageColumns(IDC_ColumnEnd);
   
}      // end "OnEnChangeLeftTopColumnEnd"

void CMMosaicTwoImagesDialog::OnEnChangeRightLineStart2(wxCommandEvent& event)
{
   SInt32            lineStart2;
   
   wxTextCtrl* lineStart2Ptr = (wxTextCtrl*) FindWindow(IDC_LineStart2);
   lineStart2 = wxAtoi(lineStart2Ptr->GetValue());
   
   if (lineStart2 > 0)
   {
      m_rightBottomDialogSelectArea.lineStart = lineStart2;
      
      MosaicTwoImagesDialogUpdateLineColumnEnds (
                                                 this,
                                                 &m_dialogSelectArea,
                                                 &m_rightBottomDialogSelectArea,
                                                 m_mosaicDirectionCode+1,
                                                 m_rightBottomWindowInfoHandle,
                                                 IDC_LineStart2);
      
   }      // end "if (lineStart2 > 0)"
   
}      // end "OnEnChangeRightLineStart2"


void CMMosaicTwoImagesDialog::OnInitDialog(wxInitDialogEvent& event)
{
   SInt16               fileNamesSelection,
   mosaicDirectionCode;
   
   Boolean               ignoreBackgroundValueFlag;
   
   
   CMDialog::OnInitDialog(event);
   
   //   int m_kNoneMenuItem = kNoneMenuItem;
   //   int m_kArcViewMenuItem = kArcViewMenuItem;
   //   int m_kERDAS74MenuItem = kERDAS74MenuItem;
   //   int m_kTIFFGeoTIFFMenuItem = kTIFFGeoTIFFMenuItem;
   
   wxComboBox* comboBoxPtr = (wxComboBox*)wxWindow::FindWindow(IDC_HeaderFormatList);
   
   comboBoxPtr->SetClientData(0, (void *)kNoneMenuItem);
   comboBoxPtr->SetClientData(1, (void *)kArcViewMenuItem);
   comboBoxPtr->SetClientData(2, (void *)kERDAS74MenuItem);
   comboBoxPtr->SetClientData(3, (void *)kTIFFGeoTIFFMenuItem);
   
   MosaicTwoImagesDialogInitialize (this,
                                    m_reformatOptionsPtr,
                                    m_fileInfoPtr,
                                    m_outputFileInfoPtr,
                                    &mosaicDirectionCode,
                                    &m_dialogSelectArea,
                                    &m_rightBottomDialogSelectArea,
                                    &fileNamesSelection,
                                    &m_rightBottomWindowInfoHandle,
                                    &ignoreBackgroundValueFlag,
                                    &m_maxDataValue,
                                    &m_backgroundValue,
                                    &m_headerOptionsSelection);
   
   m_LineStart = m_reformatOptionsPtr->lineStart;
   m_LineEnd = m_reformatOptionsPtr->lineEnd;
   m_ColumnStart = m_reformatOptionsPtr->columnStart;
   m_ColumnEnd = m_reformatOptionsPtr->columnEnd;
   
   m_ignoreBackgroundValueFlag = ignoreBackgroundValueFlag;
   m_mosaicDirectionCode = mosaicDirectionCode - 1;
   
   m_rightFileNameSelection = fileNamesSelection - 1;
   m_bottomFileNameSelection = fileNamesSelection - 1;
   
   m_headerListSelection = GetComboListSelection (IDC_HeaderFormatList,
                                                  m_headerOptionsSelection);
   if (TransferDataToWindow())
      PositionDialogWindow();
   
   SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
   
}      // end "OnInitDialog"


void CMMosaicTwoImagesDialog::ToEntireImage(wxCommandEvent& event) {
   //   Change to entire image
   
   CMDialog::ToEntireImage(event);
   /*
    m_LineStart = 1;
    m_LineEnd = m_maxNumberLines;
    m_ColumnStart = 1;
    m_ColumnEnd = m_maxNumberColumns;
    
    m_settingSelectedEntireButton = TRUE;
    
    LoadDItemValue(IDC_LineStart, m_LineStart);
    LoadDItemValue(IDC_LineEnd, m_LineEnd);
    LoadDItemValue(IDC_ColumnStart, m_ColumnStart);
    LoadDItemValue(IDC_ColumnEnd, m_ColumnEnd);
    
    m_settingSelectedEntireButton = FALSE;
    
    FindWindow(IDC_LineStart)->SetFocus();
    
    SetEntireImageButtons(
    NULL,
    m_LineStart,
    m_LineEnd,
    m_ColumnStart,
    m_ColumnEnd);
    
    this->Layout();
    this->Fit();
    SelectDialogItemText (this, IDC_LineStart, 0, SInt16_MAX);
    */
   if (m_mosaicDirectionCode == 0 ){
      LoadDItemValue(IDC_LineStart2, m_LineStart);
      LoadDItemValue(IDC_LineEnd2, m_LineEnd);
      
   }
   
   if (m_mosaicDirectionCode == 1){
      LoadDItemValue(IDC_ColumnStart3, m_ColumnStart);
      LoadDItemValue(IDC_ColumnEnd3, m_ColumnEnd);
   }
   
} // end "ToEntireImage"


void CMMosaicTwoImagesDialog::ToSelectedImage(wxCommandEvent& event) {
   LongRect selectionRect;
   
   //   Change to selected area
   
   CMDialog::ToSelectedImage(event);
   
   if (GetSelectionRectangle(
                             //               gActiveImageViewCPtr,
                             m_dialogSelectArea.windowPtr,
                             &selectionRect,
                             kDontClearSelectionArea,
                             kUseThreshold,
                             kDontAdjustToBaseImage)) {
      /*
       m_LineStart = selectionRect.top;
       m_LineEnd = selectionRect.bottom;
       m_ColumnStart = selectionRect.left;
       m_ColumnEnd = selectionRect.right;
       
       m_settingSelectedEntireButton = TRUE;
       LoadDItemValue(IDC_LineStart, m_LineStart);
       LoadDItemValue(IDC_LineEnd, m_LineEnd);
       LoadDItemValue(IDC_ColumnStart, m_ColumnStart);
       LoadDItemValue(IDC_ColumnEnd, m_ColumnEnd);
       m_settingSelectedEntireButton = FALSE;
       
       FindWindow(IDC_LineStart)->SetFocus();
       
       SetEntireImageButtons(
       NULL,
       m_LineStart,
       m_LineEnd,
       m_ColumnStart,
       m_ColumnEnd);
       this->Layout();
       this->Fit();
       SelectDialogItemText(this, IDC_LineStart, 0, SInt16_MAX);
       */
      if (m_mosaicDirectionCode == 0){
         LoadDItemValue(IDC_LineStart2, m_LineStart);
         LoadDItemValue(IDC_LineEnd2, m_LineEnd);
         
      }
      
      if (m_mosaicDirectionCode == 1){
         LoadDItemValue(IDC_ColumnStart3, m_ColumnStart);
         LoadDItemValue(IDC_ColumnEnd3, m_ColumnEnd);
      }
      
      
   } // end "if (GetLineColumnSelection(..."
   
} // end "ToSelectedImage"


bool CMMosaicTwoImagesDialog::TransferDataFromWindow()
{
   SInt16         returnCode;
   
   
   wxTextCtrl* lineStart = (wxTextCtrl*) FindWindow(IDC_LineStart);
   wxTextCtrl* lineEnd = (wxTextCtrl*) FindWindow(IDC_LineEnd);
   wxTextCtrl* columnStart = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
   wxTextCtrl* columnEnd = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
   wxCheckBox* ignoreBackgroundValueFlag = (wxCheckBox*) FindWindow(IDC_IgnoreBackgroundValue);
   wxComboBox* mosaicDirectionCode = (wxComboBox*) FindWindow(IDC_MosaicDirection);
   
   m_LineStartString = lineStart->GetValue();
   m_LineEndString = lineEnd->GetValue();
   m_ColumnStartString = columnStart->GetValue();
   m_ColumnEndString = columnEnd->GetValue();
   
   m_LineStart = wxAtoi(m_LineStartString);
   m_LineEnd = wxAtoi(m_LineEndString);
   m_ColumnStart = wxAtoi(m_ColumnStartString);
   m_ColumnEnd = wxAtoi(m_ColumnEndString);
   
   m_ignoreBackgroundValueFlag = ignoreBackgroundValueFlag->GetValue();
   m_mosaicDirectionCode = mosaicDirectionCode->GetSelection();
   
   returnCode = VerifyLineColumnValues(
                                       IDC_LineStart,
                                       IDC_ColumnStart,
                                       false);
   
   return (returnCode == 0);
   
}   // end "TransferDataFromWindow"


bool CMMosaicTwoImagesDialog::TransferDataToWindow() {
   wxTextCtrl* lineStart = (wxTextCtrl*) FindWindow(IDC_LineStart);
   wxTextCtrl* lineEnd = (wxTextCtrl*) FindWindow(IDC_LineEnd);
   wxTextCtrl* columnStart = (wxTextCtrl*) FindWindow(IDC_ColumnStart);
   wxTextCtrl* columnEnd = (wxTextCtrl*) FindWindow(IDC_ColumnEnd);
   wxCheckBox* ignoreBackgroundValueFlag = (wxCheckBox*) FindWindow(IDC_IgnoreBackgroundValue);
   wxComboBox* mosaicDirectionCode = (wxComboBox*) FindWindow(IDC_MosaicDirection);
   
   lineStart->ChangeValue(wxString::Format(wxT("%d"), m_LineStart ));
   lineEnd->ChangeValue(wxString::Format(wxT("%d"), m_LineEnd ));
   columnStart->ChangeValue(wxString::Format(wxT("%d"), m_ColumnStart ));
   columnEnd->ChangeValue(wxString::Format(wxT("%d"), m_ColumnEnd ));
   ignoreBackgroundValueFlag->SetValue(m_ignoreBackgroundValueFlag);
   mosaicDirectionCode->SetSelection(m_mosaicDirectionCode);
   
   return true;
   
}   // end "TransferDataToWindow"

