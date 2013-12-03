#pragma once

#include <GuiX/Widget.h>

#include <map>
#include <vector>

namespace guix {
namespace gui {

// ===================================================================================
// GxWidgetDatabase
// ===================================================================================

class GxWidgetDatabase
{
public:
	static GxWidgetDatabase* singleton;

	static void Create();
	static void Destroy();

	GxWidgetDatabase();
	~GxWidgetDatabase();
	
	void Remove(GxWidget* widget);

	void SetWidgetId(GxWidget* widget, const GxString& id);
	void SetGroupId(GxWidget* widget, const GxString& id);
	void SetToolTip(GxWidget* widget, const GxString& text);

	GxString GetWidgetId(const GxWidget* widget) const;
	GxString GetGroupId(const GxWidget* widget) const;
	GxString GetToolTip(const GxWidget* widget) const;

	void SendGroupEvent(GxWidget* sender);

private:
	typedef std::map<const GxWidget*, GxString> StringMap;
	typedef std::multimap<GxString, GxWidget*> GroupMap;

	GxString myGetString(const StringMap& map, const GxWidget* w) const;
	void myRemoveFromGroupMap(GxWidget* w);
	void myDisplayToolTip();

	StringMap myWidgetIdMap;
	StringMap myGroupIdMap;
	StringMap myToolTipMap;
	GroupMap myGroupMap;
};

}; // namespace gui
}; // namespace guix