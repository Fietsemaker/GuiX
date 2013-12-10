#pragma once

namespace guix {
namespace framework {

class GUIX_API GxApplicationWin32
{
public:
	virtual ~GxApplicationWin32();

	/// Returns the application singleton.
	static GxApplicationWin32* Get();

	/// Opens a console window that displays messages written to the log.
	virtual void OpenConsole() = 0;

	/// Opens a dialog window that promps the user to select a folder.
	virtual GxString FolderDialog(GxString title, GxString defaulFolder) = 0;

	/// Opens a dialog window that promps the user to select a file.
	virtual GxString FileDialog(GxString title, GxString defaulFile, GxString filters) = 0;

	/// Opens a dialog window that displays a text message.
	virtual void MessageDialog(GxString title, GxString message) = 0;

	/// Sets the current directory to the executable directory.
	virtual void SetCurrentDirToExe() = 0;
};

}; // namespace framework
}; // namespace guix