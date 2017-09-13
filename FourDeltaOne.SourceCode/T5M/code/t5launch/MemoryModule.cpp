
/*
 * Memory DLL loading code
 * Version 0.0.2
 *
 * Copyright (c) 2004-2011 by Joachim Bauch / mail@joachim-bauch.de
 * http://www.joachim-bauch.de
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is MemoryModule.c
 *
 * The Initial Developer of the Original Code is Joachim Bauch.
 *
 * Portions created by Joachim Bauch are Copyright (C) 2004-2011
 * Joachim Bauch. All Rights Reserved.
 *
 * JohnD: standard memmap loader or reflective injection
 * Does not support forwarded functions
 * Made some small adaptions:
 * - destroy import names after they have been used
 * - destroy PE header
 * - leave all sections rwx, so rdata section is also writeble.
 */

//#define DEBUG_OUTPUT

#include "MemoryModule.h"
#include <stdio.h>

#ifndef __GNUC__
// disable warnings about pointer <-> DWORD conversions
#pragma warning( disable : 4311 4312 )
#endif

#ifdef _WIN64
#define POINTER_TYPE ULONGLONG
#else
#define POINTER_TYPE DWORD
#endif

#define WIN32_LEAN_AND_MEAN
#undef UNICODE
#include <Windows.h>
#include <winnt.h>
#include <stdlib.h>
#ifdef DEBUG_OUTPUT
#include <stdio.h>
#endif

#ifndef IMAGE_SIZEOF_BASE_RELOCATION
// Vista SDKs no longer define IMAGE_SIZEOF_BASE_RELOCATION!?
#define IMAGE_SIZEOF_BASE_RELOCATION (sizeof(IMAGE_BASE_RELOCATION))
#endif

typedef struct {
	PIMAGE_NT_HEADERS headers;
	unsigned char *codeBase;
	HMODULE *modules;
	int numModules;
	int initialized;
} MEMORYMODULE, *PMEMORYMODULE;

typedef BOOL (WINAPI *DllEntryProc)(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);

#define GET_HEADER_DICTIONARY(module, idx)	&(module)->headers->OptionalHeader.DataDirectory[idx]

#ifdef DEBUG_OUTPUT
static void
OutputLastError(const char *msg)
{
	LPVOID tmp;
	char *tmpmsg;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&tmp, 0, NULL);
	tmpmsg = (char *)LocalAlloc(LPTR, strlen(msg) + strlen(tmp) + 3);
	sprintf(tmpmsg, "%s: %s", msg, tmp);
	OutputDebugString(tmpmsg);
	LocalFree(tmpmsg);
	LocalFree(tmp);
}
#endif

void PErandomstr(PCHAR name)
{
	while (*name!=0)  *name++=(char)(rand()%0x7e+1);
}

void PEmemrandom(PBYTE src, int siz, int maxno)
{
  int no;
  memset(src,0,siz);
  for (no=0; no<maxno; no++) *(char *)(src+(rand()%siz))=(BYTE)((rand()%250)+1);
}



static void
CopySections(const unsigned char *data, PIMAGE_NT_HEADERS old_headers, PMEMORYMODULE module)
{
	int i, size;
	unsigned char *codeBase = module->codeBase;
	unsigned char *dest;
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);
	for (i=0; i<module->headers->FileHeader.NumberOfSections; i++, section++) {
		if (section->SizeOfRawData == 0) {
			// section doesn't contain data in the dll itself, but may define
			// uninitialized data
			size = old_headers->OptionalHeader.SectionAlignment;
			if (size > 0) {
				/*dest = (unsigned char *)VirtualAlloc(codeBase + section->VirtualAddress,
					size,
					MEM_COMMIT,
					PAGE_EXECUTE_READWRITE);*/

				dest = codeBase + section->VirtualAddress;

				if ((DWORD)dest >= 0x06C16000)
				{
					continue;
				}

				section->Misc.PhysicalAddress = (POINTER_TYPE)dest;
				memset(dest, 0, size);
			}

			// section is empty
			continue;
		}

		// commit memory block and copy data from dll
		dest = codeBase + section->VirtualAddress;

		if ((DWORD)dest >= 0x04FC4000)
		{
			continue;
		}

		memcpy(dest, data + section->PointerToRawData, section->SizeOfRawData);
		section->Misc.PhysicalAddress = (POINTER_TYPE)dest;
	}
}

