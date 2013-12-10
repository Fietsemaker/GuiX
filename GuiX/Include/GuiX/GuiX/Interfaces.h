/// @file
/// Contains the application interface classes.

#pragma once

#include <GuiX/Common.h>
#include <GuiX/String.h>

namespace guix {
namespace core {

/// Handle to files opened by the file interface.
typedef GxHandle GxFileHandle;

/// Handle to textures created by the render interface.
typedef GxHandle GxTextureHandle;

/// Enumeration of color blending modes.
enum GxBlendMode
{
	GX_BM_NONE,     ///< No blending.
	GX_BM_ALPHA,    ///< Alpha blending.
	GX_BM_ADD,      ///< Additive blending.
	GX_BM_MULTIPLY, ///< Multiplicative blending.
	GX_BM_SUBTRACT, ///< Subtractive blending.
	GX_BM_DARKEN,   ///< Darkest value blending.
	GX_BM_LIGHTEN,  ///< Lightest value blending.
};

/// Enumeration of log message types.
enum GxLogType
{
	GX_LT_INFO,    ///< Info message.
	GX_LT_WARNING, ///< Warning message.
	GX_LT_ERROR,   ///< Error message.
};

/// Simple struct for representing 2D vertices.
struct GUIX_API GxVertex
{
	GxVertex()
		:pos(0,0), uvs(0,0), color() {}
	GxVertex(float x, float y, GxColor c)
		:pos(x,y), uvs(0,0), color(c) {}
	GxVertex(float x, float y, float u, float v, GxColor c)
		:pos(x,y), uvs(u,v), color(c) {}

	GxVec2f pos;   ///< Vertex position.
	GxVec2f uvs;   ///< Texture coordinates.
	GxColor color; ///< Vertex color.
};

/// Global logging functions, send a message to the LogMessage function of the system interface.
GUIX_API void GxLog(const char* tag, GxLogType type, const char* fmt, ...);

/// Global logging functions, send a message to the LogMessage function of the system interface.
GUIX_API void GxLog(const char* fmt, ...);

// ===================================================================================
// GxFileInterface
// ===================================================================================
/** The GxFileInterface class is an abstract interface for platform-specific file operations.

 The application can use the \c GxCore::SetFileInterface() function to provide GuiX
 with an implementation of this class. This allows GuiX to perform platform specific
 file operations.
  
 Setting the file interface is optional. If no file interface is set, a default file
 interface is used that uses stdio functions from the standard C library.

 @see GxCore, GxFileInterfaceStd
*/
class GUIX_API GxFileInterface
{
public:
	virtual ~GxFileInterface();

	static int SeekSet(); ///< Returns the value of SEEK_SET from stdio.
	static int SeekEnd(); ///< Returns the value of SEEK_END from stdio.
	static int SeekCur(); ///< Returns the value of SEEK_CUR from stdio.

	/// Returns the file interface singleton.
	static GxFileInterface* Get();

	/// Called by GuiX to open a file.
	///
	/// @param [in] path : The path of the file to open.
	/// @return A valid file handle, or 0 on failure.
	///
	virtual GxFileHandle Open(const char* path) = 0;

	/// Called by GuiX to close a file.
	///
	/// @param [in] file : Handle to a file that was created by \c Open().
	///
	virtual void Close(GxFileHandle file) = 0;

	/// Called by GuiX to read data from a file.
	///
	/// @param [in]  file   : The handle of the file.
	/// @param [out] buffer : The output buffer to store the read data in.
	/// @param [in]  size   : The number of bytes to read.
	/// @return The number of bytes successfully read from the file.
	///
	virtual size_t Read(GxFileHandle file, void* buffer, size_t size) = 0;

	/// Called by GuiX to seek to a position in a file.
	///
	/// @param [in] file   : The handle of the file.
	/// @param [in] offset : The number of bytes offset from the seek position.
	/// @param [in] origin : Can be SeekSet (beginning of the file), SeekEnd (end of the file) or SeekCur (current file position).
	/// @return True if the operation completed successfully, false otherwise.
	///
	virtual bool Seek(GxFileHandle file, long offset, int origin) = 0;

	/// Called by GuiX to get the current file position.
	///
	/// @param [in] file : The handle of the file.
	/// @return The number of bytes offset from the start of the file.
	///
	virtual size_t Tell(GxFileHandle file) = 0;

	/// Called by GuiX to check if the end of a file is reached.
	///
	/// @param [in] file : The handle of the file.
	/// @return True if the end of the file is reached, false otherwise.
	///
	virtual bool EndOfFile(GxFileHandle file) = 0;
};

// ===================================================================================
// GxFileInterfaceStd
// ===================================================================================
/** GxFileInterfaceStd implements the GxFileInterface using the standard C library.

  The GxFileInterfaceStd class is used as the default file interface by GuiX. It uses
  the stdio functions from the standard C library.

 @see GxFileInterface
*/
class GUIX_API GxFileInterfaceStd : public GxFileInterface
{
public:
	/// Opens a file using \c fopen().
	GxFileHandle Open(const char* path);

	/// Closes a file using \c fclose().
	void Close(GxFileHandle file);

	/// Reads data from a file using \c fread().
	size_t Read(GxFileHandle file, void* buffer, size_t size);

	/// Seeks to a position in a file using \c fseek().
	bool Seek(GxFileHandle file, long offset, int origin);

	/// Returns the file position using \c ftell().
	size_t Tell(GxFileHandle file);

