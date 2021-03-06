/*
	MIT License

	Copyright (c) 2020 Oleksiy Ryabchun

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
#include "IDraw7.h"
#include "Main.h"

IDraw7::~IDraw7()
{
	IDrawSurface7* surfaceEntry = this->surfaceEntries;
	while (surfaceEntry)
	{
		IDrawSurface7* curr = surfaceEntry->last;
		delete surfaceEntry;
		surfaceEntry = curr;
	}

	IDrawClipper* clipperEntry = this->clipperEntries;
	while (clipperEntry)
	{
		IDrawClipper* curr = clipperEntry->last;
		delete clipperEntry;
		clipperEntry = curr;
	}

	if (drawList == this)
		drawList = drawList->last;
	else
	{
		IDraw7* ddraw = drawList;
		while (ddraw)
		{
			if (ddraw->last == this)
			{
				ddraw->last = ddraw->last->last;
				break;
			}

			ddraw = ddraw->last;
		}
	}
}

HRESULT __stdcall IDraw7::QueryInterface(REFIID, LPVOID*) { return DD_OK; }
ULONG __stdcall IDraw7::AddRef() { return 0; }
ULONG __stdcall IDraw7::Release() { return 0; }
HRESULT __stdcall IDraw7::Compact() { return DD_OK; }
HRESULT __stdcall IDraw7::CreateClipper(DWORD, LPDIRECTDRAWCLIPPER*, IUnknown*) { return DD_OK; }
HRESULT __stdcall IDraw7::CreatePalette(DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE*, IUnknown*) { return DD_OK; }
HRESULT __stdcall IDraw7::CreateSurface(LPDDSURFACEDESC2, LPDIRECTDRAWSURFACE7*, IUnknown*) { return DD_OK; }
HRESULT __stdcall IDraw7::DuplicateSurface(LPDIRECTDRAWSURFACE7, LPDIRECTDRAWSURFACE7*) { return DD_OK; }
HRESULT __stdcall IDraw7::EnumDisplayModes(DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2) { return DD_OK; }
HRESULT __stdcall IDraw7::EnumSurfaces(DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMSURFACESCALLBACK7) { return DD_OK; }
HRESULT __stdcall IDraw7::FlipToGDISurface() { return DD_OK; }
HRESULT __stdcall IDraw7::GetCaps(LPDDCAPS, LPDDCAPS) { return DD_OK; }
HRESULT __stdcall IDraw7::GetDisplayMode(LPDDSURFACEDESC2) { return DD_OK; }
HRESULT __stdcall IDraw7::GetFourCCCodes(LPDWORD, LPDWORD) { return DD_OK; }
HRESULT __stdcall IDraw7::GetGDISurface(LPDIRECTDRAWSURFACE7*) { return DD_OK; }
HRESULT __stdcall IDraw7::GetMonitorFrequency(LPDWORD) { return DD_OK; }
HRESULT __stdcall IDraw7::GetScanLine(LPDWORD) { return DD_OK; }
HRESULT __stdcall IDraw7::GetVerticalBlankStatus(LPBOOL) { return DD_OK; }
HRESULT __stdcall IDraw7::Initialize(GUID*) { return DD_OK; }
HRESULT __stdcall IDraw7::RestoreDisplayMode() { return DD_OK; }
HRESULT __stdcall IDraw7::SetCooperativeLevel(HWND, DWORD) { return DD_OK; }
HRESULT __stdcall IDraw7::SetDisplayMode(DWORD, DWORD, DWORD, DWORD, DWORD) { return DD_OK; }
HRESULT __stdcall IDraw7::WaitForVerticalBlank(DWORD, HANDLE) { return DD_OK; }
HRESULT __stdcall IDraw7::GetAvailableVidMem(LPDDSCAPS2, LPDWORD, LPDWORD) { return DD_OK; }
HRESULT __stdcall IDraw7::GetSurfaceFromDC(HDC, LPDIRECTDRAWSURFACE7*) { return DD_OK; }
HRESULT __stdcall IDraw7::RestoreAllSurfaces() { return DD_OK; }
HRESULT __stdcall IDraw7::TestCooperativeLevel() { return DD_OK; }
HRESULT __stdcall IDraw7::GetDeviceIdentifier(LPDDDEVICEIDENTIFIER2, DWORD) { return DD_OK; }
HRESULT __stdcall IDraw7::StartModeTest(LPSIZE, DWORD, DWORD) { return DD_OK; }
HRESULT __stdcall IDraw7::EvaluateMode(DWORD, DWORD*) { return DD_OK; }