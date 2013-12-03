#pragma once

#include <map>
#include <vector>

#include <GuiX/Context.h>

namespace guix {
namespace gui {

// ===================================================================================
// GxContextImp
// ===================================================================================

class GxContextImp : public GxContext, public GxInputListener
{
public:
	GxContextImp();
	~GxContextImp();
	
	void SetView(int x, int y, int width, int height);

	void SetCursor(GxCursorImage c);

	void SetRoot(GxWidget* root);

	void PushLayer(GxWidget* root);
	void PushLayer(GxWidget* root, GxRecti viewRegion);
	void PopLayer();

	void EnableInput(bool enabled);

	// Tick and drawing
	void Tick(float dt);
	void Draw();
	void HandleEvents();

	// Input events
	void OnKeyPress(GxKeyEvent& evt);
	void OnKeyRelease(GxKeyEvent& evt);
	void OnMousePress(GxMouseEvent& evt);
	void OnMouseRelease(GxMouseEvent& evt);
	void OnMouseScroll(GxScrollEvent& evt);
	void OnTextInput(GxTextEvent& evt);
	void OnWindowInactive();

	// Widget management
	GxFrame* GetRoot();

	GxWidget* GetHoverWidget();
	GxWidget* GetFocusWidget();
	GxWidget* GetInputWidget();

	float GetWidgetHighlight(const GxWidget* w);

	GxRecti GetView();

	GxCursorImage GetCursor();

	GxVec2i GetPreferredSize();
	GxVec2i GetMinimumSize();
	bool MouseOccupied();

	// Private functions used by widgets
	void Remove(GxWidget* widget);

	void ReceiveEvent(GxWidget* widget, GxWidgetEvent& evt);

	void GrabFocus(GxWidget* widget);
	void ReleaseFocus(GxWidget* widget);

	void GrabInput(GxWidget* widget);
	void ReleaseInput(GxWidget* widget);

private:
	struct CallbackEvent
	{
		GxCallback* callback;
		GxWidgetEvent evt;
	};
	struct Layer
	{
		GxWidget* root;
		GxRecti region;
		float fade;
	};
	struct LayerAction
	{
		GxWidget* root;
		GxRecti region;
	};

	typedef std::vector<CallbackEvent> EventVec;
	typedef std::vector<LayerAction> LayerActionVec;
	typedef std::vector<Layer> LayerVec;

	void myDestroyLayers();
	void myUpdateWidgetHighlights(float dt);
	void myDisplayToolTip();

	EventVec myReadEvents;
	EventVec myWriteEvents;
	LayerActionVec myLayerActions;
	LayerVec myLayers;

	GxRecti myView;
	GxWidget* myHoverWidget;
	GxWidget* myFocusWidget;
	GxWidget* myInputWidget;

	GxWidget* myHlWidgets[3];
	float myHlValues[3];
	float myToolTipTimer;
	float myToolTipDelay;
	GxVec2i myLastMousePos;

	GxCursorImage myCursor;
	bool myInputEnabled;
};

// ===================================================================================
// GxContextNode
// ===================================================================================

class GxContextNode
{
public:
	~GxContextNode();

	GxContextNode(GxWidget* owner);

	void Clear();

	void SetParent(GxWidget* parent);
	void SetCallback(GxCallback* callback);
	void SetContext(GxContextImp* context);
	void RemoveChild(GxContextNode* child);

	void OnKeyPress(GxKeyEvent& evt);
	void OnKeyRelease(GxKeyEvent& evt);
	void OnMousePress(GxMouseEvent& evt);
	void OnMouseRelease(GxMouseEvent& evt);
	void OnMouseScroll(GxScrollEvent& evt);

	GxWidget* owner;
	GxWidget* parent;
	GxCallback* callback;
	GxContextImp* context;
	GxContextNode** children;
	int childCount;
};

}; // namespace gui
}; // namespace guix