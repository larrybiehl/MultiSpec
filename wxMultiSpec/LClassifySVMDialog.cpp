//                               MultiSpec
//
//               Laboratory for Applications of Remote Sensing
//                         Purdue University
//                        West Lafayette, IN 47907
//                         Copyright (2009-2019)
//                     (c) Purdue Research Foundation
//                           All rights reserved.
//
//   File:  					LClassifySVMDialog.cpp : class implementation file
//   Class Definition:		LClassifySVMDialog.h
//
//   Authors: 					Larry L. Biehl
//
//   Revision date: 			10/05/2019
//
//   Language: 				C++
//
//   System:               Linux and MacOS Operating Systems
//
//   Brief description:		This file contains functions that relate to the
//    							CMSVMClassifyDialog class.
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"

#include "LMultiSpec.h"
#include "LClassifySVMDialog.h"



extern void SVMClassifyDialogShowHideParameters (
				DialogPtr							dialogPtr,
				int									svmType,
				int									svmKernel);



BEGIN_EVENT_TABLE (CMSVMClassifyDialog, CMDialog)
	EVT_COMBOBOX (IDC_SVM_TYPE, CMSVMClassifyDialog::OnSelendokSVMTypeCombo)
	EVT_COMBOBOX (IDC_SVM_KERNEL_TYPE, CMSVMClassifyDialog::OnSelendokSVMKernelCombo)
	EVT_INIT_DIALOG (CMSVMClassifyDialog::OnInitDialog)
END_EVENT_TABLE ()



CMSVMClassifyDialog::CMSVMClassifyDialog (
				wxWindow*                     pParent,
				wxWindowID                    id,
				const wxString&               title)
		: CMDialog (id, pParent, title)

{
   m_initializedFlag = CMDialog::m_initializedFlag;
   
   CreateControls ();
   
}   // end "CMSVMClassifyDialog"



void CMSVMClassifyDialog::CreateControls ()

