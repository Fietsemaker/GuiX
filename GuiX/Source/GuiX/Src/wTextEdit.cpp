#include <GuiX/Config.h>

#include <math.h>

#include <GuiX/Core.h>
#include <GuiX/Draw.h>
#include <GuiX/Style.h>
#include <GuiX/Context.h>

#include <GuiX/wTextEdit.h>

namespace guix {
namespace widgets {

// ===================================================================================
// GxTextEditHelper
// ===================================================================================

GxTextEditHelper::GxTextEditHelper()
	:myRect(0,0,128,96)
	,myCursor(0,0)
	,myMaxLength(INT_MAX)
	,myBlinkTime(0.f)
	,myScrollOffset(0.f)
	,myIsMultiLine(true)
	,myIsNumerical(false)
	,myIsPassword(false)
	,myIsSelected(false)
	,myIsDragging(false)
	,myForceScrollUpdate(false)
	,myReplacementChar('*')
	,myHlColor(160)
{
}

void GxTextEditHelper::Select()
{
	myIsSelected = true;
}

void GxTextEditHelper::Deselect()
{
	myIsSelected = false;
	myIsDragging = false;
	myScrollOffset = 0.f;
	myBlinkTime = 0.f;
	myCursor.x = 0;
	myCursor.y = 0;
}

void GxTextEditHelper::OnKeyPress(GxKeyEvent& evt)
{
	if(!myIsSelected) return;

	myIsDragging = false;
	myBlinkTime = 0.f;

	GxKeyCode key = evt.key;
	bool shift = (evt.flags & GX_KF_SHIFT) != 0;
	bool ctrl  = (evt.flags & GX_KF_CTRL ) != 0;

	if(shift)
	{
		if(key == GX_KC_HOME ) myCursor.y = 0;
		if(key == GX_KC_END  ) myCursor.y = myContent.Length();
		if(key == GX_KC_LEFT ) myCursor.y = myContent.PrevChar(myCursor.y);
		if(key == GX_KC_RIGHT) myCursor.y = myContent.NextChar(myCursor.y);
		if(key == GX_KC_UP   ) myCursor.y = myAdvanceCursorLine(false);
		if(key == GX_KC_DOWN ) myCursor.y = myAdvanceCursorLine(true);
	}
	else
	{
		if(key == GX_KC_HOME) myCursor.x = myCursor.y = 0;
		if(key == GX_KC_END ) myCursor.x = myCursor.y = myContent.Length();
		if(myCursor.x != myCursor.y)
		{
			if(key == GX_KC_LEFT ) myCursor.x = myCursor.y = GxMin(myCursor.x, myCursor.y); 
			if(key == GX_KC_RIGHT) myCursor.x = myCursor.y = GxMax(myCursor.x, myCursor.y);
		}
		else
		{
			if(key == GX_KC_LEFT ) myCursor.x = myCursor.y = myContent.PrevChar(myCursor.y);
			if(key == GX_KC_RIGHT) myCursor.x = myCursor.y = myContent.NextChar(myCursor.y);
		}
		if(key == GX_KC_UP  ) myCursor.x = myCursor.y = myAdvanceCursorLine(false);
		if(key == GX_KC_DOWN) myCursor.x = myCursor.y = myAdvanceCursorLine(true);
	}
	if(myCursor.x == myCursor.y) 
	{
		if(key == GX_KC_BACKSPACE) myCursor.y = myContent.PrevChar(myCursor.y);
		if(key == GX_KC_DELETE   ) myCursor.y = myContent.NextChar(myCursor.y);
	}
	if(ctrl && key == GX_KC_A)
	{
		myCursor.x = 0;
		myCursor.y = myContent.Length();
	}
	myCursor.x = GxClamp(myCursor.x, 0, (int)myContent.Length());
	myCursor.y = GxClamp(myCursor.y, 0, (int)myContent.Length());

	if(key == GX_KC_DELETE || key == GX_KC_BACKSPACE)
	{
		myDeleteSelection();
	}
	if(ctrl && (key == GX_KC_X || key == GX_KC_C) && !myIsPassword)
	{
		int a = myCursor.x;
		int b = myCursor.y;
		if(a > b) GxSwap(a, b);
		GxString str = myContent.Substr(a, b-a);
		GxSystemInterface::Get()->SetClipboardText(str);
		if(key == GX_KC_X) myDeleteSelection();
	}
	if(ctrl && key == GX_KC_V)
	{
		GxString input;
		if(GxSystemInterface::Get()->GetClipboardText(input))
			OnTextInput(input);
	}
}

void GxTextEditHelper::OnMousePress(GxMouseEvent& evt)
{
	if(!myIsSelected) return;

	myIsDragging = true;

	GxVec2i ofs = myGetScrollOffset();
	GxVec2i pos = GxVec2i(evt.x, myIsMultiLine ? evt.y : (ofs.y + 2));
	GxString str = myGetDisplayString();

	myAdjustSettings();
	myCursor.x = myCursor.y = mySettings.GetCharIndex(ofs.x, ofs.y, pos.x, pos.y, str);
}

void GxTextEditHelper::OnMouseRelease(GxMouseEvent& evt)
{
	if(!myIsSelected) return;

	myIsDragging = false;
}

void GxTextEditHelper::OnTextInput(const GxString& text)
{
	myDeleteSelection();
	GxString input = text;
	myFilterText(input);
	input.Truncate(myMaxLength - myContent.Length());
	myContent.Insert(myCursor.y, input);
	myCursor.y += input.Length();
	myCursor.x = myCursor.y;
	myForceScrollUpdate = true;
}

void GxTextEditHelper::Tick(int mouseX, int mouseY, float dt)
{
	if(!myIsSelected) return;

	myAdjustSettings();
	GxVec2i pos = myGetScrollOffset();
	GxString string = myGetDisplayString();

	// Update cursor position
	if(myIsDragging)
	{
		GxVec2i m = GxVec2i(mouseX, myIsMultiLine ? mouseY : (pos.y+2));
		int i = mySettings.GetCharIndex(pos.x, pos.y, m.x, m.y, string);
		myBlinkTime = 0.f;
		myCursor.y = i;
	}
	myCursor.x = GxClamp(myCursor.x, 0, (int)myContent.Length());
	myCursor.y = GxClamp(myCursor.y, 0, (int)myContent.Length());

	// Update text offset
	float targetOffset = myScrollOffset;
	const float fontH = (float)mySettings.font.GetSize();

	const GxRectf textMargins(8.f, 8.f, 8.f, 8.f + fontH);
	const GxRecti textRect = mySettings.GetTextRect(pos.x, pos.y, string);
	const GxVec2f textSize = GxVec2f((float)textRect.w, (float)textRect.h);
	const GxRecti charRect = mySettings.GetCharRect(0, 0, myCursor.y, string);
	const GxVec2f cursorPos = GxVec2f((float)charRect.x, (float)charRect.y);

	if(myIsMultiLine)
	{
		if(cursorPos.y <= myScrollOffset + textMargins.y)
			targetOffset = cursorPos.y - textMargins.y;
		if(cursorPos.y >= myScrollOffset + (float)myRect.h - textMargins.h)
			targetOffset = cursorPos.y - (float)myRect.h + textMargins.h;

		targetOffset = GxMin(targetOffset, textSize.y - (float)myRect.h + textMargins.h);
		targetOffset = GxMax(targetOffset, 0.f);
	}
	else
	{
		if(cursorPos.x <= myScrollOffset + textMargins.x)
			targetOffset = cursorPos.x - textMargins.x;
		if(cursorPos.x >= myScrollOffset + (float)myRect.w - textMargins.w)
			targetOffset = cursorPos.x - (float)myRect.w + textMargins.w;

		targetOffset = GxMin(targetOffset, textSize.x - (float)myRect.w + textMargins.w);
		targetOffset = GxMax(targetOffset, 0.f);
	}

	const float delta = GxMax(fabs(myScrollOffset - targetOffset) * 10.f * dt, dt * 256.f);
	float forceOffset = (myScrollOffset < targetOffset) ?
		GxMin(myScrollOffset + delta, targetOffset) : GxMax(myScrollOffset - delta, targetOffset);
	myScrollOffset = myForceScrollUpdate ? targetOffset : forceOffset;
	myForceScrollUpdate = false;

	// Update blink time
	myBlinkTime = fmod(myBlinkTime + dt, 1);
}

void GxTextEditHelper::Draw()
{
	if(myRect.w <= 0 || myRect.h <= 0) return;

	GxDraw* draw = GxDraw::Get();

	myAdjustSettings();
	GxVec2i pos = myGetScrollOffset();
	GxString string = myGetDisplayString();

	draw->PushScissorRect(myRect.x, myRect.y, myRect.w, myRect.h);

	// Selection box rendering
	if(myIsSelected && myCursor.x != myCursor.y)
	{
		GxVec2i cursor = myCursor;
		if(cursor.x > cursor.y) GxSwap(cursor.x, cursor.y);
		const GxColor top = mySettings.top;
		const GxColor btm = mySettings.bottom;
		mySettings.SetColor(myHlColor);
		mySettings.SetRange(cursor.x, cursor.y);
		mySettings.DrawHighlight(pos.x, pos.y+2, string);
		mySettings.SetRange();
		mySettings.top = top;
		mySettings.bottom = btm;
	}

	// GxText rendering
	mySettings.Draw(pos.x, pos.y, string);

	// Selection cursor
	if(myIsSelected && myBlinkTime < 0.5f)
	{
		GxRecti r = mySettings.GetCharRect(pos.x, pos.y, myCursor.y, string);
		draw->Rect(r.x, r.y+2, 1, r.h, mySettings.top, mySettings.bottom);
	}

	draw->PopScissorRect();
}

void GxTextEditHelper::SetRect(const GxRecti& rect)
{
	myRect = rect;
	myRect.x += 2; myRect.w = GxMax(0, myRect.w - 4);
	myRect.y += 2; myRect.h = GxMax(0, myRect.h - 4);	
}

void GxTextEditHelper::SetHighlightColor(GxColor color)
{
	myHlColor = color;
}

void GxTextEditHelper::SetMaxLength(size_t maxLength)
{
	myMaxLength = maxLength;
}

void GxTextEditHelper::SetNumerical(bool isNumerical)
{
	myIsNumerical = isNumerical;
	myFilterText(myContent);
}

void GxTextEditHelper::SetMultiLine(bool isMultiLine)
{
	myIsMultiLine = isMultiLine;
	myFilterText(myContent);
}

void GxTextEditHelper::SetPassword(bool isPassword, char replacement)
{
	myIsPassword = isPassword;
	myReplacementChar = replacement;
}

void GxTextEditHelper::SetText(const GxString& text)
{
	myContent = text;
	myFilterText(myContent);
	myContent.Truncate(myMaxLength);
	myCursor.x = myCursor.y = 0;
}

bool GxTextEditHelper::IsSelected() const
{
	return myIsSelected;
}

bool GxTextEditHelper::IsMultiLine() const
{
	return myIsMultiLine;
}

bool GxTextEditHelper::IsNumerical() const
{
	return myIsNumerical;
}

bool GxTextEditHelper::IsPassword() const
{
	return myIsPassword;
}
	
const GxRecti& GxTextEditHelper::GetRect() const
{
	return myRect;
}

GxColor GxTextEditHelper::GetHighlightColor() const
{
	return myHlColor;
}

size_t GxTextEditHelper::GetMaxLength() const
{
	return myMaxLength;
}

const GxString& GxTextEditHelper::GetText() const
{
	return myContent;
}

const GxText& GxTextEditHelper::GetSettings() const
{
	return mySettings;
}

GxText& GxTextEditHelper::GetSettings()
{
	return mySettings;
}

void GxTextEditHelper::myAdjustSettings()
{
	mySettings.alignV = myIsMultiLine ? GX_TA_TOP : GX_TA_MIDDLE;
	mySettings.alignH = GX_TA_LEFT;
	mySettings.maxWidth = myIsMultiLine ? myRect.w : GxText::npos();
	mySettings.SetFlag(GX_TF_ELLIPSIS, false);
}

void GxTextEditHelper::myDeleteSelection()
{
	if(myCursor.x != myCursor.y)
	{
		if(myCursor.x > myCursor.y) 
			GxSwap(myCursor.x, myCursor.y);
		myContent.Erase(myCursor.x, myCursor.y - myCursor.x);
		myCursor.y = myCursor.x;
		myForceScrollUpdate = true;
	}
}

int GxTextEditHelper::myAdvanceCursorLine(bool forward)
{
	int pos = myCursor.y;
	if(myIsMultiLine)
	{
		const char* str = myContent.Raw();
		int len = (int)myContent.Length();
		if(forward)
		{
			pos = GxClamp(myCursor.y, 0, len);
			while(pos < len && str[pos] != '\n') ++pos;
			++pos;
		}
		else
		{
			pos = GxClamp(myCursor.y, 0, len);
			while(pos > 0 && str[pos-1] != '\n') --pos;
			if(pos > 0) --pos;
			while(pos > 0 && str[pos-1] != '\n') --pos;
		}
	}
	return pos;
}

void GxTextEditHelper::myFilterText(GxString& text) const
{
	if(myIsNumerical)
	{
		const char* validChars = "0123456789-+.eE";
		for(int i=text.Length()-1; i>=0; --i)
		{
			char c = text[i];
			bool isValid = false; 
			for(int j=0; validChars[j]; ++j)
				if(validChars[j] == c)
					{isValid = true; break;}
			if(!isValid) 
				text.Erase(i, 1);
		}
	}
	if(!myIsMultiLine)
	{
		for(int i=text.Length()-1; i>=0; --i)
			if(text[i] == '\n') 
				text.Erase(i, 1);
	}
}

GxString GxTextEditHelper::myGetDisplayString() const
{
	if(myIsPassword)
		return GxString(myReplacementChar, myContent.Length());
	return myContent;
}

GxVec2i GxTextEditHelper::myGetScrollOffset() const
{
	if(myIsMultiLine)
		return GxVec2i(myRect.x, myRect.y - (int)myScrollOffset);
	return GxVec2i(myRect.x - (int)myScrollOffset, myRect.y + myRect.h/2 - 2);
}

// ===================================================================================
// GxTextEdit
// ===================================================================================

GxTextEditAbstract::~GxTextEditAbstract()
{
}

GxTextEditAbstract::GxTextEditAbstract(const char* id, GxString text, bool multiline)
	:GxWidget(id)
{
	GxStyle& style = *GxStyle::Get();

	myField.GetSettings() = style.d.text[0];
	myField.SetHighlightColor(style.c.textSelect);

	if(multiline)
	{
		myPolicy->min.Set(48, 24);
		myPolicy->hint.Set(96, 48);
		myPolicy->flagsH = GX_SP_EXPAND;
		myPolicy->flagsV = GX_SP_EXPAND;
	}
	else
	{
		myPolicy->min.Set(48, 24);
		myPolicy->hint.Set(96, 24);
		myPolicy->flagsH = GX_SP_EXPAND;
	}

	myField.SetMultiLine(multiline);
	myField.SetText(text);
}

void GxTextEditAbstract::OnMousePress(GxMouseEvent& evt)
{
	if(IsHoverWidget())
	{
		if(evt.button == GX_MC_LEFT && !evt.handled && !IsLockedWidget())
		{
			GrabFocus();
			if(!IsInputWidget())
			{
				GrabInput();
				myField.Select();
				myFlags.Set(F_INTERACTED);
			}
			myField.OnMousePress(evt);
		}
		evt.handled = true;
	}
	if(evt.button == GX_MC_LEFT && IsInputWidget() && !IsHoverWidget())
	{
		ReleaseInput();
	}
}

void GxTextEditAbstract::OnMouseRelease(GxMouseEvent& evt)
{
	if(evt.button == GX_MC_LEFT && IsFocusWidget())
	{
		ReleaseFocus();
	}
	if(IsInputWidget())
	{
		myField.OnMouseRelease(evt);
		evt.handled = true;
	}
}

void GxTextEditAbstract::OnKeyPress(GxKeyEvent& evt)
{
	if(IsInputWidget())
	{
		myField.OnKeyPress(evt);
		if(evt.handled == false && evt.key == GX_KC_RETURN && !myField.IsMultiLine())
		{
			ReleaseInput();
		}
		evt.handled = true;
	}
}

void GxTextEditAbstract::OnTextInput(GxTextEvent& evt)
{
	myField.OnTextInput(evt.text);
}

void GxTextEditAbstract::OnLoseInput()
{
	myField.Deselect();
	EmitEvent(eChanged(), myField.GetText());
	myFlags.Set(F_CHANGED);
}

void GxTextEditAbstract::SetRect(const GxRecti& rect)
{
	myRect = rect;
	myField.SetRect(rect);
}

void GxTextEditAbstract::Tick(float dt)
{
	GxVec2i mpos = GxInput::Get()->GetMousePos();
	myField.Tick(mpos.x, mpos.y, dt);

	if(IsHoverWidget() || IsFocusWidget())
		GetContext()->SetCursor(GX_CI_CARET);
}

void GxTextEditAbstract::Draw()
{
	GxStyle& style = *GxStyle::Get();
	style.Field(myRect);
	myField.Draw();
}

void GxTextEditAbstract::SetText(GxString text)
{
	myField.SetText(text);
}

const GxString& GxTextEditAbstract::GetText() const
{
	return myField.GetText();
}

const GxTextEditHelper& GxTextEditAbstract::GetField() const
{
	return myField;
}

GxTextEditHelper& GxTextEditAbstract::GetField()
{
	return myField;
}

// ===================================================================================
// GxTextEdit
// ===================================================================================

GxTextEdit::GxTextEdit()
	:GxTextEditAbstract(NULL, GxString(), true)
{
}

GxTextEdit::GxTextEdit(const char* id, GxString text)
	:GxTextEditAbstract(id, text, true)
{
}

// ===================================================================================
// GxLineEdit
// ===================================================================================

GxLineEdit::GxLineEdit()
	:GxTextEditAbstract(NULL, GxString(), false)
{
}

GxLineEdit::GxLineEdit(const char* id, GxString text)
	:GxTextEditAbstract(id, text, false)
{
}

// ===================================================================================
// Widget events
// ===================================================================================

GxDefineWidgetEvent (GxTextEditAbstract, eChanged)

}; // namespace widgets
}; // namespace guix