//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LDisplayMultispectralDialog.h
//	Implementation:		LDisplayMultispectralDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMDisplaySpecsDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 11/15/2018
//	
//------------------------------------------------------------------------------------

#if !defined __LDISPDLG_H__
#define	__LDISPDLG_H__       

#include "SMultiSpec.h"
#include "LDialog.h"
#include "LDisplayMinMaxDialog.h"
#include "wx/wx.h"

typedef long unsigned int UINT;
/////////////////////////////////////////////////////////////////////////////
// CMDisplaySpecsDlg dialog

class CMDisplaySpecsDlg : public CMDialog
	{
    DECLARE_DYNAMIC_CLASS( CMDisplaySpecsDlg )
			// Construction
	public:
		CMDisplaySpecsDlg ();
		CMDisplaySpecsDlg (	 // standard constructor
				wxWindow*					parent,
				wxWindowID					id = wxID_ANY,
				const wxString&			title = wxT("Set Display Specifications for:"),
				const wxPoint&				pos = wxDefaultPosition,
				const wxSize&				size = /*wxDefaultSize*/wxSize (200, 200),
				long							style = wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);

		~CMDisplaySpecsDlg(); // standard desctructor

		Boolean DoDialog(
            DisplaySpecsPtr displaySpecsPtr);

    enum {
        IDD = IDD_DisplaySpecs
    };
   
    UINT m_BlueChannel;
    UINT m_GreenChannel;
    double m_Magnification;
    double m_thematicLegendFactor;
    bool m_ComputeHistogramFlag;
    UINT m_NumberLevels;
    UINT m_numberLevelsval;
    UINT m_RedChannel;
    wxString m_FileName;
    int m_BitsOfColor;
    int m_DisplayType;
    int m_Enhancement;
    UINT m_GrayChannel;
    int m_TreatZeroAs;
    int m_MinMaxPopupCode;
    bool m_redChannelInvertFlag;
    bool m_greenChannelInvertFlag;
    bool m_blueChannelInvertFlag;
    bool m_vectorOverlaysFlag;

    wxString m_RedChannelString;
    wxString m_GreenChannelString;
    wxString m_BlueChannelString;
    wxString m_GrayChannelString;
    wxString m_thematicLegendFactorString;
    wxString m_Magnificationstring;
    wxString m_NumberLevelsString;

    // Implementation
protected:
    void CheckChannel(
				int							itemID,
            UINT*							channelValuePtr);

    void CheckDisplayLevels(
            DialogPtr dialogPtr,
            UInt16 displayType);

    Boolean GetComputeHistogramDialogSetting(
            DisplaySpecsPtr displaySpecsPtr,
            SInt16 lEnhancement,
            SInt16 lMinMaxCode);

    //	void				GetDefaultPaletteSpecs (
    //							SInt16					currentDisplayType,
    //							SInt16					currentPixelSize,
    //							SInt16					numberChannels,
    //							UInt16					*newDisplayTypePtr,
    //							SInt16					*maxSystemPixelSizePtr);

    UInt16 GetMaximumDisplayLevels(
            SInt16 bitsOfColorIndex,
            SInt16 displayType,
            SInt16 duplicateChannelCode);
				
	void OnOK(wxCommandEvent& event);

	void SetComputeHistogramDialogItem(
            Boolean computeHistogramFlag,
            DialogPtr dialogPtr,
            SInt16 itemNumber);
	
	SInt16 Update8_16_24BitsOfColorIndex(
            SInt16 menuBitsOfColorIndex);

    void CreateControls();
    // Generated message map functions
    //{{AFX_MSG(CMDisplaySpecsDlg)
    void OnInitDialog(wxInitDialogEvent& event);
    bool TransferDataFromWindow();
    bool TransferDataToWindow();
	
    void OnChangeBlueChannel(wxCommandEvent& event);
    void OnChangeGreenChannel(wxCommandEvent& event);
    void OnChangeRedChannel(wxCommandEvent& event);
    void OnChangeGrayChannel(wxCommandEvent& event);
    void OnSelendokEnhancement(wxCommandEvent& event);
    void OnSelendokEnhancementCloseUp(wxCommandEvent& event);
    void OnSelendokEnhancementDropDown(wxCommandEvent& event);
    void OnSelendokBitsOfColor(wxCommandEvent& event);
    void OnChannelDescriptions(wxCommandEvent& event);
    void OnSelendokDisplayType(wxCommandEvent& event);
    void OnSelendokMinMaxValues(wxCommandEvent& event);
    void OnSelendokChannels(wxCommandEvent& event);
    void OnSelendokMinMaxValuesCloseUp (wxCommandEvent& event);
    void OnSelendokChannelsDropDown(wxCommandEvent& event);    
    //void OnButtonPress(wxKeyEvent& event); 
    void OnSelendokMinMaxValuesDropDown(wxCommandEvent& event);
    DECLARE_EVENT_TABLE()

    DisplaySpecsPtr m_displaySpecsPtr;

			 // This is used as the index into the number of levels array
			 // to allow for the Windows version having only 8 and 24 bit
			 // color choices.

    double m_gaussianStretch;

    SInt16 mBitsOfColorIndex;
    SInt16 m_SavedBitsOfColor;

    SInt16 mDuplicateChannelCode;
    SInt16 mMaxSystemPixelSize;
    SInt16 m_MinMaxCode;
    SInt16 m_PercentClip;
    SInt16 mRGBColors;

    UInt16 mColorLevelsMax[3][3];
    UInt16 mLocalDisplayType;
    UInt16 mMaxChannelNumber;
    double mMinMaxValues[6];
    UInt16 mPixelSizeVector[3];

    Boolean m_initializedFlag;
    Boolean m_optionKeyFlag;
    Boolean m_thematicDisplayWithUserSettingFlag;

    		// Dialog controls
	
	wxBoxSizer* bSizer67;

	wxButton* m_cancelbtn;
	wxButton* m_okbtn;
	
	wxCheckBox* m_bluechaninvctrl;
	wxCheckBox* m_greenchaninvctrl;
	wxCheckBox* m_newhistctrl;
	wxCheckBox* m_redchaninvctrl;
	
	wxChoice* m_channelsctrl;
	//wxChoice* m_enhancementctrl;
	
	wxComboBox* m_comboBox29;
	wxComboBox* m_displaytypectrl;
	wxComboBox* m_enhancementctrl;
	wxComboBox* m_noofdisplevelsctrl;
	
	wxStaticText* m_staticText72;
	wxStaticText* m_redpromptctrl;
	wxTextCtrl* m_redchannelctrl;
	wxStaticText* m_greenpromptctrl;
	wxTextCtrl* m_greenchannelctrl;
	wxStaticText* m_bluepromptctrl;
	wxTextCtrl* m_bluechannelctrl;
	wxTextCtrl* m_grayctrl;
	wxStaticText* m_graypromptctrl;
	wxCheckBox* m_checkBox17;
	wxStaticText* m_legendpromptctrl;
	wxTextCtrl* m_legendfactorctrl;
	wxButton* m_chandescctrl;
	wxStaticText* m_staticText76;
	wxTextCtrl* m_magnificationctrl;
	wxStaticText* m_staticText67;
	wxChoice* m_bitsofcolctrl;
	wxStaticText* m_staticText68;
	wxStaticText* m_staticText69;
	wxComboBox* m_minmaxctrl;
	wxStaticText* m_staticText70;
	wxChoice* m_treatzeroctrl;

	wxStaticText* m_staticimagename;
	wxStaticText* m_staticText701;
	wxStaticText* m_staticText681;
	
	};

#endif // !defined __LDISPDLG_H__
