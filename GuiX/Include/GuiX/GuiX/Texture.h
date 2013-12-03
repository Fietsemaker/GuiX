/// @file
/// Contains the texture class and texture database.

#pragma once

#include <GuiX/Interfaces.h>

namespace guix {
namespace graphics {

// ===================================================================================
// GxTexture
// ===================================================================================
/** The GxTexture class keeps track of a texture handle.

 The GxTexture class contains a handle to a texture. Empty textures or invalid textures
 have a handle that is zero. Loaded textures are tracked internally by the texture
 database singleton to avoid duplicates. GxTexture objects only store a (reference
 counted) handle to a texture, so it is safe to copy them or pass them around as
 parameters.

 @see GxTextureDatabase, GxDraw
*/
class GUIX_API GxTexture
{
public:
	~GxTexture();

	/// Constructs an empty texture with a zero handle.
	GxTexture();

	/// Loads a texture from either a path to an image file, or the ID of an entry in the texture database.
	GxTexture(const char* path);

	/// Generates a texture through the GxRenderInterface with the specified parameters.
	GxTexture(int width, int height, const uchar* pixeldata);

	/// Copies the data of another texture; the texture handle is implicitly shared.
	GxTexture(const GxTexture& other);
	
	/// Loads a texture from either a path to an image file, or a texture ID in the texture database.
	bool Create(const char* path);

	/// Generates a texture through the GxRenderInterface with the specified parameters.
	bool Create(int width, int height, const uchar* pixeldata);

	/// Releases the current texture.
	void Destroy();

	/// Copies the data of another texture; the texture handle is implicitly shared.
	void Assign(const GxTexture& other);

	/// Sets the UV region of the texture that is used for drawing.
	void SetUVs(float u1, float v1, float u2, float v2);

	/// Sets the UV region of the texture that is used to drawing.
	void SetUVs(const GxAreaf& uvs);

	/// Returns the width of the current UV region in pixels.
	float GetWidthU() const;

	/// Returns the height of the current UV region in pixels.
	float GetHeightV() const;

	GxTextureHandle GetHandle() const; ///< Returns the texture handle.
	int GetWidth() const;              ///< Returns the number of horizontal pixels.
	int GetHeight() const;             ///< Returns the number of vertical pixels. 
	GxVec2i GetSize() const;           ///< Returns the size of the texture in pixels.
	GxAreaf GetUVs() const;            ///< Returns the UV region that is used for drawing.

	GxTexture& operator = (const GxTexture& other); ///< Calls \c Assign().

private:
	void mySetUVs(const GxAreaf* uvs);

	GxTextureHandle myHandle;
	GxAreaf* myUVs;
	GxVec2i mySize;
};

// ===================================================================================
// GxTextureDatabase
// ===================================================================================
/** The GxTextureDatabase class stores and keeps track of loaded textures.

 GxTexture objects us the texture database singleton to load textures. The texture
 database keeps track of which textures are already loaded in order to avoid loading 
 duplicate textures. It also keeps a reference count on every texture; texture handles
 that are no longer used by any GxTexture object are released automatically.

 The GxTextureDatabase uses the GxRenderInterface to load, create and release texture
 objects.

 @see GxRenderInterface, GxTexture
*/
class GUIX_API GxTextureDatabase
{
public:
	virtual ~GxTextureDatabase();

	/// Returns the texture database singleton.
	static GxTextureDatabase* Get();

	/// Returns the number currently loaded textures.
	virtual int GetLoadedTextureCount() const = 0;

	/// GxLogs info about the loaded textures.
	virtual void LogInfo() const = 0;
};

}; // namespace graphics
}; // namespace guix