{
   this->SetSizeHints (wxDefaultSize, wxDefaultSize);
	
   m_bVSizer151 = new wxBoxSizer (wxVERTICAL);
	
   wxBoxSizer* bHSizer161;
   bHSizer161 = new wxBoxSizer (wxHORIZONTAL);
	wxStaticText*	staticText1611;
   staticText1611 = new wxStaticText (this,
													wxID_ANY,
													wxT("SVM Type:"),
													wxDefaultPosition,
													wxSize (90, -1),
													0);
   staticText1611->Wrap (-1);
	SetUpToolTip (staticText1611, IDS_ToolTip8402);
   bHSizer161->Add (staticText1611, 0, wxALIGN_CENTER|wxALL, 5);
	
	wxComboBox*	comboBox1612;
   comboBox1612 = new wxComboBox (this,
												IDC_SVM_TYPE,
												wxT("C-SVC"),
												wxDefaultPosition,
												wxSize (210, -1),
												0,
												NULL,
												0);
   comboBox1612->Append (wxT("C-SVC"));
   comboBox1612->Append (wxT("nu-SVC"));
   comboBox1612->Append (wxT("one-class"));
   comboBox1612->Append (wxT("epsilon-SVR"));
   comboBox1612->Append (wxT("nu-SVR"));

	SetUpToolTip (comboBox1612, IDS_ToolTip8402);
   bHSizer161->Add (comboBox1612, 0, wxALIGN_CENTER|wxALL, 5);
   m_bVSizer151->Add (bHSizer161, 1, wxALL|wxEXPAND, 2);
	
   wxBoxSizer* bHSizer162;
   bHSizer162 = new wxBoxSizer (wxHORIZONTAL);
   wxStaticText*	staticText1621;
   staticText1621 = new wxStaticText (this,
													wxID_ANY,
													wxT("Kernel Type:"),
													wxDefaultPosition,
													wxSize (90, -1),
													0);
   staticText1621->Wrap (-1);
	SetUpToolTip (staticText1621, IDS_ToolTip8403);
   bHSizer162->Add (staticText1621, 0, wxALIGN_CENTER|wxALL, 5);
	
	wxComboBox*	comboBox1622;
   comboBox1622 = new wxComboBox (this,
												IDC_SVM_KERNEL_TYPE,
												wxT("Radial Basis Function"),
												wxDefaultPosition,
												wxSize (210, -1),
												0,
												NULL,
												0);
   comboBox1622->Append (wxT("Linear"));
   comboBox1622->Append (wxT("Polynomial"));
   comboBox1622->Append (wxT("Radial Basis Function (RBF)"));
   comboBox1622->Append (wxT("Sigmoid"));
	
	SetUpToolTip (comboBox1622, IDS_ToolTip8403);
   bHSizer162->Add (comboBox1622, 0, wxALIGN_CENTER|wxALL, 5);
   m_bVSizer151->Add (bHSizer162, 1, wxALL|wxEXPAND, 2);
	
	wxBoxSizer* bHSizer167 = new wxBoxSizer (wxHORIZONTAL);
	wxStaticText* staticText1671;
	staticText1671 = new wxStaticText (this,
													wxID_ANY,
													wxT("eps:"),
													wxDefaultPosition,
													wxSize (90, -1),
													0);
   staticText1671->Wrap (-1);
	SetUpToolTip (staticText1671, IDS_ToolTip8408);
   bHSizer167->Add (staticText1671, 0, wxALIGN_CENTER|wxALL, 5);
	
	wxTextCtrl* textControl1672;
   textControl1672 = new wxTextCtrl (this,
													IDC_SVM_EPS,
													wxEmptyString,
													wxDefaultPosition,
													wxSize (110, -1),
													0);
	textControl1672->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
																	&m_stringCheck));
	SetUpToolTip (textControl1672, IDS_ToolTip8408);
   bHSizer167->Add (textControl1672, 0, wxALIGN_CENTER|wxALL, 5);
	
	wxStaticText* staticText1673;
   staticText1673 = new wxStaticText (this,
                                       wxID_ANY,
                                       wxT("(stopping criteria)"),
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       0);
   staticText1673->Wrap (-1);
   bHSizer167->Add (staticText1673, 0, wxALIGN_CENTER|wxALL, 5);
   m_bVSizer151->Add (bHSizer167, 1, wxALL|wxEXPAND, 2);
	
	wxBoxSizer* bHSizer172 = new wxBoxSizer (wxHORIZONTAL);
	wxCheckBox*	checkBox1721;
	checkBox1721 = new wxCheckBox (this,
												IDC_SVM_SHRINKING,
												wxT("Use the shrinking heuristics"),
												wxDefaultPosition,
												wxSize (210, -1),
												0);
	SetUpToolTip (checkBox1721, IDS_ToolTip8413);
	bHSizer172->Add (checkBox1721, 0, wxALIGN_CENTER|wxALL, 5);
   m_bVSizer151->Add (bHSizer172, 1, wxALL|wxEXPAND, 2);
	
	wxBoxSizer* bHSizer166 = new wxBoxSizer (wxHORIZONTAL);
	wxStaticText* staticText1661;
	staticText1661 = new wxStaticText (this,
													wxID_ANY,
													wxT("Cache Size:"),
													wxDefaultPosition,
													wxSize (90, -1),
													0);
   staticText1661->Wrap (-1);
	SetUpToolTip (staticText1661, IDS_ToolTip8407);
   bHSizer166->Add (staticText1661, 0, wxALIGN_CENTER|wxALL, 5);
	
	wxTextCtrl* textControl1662;
   textControl1662 = new wxTextCtrl (this,
													IDC_SVM_CACHE_SIZE,
													wxEmptyString,
													wxDefaultPosition,
													wxSize (110, -1),
													0);
	textControl1662->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
																	&m_stringCheck));
	SetUpToolTip (textControl1662, IDS_ToolTip8407);
	
   bHSizer166->Add (textControl1662, 0, wxALIGN_CENTER|wxALL, 5);
	
	wxStaticText* staticText1663;
   staticText1663 = new wxStaticText (this,
                                       wxID_ANY,
                                       wxT("(in MB)"),
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       0);
   staticText1663->Wrap (-1);
   bHSizer166->Add (staticText1663, 0, wxALIGN_CENTER|wxALL, 5);
   m_bVSizer151->Add (bHSizer166, 1, wxALL|wxEXPAND, 2);
	
   wxBoxSizer* bHSizer163 = new wxBoxSizer (wxHORIZONTAL);
	wxStaticText* staticText1631;
   staticText1631 = new wxStaticText (this,
                                       IDC_SVM_DEGREE_PROMPT,
                                       wxT("Degree Value:"),
                                       wxDefaultPosition,
													wxSize (90, -1),
                                       0);
   staticText1631->Wrap (-1);
	SetUpToolTip (staticText1631, IDS_ToolTip8404);
   bHSizer163->Add (staticText1631,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));

	wxTextCtrl* textControl1632;
   textControl1632 = new wxTextCtrl( this,
													IDC_SVM_DEGREE,
													wxEmptyString,
													wxDefaultPosition,
													wxSize (110, -1),
													0);
	textControl1632->SetValidator (wxTextValidator (wxFILTER_DIGITS,
																	&m_stringCheck));
	SetUpToolTip (textControl1632, IDS_ToolTip8404);
   bHSizer163->Add (textControl1632,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));

	wxStaticText* staticText1633;
   staticText1633 = new wxStaticText (this,
                                       IDC_SVM_DEGREE_INFO,
                                       wxT("(used for Polynomial Kernels)"),
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       0);
   staticText1633->Wrap (-1);
   bHSizer163->Add (staticText1633,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
   m_bVSizer151->Add (bHSizer163, 1, wxALL|wxEXPAND, 2);
	
   wxBoxSizer* bHSizer164 = new wxBoxSizer (wxHORIZONTAL);
	wxStaticText* staticText1641;
   staticText1641 = new wxStaticText (this,
                                       IDC_SVM_GAMMA_PROMPT,
                                       wxT("Gamma Value:"),
                                       wxDefaultPosition,
													wxSize (90, -1),
                                       0);
   staticText1641->Wrap (-1);
	SetUpToolTip (staticText1641, IDS_ToolTip8405);
   bHSizer164->Add (staticText1641,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));

	wxTextCtrl* textControl1642;
   textControl1642 = new wxTextCtrl (this,
													IDC_SVM_GAMMA,
													wxEmptyString,
													wxDefaultPosition,
													wxSize (110, -1),
													0);
	textControl1642->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
																	&m_stringCheck));
	SetUpToolTip (textControl1642, IDS_ToolTip8405);
   bHSizer164->Add (textControl1642,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
	
	wxStaticText* staticText1643;
   staticText1643 = new wxStaticText (this,
                                       IDC_SVM_GAMMA_INFO,
                                       wxT("(used for Polynomial, RBF, and Sigmoid Kernels)"),
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       0);
   staticText1643->Wrap (-1);
   bHSizer164->Add (staticText1643,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
   m_bVSizer151->Add (bHSizer164, 1, wxALL|wxEXPAND, 2);
	
   wxBoxSizer* bHSizer165 = new wxBoxSizer (wxHORIZONTAL);
	wxStaticText* staticText1651;
   staticText1651 = new wxStaticText (this,
                                       IDC_SVM_COEF0_PROMPT,
                                       wxT("Coef0:"),
                                       wxDefaultPosition,
													wxSize (90, -1),
                                       0);
   staticText1651->Wrap (-1);
	SetUpToolTip (staticText1651, IDS_ToolTip8406);
   bHSizer165->Add (staticText1651,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));

	wxTextCtrl* textControl1652;
   textControl1652 = new wxTextCtrl (this,
													IDC_SVM_COEF0,
													wxEmptyString,
													wxDefaultPosition,
													wxSize (110, -1),
													0);
	textControl1652->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
																	&m_stringCheck));
	SetUpToolTip (textControl1652, IDS_ToolTip8406);
   bHSizer165->Add (textControl1652,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
	
	wxStaticText* staticText1653;
   staticText1653 = new wxStaticText (this,
                                       IDC_SVM_COEF0_INFO,
                                       wxT("(used for Polynomial and Sigmoid Kernels)"),
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       0);
   staticText1653->Wrap (-1);
   bHSizer165->Add (staticText1653,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
   m_bVSizer151->Add (bHSizer165, 1, wxALL|wxEXPAND, 2);
	
	wxBoxSizer* bHSizer168 = new wxBoxSizer (wxHORIZONTAL);
	wxStaticText* staticText1681;
	staticText1681 = new wxStaticText (this,
													IDC_SVM_COST_PROMPT,
													wxT("Cost Value:"),
													wxDefaultPosition,
													wxSize (90, -1),
													0);
   staticText1681->Wrap (-1);
	SetUpToolTip (staticText1681, IDS_ToolTip8409);
   bHSizer168->Add (staticText1681,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
	
	wxTextCtrl* textControl1682;
   textControl1682 = new wxTextCtrl (this,
													IDC_SVM_COST,
													wxEmptyString,
													wxDefaultPosition,
													wxSize (110, -1),
													0);
	textControl1682->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
																	&m_stringCheck));
	SetUpToolTip (textControl1682, IDS_ToolTip8409);
   bHSizer168->Add (textControl1682,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
	
	wxStaticText* staticText1683;
   staticText1683 = new wxStaticText (this,
                                       IDC_SVM_COST_INFO,
                                       wxT("(used for C-SVC, epsilon-SVR, and nu-SVR SVM types)"),
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       0);
   staticText1683->Wrap (-1);
   bHSizer168->Add (staticText1683,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
   m_bVSizer151->Add (bHSizer168, 1, wxALL|wxEXPAND, 2);
	
	wxBoxSizer* bHSizer170 = new wxBoxSizer (wxHORIZONTAL);
	wxStaticText* staticText1701;
	staticText1701 = new wxStaticText (this,
													IDC_SVM_NU_PROMPT,
													wxT("nu:"),
													wxDefaultPosition,
													wxSize (90, -1),
													0);
   staticText1701->Wrap (-1);
	SetUpToolTip (staticText1701, IDS_ToolTip8411);
   bHSizer170->Add (staticText1701,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
	
	wxTextCtrl* textControl1702;
   textControl1702 = new wxTextCtrl (this,
													IDC_SVM_NU,
													wxEmptyString,
													wxDefaultPosition,
													wxSize (110, -1),
													0);
	textControl1702->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
																	&m_stringCheck));
	SetUpToolTip (textControl1702, IDS_ToolTip8411);
   bHSizer170->Add (textControl1702,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
	
	wxStaticText* staticText1703;
   staticText1703 = new wxStaticText (this,
                                       IDC_SVM_NU_INFO,
                                       wxT("(used for nu-SVC, one-class, and nu-SVR SVM types)"),
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       0);
   staticText1703->Wrap (-1);
   bHSizer170->Add (staticText1703,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
   m_bVSizer151->Add (bHSizer170, 1, wxALL|wxEXPAND, 2);
	
	wxBoxSizer* bHSizer171 = new wxBoxSizer (wxHORIZONTAL);
	wxStaticText* staticText1711;
	staticText1711 = new wxStaticText (this,
													IDC_SVM_P_PROMPT,
													wxT("p:"),
													wxDefaultPosition,
													wxSize (90, -1),
													0);
   staticText1711->Wrap (-1);
	SetUpToolTip (staticText1711, IDS_ToolTip8412);
   bHSizer171->Add (staticText1711,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
	
	wxTextCtrl* textControl1712;
   textControl1712 = new wxTextCtrl (this,
													IDC_SVM_P,
													wxEmptyString,
													wxDefaultPosition,
													wxSize (110, -1),
													0);
	textControl1712->SetValidator (wxTextValidator (wxFILTER_NUMERIC,
																	&m_stringCheck));
	SetUpToolTip (textControl1712, IDS_ToolTip8412);
   bHSizer171->Add (textControl1712,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
	
	wxStaticText* staticText1713;
   staticText1713 = new wxStaticText (this,
                                       IDC_SVM_P_INFO,
                                       wxT("(used for epsilon-SVR SVM type)"),
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       0);
   staticText1713->Wrap (-1);
   bHSizer171->Add (staticText1713,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
   m_bVSizer151->Add (bHSizer171, 1, wxALL|wxEXPAND, 2);
	
	wxBoxSizer* bHSizer173 = new wxBoxSizer (wxHORIZONTAL);
	wxCheckBox*	checkBox1731;
	checkBox1731 = new wxCheckBox (this,
												IDC_SVM_PROBABILITY,
												wxT("Train for probability estimates"),
												wxDefaultPosition,
												wxSize (210, -1),
												0);
	SetUpToolTip (checkBox1731, IDS_ToolTip8414);
   bHSizer173->Add (checkBox1731,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
	
	wxStaticText* staticText1732;
   staticText1732 = new wxStaticText (this,
                                       IDC_SVM_PROBABILITY_INFO,
                                       wxT("(used for SVC and SVR SVM types)"),
                                       wxDefaultPosition,
                                       wxDefaultSize,
                                       0);
   staticText1732->Wrap (-1);
   bHSizer173->Add (staticText1732,
   						wxSizerFlags(0).ReserveSpaceEvenIfHidden().
   													Align(wxALIGN_CENTER).Border(wxALL, 5));
	
   m_bVSizer151->Add (bHSizer173, 1, wxALL|wxEXPAND, 2);
	
   CreateStandardButtons (m_bVSizer151);
	
   SetSizerAndFit (m_bVSizer151);
   
}   // end "CreateControls"



//------------------------------------------------------------------------------------
//                         Copyright (2012-2019)
//                     (c) Purdue Research Foundation
//                           All rights reserved.
//
//   Function name:		void DoDialog
//
//   Software purpose:	The purpose of this routine is to present the SVM Classifier
//                   	parameters dialog box to the user and copy the
//                    	revisions back to the classify specification structure if
//                    	the user selected OK.
//
//   Parameters in: 		None
//
//   Parameters out: 	None
//
//   Value Returned:		None
//
//   Called By:
//
//   Coded By:  			Tsung Tai Yeh			Date: 09/??/2019
//   Revised By:  		Larry L. Biehl    	Date: 09/20/2019

Boolean CMSVMClassifyDialog::DoDialog ()

{
   SInt16                        	returnCode;
   
   bool                          	continueFlag = false;
   
   
   		// Make sure intialization has been completed.
   
   if (!m_initializedFlag)
      																				return (false);
   
   returnCode = ShowModal ();
   
   if (returnCode == wxID_OK)
   	{
		continueFlag = true;

		if (gClassifySpecsPtr->svm_type != m_svm_type ||
				gClassifySpecsPtr->svm_kernel_type != m_kernel_type ||
					gClassifySpecsPtr->svm_degree != m_degree ||
						gClassifySpecsPtr->svm_gamma != m_gamma ||
							gClassifySpecsPtr->svm_coef0 != m_coef0 ||
								gClassifySpecsPtr->svm_cache_size != m_cache_size ||
									gClassifySpecsPtr->svm_eps != m_eps ||
										gClassifySpecsPtr->svm_cost != m_cost ||
											//gClassifySpecsPtr->svm_nr_weight != m_nr_weight ||
												gClassifySpecsPtr->svm_nu != m_nu ||
													gClassifySpecsPtr->svm_p != m_p ||
				gClassifySpecsPtr->svm_shrinking != m_shrinking ||
					gClassifySpecsPtr->svm_probability != m_probability)
			gClassifySpecsPtr->supportVectorMachineModelAvailableFlag = false;
		
		gClassifySpecsPtr->svm_type = m_svm_type;
		gClassifySpecsPtr->svm_kernel_type = m_kernel_type;
		gClassifySpecsPtr->svm_degree = m_degree;
		gClassifySpecsPtr->svm_gamma = m_gamma;
		gClassifySpecsPtr->svm_coef0 = m_coef0;
		gClassifySpecsPtr->svm_nu = m_nu;
		gClassifySpecsPtr->svm_cache_size = m_cache_size;
		gClassifySpecsPtr->svm_cost = m_cost;
		gClassifySpecsPtr->svm_eps = m_eps;
		gClassifySpecsPtr->svm_p = m_p;
		gClassifySpecsPtr->svm_shrinking = m_shrinking;
		gClassifySpecsPtr->svm_probability = m_probability;
		
		}   // end "if (returnCode == IDOK)"
   
   return (continueFlag);
   
}   // end "DoDialog"



void CMSVMClassifyDialog::OnInitDialog (
				wxInitDialogEvent&            event)

{
	m_svm_type = gClassifySpecsPtr->svm_type;
	m_kernel_type = gClassifySpecsPtr->svm_kernel_type;
	m_degree = gClassifySpecsPtr->svm_degree;
	m_gamma = gClassifySpecsPtr->svm_gamma;
	m_coef0 = gClassifySpecsPtr->svm_coef0;
	m_cache_size = gClassifySpecsPtr->svm_cache_size;
	m_eps = gClassifySpecsPtr->svm_eps;
	m_cost = gClassifySpecsPtr->svm_cost;
	m_nu = gClassifySpecsPtr->svm_nu;
	m_p = gClassifySpecsPtr->svm_p;
	m_shrinking = gClassifySpecsPtr->svm_shrinking;
	m_probability = gClassifySpecsPtr->svm_probability;
	
   CMDialog::OnInitDialog (event);
	
   SVMClassifyDialogShowHideParameters (this,
														m_svm_type,
														m_kernel_type);
   SetSizerAndFit (m_bVSizer151);
	
}   // end "OnInitDialog"



void CMSVMClassifyDialog::OnSelendokSVMKernelCombo (
				wxCommandEvent& 				event)

{
   wxComboBox* comboBoxPr = (wxComboBox*)FindWindow (IDC_SVM_KERNEL_TYPE);
   m_kernel_type = comboBoxPr->GetSelection ();
	
   SVMClassifyDialogShowHideParameters (this,
														m_svm_type,
														m_kernel_type);
   SetSizerAndFit (m_bVSizer151);

}	// end "OnSelendokSVMKernelCombo"



void CMSVMClassifyDialog::OnSelendokSVMTypeCombo (
				wxCommandEvent& 				event)

{
	wxComboBox* comboBoxPr = (wxComboBox*)FindWindow (IDC_SVM_TYPE);
   m_svm_type = comboBoxPr->GetSelection ();
	
   SVMClassifyDialogShowHideParameters (this,
														m_svm_type,
														m_kernel_type);
   SetSizerAndFit (m_bVSizer151);

}	// end "OnSelendokSVMTypeCombo"



bool CMSVMClassifyDialog::TransferDataFromWindow ()

{
	wxCheckBox*					checkBoxPtr;
	wxComboBox* 				comboBoxPr;
	wxTextCtrl* 				textCtrlPtr;
	
	
	comboBoxPr = (wxComboBox*)FindWindow (IDC_SVM_TYPE);
   m_svm_type = comboBoxPr->GetSelection ();
	
   comboBoxPr = (wxComboBox*)FindWindow (IDC_SVM_KERNEL_TYPE);
   m_kernel_type = comboBoxPr->GetSelection ();
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_DEGREE);
   m_degree = wxAtoi (textCtrlPtr->GetValue ());
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_GAMMA);
   m_gamma = wxAtof (textCtrlPtr->GetValue ());
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_COEF0);
   m_coef0 = wxAtof (textCtrlPtr->GetValue ());
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_CACHE_SIZE);
   m_cache_size = wxAtof (textCtrlPtr->GetValue ());
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_EPS);
   m_eps = wxAtof (textCtrlPtr->GetValue ());
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_COST);
   m_cost = wxAtof (textCtrlPtr->GetValue ());
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_NU);
   m_nu = wxAtof (textCtrlPtr->GetValue ());
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_P);
   m_p = wxAtof (textCtrlPtr->GetValue ());
	
   checkBoxPtr = (wxCheckBox*)FindWindow (IDC_SVM_SHRINKING);
   m_shrinking = checkBoxPtr->GetValue ();
	
   checkBoxPtr = (wxCheckBox*)FindWindow (IDC_SVM_PROBABILITY);
   m_probability = checkBoxPtr->GetValue ();
	
   return true;
   
}   // end "TransferDataFromWindow"



