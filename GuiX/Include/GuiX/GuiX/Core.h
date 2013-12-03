/// @file
/// Contains the static core class.

#pragma once

#include <GuiX/Interfaces.h>

namespace guix {
namespace core {

// ===================================================================================
// GxCore
// ===================================================================================
/** The GxCore class contains functions for initializing and sutting down GuiX.
 
 The GxCore class is at the center of the GuiX library. It keeps track of the GuiX
 singletons and manages the application interfaces. In order to use the GuiX library,
 the following steps need to be taken:

1. Use the \c SetRenderInterface() functions to provide GuiX with a way to perform
   rendering operations. Setting a file interface or system interface is optional,
   and will depend on the needs of your application.

2. Call \c Initialize(), which initializes the GuiX library. At this point, the GuiX
   library is ready to be used.

3. After the application is finished using the GuiX library, call \c Shutdown().
   This destroys the singletons and resources associated with GuiX.

 If you are using the Win32Framework, these steps are performed by the framework
 automatically.

 @see GxFileInterface, GxSystemInterface, GxRenderInterface
*/
class GUIX_API GxCore
{
public:
	/// Initializes the GuiX library, singletons and resources.
	static void Initialize();

	/// Shuts down the GuiX library and releases the singletons and resources.
	static void Shutdown();

	/// Sets the global file interface that is used by GuiX classes.
	static void SetFileInterface(GxFileInterface* fileInterface = NULL);

	/// Sets the global system interface that is used by GuiX classes.
	static void SetSystemInterface(GxSystemInterface* systemInterface = NULL);

	/// Sets the global render interface that is used by GuiX classes.
	static void SetRenderInterface(GxRenderInterface* renderInterface = NULL);	
};

}; // namespace core
}; // namespace guix