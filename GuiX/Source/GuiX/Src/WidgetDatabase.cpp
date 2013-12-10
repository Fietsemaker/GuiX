#include <GuiX/Config.h>

#include <Src/WidgetDatabase.h>

namespace guix {
namespace gui {

// ===================================================================================
// GxWidgetDatabase
// ===================================================================================

GxWidgetDatabase* GxWidgetDatabase::singleton = NULL;

void GxWidgetDatabase::Create()
{
	singleton = new GxWidgetDatabase();
}

void GxWidgetDatabase::Destroy()
{
	delete singleton;
	singleton = NULL;
}

GxWidgetDatabase::GxWidgetDatabase()
{
}

GxWidgetDatabase::~GxWidgetDatabase()
{
}

void GxWidgetDatabase::Remove(GxWidget* w)
{
	myWidgetIdMap.erase(w);
	myGroupIdMap.erase(w);
	myToolTipMap.erase(w);
	myRemoveFromGroupMap(w);
}

void GxWidgetDatabase::SetWidgetId(GxWidget* w, const GxString& id)
{
	myWidgetIdMap.insert(std::make_pair(w, id));
}

void GxWidgetDatabase::SetGroupId(GxWidget* w, const GxString& id)
{
	myRemoveFromGroupMap(w);
	myGroupMap.insert(std::make_pair(id, w));
	myGroupIdMap.insert(std::make_pair(w, id));
}

void GxWidgetDatabase::SetToolTip(GxWidget* w, const GxString& text)
{
	myToolTipMap.insert(std::make_pair(w, text));
}

GxString GxWidgetDatabase::GetWidgetId(const GxWidget* widget) const
{
	return myGetString(myWidgetIdMap, widget);
}

GxString GxWidgetDatabase::GetGroupId(const GxWidget* widget) const
{
	return myGetString(myGroupIdMap, widget);
}

GxString GxWidgetDatabase::GetToolTip(const GxWidget* widget) const
{
	return myGetString(myToolTipMap, widget);
}

void GxWidgetDatabase::SendGroupEvent(GxWidget* sender)
{
	StringMap::const_iterator i = myGroupIdMap.find(sender);
	if(i != myGroupIdMap.end())
	{
		std::pair<GroupMap::iterator, GroupMap::iterator> range;
		range = myGroupMap.equal_range(i->second);

		for(GroupMap::iterator j = range.first; j != range.second; ++j)
			if(j->second != sender)
				j->second->OnGroupEvent(sender);
	}
}

GxString GxWidgetDatabase::myGetString(const StringMap& map, const GxWidget* w) const
{
	StringMap::const_iterator i = map.find(w);
	if(i != map.end()) return i->second;
	return GxString();
}

void GxWidgetDatabase::myRemoveFromGroupMap(GxWidget* w)
{
	GroupMap::const_iterator it = myGroupMap.begin();
	GroupMap::const_iterator end = myGroupMap.end();

	while(it != end && it->second != w) ++it;
	if(it != end) myGroupMap.erase(it);
}

}; // namespace gui
}; // namespace guix