bool CMSVMClassifyDialog::TransferDataToWindow ()

{
	wxCheckBox*					checkBoxPtr;
	wxComboBox* 				comboBoxPr;
	wxTextCtrl* 				textCtrlPtr;
	
	
   comboBoxPr = (wxComboBox*)FindWindow (IDC_SVM_TYPE);
   comboBoxPr->SetSelection (m_svm_type);
	
   comboBoxPr = (wxComboBox*)FindWindow (IDC_SVM_KERNEL_TYPE);
   comboBoxPr->SetSelection (m_kernel_type);
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_DEGREE);
   textCtrlPtr->SetValue (wxString::Format (wxT("%d"), m_degree));
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_GAMMA);
   textCtrlPtr->SetValue (wxString::Format (wxT("%f"), m_gamma));
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_COEF0);
   textCtrlPtr->SetValue (wxString::Format (wxT("%f"), m_coef0));
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_CACHE_SIZE);
   textCtrlPtr->SetValue (wxString::Format (wxT("%.0f"), m_cache_size));
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_EPS);
   textCtrlPtr->SetValue (wxString::Format (wxT("%f"), m_eps));
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_COST);
   textCtrlPtr->SetValue (wxString::Format (wxT("%f"), m_cost));
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_NU);
   textCtrlPtr->SetValue (wxString::Format (wxT("%f"), m_nu));
	
   textCtrlPtr = (wxTextCtrl*)FindWindow (IDC_SVM_P);
   textCtrlPtr->SetValue (wxString::Format (wxT("%f"), m_p));
	
   checkBoxPtr = (wxCheckBox*)FindWindow (IDC_SVM_SHRINKING);
   checkBoxPtr->SetValue (m_shrinking);
	
   checkBoxPtr = (wxCheckBox*)FindWindow (IDC_SVM_PROBABILITY);
   checkBoxPtr->SetValue (m_probability);
	
   return true;
   
}   // end "TransferDataToWindow"

