/*----------------------------------------------------------------------
Debugging Applications for Microsoft .NET and Microsoft Windows
Copyright © 1997-2003 John Robbins -- All rights reserved.
----------------------------------------------------------------------*/
#ifndef _JFX_H
#define _JFX_H

// Ensure common headers are included.
#include <windows.h>
#include <commctrl.h>
#include <shlwapi.h>

#ifndef _BUGSLAYERUTIL_H
#include <BugslayerUtil.h>
#endif // _BUGSLAYERUTIL_H

// Disable common warnings.

// Turn off "unreferenced formal parameter"
#pragma warning ( disable : 4100 )
// Turn off "function '' not inlined"
#pragma warning ( disable : 4710 )

#include <WindowsX.h>

#include "JString.h"
#include "JWnd.h"
#include "JWinApp.h"
#include "JModalDlg.h"
#include "JButton.h"
#include "JMenu.h"
#include "ShellAPI.h"
#include "JTrayNotification.h"
#include "JStatusBar.h"
#include "JGDI.h"
#include "JRegistry.h"
#include "JListView.h"
#include "JComboBox.h"
#include "Layout.h"
#include "JListBox.h"
#include "JTabControl.h"
#include "JImageList.h"
#include "JSimpleColl.h"

// Restore the warnings I turned off earlier.

// Turn on "unreferenced formal parameter" warnings from here on out.
#pragma warning ( default : 4100 )
// Turn off "function '' not inlined"
//#pragma warning ( default : 4710 )

#endif  // _JFX_H

