#include <GuiX/Config.h>

#include <GuiX/Container.h>
#include <GuiX/ListLayout.h>

namespace guix {
namespace gui {

// ===================================================================================
// GxContainer
// ===================================================================================

GxContainer::~GxContainer()
{
	delete myLayout;
}

GxContainer::GxContainer(GxLayout* layout)
	:myLayout(layout)
{
	myLayout->SetOwner(this);
}

void GxContainer::SetLayout(GxLayout* layout)
{
	if(layout)
	{
		delete myLayout;
		myLayout = layout;
		myLayout->SetOwner(this);
	}
}

void GxContainer::Add(GxWidget* widget)
{
	myLayout->Add(widget);
}

GxLayout& GxContainer::GetLayout()
{
	return *myLayout;
}

const GxLayout& GxContainer::GetLayout() const
{
	return *myLayout;
}

// ===================================================================================
// GxTab
// ===================================================================================

GxTab::~GxTab()
{
	delete myLayout;
}

GxTab::GxTab(GxTabContainer* owner, GxLayout* layout)
	:myOwner(owner)
	,myLayout(layout)
{
	myLayout->SetOwner(owner);
}

void GxTab::SetLayout(GxLayout* layout)
{
	if(layout)
	{
		delete myLayout;
		myLayout = layout;
		myLayout->SetOwner(myOwner);
	}
}

void GxTab::Add(GxWidget* widget)
{
	myLayout->Add(widget);
}

void GxTab::SetText(const GxString& text)
{
	myText = text;
}

void GxTab::Select()
{
	myOwner->SelectTab(this);
}

const GxString& GxTab::GetText() const
{
	return myText;
}

bool GxTab::IsSelected() const
{
	return myOwner->GetSelectedTab() == this;
}

GxLayout& GxTab::GetLayout()
{
	return *myLayout;
}

const GxLayout& GxTab::GetLayout() const
{
	return *myLayout;
}

// ===================================================================================
// GxTabContainer
// ===================================================================================

GxTabContainer::GxTabContainer()
	:mySelectedTab(0)
{
}

GxTabContainer::~GxTabContainer()
{
	ClearTabs();
}

void GxTabContainer::ClearTabs()
{
	for(int i=0; i<myTabs.Size(); ++i)
		delete myTabs[i];

	myTabs.Clear();
	mySelectedTab = NULL;
}

GxTab* GxTabContainer::AddTab()
{
	GxTab* tab = new GxTab(this, new GxListLayout);

	myTabs.Append(tab);
	if(!mySelectedTab)
		mySelectedTab = tab;

	return tab;
}

void GxTabContainer::RemoveTab(int index)
{
	if(index >= 0 && index < myTabs.Size())
	{
		if(mySelectedTab == myTabs[index])
			mySelectedTab = NULL;

		myTabs.Erase(index);
	}

	if(mySelectedTab == NULL && !myTabs.Empty())
		mySelectedTab = myTabs[0];
}

void GxTabContainer::SelectTab(int index)
{
	if(index >= 0 && index < myTabs.Size())
		mySelectedTab = myTabs[index];
}

void GxTabContainer::RemoveTab(GxTab* tab)
{
	for(int i=0; i<myTabs.Size(); ++i)
	{
		if(myTabs[i] == tab)
		{
			RemoveTab(i);
			return;
		}
	}
}

void GxTabContainer::SelectTab(GxTab* tab)
{
	for(int i=0; i<myTabs.Size(); ++i)
	{
		if(myTabs[i] == tab)
		{
			SelectTab(i);
			return;
		}
	}
}

GxTab* GxTabContainer::GetSelectedTab() const
{
	return mySelectedTab;
}

GxTab* GxTabContainer::GetTab(int index)
{
	if(index >= 0 && index < myTabs.Size())
		return myTabs[index];

	return NULL;
}

const GxTab* GxTabContainer::GetTab(int index) const
{
	if(index >= 0 && index < myTabs.Size())
		return myTabs[index];

	return NULL;
}

}; // namespace gui
}; // namespace guix