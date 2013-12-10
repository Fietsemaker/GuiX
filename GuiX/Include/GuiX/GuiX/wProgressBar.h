/// @file
/// Contains the slider widget classes.

#pragma once

#include <GuiX/Widget.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxProgressBar
// ===================================================================================
/** The GxProgressBar widget is non-interactive widget that indicates progress.

 The GxProgressBar widget is used to display a horizontal progress bar that can show
 both definite and indefinite progress. When showing definite progress, the bar fills
 up from left to right based on the given progress value. When showing indefinite
 progress, the bar shows a generic animation that indicates something is happening.

 @see GxWidget
*/
class GUIX_API GxProgressBar : public GxWidget
{
public:
	GxDeclareWidgetClass(GxProgressBar);

	virtual ~GxProgressBar();
	GxProgressBar();

	// ===================================================================================
	// Overloaded widget functions

	void Tick(float dt);
	void Draw();

	// ===================================================================================
	// Functions

	/// Sets the progress to an indefinite state, which will display a generic animation
	/// that indicates something is happening.
	void SetProgressIndefinite();

	/// Sets the progress to the given percentage. This will display a bar filling from
	/// left to right, with overlay text "[percentage]%".
	void SetProgress(double percentage);

	/// Sets the progress to the given value. This will display a bar filling from left
	/// to right, with overlay text "[value]/[maximum]".
	void SetProgress(double value, double maximum);

	/// Returns the current progress value. This is either zero for an indefinite state,
	/// percentage for a percentage-based state, or value for a value-based state.
	double GetProgress() const;

protected:
	double myValue, myMaximum;
};

}; // namespace widgets
}; // namespace guix