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
#include "GLib.h"
#include "DirectDrawSurface.h"
#include "DirectDrawClipper.h"

#pragma region Not Implemented
HRESULT DirectDrawSurface::QueryInterface(REFIID riid, LPVOID * ppvObj) { return DD_OK; }
ULONG DirectDrawSurface::AddRef() { return 0; }
HRESULT DirectDrawSurface::AddAttachedSurface(LPDIRECTDRAWSURFACE7) { return DD_OK; }
HRESULT DirectDrawSurface::AddOverlayDirtyRect(LPRECT) { return DD_OK; }
HRESULT DirectDrawSurface::BltBatch(LPDDBLTBATCH, DWORD, DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::DeleteAttachedSurface(DWORD, LPDIRECTDRAWSURFACE7) { return DD_OK; }
HRESULT DirectDrawSurface::EnumAttachedSurfaces(LPVOID, LPDDENUMSURFACESCALLBACK7) { return DD_OK; }
HRESULT DirectDrawSurface::EnumOverlayZOrders(DWORD, LPVOID, LPDDENUMSURFACESCALLBACK7) { return DD_OK; }
HRESULT DirectDrawSurface::Flip(LPDIRECTDRAWSURFACE7, DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::GetAttachedSurface(LPDDSCAPS2, LPDIRECTDRAWSURFACE7 *) { return DD_OK; }
HRESULT DirectDrawSurface::GetCaps(LPDDSCAPS2) { return DD_OK; }
HRESULT DirectDrawSurface::GetClipper(LPDIRECTDRAWCLIPPER *) { return DD_OK; }
HRESULT DirectDrawSurface::GetColorKey(DWORD, LPDDCOLORKEY) { return DD_OK; }
HRESULT DirectDrawSurface::GetFlipStatus(DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::GetOverlayPosition(LPLONG, LPLONG) { return DD_OK; }
HRESULT DirectDrawSurface::GetPalette(LPDIRECTDRAWPALETTE *) { return DD_OK; }
HRESULT DirectDrawSurface::Initialize(LPDIRECTDRAW, LPDDSURFACEDESC2) { return DD_OK; }
HRESULT DirectDrawSurface::SetColorKey(DWORD, LPDDCOLORKEY) { return DD_OK; }
HRESULT DirectDrawSurface::SetOverlayPosition(LONG, LONG) { return DD_OK; }
HRESULT DirectDrawSurface::SetPalette(LPDIRECTDRAWPALETTE) { return DD_OK; }
HRESULT DirectDrawSurface::UpdateOverlay(LPRECT, LPDIRECTDRAWSURFACE7, LPRECT, DWORD, LPDDOVERLAYFX) { return DD_OK; }
HRESULT DirectDrawSurface::UpdateOverlayDisplay(DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::UpdateOverlayZOrder(DWORD, LPDIRECTDRAWSURFACE7) { return DD_OK; }
HRESULT DirectDrawSurface::GetDDInterface(LPVOID *) { return DD_OK; }
HRESULT DirectDrawSurface::PageLock(DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::PageUnlock(DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::SetSurfaceDesc(LPDDSURFACEDESC2, DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::SetPrivateData(REFGUID, LPVOID, DWORD, DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::GetPrivateData(REFGUID, LPVOID, LPDWORD) { return DD_OK; }
HRESULT DirectDrawSurface::FreePrivateData(REFGUID) { return DD_OK; }
HRESULT DirectDrawSurface::GetUniquenessValue(LPDWORD) { return DD_OK; }
HRESULT DirectDrawSurface::ChangeUniquenessValue() { return DD_OK; }
HRESULT DirectDrawSurface::SetPriority(DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::GetPriority(LPDWORD) { return DD_OK; }
HRESULT DirectDrawSurface::SetLOD(DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::GetLOD(LPDWORD) { return DD_OK; }
HRESULT DirectDrawSurface::IsLost() { return DD_OK; }
HRESULT DirectDrawSurface::Restore() { return DD_OK; }
HRESULT DirectDrawSurface::GetBltStatus(DWORD) { return DD_OK; }
HRESULT DirectDrawSurface::Unlock(LPRECT) { return DD_OK; }
HRESULT DirectDrawSurface::ReleaseDC(HDC dcMem) { return DD_OK; }
#pragma endregion

DirectDrawSurface::DirectDrawSurface(DirectDraw* lpDD, DWORD index)
{
	this->ddraw = lpDD;
	this->last = lpDD->surfaceEntries;

	this->attachedClipper = NULL;

	this->index = index;

	this->hBmp = NULL;
	this->hDc = NULL;

	this->width = 0;
	this->height = 0;
}

DirectDrawSurface::~DirectDrawSurface()
{
	if (this->hBmp)
		DeleteObject(this->hBmp);
}

VOID DirectDrawSurface::ReleaseBuffer()
{
	if (this->hBmp)
		DeleteObject(this->hBmp);

	if (this->hDc)
		DeleteDC(this->hDc);
}

VOID DirectDrawSurface::CreateBuffer(DWORD width, DWORD height)
{
	if (this->hBmp)
		DeleteObject(this->hBmp);

	this->width = width;
	this->height = height;

	BITMAPINFO* bmi = (BITMAPINFO*)calloc(1, sizeof(BITMAPINFO) + 8);
	{
		bmi->bmiHeader.biSize = sizeof(BITMAPINFO) + 8; // 108
		bmi->bmiHeader.biWidth = width;
		bmi->bmiHeader.biHeight = -*(LONG*)&height;
		bmi->bmiHeader.biPlanes = 1;
		bmi->bmiHeader.biBitCount = 16;
		bmi->bmiHeader.biCompression = BI_BITFIELDS;
		DWORD* mask = (DWORD*)&bmi->bmiColors;
		mask[0] = 0xF800;
		mask[1] = 0x07E0;
		mask[2] = 0x001F;

		HDC hDc = ::GetDC(NULL);
		{
			this->hBmp = CreateDIBSection(hDc, bmi, 0, (VOID**)&this->indexBuffer, 0, 0);
		}
		::ReleaseDC(this->ddraw->hDraw, hDc);
	}
	free(bmi);

	this->hDc = CreateCompatibleDC(this->ddraw->hDc);
	SelectObject(this->hDc, this->hBmp);
}


ULONG DirectDrawSurface::Release()
{
	if (this->ddraw->attachedSurface == this)
	{
		this->ddraw->RenderStop();
		this->ddraw->attachedSurface = NULL;
	}

	if (this->ddraw->surfaceEntries == this)
		this->ddraw->surfaceEntries = NULL;
	else
	{
		DirectDrawSurface* entry = this->ddraw->surfaceEntries;
		while (entry)
		{
			if (entry->last == this)
			{
				entry->last = this->last;
				break;
			}

			entry = entry->last;
		}
	}

	delete this;
	return 0;
}

HRESULT DirectDrawSurface::GetPixelFormat(LPDDPIXELFORMAT lpDDPixelFormat)
{
	lpDDPixelFormat->dwRGBBitCount = 16;
	lpDDPixelFormat->dwRBitMask = 0xF800;
	lpDDPixelFormat->dwGBitMask = 0x07E0;
	lpDDPixelFormat->dwBBitMask = 0x001F;

	return DD_OK;
}

HRESULT DirectDrawSurface::SetClipper(LPDIRECTDRAWCLIPPER lpDDClipper)
{
	this->attachedClipper = (DirectDrawClipper*)lpDDClipper;
	return DD_OK;
}

HRESULT DirectDrawSurface::GetSurfaceDesc(LPDDSURFACEDESC2 lpDDSurfaceDesc)
{
	lpDDSurfaceDesc->dwWidth = this->width;
	lpDDSurfaceDesc->dwHeight = this->height;

	return DD_OK;
}

HRESULT DirectDrawSurface::GetDC(HDC* dcMem)
{
	*dcMem = this->hDc;
	return DD_OK;
}

HRESULT DirectDrawSurface::Lock(LPRECT lpDestRect, LPDDSURFACEDESC2 lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent)
{
	lpDDSurfaceDesc->dwWidth = this->width;
	lpDDSurfaceDesc->dwHeight = this->height;
	lpDDSurfaceDesc->lPitch = this->width * 2;
	lpDDSurfaceDesc->lpSurface = this->indexBuffer;

	this->ddraw->RenderStart();

	return DD_OK;
}

HRESULT DirectDrawSurface::Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx)
{
	DirectDrawSurface* surface = (DirectDrawSurface*)lpDDSrcSurface;

	DWORD sWidth = surface->width;
	DWORD dWidth;
	if (this->attachedClipper)
	{
		RECT clip = this->attachedClipper->rgnData.rdh.rcBound;

		lpDestRect->left -= clip.left;
		lpDestRect->top -= clip.top;

		dWidth = clip.right - clip.left;
	}
	else
		dWidth = this->width;

	WORD* source = surface->indexBuffer + lpSrcRect->top * sWidth + lpSrcRect->left;
	WORD* destination = this->indexBuffer + lpDestRect->top * dWidth + lpDestRect->left;

	INT width = lpSrcRect->right - lpSrcRect->left;
	INT height = lpSrcRect->bottom - lpSrcRect->top;
	if (width & 1)
	{
		do
		{
			WORD* src = source;
			WORD* dest = destination;
			source += sWidth;
			destination += dWidth;

			DWORD count = width;
			do
				*dest++ = *src++;
			while (--count);
		} while (--height);
	}
	else
	{
		width >>= 1;
		do
		{
			DWORD* src = (DWORD*)source;
			DWORD* dest = (DWORD*)destination;
			source += sWidth;
			destination += dWidth;

			DWORD count = width;
			do
				*dest++ = *src++;
			while (--count);
		} while (--height);
	}

	if (this->ddraw->attachedSurface == this)
		SetEvent(this->ddraw->hDrawEvent);

	return DD_OK;
}

HRESULT DirectDrawSurface::BltFast(DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE7 lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags)
{
	DirectDrawSurface* surface = (DirectDrawSurface*)lpDDSrcSurface;

	DWORD sWidth = surface->width;
	DWORD dWidth = this->width;

	WORD* source = surface->indexBuffer + lpSrcRect->top * sWidth + lpSrcRect->left;
	WORD* destination = this->indexBuffer + dwY * dWidth + dwX;

	INT width = lpSrcRect->right - lpSrcRect->left;
	INT height = lpSrcRect->bottom - lpSrcRect->top;
	if (width & 1)
	{
		do
		{
			WORD* src = source;
			WORD* dest = destination;
			source += sWidth;
			destination += dWidth;

			DWORD count = width;
			do
				*dest++ = *src++;
			while (--count);
		} while (--height);
	}
	else
	{
		width >>= 1;
		do
		{
			DWORD* src = (DWORD*)source;
			DWORD* dest = (DWORD*)destination;
			source += sWidth;
			destination += dWidth;

			DWORD count = width;
			do
				*dest++ = *src++;
			while (--count);
		} while (--height);
	}

	if (this->ddraw->attachedSurface == this)
		SetEvent(this->ddraw->hDrawEvent);

	return DD_OK;
}