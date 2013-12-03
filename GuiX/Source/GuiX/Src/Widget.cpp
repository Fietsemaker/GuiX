#include <GuiX/Config.h>

#include <GuiX/Widget.h>

#include <Src/ContextImp.h>
#include <Src/WidgetDatabase.h>

namespace guix {
namespace gui {

// ===================================================================================
// GxSizePolicy
// ===================================================================================

GxSizePolicy::GxSizePolicy()
	:hint(96, 24)
	,min(0, 0)
	,max(1 << 20, 1 << 20)
	,stretchH(1)
	,stretchV(1)
	,flagsH(GX_SP_RESIZE)
	,flagsV(GX_SP_RESIZE)
	,adjustHint(true)
	,adjustMin(true)
	,adjustFlags(true)
{
}

GxVec2i GxSizePolicy::GetMinSize() const
{
	const int x = flagsH[GX_SPF_SHRINK] ? min.x : hint.x;
	const int y = flagsV[GX_SPF_SHRINK] ? min.y : hint.y;
	return GxVec2i(x, y);
}

GxVec2i GxSizePolicy::GetMaxSize() const
{
	const int x = flagsH[GX_SPF_GROW] ? max.x : hint.x;
	const int y = flagsV[GX_SPF_GROW] ? max.y : hint.y;
	return GxVec2i(x, y);
}

GxVec2i GxSizePolicy::GetHintSize() const
{
	return hint;
}

// ===================================================================================
// GxSizePolicyResult
// ===================================================================================

GxSizePolicyResult::GxSizePolicyResult(const GxWidget* w, bool transpose)
{
	hint = w->GetPreferredSize();
	min = w->GetMinimumSize();
	max = w->GetMaximumSize();

	const GxSizePolicy& pol = w->GetPolicy();

	stretchH = pol.stretchH;
	stretchV = pol.stretchV;
	flagsH   = pol.flagsH;
	flagsV   = pol.flagsV;

	if(transpose)
	{
		GxSwap(hint.x, hint.y);
		GxSwap(min.x, min.y);
		GxSwap(max.x, max.y);
		GxSwap(flagsH, flagsV);
		GxSwap(stretchH, stretchV);
	}
}

// ===================================================================================
// GxWidget
// ===================================================================================

GxWidget::~GxWidget()
{
	GxWidgetDatabase* database = GxWidgetDatabase::singleton;
	database->Remove(this);

	delete myContextNode;
	delete myPolicy;
}

GxWidget::GxWidget()
{
	myPolicy = new GxSizePolicy;
	myContextNode = new GxContextNode(this);
}

GxWidget::GxWidget(const char* id)
{
	myPolicy = new GxSizePolicy;
	myContextNode = new GxContextNode(this);

	if(id) SetWidgetId(id);
}

void GxWidget::SetWidgetId(const char* id)
{
	GxWidgetDatabase* database = GxWidgetDatabase::singleton;
	database->SetWidgetId(this, id ? id : "");
}

void GxWidget::SetGroupId(const char* id)
{
	GxWidgetDatabase* database = GxWidgetDatabase::singleton;
	database->SetGroupId(this, id ? id : "");
}

void GxWidget::SetToolTip(GxString text)
{
	GxWidgetDatabase* database = GxWidgetDatabase::singleton;
	database->SetToolTip(this, text);
}

void GxWidget::SetParent(GxWidget* parent)
{
	myContextNode->SetParent(parent);
}

void GxWidget::SetCallback(GxCallback* callback)
{
	myContextNode->SetCallback(callback);
}

void GxWidget::EmitEvent(const char* type)
{
	EmitEvent(type, GxVariant());
}

void GxWidget::EmitEvent(const char* type, const GxVariant& value)
{
	GxContextImp* context = myContextNode->context;
	if(context)
	{
		GxWidgetEvent evt;
		evt.id = GetWidgetId();
		evt.type = type;
		evt.value = value;
		context->ReceiveEvent(this, evt);
	}
}

void GxWidget::GrabFocus()
{
	GxContextImp* context = myContextNode->context;
	if(context)	context->GrabFocus(this);
}

void GxWidget::GrabInput()
{
	GxContextImp* context = myContextNode->context;
	if(context)	context->GrabInput(this);
}

void GxWidget::ReleaseFocus()
{
	GxContextImp* context = myContextNode->context;
	if(context)	context->ReleaseFocus(this);
}

void GxWidget::ReleaseInput()
{
	GxContextImp* context = myContextNode->context;
	if(context)	context->ReleaseInput(this);
}

void GxWidget::EmitGroupEvent()
{
	GxWidgetDatabase* database = GxWidgetDatabase::singleton;
	database->SendGroupEvent(this);
}

bool GxWidget::IsHoverWidget() const
{
	GxContextImp* context = myContextNode->context;
	if(context)	return context->GetHoverWidget() == this;

	return false;
}

bool GxWidget::IsFocusWidget() const
{
	GxContextImp* context = myContextNode->context;
	if(context)	return context->GetFocusWidget() == this;

	return false;
}

bool GxWidget::IsInputWidget() const
{
	GxContextImp* context = myContextNode->context;
	if(context)	return context->GetInputWidget() == this;

	return false;
}

float GxWidget::GetHighlightValue() const
{
	GxContextImp* context = myContextNode->context;
	if(context)	return context->GetWidgetHighlight(this);

	return 0.f;
}

bool GxWidget::IsLockedWidget() const
{
	bool locked = false;
	const GxWidget* widget = this;
	do
	{
		locked = widget->myFlags[F_LOCKED];
		widget = widget->myContextNode->parent;
	}
	while(widget && !locked);
	return locked;
}

GxWidget* GxWidget::GetParent() const
{
	return myContextNode->parent;
}

GxContext* GxWidget::GetContext() const
{
	return myContextNode->context;
}

GxCallback* GxWidget::GetCallback() const
{
	return myContextNode->callback;
}

GxString GxWidget::GetWidgetId() const
{
	GxWidgetDatabase* database = GxWidgetDatabase::singleton;
	return database->GetWidgetId(this);
}

GxString GxWidget::GetGroupId() const
{
	GxWidgetDatabase* database = GxWidgetDatabase::singleton;
	return database->GetGroupId(this);
}

GxString GxWidget::GetToolTip() const
{
	GxWidgetDatabase* database = GxWidgetDatabase::singleton;
	return database->GetToolTip(this);
}

}; // namespace gui
}; // namespace guix