#pragma once

#include <GuiX/Widget.h>
#include <GuiX/Style.h>

namespace guix {
namespace gui {

// ===================================================================================
// Utility functions used internally by Gui classes

inline GxVec2i VecTr(bool tr, const GxVec2i& v)
{
	return tr ? GxVec2i(v.y, v.x) : v;
}

inline GxVec2i VecTr(bool tr, int x, int y)
{
	return tr ? GxVec2i(y, x) : GxVec2i(x, y);
}

inline GxRecti RectTr(bool tr, const GxRecti& r)
{
	return tr ? GxRecti(r.y, r.x, r.h, r.w) : r;
}

inline GxRecti RectTr(bool tr, int x, int y, int w, int h)
{
	return tr ? GxRecti(y, x, h, w) : GxRecti(x, y, w, h);
}

}; // namespace gui
}; // namespace guix