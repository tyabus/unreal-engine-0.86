/*=============================================================================
	stdafx.h: Include file for all commonly used but infrequently
	modified headers.

	Copyright 1997 Epic MegaGames, Inc. This software is a trade secret.
	Compiled with Visual C++ 4.0. Best viewed with Tabs=4.

	Revision history:
		* Created by Tim Sweeney
=============================================================================*/

/////////////////////////////////////////////////////////////////////////////
// ANSI C components.
/////////////////////////////////////////////////////////////////////////////
#include <math.h>

/////////////////////////////////////////////////////////////////////////////
// MFC components.
/////////////////////////////////////////////////////////////////////////////
#include <afxwin.h>		// MFC core and standard components.
#include <afxext.h>		// MFC extensions.
#include <afxdisp.h>	// MFC OLE automation classes.
#include <afxtempl.h>	// MFC template classes.
#include <afxcmn.h>		// MFC support for Windows 95 Common Controls.

/////////////////////////////////////////////////////////////////////////////
// Windows API components.
/////////////////////////////////////////////////////////////////////////////
#include <mmsystem.h>	// Multimedia timers.

/////////////////////////////////////////////////////////////////////////////
// Unreal components.
/////////////////////////////////////////////////////////////////////////////
#include "Unreal.h"		// Standard Unreal include.

/////////////////////////////////////////////////////////////////////////////
// Custom.
/////////////////////////////////////////////////////////////////////////////

// Standard Unreal windows messages.
enum ECustomUnrealWindowsMessages
{
	WM_RAWSYSCOMMAND = WM_USER+0x202,
};

/////////////////////////////////////////////////////////////////////////////
// Macros.
/////////////////////////////////////////////////////////////////////////////

// Macro replacing IMPLEMENT_OLECREATE to register a server as 
// single-use rather than multiple-use.
#define SINGLEUSE_IMPLEMENT_OLECREATE(class_name, external_name, \
	l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    COleObjectFactory class_name::factory(class_name::guid, \
    RUNTIME_CLASS(class_name), TRUE, _T(external_name)); \
    const GUID class_name::guid = \
    { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } };

/////////////////////////////////////////////////////////////////////////////
// The End.
/////////////////////////////////////////////////////////////////////////////
