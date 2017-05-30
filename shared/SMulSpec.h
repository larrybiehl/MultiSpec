//------------------------------------------------------------------------------------
//
//	SMulSpec.h
//	
//	(C) Copyright Purdue Research Foundation 1988-2015  All rights reserved.
//	
//	Main header file for MultiSpec which contains calls to other include
// files for constants, resource constants, definitions, and function 
// prototypes.
//	
//	Written 03/29/1988 by Larry L. Biehl
//	Revised 03/07/2017 by Larry L. Biehl
//	Revised 06/24/2013 by Abdur Maud
//	
//------------------------------------------------------------------------------------

#ifndef __SMulSpec__
#define __SMulSpec__

#if defined __MWERKS__ 
	#pragma once
#endif	// defined __MWERKS__ 

//#if !defined __MWERKS__
//	#define _X86_ 
//#endif	// !defined __MWERKS__

#if defined multispec_mac
#	include "wchar.h"
#endif

#if defined multispec_win
	#include "stdafx.h"
	//	#ifndef WINVER
	//		#define  WINVER  0x0400
	//	#endif	// !WINVER

	//	#ifndef _WIN32_WINNT
	//#define _WIN32_WINNT	0x0501 // _WIN32_WINNT_NT4
	//	#endif	// !_WIN32_WINNT

	//	#ifndef _WIN32_WINDOWS
	//#define _WIN32_WINDOWS	0x0501 // _WIN32_WINNT_NT4
	//	#endif	// !_WIN32_WINDOWS

	//	#ifndef _WIN32_IE
	//#define _WIN32_IE		0x0501 // _WIN32_WINNT_NT4
	//	#endif	// !_WIN32_IE

	//#ifndef DOUBLE
	//	typedef double DOUBLE;
	//#endif // !DOUBLE_ 

	#define include_hdf_capability 1 
	#define include_gdal_capability 1
	#define include_hdf5_capability 1

	//	#include "stdafx.h"
	#include <math.h>
	#include "float.h" 
	#include "limits.h" 
	#include "windowsx.h" 
	#include "resource.h"       // main symbols
#endif	// defined multispec_win 

#if defined multispec_lin
         // GDAL capability
	#ifdef NetBeansProject
		#define include_hdf_capability 1
		#define include_gdal_capability 0
		#define include_hdf5_capability 0
	#else	// ifndef NetBeansProject
		#define include_hdf_capability 1
		#define include_gdal_capability 1
		#define include_hdf5_capability 1 
	#endif
#endif	// end "defined multispec_lin"

#include "SConsnts.h" 
#include "SRConsnt.h"
#include "SDefines.h" 
#include "SGraphic.h"
#include "SExtGlob.h"
#include "SProtype.h"  

#if defined multispec_lin
	//#include "SDecGlob.h" // DO NOT include this. SExtGlob.h is sufficient
	#include "resource.h"
	//#include "SGrafVew.h"
	#include <time.h>
	#include <cmath>
	#include <cfloat>
	#include <float.h>
	#include <math.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <limits.h>
	#include <iostream>
	#include <string>
	#include <wx/gdicmn.h>
#endif		// defined multispec_lin

#endif // __SMulSpec__             
