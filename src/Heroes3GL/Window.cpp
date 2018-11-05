/*
	MIT License

	Copyright (c) 2018 Oleksiy Ryabchun

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "stdafx.h"
#include "Window.h"
#include "CommCtrl.h"
#include "Shellapi.h"
#include "Resource.h"
#include "Config.h"
#include "Hooks.h"

namespace Window
{
	WNDPROC OldWindowProc;

	LRESULT __stdcall AboutProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_INITDIALOG:
		{
			SetWindowLong(hDlg, GWL_EXSTYLE, NULL);
			EnumChildWindows(hDlg, Hooks::EnumChildProc, NULL);

			CHAR email[50];
			GetDlgItemText(hDlg, IDC_LNK_EMAIL, email, sizeof(email) - 1);
			CHAR anchor[256];
			StrPrint(anchor, "<A HREF=\"mailto:%s\">%s</A>", email, email);
			SetDlgItemText(hDlg, IDC_LNK_EMAIL, anchor);

			break;
		}

		case WM_NOTIFY:
		{
			if (((NMHDR*)lParam)->code == NM_CLICK && wParam == IDC_LNK_EMAIL)
			{
				NMLINK* pNMLink = (NMLINK*)lParam;
				LITEM iItem = pNMLink->item;

				CHAR url[256];
				StrToAnsi(url, pNMLink->item.szUrl, sizeof(url) - 1);

				SHELLEXECUTEINFO shExecInfo;
				MemoryZero(&shExecInfo, sizeof(SHELLEXECUTEINFO));
				shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
				shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
				shExecInfo.lpFile = url;
				shExecInfo.nShow = SW_SHOW;

				ShellExecuteEx(&shExecInfo);
			}

			break;
		}

		case WM_COMMAND:
		{
			if (wParam == IDC_BTN_OK)
				EndDialog(hDlg, TRUE);
			break;
		}

		default:
			break;
		}

		return DefWindowProc(hDlg, uMsg, wParam, lParam);
	}

	BOOL __fastcall FindMenuByChildId(HMENU hMenu, DWORD childId, HMENU* hParent, DWORD* pos)
	{
		DWORD count = GetMenuItemCount(hMenu);
		while (count--)
		{
			HMENU hSub = GetSubMenu(hMenu, count);
			if (hSub)
			{
				DWORD subCount = GetMenuItemCount(hSub);
				while (subCount--)
				{
					if (GetMenuItemID(hSub, subCount) == childId)
					{
						*hParent = hMenu;
						*pos = count;
						return TRUE;
					}

					if (FindMenuByChildId(hSub, childId, hParent, pos))
						return TRUE;
				}
			}
		}

		return FALSE;
	}

	VOID __fastcall CheckMenu(HMENU hMenu)
	{
		if (!hMenu)
			return;

		CheckMenuItem(hMenu, IDM_PATCH_CPU, MF_BYCOMMAND | (configColdCPU ? MF_CHECKED : MF_UNCHECKED));

		EnableMenuItem(hMenu, IDM_ASPECT_RATIO, MF_BYCOMMAND | (!isNoGL ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)));
		CheckMenuItem(hMenu, IDM_ASPECT_RATIO, MF_BYCOMMAND | (!isNoGL && configImageAspect ? MF_CHECKED : MF_UNCHECKED));

		EnableMenuItem(hMenu, IDM_VSYNC, MF_BYCOMMAND | (!isNoGL && (!glVersion || WGLSwapInterval) ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)));
		CheckMenuItem(hMenu, IDM_VSYNC, MF_BYCOMMAND | (!isNoGL && configImageVSync && (!glVersion || WGLSwapInterval) ? MF_CHECKED : MF_UNCHECKED));

		CheckMenuItem(hMenu, IDM_FILT_OFF, MF_BYCOMMAND | MF_UNCHECKED);

		EnableMenuItem(hMenu, IDM_FILT_LINEAR, MF_BYCOMMAND | (!isNoGL ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)));
		CheckMenuItem(hMenu, IDM_FILT_LINEAR, MF_BYCOMMAND | MF_UNCHECKED);

		DWORD isFiltersEnabled = !isNoGL && (!glVersion || glVersion >= GL_VER_3_0) ? MF_ENABLED : (MF_DISABLED | MF_GRAYED);
		EnableMenuItem(hMenu, IDM_FILT_CUBIC, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_CUBIC, MF_BYCOMMAND | MF_UNCHECKED);

		// ScaleNx
		HMENU hMenuScaleNx = NULL; DWORD posScaleNx;
		FindMenuByChildId(hMenu, IDM_FILT_SCALENX_LINEAR, &hMenuScaleNx, &posScaleNx);
		if (hMenuScaleNx)
		{
			EnableMenuItem(hMenuScaleNx, posScaleNx, MF_BYPOSITION | isFiltersEnabled);
			CheckMenuItem(hMenuScaleNx, posScaleNx, MF_BYPOSITION | MF_UNCHECKED);
		}

		EnableMenuItem(hMenu, IDM_FILT_SCALENX_LINEAR, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_SCALENX_LINEAR, MF_BYCOMMAND | MF_UNCHECKED);
		EnableMenuItem(hMenu, IDM_FILT_SCALENX_CUBIC, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_SCALENX_CUBIC, MF_BYCOMMAND | MF_UNCHECKED);
		EnableMenuItem(hMenu, IDM_FILT_SCALENX_2X, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_SCALENX_2X, MF_BYCOMMAND | MF_UNCHECKED);
		EnableMenuItem(hMenu, IDM_FILT_SCALENX_3X, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_SCALENX_3X, MF_BYCOMMAND | MF_UNCHECKED);

		// Eagle
		HMENU hMenuEagle = NULL; DWORD posEagle;
		FindMenuByChildId(hMenu, IDM_FILT_EAGLE_LINEAR, &hMenuEagle, &posEagle);
		if (hMenuEagle)
		{
			EnableMenuItem(hMenuEagle, posEagle, MF_BYPOSITION | isFiltersEnabled);
			CheckMenuItem(hMenuEagle, posEagle, MF_BYPOSITION | MF_UNCHECKED);
		}

		EnableMenuItem(hMenu, IDM_FILT_EAGLE_LINEAR, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_EAGLE_LINEAR, MF_BYCOMMAND | MF_UNCHECKED);
		EnableMenuItem(hMenu, IDM_FILT_EAGLE_CUBIC, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_EAGLE_CUBIC, MF_BYCOMMAND | MF_UNCHECKED);

		// XSal
		HMENU hMenuXSal = NULL; DWORD posXSal;
		FindMenuByChildId(hMenu, IDM_FILT_XSAL_LINEAR, &hMenuXSal, &posXSal);
		if (hMenuXSal)
		{
			EnableMenuItem(hMenuXSal, posXSal, MF_BYPOSITION | isFiltersEnabled);
			CheckMenuItem(hMenuXSal, posXSal, MF_BYPOSITION | MF_UNCHECKED);
		}

		EnableMenuItem(hMenu, IDM_FILT_XSAL_LINEAR, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_XSAL_LINEAR, MF_BYCOMMAND | MF_UNCHECKED);
		EnableMenuItem(hMenu, IDM_FILT_XSAL_CUBIC, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_XSAL_CUBIC, MF_BYCOMMAND | MF_UNCHECKED);

		// ScaleHQ
		HMENU hMenuScaleHQ = NULL; DWORD posScaleHQ;
		FindMenuByChildId(hMenu, IDM_FILT_SCALEHQ_LINEAR, &hMenuScaleHQ, &posScaleHQ);
		if (hMenuScaleHQ)
		{
			EnableMenuItem(hMenuScaleHQ, posScaleHQ, MF_BYPOSITION | isFiltersEnabled);
			CheckMenuItem(hMenuScaleHQ, posScaleHQ, MF_BYPOSITION | MF_UNCHECKED);
		}

		EnableMenuItem(hMenu, IDM_FILT_SCALEHQ_LINEAR, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_SCALEHQ_LINEAR, MF_BYCOMMAND | MF_UNCHECKED);
		EnableMenuItem(hMenu, IDM_FILT_SCALEHQ_CUBIC, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_SCALEHQ_CUBIC, MF_BYCOMMAND | MF_UNCHECKED);
		EnableMenuItem(hMenu, IDM_FILT_SCALEHQ_2X, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_SCALEHQ_2X, MF_BYCOMMAND | MF_UNCHECKED);
		EnableMenuItem(hMenu, IDM_FILT_SCALEHQ_4X, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_SCALEHQ_4X, MF_BYCOMMAND | MF_UNCHECKED);

		// xBRz
		HMENU hMenuXBRZ = NULL; DWORD posXBRZ;
		FindMenuByChildId(hMenu, IDM_FILT_XRBZ_LINEAR, &hMenuXBRZ, &posXBRZ);
		if (hMenuXBRZ)
		{
			EnableMenuItem(hMenuXBRZ, posXBRZ, MF_BYPOSITION | isFiltersEnabled);
			CheckMenuItem(hMenuXBRZ, posXBRZ, MF_BYPOSITION | MF_UNCHECKED);
		}

		EnableMenuItem(hMenu, IDM_FILT_XRBZ_LINEAR, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_XRBZ_LINEAR, MF_BYCOMMAND | MF_UNCHECKED);
		EnableMenuItem(hMenu, IDM_FILT_XRBZ_CUBIC, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_XRBZ_CUBIC, MF_BYCOMMAND | MF_UNCHECKED);
		EnableMenuItem(hMenu, IDM_FILT_XRBZ_2X, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_XRBZ_2X, MF_BYCOMMAND | MF_UNCHECKED);
		EnableMenuItem(hMenu, IDM_FILT_XRBZ_3X, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_XRBZ_3X, MF_BYCOMMAND | MF_UNCHECKED);
		EnableMenuItem(hMenu, IDM_FILT_XRBZ_4X, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_XRBZ_4X, MF_BYCOMMAND | MF_UNCHECKED);
		EnableMenuItem(hMenu, IDM_FILT_XRBZ_5X, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_XRBZ_5X, MF_BYCOMMAND | MF_UNCHECKED);
		EnableMenuItem(hMenu, IDM_FILT_XRBZ_6X, MF_BYCOMMAND | isFiltersEnabled);
		CheckMenuItem(hMenu, IDM_FILT_XRBZ_6X, MF_BYCOMMAND | MF_UNCHECKED);

		switch (configImageFilter)
		{
		case FilterLinear:
			CheckMenuItem(hMenu, IDM_FILT_LINEAR, MF_BYCOMMAND | MF_CHECKED);
			break;

		case FilterCubic:
			CheckMenuItem(hMenu, isFiltersEnabled == MF_ENABLED ? IDM_FILT_CUBIC : IDM_FILT_OFF, MF_BYCOMMAND | MF_CHECKED);
			break;

		case FilterScaleNx:
			if (isFiltersEnabled == MF_ENABLED)
			{
				if (hMenuScaleNx)
					CheckMenuItem(hMenuScaleNx, posScaleNx, MF_BYPOSITION | MF_CHECKED);

				CheckMenuItem(hMenu, HIWORD(configImageScaleNx) ? IDM_FILT_SCALENX_CUBIC : IDM_FILT_SCALENX_LINEAR, MF_BYCOMMAND | MF_CHECKED);
				switch (LOWORD(configImageScaleNx))
				{
				case 3:
					CheckMenuItem(hMenu, IDM_FILT_SCALENX_3X, MF_BYCOMMAND | MF_CHECKED);
					break;

				default:
					CheckMenuItem(hMenu, IDM_FILT_SCALENX_2X, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
			}
			else
				CheckMenuItem(hMenu, IDM_FILT_OFF, MF_BYCOMMAND | MF_CHECKED);

			break;

		case FilterEagle:
			if (isFiltersEnabled == MF_ENABLED)
			{
				if (hMenuEagle)
					CheckMenuItem(hMenuEagle, posEagle, MF_BYPOSITION | MF_CHECKED);

				CheckMenuItem(hMenu, HIWORD(configImageEagle) ? IDM_FILT_EAGLE_CUBIC : IDM_FILT_EAGLE_LINEAR, MF_BYCOMMAND | MF_CHECKED);
				CheckMenuItem(hMenu, IDM_FILT_EAGLE_2X, MF_BYCOMMAND | MF_CHECKED);
			}
			else
				CheckMenuItem(hMenu, IDM_FILT_OFF, MF_BYCOMMAND | MF_CHECKED);

			break;

		case FilterXSal:
			if (isFiltersEnabled == MF_ENABLED)
			{
				if (hMenuXSal)
					CheckMenuItem(hMenuXSal, posXSal, MF_BYPOSITION | MF_CHECKED);

				CheckMenuItem(hMenu, HIWORD(configImageXSal) ? IDM_FILT_XSAL_CUBIC : IDM_FILT_XSAL_LINEAR, MF_BYCOMMAND | MF_CHECKED);
				CheckMenuItem(hMenu, IDM_FILT_XSAL_2X, MF_BYCOMMAND | MF_CHECKED);
			}
			else
				CheckMenuItem(hMenu, IDM_FILT_OFF, MF_BYCOMMAND | MF_CHECKED);

			break;

		case FilterScaleHQ:
			if (isFiltersEnabled == MF_ENABLED)
			{
				if (hMenuScaleHQ)
					CheckMenuItem(hMenuScaleHQ, posScaleHQ, MF_BYPOSITION | MF_CHECKED);

				CheckMenuItem(hMenu, HIWORD(configImageScaleHQ) ? IDM_FILT_SCALEHQ_CUBIC : IDM_FILT_SCALEHQ_LINEAR, MF_BYCOMMAND | MF_CHECKED);
				switch (LOWORD(configImageScaleHQ))
				{
				case 4:
					CheckMenuItem(hMenu, IDM_FILT_SCALEHQ_4X, MF_BYCOMMAND | MF_CHECKED);
					break;

				default:
					CheckMenuItem(hMenu, IDM_FILT_SCALEHQ_2X, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
			}
			else
				CheckMenuItem(hMenu, IDM_FILT_OFF, MF_BYCOMMAND | MF_CHECKED);

			break;

		case FilterXRBZ:
			if (isFiltersEnabled == MF_ENABLED)
			{
				if (hMenuXBRZ)
					CheckMenuItem(hMenuXBRZ, posXBRZ, MF_BYPOSITION | MF_CHECKED);

				CheckMenuItem(hMenu, HIWORD(configImageXBRZ) ? IDM_FILT_XRBZ_CUBIC : IDM_FILT_XRBZ_LINEAR, MF_BYCOMMAND | MF_CHECKED);
				switch (LOWORD(configImageXBRZ))
				{
				case 3:
					CheckMenuItem(hMenu, IDM_FILT_XRBZ_3X, MF_BYCOMMAND | MF_CHECKED);
					break;

				case 4:
					CheckMenuItem(hMenu, IDM_FILT_XRBZ_4X, MF_BYCOMMAND | MF_CHECKED);
					break;

				case 5:
					CheckMenuItem(hMenu, IDM_FILT_XRBZ_5X, MF_BYCOMMAND | MF_CHECKED);
					break;

				case 6:
					CheckMenuItem(hMenu, IDM_FILT_XRBZ_6X, MF_BYCOMMAND | MF_CHECKED);
					break;

				default:
					CheckMenuItem(hMenu, IDM_FILT_XRBZ_2X, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
			}
			else
				CheckMenuItem(hMenu, IDM_FILT_OFF, MF_BYCOMMAND | MF_CHECKED);

			break;

		default:
			CheckMenuItem(hMenu, IDM_FILT_OFF, MF_BYCOMMAND | MF_CHECKED);
			break;
		}
	}

	VOID __fastcall CheckMenu(HWND hWnd)
	{
		CheckMenu(GetMenu(hWnd));
	}
}