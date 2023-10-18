#ifdef _MSC_VER
	#pragma once
#endif // _MSC_VER
#ifndef ___win32api_h___
#define ___win32api_h___

#pragma region excludeDefinitions
#ifndef _DEBUG
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
	#endif // !WIN32_LEAN_AND_MEAN

	#ifndef VC_EXTRALEAN
		#define VC_EXTRALEAN                    // Exclude even more stuff from Windows headers
	#endif // !VC_EXTRALEAN

	#ifndef NOMINMAX
		#define NOMINMAX                        // Exclude min and max macros from Windows headers
	#endif // !NOMINMAX

	// Exclude specific Windows headers

	#ifndef NOGDICAPMASKS
		#define NOGDICAPMASKS                   // Exclude GDI device capabilities
	#endif // !NOGDICAPMASKS

	#ifndef NOMENUS
		#define NOMENUS                         // Exclude menu-related APIs
	#endif // !NOMENUS

	#ifndef NOSERVICE
		#define NOSERVICE                       // Exclude service-related APIs
	#endif // !NOSERVICE

	#ifndef NOSOUND
		#define NOSOUND                         // Exclude sound-related APIs
	#endif // !NOSOUND

	#ifndef NOCOMM
		#define NOCOMM                          // Exclude communications APIs
	#endif // !NOCOMM

	#ifndef NOKANJI
		#define NOKANJI                         // Exclude Kanji-related APIs
	#endif // !NOKANJI

	#ifndef NOHELP
		#define NOHELP                          // Exclude Help-related APIs
	#endif // !NOHELP

	#ifndef NOSYSCOMMANDS
		#define NOSYSCOMMANDS                   // Exclude system command APIs
	#endif // !NOSYSCOMMANDS

	#ifndef NOMDI
		#define NOMDI                           // Exclude multiple document interface APIs
	#endif // !NOMDI

	#ifndef NOCLIPBOARD
		#define NOCLIPBOARD                     // Exclude clipboard APIs
	#endif // !NOCLIPBOARD

	#ifndef NOIME
		#define NOIME                           // Exclude input method editor APIs
	#endif // !NOIME

	#ifndef NOBITMAP
		#define NOBITMAP                        // Exclude bitmap APIs
	#endif // !NOBITMAP

	#ifndef NOWINDOWSX
		#define NOWINDOWSX                      // Exclude windows APIs
	#endif // !NOWINDOWSX

	#ifndef NOCOLOR
		#define NOCOLOR                         // Exclude color APIs
	#endif // !NOCOLOR

	#ifndef NOKERNEL
		#define NOKERNEL                        // Exclude kernel APIs
	#endif // !NOKERNEL

	#ifndef NOATOM
		#define NOATOM                          // Exclude atom APIs
	#endif // !NOATOM

	#ifndef NOSHOWWINDOW
		#define NOSHOWWINDOW                    // Exclude ShowWindow API
	#endif // !NOSHOWWINDOW

	#ifndef NOCRYPT
		#define NOCRYPT                         // Exclude cryptography APIs
	#endif // !NOCRYPT

	#ifndef NOIMAGE
		#define NOIMAGE                         // Exclude image APIs
	#endif // !NOIMAGE

	#ifndef NOSYSMETRICS
		#define NOSYSMETRICS                    // Exclude system metrics APIs
	#endif // !NOSYSMETRICS

	#ifndef NOICONS
		#define NOICONS                         // Exclude icons APIs
	#endif // !NOICONS

	#ifndef NORASTEROPS
		#define NORASTEROPS                     // Exclude raster operations APIs
	#endif // !NORASTEROPS

	#ifndef OEMRESOURCE
		#define OEMRESOURCE                     // Exclude OEM resources
	#endif // !OEMRESOURCE

	#ifndef NONLS
		#define NONLS                           // Exclude national language support APIs and the 'MultiByteToWideChar' function
	#endif // !NONLS

	#ifndef NOMEMMGR
		#define NOMEMMGR                        // Exclude memory management APIs
	#endif // !NOMEMMGR

	#ifndef NOTAPE
		#define NOTAPE                          // Exclude tape APIs
	#endif // !NOTAPE

	#ifndef NOPROXYSTUB
		#define NOPROXYSTUB                     // Exclude proxy stub APIs
	#endif // !NOPROXYSTUB

	#ifndef NORPC
		#define NORPC                           // Exclude RPC APIs
	#endif // !NORPC

	#ifndef NOMCX
		#define NOMCX                           // Exclude MAPI support
	#endif // !NOMCX

	#ifndef NODEFERWINDOWPOS
		#define NODEFERWINDOWPOS                // Exclude DeferWindowPos API
	#endif // !NODEFERWINDOWPOS

	#ifndef NOPROFILER
		#define NOPROFILER                      // Exclude Profiler APIs
	#endif // !NOPROFILER

	#ifndef NOWH
		#define NOWH                            // Exclude WinHelp API
	#endif // !NOWH

	#ifndef NOSCROLL
		#define NOSCROLL                        // Exclude scroll bar APIs
	#endif // !NOSCROLL

	#ifndef NOOPENFILE
		#define NOOPENFILE                      // Exclude OpenFile API
	#endif // !NOOPENFILE

	#ifndef NOMETAFILE
		#define NOMETAFILE                      // Exclude metafile APIs
	#endif // !NOMETAFILE

	#ifndef STRICT
		#define STRICT                          // Use strict type checking
	#endif // !STRICT
#endif // !_DEBUG
#pragma endregion // excludeDefinitions

// Windows Header Files
#include<Windows.h>                           // Windows API header

#endif // !___win32api_h___