// Protection flags for memory pages (Executable, Readable, Writeable)
static int ProtectionFlags[2][2][2] = {
	{
		// not executable
		{PAGE_NOACCESS, PAGE_WRITECOPY},
		{PAGE_READONLY, PAGE_READWRITE},
	}, {
		// executable
		{PAGE_EXECUTE, PAGE_EXECUTE_WRITECOPY},
		{PAGE_EXECUTE_READ, PAGE_EXECUTE_READWRITE},
	},
};

static void
FinalizeSections(PMEMORYMODULE module)
{
	int i;
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(module->headers);
#ifdef _WIN64
	POINTER_TYPE imageOffset = (module->headers->OptionalHeader.ImageBase & 0xffffffff00000000);
#else
	#define imageOffset 0
#endif
	
	// loop through all sections and change access flags
	for (i=0; i<module->headers->FileHeader.NumberOfSections; i++, section++) {
		DWORD protect, oldProtect, size;
		int executable = (section->Characteristics & IMAGE_SCN_MEM_EXECUTE) != 0;
		int readable =   (section->Characteristics & IMAGE_SCN_MEM_READ) != 0;
		int writeable =  (section->Characteristics & IMAGE_SCN_MEM_WRITE) != 0;

		if (section->Characteristics & IMAGE_SCN_MEM_DISCARDABLE) {
			// section is not needed any more and can safely be freed
			VirtualFree((LPVOID)((POINTER_TYPE)section->Misc.PhysicalAddress | imageOffset), section->SizeOfRawData, MEM_DECOMMIT);
			continue;
		}
		// force it rwx
		readable=executable=writeable=1;
		// determine protection flags based on characteristics
		protect = ProtectionFlags[executable][readable][writeable];
		if (section->Characteristics & IMAGE_SCN_MEM_NOT_CACHED) {
			protect |= PAGE_NOCACHE;
		}

		// determine size of region
		size = section->SizeOfRawData;
		if (size == 0) {
			if (section->Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) {
				size = module->headers->OptionalHeader.SizeOfInitializedData;
			} else if (section->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) {
				size = module->headers->OptionalHeader.SizeOfUninitializedData;
			}
		}

		if (size > 0) {
			// change memory access flags
			if (VirtualProtect((LPVOID)((POINTER_TYPE)section->Misc.PhysicalAddress | imageOffset), size, protect, &oldProtect) == 0)
#ifdef DEBUG_OUTPUT
				OutputLastError("Error protecting memory page")
#endif
			;
		}
	}
#ifndef _WIN64
#undef imageOffset
#endif
}

static void
PerformBaseRelocation(PMEMORYMODULE module, SIZE_T delta)
{
	DWORD i;
	unsigned char *codeBase = module->codeBase;

	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_BASERELOC);
	if (directory->Size > 0) {
		PIMAGE_BASE_RELOCATION relocation = (PIMAGE_BASE_RELOCATION) (codeBase + directory->VirtualAddress);
		for (; relocation->VirtualAddress > 0; ) {
			unsigned char *dest = codeBase + relocation->VirtualAddress;
			unsigned short *relInfo = (unsigned short *)((unsigned char *)relocation + IMAGE_SIZEOF_BASE_RELOCATION);
			for (i=0; i<((relocation->SizeOfBlock-IMAGE_SIZEOF_BASE_RELOCATION) / 2); i++, relInfo++) {
				DWORD *patchAddrHL;
#ifdef _WIN64
				ULONGLONG *patchAddr64;
#endif
				int type, offset;

				// the upper 4 bits define the type of relocation
				type = *relInfo >> 12;
				// the lower 12 bits define the offset
				offset = *relInfo & 0xfff;
				
				switch (type)
				{
				case IMAGE_REL_BASED_ABSOLUTE:
					// skip relocation
					break;

				case IMAGE_REL_BASED_HIGHLOW:
					// change complete 32 bit address
					patchAddrHL = (DWORD *) (dest + offset);
					*patchAddrHL += delta;
					break;
				
#ifdef _WIN64
				case IMAGE_REL_BASED_DIR64:
					patchAddr64 = (ULONGLONG *) (dest + offset);
					*patchAddr64 += delta;
					break;
#endif

				default:
					//printf("Unknown relocation: %d\n", type);
					break;
				}
				*relInfo= (unsigned short )(rand() & 0xfff);   // destroy it

			}

			// advance to next relocation block
			relocation = (PIMAGE_BASE_RELOCATION) (((char *) relocation) + relocation->SizeOfBlock);
		}
	}
}