	/// Returns the end of the file using \c feof().
	bool EndOfFile(GxFileHandle file);
};

// ===================================================================================
// GxSystemInterface
// ===================================================================================
/** The GxSystemInterface class is an abstract interface for platform-specific system operations.

 The application can use the \c GxCore::SetSystemInterface() function to provide GuiX
 with an implementation of this class. This allows GuiX to perform platform specific
 system operations.

 Setting the system interface is optional. If no system interface is set, a default
 system interface is used with dummy functions. This limits some of the functionality
 of GuiX but should not cause any problems.

 @see GxCore
*/
class GUIX_API GxSystemInterface
{
public:
	virtual ~GxSystemInterface();

	/// Returns the system interface singleton.
	static GxSystemInterface* Get();

	/// Called by GuiX to log a message.
	///
	/// @param [in] type    : Gives information about the type of the message.
	/// @param [in] message : The message to be logged.
	///
	virtual void LogMessage(GxLogType type, const char* message);

	/// Called by GuiX to get text from the clipboard.
	///
	/// @param [out] outText : Output string to write the clipboard text to.
	/// @return True if the operation completed successfully, false otherwise.
	///
	virtual bool GetClipboardText(GxString& outText);

	/// Called by GuiX to send text to the clipboard.
	///
	/// @param [in] text : The text to write to the clipboard.
	///
	virtual void SetClipboardText(const GxString& text);
};

// ===================================================================================
// GxRenderInterface
// ===================================================================================
/** The GxRenderInterface class is an abstract interface for platform-specific render operations.

 The application must use the \c GxCore::SetRenderInterface() function to provide 
 GuiX with an implementation of this class. This allows GuiX to perform 2D rendering
 operations. If no render interface is set, a default render interface is used with
 empty function implementations.

 @see GxCore
*/
class GUIX_API GxRenderInterface
{
public:
	virtual ~GxRenderInterface();

	/// Returns the render interface singleton.
	static GxRenderInterface* Get();

	/// Called by GuiX to get the view size. This affects some of the rendering behaviour;
	/// for example, GxDraw uses the view size as the size of the topmost scissor region.
	///
	/// @return A 2D vector containing the width and height of the view in pixels.
	///
	virtual GxVec2i GetViewSize();

	/// Called by GuiX to render colored and textured triangles.
	///
	/// @param [in] vertices    : An array of vertex data.
	/// @param [in] vertexCount : The number of vertices. There are 3 vertices per triangle.
	/// @param [in] texture     : The texture to be applied to the geometry, or 0 for untextured geometry.
	///
	virtual void DrawTriangles(const GxVertex* vertices, int vertexCount, GxTextureHandle texture);

	/// Called by GuiX to render colored and textured triangles with indexed vertices.
	///
	/// @param [in] vertices    : An array of vertex data.
	/// @param [in] vertexCount : The number of vertices.
	/// @param [in] indices     : An array of index data; used as an indices into the vertex array.
	/// @param [in] indexCount  : The number of indices. There are 3 indices per triangle.
	/// @param [in] texture     : The texture to be applied to the geometry, or 0 for untextured geometry.
	///
	virtual void DrawTriangles(const GxVertex* vertices, int vertexCount, const uint* indices, int indexCount, GxTextureHandle texture);

	/// Called by GuiX to set the color blending mode. None of the blend modes are
	/// required to be implemented, it is entirely up to the application. However,
	/// implementing at least alpha blending is strongly recommended in order to
	/// render text and sprites properly.
	///
	/// @param [in] blendMode : The new blending mode to be set. 
	///
	virtual void SetBlendMode(GxBlendMode blendMode);

	/// Called by GuiX to enable or disable scissor rectangle clipping.
	///
	/// @param [in] enable : true if scissor rectangle clipping is to be enabled, false otherwise.
	///
	virtual void EnableScissorRect(bool enable);

	/// Called by GuiX to set the scissor rectangle.
	///
	/// @param [in] x      : The left-most pixel to be rendered. All pixels left of x should be clipped.
	/// @param [in] y      : The top-most pixel to be rendered. All pixels above y should be clipped.
	/// @param [in] width  : The width of the scissored region. All pixels right of (x + width) should be clipped.
	/// @param [in] height : The height of the scissored region. All pixels below (y + height) should be clipped.
	///
	virtual void SetScissorRect(int x, int y, int width, int height);

	/// Called by GuiX to load a texture from an image file.
	///
	/// @param [out] outTexture : The output handle to write the handle of the loaded texture to.
	/// @param [out] outWidth   : The output value to write the width of the loaded texture to.
	/// @param [out] outHeight  : The output value to write the height of the loaded texture to.
	/// @param [in]  path       : The path of the image file to load.
	/// @return True if loading succeeded and the handle and dimensions are valid, false otherwise.
	///
	virtual bool LoadTexture(GxTextureHandle& outTexture, int& outWidth, int& outHeight, const char* path);

	/// Called by GuiX to generate a texture from a pixel buffer.
	///
	/// @param [out] outTexture : The output handle to write the handle of the loaded texture to.
	/// @param [in]  width      : The width of the texture.
	/// @param [in]  height     : The height of the texture.
	/// @param [in]  pixeldata  : The array of pixels to generate the texture; has 4 values per pixel in RGBA format.
	/// @return True if the load attempt succeeded and the handle is valid, false otherwise.
	///
	virtual bool GenerateTexture(GxTextureHandle& outTexture, int width, int height, const uchar* pixeldata);

	/// Called by GuiX to destroy a texture that is no longer used.
	///
	/// @param [in] texture : A texture handle that was created by \c LoadTexture() or \c GenerateTexture().
	///
	virtual void ReleaseTexture(GxTextureHandle texture);
};

}; // namespace core
}; // namespace guix