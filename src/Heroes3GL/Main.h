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

#pragma once

#include "IDraw.h"
#include "OpenDraw.h"

extern IDraw* drawList;

namespace Main
{
	HRESULT __stdcall DirectDrawCreate(GUID*, LPDIRECTDRAW*, IUnknown*);

	OpenDraw* __fastcall FindOpenDrawByWindow(HWND);

	VOID __fastcall ShowError(UINT, CHAR*, DWORD);
	VOID __fastcall ShowError(CHAR*, CHAR*, DWORD);
	VOID __fastcall ShowInfo(UINT);
	VOID __fastcall ShowInfo(CHAR*);

#ifdef _DEBUG
	VOID __fastcall CheckError(CHAR*, DWORD);
#endif
}