static int
BuildImportTable(PMEMORYMODULE module)
{
	int result=1;
	unsigned char *codeBase = module->codeBase;

	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY(module, IMAGE_DIRECTORY_ENTRY_IMPORT);
	if (directory->Size > 0) {
		PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR) (codeBase + directory->VirtualAddress);
		for (; !IsBadReadPtr(importDesc, sizeof(IMAGE_IMPORT_DESCRIPTOR)) && importDesc->Name; importDesc++) {
			POINTER_TYPE *thunkRef;
			FARPROC *funcRef;
			HMODULE handle = LoadLibraryA((LPCSTR) (codeBase + importDesc->Name));
			if (handle == INVALID_HANDLE_VALUE || handle == nullptr) {
#ifdef DEBUG_OUTPUT
				OutputLastError("Can't load library");
#endif
				MessageBoxA(NULL, (LPCSTR) (codeBase + importDesc->Name), "Couldn't load library", MB_OK | MB_ICONSTOP);


				result = 0;
				break;
			}

			module->modules = (HMODULE *)realloc(module->modules, (module->numModules+1)*(sizeof(HMODULE)));
			if (module->modules == NULL) {
				result = 0;
				break;
			}

			module->modules[module->numModules++] = handle;
			if (importDesc->OriginalFirstThunk) {
				thunkRef = (POINTER_TYPE *) (codeBase + importDesc->OriginalFirstThunk);
				funcRef = (FARPROC *) (codeBase + importDesc->FirstThunk);
			} else {
				// no hint table
				thunkRef = (POINTER_TYPE *) (codeBase + importDesc->FirstThunk);
				funcRef = (FARPROC *) (codeBase + importDesc->FirstThunk);
			}
			for (; *thunkRef; thunkRef++, funcRef++) {
				if (IMAGE_SNAP_BY_ORDINAL(*thunkRef)) {
					*funcRef = (FARPROC)GetProcAddress(handle, (LPCSTR)IMAGE_ORDINAL(*thunkRef));
				} else {
					PIMAGE_IMPORT_BY_NAME thunkData = (PIMAGE_IMPORT_BY_NAME) (codeBase + (*thunkRef));
					*funcRef = (FARPROC)GetProcAddress(handle, (LPCSTR)&thunkData->Name);
					//PErandomstr((char *)&thunkData->Name);
				}
				if (*funcRef == 0) {
					if (!IMAGE_SNAP_BY_ORDINAL(*thunkRef))
					{
						PIMAGE_IMPORT_BY_NAME thunkData = (PIMAGE_IMPORT_BY_NAME) (codeBase + (*thunkRef));

						char errorString[512];
						sprintf(errorString, "function %s not found\n", (char*)&thunkData->Name);

						OutputDebugStringA(errorString);
					}

					//result = 0;
					//break;
				}
			}
			//PErandomstr((PCHAR)(codeBase + importDesc->Name));		// remove libraryname
			if (!result) {
				break;
			}
		}
	}

	return result;
}

extern bool launchMP;

