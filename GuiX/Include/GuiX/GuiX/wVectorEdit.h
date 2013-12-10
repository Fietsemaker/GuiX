/// @file
/// Contains a collection of the most commonly used widget classes.

#pragma once

#include <GuiX/wExpandingButton.h>

namespace guix {
namespace widgets {

class GxSpinner;

// ===================================================================================
// GxVectorEdit
// ===================================================================================
/** The GxVectorEdit widget displays collapsable spinners to edit a vector.

 @see GxExpandingButton
*/
class GUIX_API GxVectorEdit : public GxExpandingButton
{
public:
	virtual ~GxVectorEdit();

	/// Creates a vector edit with the given number of components.
	GxVectorEdit(int numComponents);

	void Draw();

	/// Limits the range of the spinners to the given range.
	void SetRange(double min, double max);

protected:
	GxSpinner** mySpinners;
	int myComponentCount;
};

// ===================================================================================
// GxVectorEdit2D
// ===================================================================================
/** The GxVectorEdit2D widget displays collapsable spinners to edit a 2D vector.

 This class is an implementation of a GxVectorEdit with 2 components.

 @see GxVectorEdit
*/
class GUIX_API GxVectorEdit2D : public GxVectorEdit
{
public:
	GxDeclareWidgetClass(GxVectorEdit2D);

	/// Creates a 2D vector edit set to (0, 0).
	GxVectorEdit2D();

	void SetValue(GxVec2d value);  ///< Sets the current vector.
	GxVec2d GetValue() const;      ///< Returns the current vector.
};

// ===================================================================================
// GxVectorEdit3D
// ===================================================================================
/** The GxVectorEdit3D widget displays collapsable spinners to edit a 3D vector.

 This class is an implementation of a GxVectorEdit with 3 components.

 @see GxVectorEdit
*/
class GUIX_API GxVectorEdit3D : public GxVectorEdit
{
public:
	GxDeclareWidgetClass(GxVectorEdit3D);

	/// Creates a 3D vector edit set to (0, 0, 0).
	GxVectorEdit3D();

	void SetValue(GxVec3d value);  ///< Sets the current vector.
	GxVec3d GetValue() const;      ///< Returns the current vector.
};

// ===================================================================================
// GxVectorEdit4D
// ===================================================================================
/** The GxVectorEdit4D widget displays collapsable spinners to edit a 4D vector.

 This class is an implementation of a GxVectorEdit with 4 components.

 @see GxVectorEdit
*/
class GUIX_API GxVectorEdit4D : public GxVectorEdit
{
public:
	GxDeclareWidgetClass(GxVectorEdit4D);

	/// Creates a 4D vector edit set to (0, 0, 0, 0).
	GxVectorEdit4D();

	void SetValue(GxVec4d value);  ///< Sets the current vector.
	GxVec4d GetValue() const;      ///< Returns the current vector.
};

}; // namespace widgets
}; // namespace guix