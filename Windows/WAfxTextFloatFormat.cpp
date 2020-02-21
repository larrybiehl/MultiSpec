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
//	File:						WAfxTextFloatFormat.cpp
//
//	Authors:					Larry L. Biehl
//
//	Revision date:			10/25/2019
//
//	Language:				C++
//
//	System:					Windows Operating System
//
//	Brief description:	Reimplementation of AfxTextFloatFormat because of
//								buggy _sntscanf_s function
// See:
// - https://social.msdn.microsoft.com/Forums/vstudio/en-US/4a04b861-8b16-4bb3-8a0d-f4e78b1c2a5a/sntscanfs-doesnt-work-as-expected-in-vs2015?forum=vcgeneral
// - https://connect.microsoft.com/VisualStudio/feedback/details/1773279/bug-in-sntscanf-s
// - https://connect.microsoft.com/VisualStudio/feedback/details/1642606/mfc-cdialog-dodataexchange-cdataexchange-pdx-fails-when-value-is-zero
//
//------------------------------------------------------------------------------------

#include "SMultiSpec.h"
#include "stdafx.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


// Copied from MFC source code

static void AFXAPI AfxTextFloatFormat (CDataExchange* pDX, int nIDC,
  void* pData, double value, int nSizeGcvt)
{
  ASSERT(pData != NULL);

  pDX->PrepareEditCtrl (nIDC);
  HWND hWndCtrl;
  pDX->m_pDlgWnd->GetDlgItem (nIDC, &hWndCtrl);

  const int TEXT_BUFFER_SIZE = 400;
  TCHAR szBuffer[TEXT_BUFFER_SIZE] = { 0 };          // added: initialize whole buffer with 0

  if (pDX->m_bSaveAndValidate)
  {
    ::GetWindowText (hWndCtrl, szBuffer, _countof (szBuffer));

    double d;
    if (_sntscanf_s (szBuffer, _countof (szBuffer), _T("%lf"), &d) != 1)
    {
      AfxMessageBox (AFX_IDP_PARSE_REAL);
      pDX->Fail ();            // throws exception
    }
    if (nSizeGcvt == FLT_DIG)
      *((float*)pData) = (float)d;
    else
      *((double*)pData) = d;
	 /*
	 int numberChars = sprintf ((char*)&gTextString3,
		 " WAfxTextFloatFormat::AfxTextFloatFormat (d) %f%s",
		 d,
		 gEndOfLine);
	 ListString ((char*)&gTextString3, numberChars, gOutputTextH);
	 */
  }
  else
  {
    ATL_CRT_ERRORCHECK_SPRINTF (_sntprintf_s (szBuffer, _countof (szBuffer), _countof (szBuffer) - 1, _T("%.*g"), nSizeGcvt, value));
    AfxSetWindowText (hWndCtrl, szBuffer);
  }
}


void AFXAPI DDX_Text2 (CDataExchange* pDX, int nIDC, float& value)
{
    AfxTextFloatFormat (pDX, nIDC, &value, value, FLT_DIG);
}


void AFXAPI DDX_Text2 (CDataExchange* pDX, int nIDC, double& value)
{
    AfxTextFloatFormat (pDX, nIDC, &value, value, DBL_DIG);
}