HMEMORYMODULE MemoryLoadLibrary(const void *data)
{
	PMEMORYMODULE result;
	PIMAGE_DOS_HEADER dos_header;
	PIMAGE_NT_HEADERS old_header;
	unsigned char *code, *headers;
	SIZE_T locationDelta;
	DllEntryProc DllEntry;
	BOOL successfull;

	//DWORD tlsIndex = TlsAlloc();
	//*(DWORD*)0x66D94A8 = tlsIndex;
	//TlsSetValue(tlsIndex, new char[0x28]);

	dos_header = (PIMAGE_DOS_HEADER)data;
	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
#ifdef DEBUG_OUTPUT
		OutputDebugString("Not a valid executable file.\n");
#endif
		return NULL;
	}

	old_header = (PIMAGE_NT_HEADERS)&((const unsigned char *)(data))[dos_header->e_lfanew];
	if (old_header->Signature != IMAGE_NT_SIGNATURE) {
#ifdef DEBUG_OUTPUT
		OutputDebugString("No PE header found.\n");
#endif
		return NULL;
	}

	// reserve memory for image of library
	/*code = (unsigned char *)VirtualAlloc((LPVOID)(old_header->OptionalHeader.ImageBase),
		old_header->OptionalHeader.SizeOfImage,
		MEM_RESERVE,
		PAGE_EXECUTE_READWRITE);

    if (code == NULL) {
        // try to allocate memory at arbitrary position
        code = (unsigned char *)VirtualAlloc(NULL,
            old_header->OptionalHeader.SizeOfImage,
            MEM_RESERVE,
            PAGE_EXECUTE_READWRITE);
		if (code == NULL) {
#ifdef DEBUG_OUTPUT
			OutputDebugString("Can't reserve memory");
#endif
			return NULL;
		}
	}
    
	result = (PMEMORYMODULE)HeapAlloc(GetProcessHeap(), 0, sizeof(MEMORYMODULE));
	result->codeBase = code;
	result->numModules = 0;
	result->modules = NULL;
	result->initialized = 0;

	// XXX: is it correct to commit the complete memory region at once?
    //      calling DllEntry raises an exception if we don't...
	VirtualAlloc(code,
		old_header->OptionalHeader.SizeOfImage,
		MEM_COMMIT,
		PAGE_EXECUTE_READWRITE);

	// commit memory for headers
	headers = (unsigned char *)VirtualAlloc(code,
		old_header->OptionalHeader.SizeOfHeaders,
		MEM_COMMIT,
		PAGE_EXECUTE_READWRITE);*/

	code = (unsigned char*)0x400000;
	headers = (unsigned char *)malloc(dos_header->e_lfanew + old_header->OptionalHeader.SizeOfHeaders);

	result = (PMEMORYMODULE)HeapAlloc(GetProcessHeap(), 0, sizeof(MEMORYMODULE));
	result->codeBase = code;
	result->numModules = 0;
	result->modules = NULL;
	result->initialized = 0;
	
	// copy PE header to code
	memcpy(headers, dos_header, dos_header->e_lfanew + old_header->OptionalHeader.SizeOfHeaders);
	result->headers = (PIMAGE_NT_HEADERS)&((const unsigned char *)(headers))[dos_header->e_lfanew];

	// update position
	result->headers->OptionalHeader.ImageBase = (POINTER_TYPE)code;

	// copy sections from DLL file block to new memory location
	CopySections((unsigned char*)data, old_header, result);

	// adjust base address of imported data
	/*locationDelta = (SIZE_T)(code - old_header->OptionalHeader.ImageBase);
	if (locationDelta != 0) {
		PerformBaseRelocation(result, locationDelta);
	}*/

	// load required dlls and adjust function table of imports
	if (!BuildImportTable(result)) {
		//goto error;
	}

	// mark memory pages depending on section headers and release
	// sections that are marked as "discardable"
	//FinalizeSections(result);

	// get entry point of loaded library
	/*if (result->headers->OptionalHeader.AddressOfEntryPoint != 0) {
		DllEntry = (DllEntryProc) (code + result->headers->OptionalHeader.AddressOfEntryPoint);
		if (DllEntry == 0) {
#ifdef DEBUG_OUTPUT
			OutputDebugString("Library has no entry point.\n");
#endif
			goto error;
		}
		// destroy PEheader
	    //PEmemrandom(code,result->headers->OptionalHeader.SizeOfHeaders,80);

		// notify library about attaching to process
		successfull = (*DllEntry)((HINSTANCE)code, DLL_PROCESS_ATTACH, 0);
		if (!successfull) {
#ifdef DEBUG_OUTPUT
			OutputDebugString("Can't attach library.\n");
#endif
			goto error;
		}
		result->initialized = 1;
	}*/

	if (launchMP)
	{
		// initialize TLS
		*(BYTE*)0x9D377A = 0xCC;

	// always use EncodePointer/similar
	/**(BYTE*)0x6BE653 = 0xB8;
	*(DWORD*)0x6BE654 = 1;
	*(BYTE*)0x6BE658 = 0xC3;*/

		*(WORD*)0x9D2AC2 = 0x9090;
	}
	else
	{
		*(BYTE*)0x96DA6B = 0xCC;

		*(WORD*)0x96CEA6 = 0x9090;
	}

	((void(*)())(code + result->headers->OptionalHeader.AddressOfEntryPoint))();

	return (HMEMORYMODULE)result;

