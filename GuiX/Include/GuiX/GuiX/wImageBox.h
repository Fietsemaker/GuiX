/// @file
/// Contains a collection of the most commonly used widget classes.

#pragma once

#include <GuiX/Sprites.h>
#include <GuiX/Widget.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxImage
// ===================================================================================
/** The GxImageBox displays a sprite inside a rectangle.

 The GxImageBox is used to display an image inside a frame.

 @see GxWidget
*/
class GUIX_API GxImageBox : public GxWidget
{
public:
	GxDeclareWidgetClass(GxImageBox);

	enum Flags
	{
		F_FRAME = F_WBIT << 0, ///< Set if the frame display is enabled.
	};

	virtual ~GxImageBox();

	/// Creates an image widget without a sprite.
	GxImageBox();

	/// Creates an image widget with the given sprite.
	GxImageBox(const GxSprite& sprite);

	/// Creates an image widget with a sprite created from the given texture.
	GxImageBox(const GxTexture& texture);

	// ===================================================================================
	// Overloaded widget functions

	void Adjust();
	void Draw();

	// ===================================================================================
	// Functions

	// When enabled, displays a frame/border around the image.
	void SetFrame(bool enabled);

	/// Sets the background color of the area behind the image.
	void SetBackgroundColor(GxColor c);

	GxSprite& GetSprite();             ///< Returns the sprite.
	const GxSprite& GetSprite() const; ///< Returns the sprite.

protected:
	void myInit();
	GxSprite mySprite;
	GxColor myBgColor;
};

}; // namespace widgets
}; // namespace guix