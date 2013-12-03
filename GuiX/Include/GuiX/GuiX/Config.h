/// @file
/// Defines the GuiX API and basic types; it is implicitly included in every GuiX header.

#pragma once

#include <stdlib.h>

// Debugging with memory leak detection on windows.
#define _CRTDBG_MAP_ALLOC
#ifdef _CRTDBG_MAP_ALLOC
	#include <crtdbg.h>
#endif

/// The guix namespace is the root namespace of all GuiX classes and functions.
namespace guix {

#ifdef GUIX_EXPORT
	#define GUIX_API __declspec(dllexport)
#else
	#define GUIX_API
#endif

// Unsigned integer types.
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

// Handle type.
typedef uintptr_t GxHandle;

// Unicode character types.
typedef unsigned short GxUTF16;
typedef unsigned int GxUTF32;

/// The core namespace contains the core functionality of GuiX.
namespace core {};

/// The graphics namespace contains classes related to 2D graphics.
namespace graphics {};

/// The gui namespace contains general classes related to the gui.
namespace gui {};

/// The layouts namespace contains a variety of layout classes.
namespace layouts {};

/// The widgets namespace contains a variety of widget classes.
namespace widgets {};

using namespace core;
using namespace graphics;
using namespace layouts;
using namespace widgets;
using namespace gui;

}; // namespace guix