error:
	// cleanup
	MemoryFreeLibrary(result);
	return NULL;
}

FARPROC MemoryGetProcAddress(HMEMORYMODULE module, const char *name)
{
	unsigned char *codeBase = ((PMEMORYMODULE)module)->codeBase;
	int idx=-1;
	DWORD i, *nameRef;
	WORD *ordinal;
	PIMAGE_EXPORT_DIRECTORY exports;
	PIMAGE_DATA_DIRECTORY directory = GET_HEADER_DICTIONARY((PMEMORYMODULE)module, IMAGE_DIRECTORY_ENTRY_EXPORT);
	if (directory->Size == 0) {
		// no export table found
		return NULL;
	}

	exports = (PIMAGE_EXPORT_DIRECTORY) (codeBase + directory->VirtualAddress);
	if (exports->NumberOfNames == 0 || exports->NumberOfFunctions == 0) {
		// DLL doesn't export anything
		return NULL;
	}

	// search function name in list of exported names
	nameRef = (DWORD *) (codeBase + exports->AddressOfNames);
	ordinal = (WORD *) (codeBase + exports->AddressOfNameOrdinals);
	for (i=0; i<exports->NumberOfNames; i++, nameRef++, ordinal++) {
		if (stricmp(name, (const char *) (codeBase + (*nameRef))) == 0) {
			idx = *ordinal;
			break;
		}
	}

	if (idx == -1) {
		// exported symbol not found
		return NULL;
	}

	if ((DWORD)idx > exports->NumberOfFunctions) {
		// name <-> ordinal number don't match
		return NULL;
	}

	// AddressOfFunctions contains the RVAs to the "real" functions
	return (FARPROC) (codeBase + (*(DWORD *) (codeBase + exports->AddressOfFunctions + (idx*4))));
}

void MemoryFreeLibrary(HMEMORYMODULE mod)
{
	int i;
	PMEMORYMODULE module = (PMEMORYMODULE)mod;

	if (module != NULL) {
		if (module->initialized != 0) {
			// notify library about detaching from process
			DllEntryProc DllEntry = (DllEntryProc) (module->codeBase + module->headers->OptionalHeader.AddressOfEntryPoint);
			(*DllEntry)((HINSTANCE)module->codeBase, DLL_PROCESS_DETACH, 0);
			module->initialized = 0;
		}

		if (module->modules != NULL) {
			// free previously opened libraries
			for (i=0; i<module->numModules; i++) {
				if (module->modules[i] != INVALID_HANDLE_VALUE) {
					FreeLibrary(module->modules[i]);
				}
			}

			free(module->modules);
		}

		if (module->codeBase != NULL) {
			// release memory of library
			VirtualFree(module->codeBase, 0, MEM_RELEASE);
		}

		HeapFree(GetProcessHeap(), 0, module);
	}
}
