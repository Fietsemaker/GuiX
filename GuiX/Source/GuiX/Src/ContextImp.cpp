#include <GuiX/Config.h>

#include <GuiX/Core.h>
#include <GuiX/Draw.h>
#include <GuiX/Localize.h>
#include <GuiX/Interfaces.h>

#include <GuiX/Style.h>
#include <GuiX/Context.h>

#include <Src/ContextImp.h>

namespace guix {
namespace gui {
	
namespace {

static const char* LOG_TAG = "Gui";

}; // anonymous namespace

#define GX_LAYER_ITER(func) \
	for(LayerVec::reverse_iterator it = myLayers.rbegin(); it != myLayers.rend(); ++it) \
		it->root->GetContextNode()->##func;

class DefaultCallback : public GxCallback
{
public:
	void OnEvent(const GxWidgetEvent& evt)
	{
		const char* id = evt.id.Raw();
		const char* type = evt.type.Raw();
		GxLog(LOG_TAG, GX_LT_INFO, "New event from \"%s\": %s", id, type);
	}
};

template <typename T>
static void GxArrayErase(T* ptr, int size, int i)
{
	int n = size - i - 1;
	T* dst = ptr + i;
	T* src = dst + 1;
	while(n-- > 0) *dst++ = *src++;
}

// ===================================================================================
// GxContext
// ===================================================================================

GxContext::~GxContext()
{
}

GxContext* GxContext::New()
{
	return new GxContextImp;
}

void GxContext::Delete(GxContext* context)
{
	delete context;
}

// ===================================================================================
// GxContextImp
// ===================================================================================

GxContextImp::GxContextImp()
	:myView(0, 0, 640, 480)
	,myHoverWidget(NULL)
	,myFocusWidget(NULL)
	,myInputWidget(NULL)
	,myToolTipTimer(0)
	,myToolTipDelay(0.5f)
	,myCursor(GX_CI_ARROW)
	,myInputEnabled(true)
{
	GxInput* input = GxInput::Get();
	if(input) input->AddListener(this);

	for(int i=0; i<3; ++i)
		myHlWidgets[i] = NULL;

	SetRoot(new GxFrame);
}

GxContextImp::~GxContextImp()
{
	myDestroyLayers();

	GxInput* input = GxInput::Get();
	if(input) input->RemoveListener(this);
}

void GxContextImp::SetView(int x, int y, int width, int height)
{
	myView = GxRecti(x, y, width, height);
}

void GxContextImp::SetCursor(GxCursorImage c)
{
	myCursor = c;
}

void GxContextImp::SetRoot(GxWidget* root)
{
	myDestroyLayers();

	Layer layer = {root, GxRecti(), 0.f};
	root->GetContextNode()->SetContext(this);
	myLayers.push_back(layer);
}

void GxContextImp::PushLayer(GxWidget* root)
{
	LayerAction act = {root, GxRecti()};
	myLayerActions.push_back(act);
}

void GxContextImp::PushLayer(GxWidget* root, GxRecti viewRegion)
{
	LayerAction act = {root, viewRegion};
	myLayerActions.push_back(act);
}

void GxContextImp::PopLayer()
{
	LayerAction act = {NULL, GxRecti()};
	myLayerActions.push_back(act);
}

void GxContextImp::EnableInput(bool enabled)
{
	myInputEnabled = enabled;
}

void GxContextImp::Tick(float dt)
{
	// Reset the mouse cursor type.
	myCursor = GX_CI_ARROW;

	// Update the tooltip timer.
	GxVec2i mpos = GxInput::Get()->GetMousePos();
	if(mpos == myLastMousePos)
		myToolTipTimer += dt;
	else
		myToolTipTimer = 0.f;
	myLastMousePos = mpos;

	// Push and pop layers that have been removed/added.
	for(size_t i=0; i<myLayerActions.size(); ++i)
	{
		LayerAction& act = myLayerActions[i];
		if(act.root)
		{
			// Push a new layer.
			Layer layer = {act.root, act.region, 0.f};
			act.root->GetContextNode()->SetContext(this);
			myLayers.push_back(layer);
		}
		else
		{
			// Pop the top layer.
			delete myLayers.back().root;
			myLayers.pop_back(); 
		}
	}
	myLayerActions.clear();

	// Update all widget layers. 
	for(size_t i=0; i<myLayers.size(); ++i)
	{
		GxWidget* w = myLayers[i].root;

		// Adjust the layout of widgets.
		w->Adjust();

		// Update the rectangles of widgets.
		GxRecti r = myLayers[i].region;
		w->SetRect((r.w > 0 && r.h > 0) ? r : myView);

		// Update the layer alpha.
		float& fade = myLayers[i].fade;
		if(i < myLayers.size() - 1)
			fade = GxMin(fade + dt*2, 0.4f);
		else
			fade = GxMax(fade - dt*2, 0.0f);
	}

	// Find the hover widget.
	GxWidget* hover = NULL;
	if(myInputEnabled && !myFocusWidget)
	{
		GxVec2i m = GxInput::Get()->GetMousePos();
		GxWidget* root = myLayers.back().root;
		hover = root->FindHoverWidget(m.x, m.y);
	}
	myHoverWidget = hover;

	// Update widghet highlight values.
	myUpdateWidgetHighlights(dt);

	// Tick all widgets.
	for(size_t i=0; i<myLayers.size(); ++i)
		myLayers[i].root->Tick(dt);

	// Execute callback events
	for(int tries = 256; tries && !myWriteEvents.empty(); --tries)
	{
		myReadEvents.swap(myWriteEvents);
		myWriteEvents.clear();
		for(size_t i=0; i<myReadEvents.size(); ++i)
		{
			CallbackEvent& entry = myReadEvents[i];
			entry.callback->OnEvent(entry.evt);
		}
	}
	myWriteEvents.clear();
	myReadEvents.clear();
}

void GxContextImp::Draw()
{
	// Draw all widgets
	for(size_t i=0; i<myLayers.size(); ++i)
	{
		myLayers[i].root->Draw();
		if(myLayers[i].fade > 0.01f)
		{
			GxRecti r(myView);
			GxColor c(0.f, 0.f, 0.f, myLayers[i].fade);
			GxDraw::Get()->Rect(r.x, r.y, r.w, r.h, c);
		}
	}

	// Display the tooltip
	myDisplayToolTip();

	// Flush any left over drawing operations
	GxDraw::Get()->Flush();
}

void GxContextImp::OnKeyPress(GxKeyEvent& evt)
{
	if(myInputEnabled)
		GX_LAYER_ITER(OnKeyPress(evt));
}

void GxContextImp::OnKeyRelease(GxKeyEvent& evt)
{
	if(myInputEnabled)
		GX_LAYER_ITER(OnKeyRelease(evt));
}

void GxContextImp::OnMousePress(GxMouseEvent& evt)
{
	if(myInputEnabled)
		GX_LAYER_ITER(OnMousePress(evt));
}

void GxContextImp::OnMouseRelease(GxMouseEvent& evt)
{
	if(myInputEnabled)
		GX_LAYER_ITER(OnMouseRelease(evt));
}

void GxContextImp::OnMouseScroll(GxScrollEvent& evt)
{
	if(myInputEnabled)
		GX_LAYER_ITER(OnMouseScroll(evt));
}

void GxContextImp::OnTextInput(GxTextEvent& evt)
{
	if(myInputEnabled && myInputWidget)
		myInputWidget->OnTextInput(evt);
}

void GxContextImp::OnWindowInactive()
{
	ReleaseFocus(myFocusWidget);
	ReleaseInput(myInputWidget);
}

GxWidget* GxContextImp::GetHoverWidget()
{
	return myHoverWidget;
}

GxWidget* GxContextImp::GetFocusWidget()
{
	return myFocusWidget;
}

GxWidget* GxContextImp::GetInputWidget()
{
	return myInputWidget;
}

float GxContextImp::GetWidgetHighlight(const GxWidget* w)
{
	if(w == myHlWidgets[0])	return myHlValues[0];
	if(w == myHlWidgets[1])	return myHlValues[1];
	if(w == myHlWidgets[2])	return myHlValues[2];

	return 0.f;
}

GxRecti GxContextImp::GetView()
{
	return myView;
}

GxCursorImage GxContextImp::GetCursor()
{
	return myCursor;
}

GxVec2i GxContextImp::GetPreferredSize()
{
	if(!myLayers.empty())
	{
		GxWidget* root = myLayers.begin()->root;
		root->Adjust();
		return root->GetPreferredSize();
	}
	return GxVec2i(0, 0);
}

GxVec2i GxContextImp::GetMinimumSize()
{
	if(!myLayers.empty())
	{
		GxWidget* root = myLayers.begin()->root;
		root->Adjust();
		return root->GetMinimumSize();
	}
	return GxVec2i(0, 0);
}

bool GxContextImp::MouseOccupied()
{
	return myFocusWidget || myHoverWidget;
}

// ===================================================================================
// Private functions used by widgets
// ===================================================================================

void GxContextImp::ReceiveEvent(GxWidget* widget, GxWidgetEvent& evt)
{
	GxCallback* callback = NULL;

	while(widget && !callback)
	{
		callback = widget->GetCallback();
		widget = widget->GetParent();
	}

	if(callback)
	{
		CallbackEvent entry;
		entry.evt = evt;
		entry.callback = callback;
		myWriteEvents.push_back(entry);
	}
}

void GxContextImp::Remove(GxWidget* w)
{
	if(myHoverWidget == w) myHoverWidget = NULL;
	if(myFocusWidget == w) myFocusWidget = NULL;
	if(myInputWidget == w) myInputWidget = NULL;
}

void GxContextImp::GrabFocus(GxWidget* widget)
{
	if(myFocusWidget != widget)
	{
		if(myFocusWidget)
			myFocusWidget->OnLoseFocus();

		myFocusWidget = widget;
	}
}

void GxContextImp::ReleaseFocus(GxWidget* widget)
{
	if(myFocusWidget == widget)
	{
		if(myFocusWidget)
			myFocusWidget->OnLoseFocus();

		myFocusWidget = NULL;
	}
}

void GxContextImp::GrabInput(GxWidget* widget)
{
	if(myInputWidget != widget)
	{
		if(myInputWidget)
			myInputWidget->OnLoseInput();

		myInputWidget = widget;
	}
}

void GxContextImp::ReleaseInput(GxWidget* widget)
{
	if(myInputWidget == widget)
	{
		if(myInputWidget)
			myInputWidget->OnLoseInput();

		myInputWidget = NULL;
	}
}

void GxContextImp::myDestroyLayers()
{
	LayerVec::reverse_iterator it;
	for(it = myLayers.rbegin(); it != myLayers.rend(); ++it)
		delete it->root;

	myLayers.clear();
}

void GxContextImp::myUpdateWidgetHighlights(float dt)
{
	// Update highlight widgets.
	GxWidget* widgets[] = {myFocusWidget, myHoverWidget};
	GxWidget** h = myHlWidgets;
	float* v = myHlValues;
	for(int i=0; i<2; ++i)
	{
		GxWidget* w = widgets[i];
		if(w && w != h[0] && w != h[1] && w != h[2])
		{
			int j = (v[0] < v[1]) ? ((v[0] < v[2]) ? 0 : 2) : ((v[1] < v[2]) ? 1 : 2);
			myHlWidgets[j] = w;
			myHlValues[j] = 0.5f;
		}
	}

	// Update highlight values.
	for(int i=0; i<3; ++i)
	{
		if(h[i] == myFocusWidget)
			v[i] = 0.5f;
		else if(h[i] == myHoverWidget)
			v[i] = GxClamp(v[i] + dt*4, 0.5f, 1.0f);
		else
			v[i] = GxMax(0.f, v[i] - dt * 2);
	}
}

void GxContextImp::myDisplayToolTip()
{
	GxDraw* draw = GxDraw::Get();
	GxStyle& style = *GxStyle::Get();

	if(myHoverWidget && myToolTipTimer > myToolTipDelay)
	{
		GxString text = myHoverWidget->GetToolTip();
		if(text.Empty()) return;

		float alpha = GxMin(1.f, (myToolTipTimer - myToolTipDelay) * 4.f);

		GxText settings = style.d.text[0];
		settings.SetAlign(GX_TA_LEFT, GX_TA_TOP);
		settings.SetColor(settings.top.Alpha(alpha));
		if(settings.shadow.a > 0)
		{
			uchar a = (uchar)((float)settings.shadow.a * alpha);
			settings.shadow = settings.shadow.Alpha(a);
		}
		settings.maxWidth = GxMax(0, GxMin(256, myView.w - 8));
		settings.flags = GX_TF_JUSTIFIED;

		GxRecti r = settings.GetTextRect(0, 0, text);

		GxVec2i mpos = GxInput::Get()->GetMousePos();
		r.x = GxMin(myView.x + mpos.x + 12, myView.x + myView.w - r.w - 8);
		r.x = GxMax(r.x, 8);
		r.y = mpos.y + 16;
	
		GxRecti br = r; br.Expand(6, 3, 6, 7);
		GxColor bgPanel = style.c.bgPanel.Alpha(alpha);
		GxColor bgFrame = style.c.frameOutline.Alpha(alpha);

		draw->Rect(br.x, br.y, br.w, br.h, bgFrame);
		br.Shrink(1);
		draw->Rect(br.x+2, br.y+2, br.w, br.h, GxColor(0.f, 0.f, 0.f, alpha*0.25f));
		draw->Rect(br.x, br.y, br.w, br.h, bgPanel);

		settings.Draw(r.x, r.y, text);
	}
}

// ===================================================================================
// GxContextNode
// ===================================================================================

GxContextNode::~GxContextNode()
{
	if(parent)
		parent->GetContextNode()->RemoveChild(this);

	if(context)
		context->Remove(owner);

	Clear();
}

GxContextNode::GxContextNode(GxWidget* _owner)
	:owner(_owner)
	,parent(NULL)
	,callback(NULL)
	,context(NULL)
	,children(NULL)
	,childCount(0)
{
}

void GxContextNode::Clear()
{
	for(int i=0; i<childCount; ++i)
	{
		children[i]->parent = NULL;
		children[i]->SetContext(NULL);
	}

	GxFree(children);
	children = NULL;
	childCount = 0;
}

void GxContextNode::SetParent(GxWidget* newParent)
{
	if(parent == newParent)
		return;

	if(parent)
		parent->GetContextNode()->RemoveChild(this);

	if(newParent)
	{
		GxContextNode* p = newParent->GetContextNode();
		p->children = GxRealloc(p->children, p->childCount+1);
		p->children[p->childCount++] = this;
		SetContext(p->context);
	}
	else
	{
		SetContext(NULL);
	}

	parent = newParent;
}

void GxContextNode::SetCallback(GxCallback* newCallback)
{
	callback = newCallback;
}

void GxContextNode::SetContext(GxContextImp* newContext)
{
	if(context == newContext)
		return;

	if(context)
		context->Remove(owner);

	for(int i=0; i<childCount; ++i)
		children[i]->SetContext(newContext);

	context = newContext;
}

void GxContextNode::RemoveChild(GxContextNode* child)
{
	for(int i=0; i<childCount; ++i)
	{
		if(children[i] == child)
		{
			GxArrayErase(children, childCount, i);
			children[--childCount] = NULL;
		}
	}
}

void GxContextNode::OnKeyPress(GxKeyEvent& evt)
{
	for(int i=childCount-1; i>=0; --i)
		children[i]->OnKeyPress(evt);

	owner->OnKeyPress(evt);
}

void GxContextNode::OnKeyRelease(GxKeyEvent& evt)
{
	for(int i=childCount-1; i>=0; --i)
		children[i]->OnKeyRelease(evt);

	owner->OnKeyRelease(evt);
}

void GxContextNode::OnMousePress(GxMouseEvent& evt)
{
	for(int i=childCount-1; i>=0; --i)
		children[i]->OnMousePress(evt);

	owner->OnMousePress(evt);
}

void GxContextNode::OnMouseRelease(GxMouseEvent& evt)
{
	for(int i=childCount-1; i>=0; --i)
		children[i]->OnMouseRelease(evt);

	owner->OnMouseRelease(evt);
}

void GxContextNode::OnMouseScroll(GxScrollEvent& evt)
{
	for(int i=childCount-1; i>=0; --i)
		children[i]->OnMouseScroll(evt);

	owner->OnMouseScroll(evt);
}

}; // namespace gui
}; // namespace guix