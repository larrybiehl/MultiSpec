// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and Microsoft
// QuickHelp and/or WinHelp documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define WINVER			0x0600 // _WIN32_WINNT_NT4
#define _WIN32_WINNT	0x0600 // _WIN32_WINNT_NT4
#define _WIN32_WINDOWS	0x0600 // _WIN32_WINNT_NT4
#define _WIN32_IE		0x0600 // _WIN32_WINNT_NT4

#define _CRT_SECURE_NO_WARNINGS

#if !defined DOUBLE
	#define	DOUBLE		double
#endif		// !defined DOUBLE

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
