//	 									MultiSpec
//
//					Laboratory for Applications of Remote Sensing
//									Purdue University
//								West Lafayette, IN 47907
//								 Copyright (2009-2018)
//							(c) Purdue Research Foundation
//									All rights reserved.
//
//	File:						LBiplotDialog.h : CMBiPlotDialog definition
//	Implementation:		LBiplotDialog.cpp
//
//	Authors:					Larry L. Biehl
//
//	Language:				C++
//
//	System:					Linux Operating System
//
//	Brief description:	Header file for the CMBiPlotDialog class
//
//	Written By:				Abdur Rahman Maud		Date: ??/??/2013
//	Revised By:				Larry L. Biehl			Date: 10/04/2018
//	
//------------------------------------------------------------------------------------

#if !defined __LBIPLOTDLG_H__
	#define	__LBIPLOTDLG_H__     

	#include "SMultiSpec.h"
	#include "LDialog.h"

	class CMBiPlotDialog : public CMDialog 
		{
		public:
					 // standard constructor
			CMBiPlotDialog (wxWindow* pParent = NULL, 
									wxWindowID id = IDS_ProcessorBiPlot, 
									const wxString& title = wxT("Set Biplot Specifications"));  
									   
			~CMBiPlotDialog (); // standard desctructor 
			  
			Boolean DoDialog(void);
			  
			enum {IDD = IDS_ProcessorBiPlot};
		
			float*		m_classWeightsPtr;

			double		m_thresholdPercent,
							m_saveThresholdPercent;
							
			int			m_classWeightsSelection;

			SInt32		m_maxChannelFeatureNum;
		
			SInt16		m_displayPixelCode,
							m_entireIconItem,
							m_outlineClassCode,
							m_plotDataCode,
							m_newXAxisFeature,
							m_newYAxisFeature;
		
			Boolean		m_checkChannelStatisticsFlag,
							m_checkClassesPopUpFlag,
							m_checkFeatureTransformFlag,
							m_createNewGraphWindowFlag,
							m_featureTransformAllowedFlag,
							m_featureTransformationFlag,
							m_createNewGraphicsWindowFlag,
							m_thresholdFlag,
							m_trainingAreaFlag,
							m_testFlag,
							m_imageAreaFlag;
				  
		 protected:        
			void CheckOKButton (void);
			void OnChangeHorizontalAxis (wxCommandEvent& event);
			void OnChangeVerticalAxis (wxCommandEvent& event);
			void OnCreateNewWindowFlag (wxCommandEvent& event);
			void OnDisplayPixelAsSymbol (wxCommandEvent& event);
			void OnFeatureTranformation (wxCommandEvent& event);       
			void OnImageArea (wxCommandEvent& event);
			void OnInitDialog (wxInitDialogEvent& event); 
			void OnOutlineClassAsEllipse (wxCommandEvent& event);
			void OnSelendokClassWeightsCombo (wxCommandEvent& event);
			void OnTestArea (wxCommandEvent& event);
			void OnThresholdPixelFlag (wxCommandEvent& event);
			void OnTrainArea (wxCommandEvent& event);
			bool TransferDataFromWindow ();
			bool TransferDataToWindow ();

			DECLARE_EVENT_TABLE ()

			void CreateControls ();  
			
			wxBitmapButton*	m_bpButton4;
			wxBitmapButton*	m_bpButton5;

			wxBoxSizer*			bSizer278;
			wxBoxSizer*			bSizer282;
			  			
			wxButton*			m_button69;
			wxButton*			m_button70;
			
			wxCheckBox*			m_checkBox58;
			wxCheckBox*			m_checkBox59;
			wxCheckBox*			m_checkBox60;
			wxCheckBox*			m_checkBox61;
			wxCheckBox*			m_checkBox62;
			wxCheckBox*			m_checkBox63;
			wxCheckBox*			m_checkBox64;
			wxCheckBox*			m_checkBox65;
			
			wxComboBox*			m_comboBox51;
			wxComboBox*			m_comboBox52;
			wxComboBox*			m_comboBox53;
			
			wxStaticText*		m_staticText26;       
			//wxStaticText*		m_staticText60;
			//wxStaticText*		m_staticText61;
			//wxStaticText*		m_staticText62;
			wxStaticText*		m_staticText177;
			wxStaticText*		m_staticText265;
			wxStaticText*		m_staticText266;
			wxStaticText*		m_staticText267;
			wxStaticText*		m_staticText268;
			wxStaticText*		m_staticText269;
			wxStaticText*		m_staticText270;
			wxStaticText*		m_staticText271;
			wxStaticText*		m_staticText278;
			wxStaticText*		m_staticText279;
			wxStaticText*		m_staticText280;
			wxStaticText*		m_staticText281;
			wxStaticText*		m_staticText282;
			wxStaticText*		m_staticText283;
			wxStaticText*		m_staticText284;
			wxStaticText*		m_staticText2811; 
			
			//wxTextCtrl*			m_colendctrl;
			//wxTextCtrl*			m_colintctrl;
			//wxTextCtrl*			m_colstartctrl;
			//wxTextCtrl*			m_lineendctrl;
			//wxTextCtrl*			m_lineintctrl;
			//wxTextCtrl*			m_linestartctrl;
			wxTextCtrl*			m_textCtrl115;
			wxTextCtrl*			m_textCtrl116;
			wxTextCtrl*			m_textCtrl129;

		};
#endif	// !defined __LBIPLOTDLG_H__
