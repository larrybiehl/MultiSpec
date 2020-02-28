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
//	File:						xDisplayMultispectralDialog.h
//	Implementation:		xDisplayMultispectralDialog.cpp
//
//	Authors:					Larry L. Biehl, Abdur Rahman Maud
//
//	Language:				C++
//
//	System:					Linux and MacOS Operating Systems
//
//	Brief description:	Header file for the CMDisplaySpecsDlg class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2009
//	Revised By:				Larry L. Biehl			Date: 02/23/2020
//	
//------------------------------------------------------------------------------------

#pragma once

#include "SMultiSpec.h"
#include "xDialog.h"
#include "xDisplayMinMaxDialog.h"
#include "wx/wx.h"


class CMDisplaySpecsDlg : public CMDialog
{
	DECLARE_DYNAMIC_CLASS (CMDisplaySpecsDlg)
	
			// Construction
	public:
		CMDisplaySpecsDlg ();
	
		CMDisplaySpecsDlg ( // standard constructor
				wxWindow*							parent,
				wxWindowID							id = wxID_ANY,
				const wxString&					title =
															wxT("Set Display Specifications for:"),
				const wxPoint&						pos = wxDefaultPosition,
				const wxSize&						size = /*wxDefaultSize*/wxSize (200, 200),
				long									style =
															wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP);

		~CMDisplaySpecsDlg (); // standard desctructor
	
		Boolean DoDialog (
            DisplaySpecsPtr 					displaySpecsPtr);


		enum {
			IDD = IDD_DisplaySpecs
				};

		 // Implementation
	protected:
		void OnChangeBlueChannel (
				wxCommandEvent&					event);

		void OnChangeGrayChannel (
				wxCommandEvent&					event);

		void OnChangeGreenChannel (
				wxCommandEvent&					event);

		void OnChangeRedChannel (
				wxCommandEvent&					event);

		void OnChannelDescriptions (
				wxCommandEvent&					event);

		void OnChannelsComboCloseUp (
				wxCommandEvent&					event);

		void OnChannelsComboDropDown (
				wxCommandEvent&					event);

		void OnChannelsSelendok (
				wxCommandEvent&					event);

		void OnEnhancementComboCloseUp (
				wxCommandEvent&					event);

		void OnEnhancementComboDropDown (
				wxCommandEvent&					event);

		void OnEnhancementComboSelendok (
				wxCommandEvent&					event);
	
		void OnInitDialog (
				wxInitDialogEvent& 				event);
	
		void OnOK (
				wxCommandEvent& 					event);

		void OnSelendokBitsOfColor (
				wxCommandEvent&					event);

		void OnSelendokDisplayType (
				wxCommandEvent&					event);

		void OnSelendokMinMaxValues (
				wxCommandEvent&					event);

		void OnSelendokMinMaxValuesCloseUp (
				wxCommandEvent&					event);

		void OnSelendokMinMaxValuesDropDown (
				wxCommandEvent&					event);
		DECLARE_EVENT_TABLE ()

		void CheckChannel (
				int									itemID,
				UInt32*								channelValuePtr);

		void CheckDisplayLevels (
				DialogPtr 							dialogPtr,
				UInt16 								displayType);

		void CreateControls ();

		Boolean GetComputeHistogramDialogSetting (
				DisplaySpecsPtr 					displaySpecsPtr,
				SInt16 								lEnhancement,
				SInt16 								lMinMaxCode);

		UInt16 GetMaximumDisplayLevels (
				SInt16 								bitsOfColorIndex,
				SInt16 								displayType,
				SInt16 								duplicateChannelCode);

		void SetComputeHistogramDialogItem (
				Boolean 								computeHistogramFlag,
				DialogPtr 							dialogPtr,
				SInt16 								itemNumber);
		
		bool TransferDataFromWindow ();
	
		bool TransferDataToWindow ();
	

		wxString 							m_BlueChannelString,
												m_FileName,
												m_GrayChannelString,
												m_GreenChannelString,
												m_Magnificationstring,
												m_NumberLevelsString,
												m_RedChannelString,
												m_thematicLegendFactorString;
	
		DisplaySpecsPtr 					m_displaySpecsPtr;

				// Dialog controls
		
		wxBoxSizer 							*bSizer67;

		wxButton								*m_cancelbtn,
												*m_chandescctrl,
												*m_okbtn;
		
		wxCheckBox							*m_bluechaninvctrl,
												*m_checkBox17,
												*m_greenchaninvctrl,
												*m_newhistctrl,
												*m_redchaninvctrl;
	
		wxChoice 							*m_bitsofcolctrl,
												*m_channelsctrl,
												*m_treatzeroctrl;

		wxComboBox 							*m_displaytypectrl,
												*m_noofdisplevelsctrl;
	
		wxStaticText 						*m_bluepromptctrl,
												*m_graypromptctrl,
												*m_greenpromptctrl,
												*m_legendpromptctrl,
												*m_redpromptctrl,
												*m_staticimagename,
												*m_staticText67,
												*m_staticText68,
												*m_staticText69,
												*m_staticText70,
												*m_staticText72,
												*m_staticText76,
												*m_staticText681,
												*m_staticText701;
	
		wxTextCtrl 							*m_bluechannelctrl,
												*m_grayctrl,
												*m_greenchannelctrl,
												*m_legendfactorctrl,
												*m_magnificationctrl,
												*m_redchannelctrl;
	
		#if defined multispec_wxmac
			wxChoice 							*m_enhancementCtrl,
						 							*m_minMaxCtrl;
		#endif
		#if defined multispec_wxlin
			wxComboBox	 						*m_enhancementCtrl,
													*m_minMaxCtrl;
		#endif
	

		double 								m_gaussianStretch,
			 									m_Magnification,
			 									mMinMaxValues[6],
				 								m_thematicLegendFactor;
	
		UInt32 								m_BlueChannel,
			 									m_GrayChannel,
			 									m_GreenChannel,
			 									m_NumberLevels,
			 									m_numberLevelsval,
			 									m_RedChannel;
	
		int 									m_BitsOfColor,
			 									m_DisplayType,
			 									m_Enhancement,
			 									m_MinMaxPopupCode,
			 									m_TreatZeroAs;

		SInt16 								mBitsOfColorIndex,
												mDuplicateChannelCode,
												mMaxSystemPixelSize,
												mRGBColors,
												m_MinMaxCode,
												m_PercentClip,
												m_SavedBitsOfColor;

		UInt16 								mColorLevelsMax[3][3],
												mLocalDisplayType,
												mMaxChannelNumber,
												mPixelSizeVector[3];
	
		bool 									m_blueChannelInvertFlag,
			 									m_ComputeHistogramFlag,
			 									m_greenChannelInvertFlag,
			 									m_redChannelInvertFlag,
			 									m_vectorOverlaysFlag;

		Boolean 								m_initializedFlag,
												m_optionKeyFlag,
												m_thematicDisplayWithUserSettingFlag;
	
};	// end "class CMDisplaySpecsDlg"
