#include <GuiX/Config.h>

#include <GuiX/Style.h>
#include <GuiX/Canvas.h>

namespace guix {
namespace gui {

namespace {

static GxColor hueSlice[7] =
{
	GxColor (255, 0,   0  ),
	GxColor (255, 255, 0  ),
	GxColor (0,   255, 0  ),
	GxColor (0,   255, 255),
	GxColor (0,   0,   255),
	GxColor (255, 0,   255),
	GxColor (255, 0,   0  ),
};

static void GetBlackScheme(GxStyle::Colors& c)
{
	c.bgPanel = GxColor(65, 65, 65);
	c.bgField = GxColor(30, 30, 30);
	c.bgBar   = GxColor(40, 40, 40);
	c.bgTab   = GxColor(50, 50, 50);

	c.frameOutline      = GxColor(20, 20, 20);
	c.frameOutEmboss[0] = GxColor(60, 60, 60);
	c.frameOutEmboss[1] = GxColor(100, 100, 100);
	c.frameInEmboss[0]  = GxColor(130, 130, 130);
	c.frameInEmboss[1]  = GxColor(110, 110, 110);
	c.frameInColor[0]   = GxColor(100, 100, 100);
	c.frameInColor[1]   = GxColor(80, 80, 80);
	c.fieldBorder[0]    = GxColor(20, 20, 20);
	c.fieldBorder[1]    = GxColor(20, 20, 20);

	c.frameInEmbossHl[0] = GxColor(150, 150, 170);
	c.frameInEmbossHl[1] = GxColor(120, 120, 170);
	c.frameInColorHl[0]  = GxColor(120, 120, 140);
	c.frameInColorHl[1]  = GxColor(90, 90, 100);

	c.dockBg[0] = GxColor(20, 20, 20);
	c.dockBg[1] = GxColor(40, 40, 40);
	c.dockHl = GxColor(80, 80, 90);

	c.textColor  = GxColor(230, 230, 230);
	c.textSelect = GxColor(100, 100, 110);
	c.textShadow = GxColor(20, 20, 20);
	c.textLocked = GxColor(150, 150, 150);
}

static void GetWhiteScheme(GxStyle::Colors& c)
{
	c.bgPanel = GxColor(240, 240, 240);
	c.bgField = GxColor(255, 255, 255);
	c.bgBar   = GxColor(195, 195, 195);
	c.bgTab   = GxColor(170, 170, 170);

	c.frameOutline      = GxColor(150, 150, 150);
	c.frameOutEmboss[0] = GxColor(240, 240, 240);
	c.frameOutEmboss[1] = GxColor(240, 240, 240);
	c.frameInEmboss[0]  = GxColor(250, 250, 250);
	c.frameInEmboss[1]  = GxColor(220, 220, 220);
	c.frameInColor[0]   = GxColor(240, 240, 240);
	c.frameInColor[1]   = GxColor(200, 200, 200);
	c.fieldBorder[0]    = GxColor(180, 180, 180);
	c.fieldBorder[1]    = GxColor(230, 230, 230);

	c.frameInEmbossHl[0] = GxColor(240, 240, 255);
	c.frameInEmbossHl[1] = GxColor(210, 210, 250);
	c.frameInColorHl[0]  = GxColor(220, 220, 240);
	c.frameInColorHl[1]  = GxColor(190, 190, 200);

	c.dockBg[0] = GxColor(160, 160, 160);
	c.dockBg[1] = GxColor(140, 140, 140);
	c.dockHl = GxColor(180, 180, 190);

	c.textColor  = GxColor(0, 0, 0, 255);
	c.textSelect = GxColor(200, 200, 220);
	c.textShadow = GxColor(0, 0, 0, 0);
	c.textLocked = GxColor(120, 120, 120);
}

static void CreateStyleData(const GxStyle::Colors& c, GxStyle::Data& d)
{
	// Text settings
	GxFontDatabase* fontdb = GxFontDatabase::Get();
	if(fontdb->HasEntry("default"))
	{
		GxFont defaultFont("default");
		d.text[0].font = defaultFont;
		d.text[1].font = defaultFont;
	}

	d.text[0].SetColor(c.textColor);
	d.text[0].shadow = c.textShadow;
	d.text[1].SetColor(c.textLocked);

	d.text[0].flags = GX_TF_ELLIPSIS | GX_TF_FORMATTING;
	d.text[1].flags = GX_TF_ELLIPSIS | GX_TF_FORMATTING;

	// Create bar frame
	{
		GxCanvas canvas(16, 16);

		canvas.SetColor(c.frameOutEmboss[0], c.frameOutEmboss[1]);
		canvas.Rect(0, 0, 16, 16);
		canvas.SetColor(c.frameOutline);
		canvas.Rect(1, 1, 15, 15);
		canvas.SetColor(c.bgBar);
		canvas.Rect(2, 2, 14, 14);

		d.bar.SetTexture(canvas.CreateTexture());
		d.bar.SetBorderSize(3, 3, 3, 3);
	}

	// Create field frame
	{
		GxCanvas canvas(16, 16);

		canvas.SetColor(c.frameOutEmboss[0], c.frameOutEmboss[1]);
		canvas.Rect(0, 0, 16, 16);
		canvas.SetColor(c.fieldBorder[0]);
		canvas.Rect(1, 1, 15, 15);
		canvas.SetColor(c.fieldBorder[1]);
		canvas.Rect(2, 2, 15, 15);
		canvas.SetColor(c.bgField);
		canvas.Rect(2, 2, 14, 14);

		d.field.SetTexture(canvas.CreateTexture());
		d.field.SetBorderSize(3, 3, 3, 3);
	}

	// Create box frame
	{
		GxCanvas canvas(16, 16);

		canvas.SetColor(c.frameOutEmboss[1].Alpha(96));
		canvas.Rect(0, 0, 16, 16);
		canvas.SetColor(c.frameOutline.Alpha(192));
		canvas.Rect(1, 1, 15, 15);
		canvas.SetColor(c.frameOutEmboss[1].Alpha(96));
		canvas.Rect(2, 2, 14, 14);
		canvas.SetColor(c.bgPanel);
		canvas.Rect(3, 3, 13, 13);

		d.box.SetTexture(canvas.CreateTexture());
		d.box.SetBorderSize(4, 4, 4, 4);
	}

	// Create button texture
	for(int i=0; i<2; ++i)
	{
		GxCanvas canvas(16, 16);

		const GxColor* cemb = (i == 1) ? c.frameInEmbossHl : c.frameInEmboss;
		const GxColor* cbtn = (i == 1) ? c.frameInColorHl  : c.frameInColor;

		canvas.SetColor(c.frameOutline);
		canvas.Rect(0, 0, 16, 16);
		canvas.SetColor(cemb[0], cemb[1]);
		canvas.Rect(1, 1, 15, 15);
		canvas.SetColor(cbtn[0], cbtn[1]);
		canvas.Rect(2, 2, 14, 14);

		if(i == 1)
		{
			canvas.SetInnerGlow(1.75f);
			canvas.SetFill(false);
			canvas.SetColor(cemb[0].Alpha(128));
			canvas.Rect(1, 1, 15, 15);
		}

		d.button[i].SetTexture(canvas.CreateTexture());
		d.button[i].SetBorderSize(4, 4, 4, 4);
	}

	// Create tabs texture
	for(int i=0; i<3; ++i)
	{
		GxCanvas canvas(16, 16);
		canvas.Clear(c.bgPanel.Alpha(0));

		const GxColor* cemb = (i==2) ? c.frameInEmbossHl : c.frameInEmboss;
		const GxColor* cbtn = (i==2) ? c.frameInColorHl  : c.frameInColor;

		GxColor panelFade = c.bgPanel;
		panelFade.a = 0;
		if(i > 0)
		{
			canvas.SetColor(cemb[0], panelFade);
			canvas.Rect(1, 1, 15, 15);
			canvas.SetColor(cbtn[0], panelFade);
			canvas.Rect(2, 2, 14, 14);
		}
		canvas.SetColor(c.frameOutline, panelFade);
		canvas.SetFill(false);
		canvas.Rect(0, 0, 16, 16);
		
		d.tab[i].SetTexture(canvas.CreateTexture());
		d.tab[i].SetBorderSize(3, 3, 3, 3);
	}

	// Create radio button images
	{
		GxCanvas canvas(16, 16);
		for(int i=0; i<2; ++i)
		{
			canvas.Clear(c.bgPanel.Alpha(0));
			canvas.SetColor(c.frameOutEmboss[0], c.frameOutEmboss[1]);
			canvas.Circle(8, 8, 8);
			canvas.SetColor(c.frameOutline);
			canvas.Circle(8, 8, 7);

			const GxColor* cemb = (i == 1) ? c.frameInEmbossHl : c.frameInEmboss;
			const GxColor* cbtn = (i == 1) ? c.frameInColorHl  : c.frameInColor;

			canvas.SetColor(cemb[0], cemb[1]);
			canvas.Circle(8, 8, 6);
			canvas.SetColor(cbtn[0], cbtn[1]);
			canvas.Circle(8, 8, 5);

			if(i == 1)
			{
				canvas.SetInnerGlow(1.75f);
				canvas.SetFill(false);
				canvas.SetColor(cemb[0].Alpha(128));
				canvas.Circle(8, 8, 6);
			}

			d.radio[i].SetTexture(canvas.CreateTexture());
			d.radio[i].SetOrigin(0.5f, 0.5f);
		}

		// Button selection
		canvas.Clear(c.textColor.Alpha(0));
		canvas.SetFill(true);
		canvas.SetColor(c.textColor);
		canvas.Circle(8, 8, 3.5);

		d.radio[2].SetTexture(canvas.CreateTexture());
		d.radio[2].SetOrigin(0.5f, 0.5f);
	}

	// Create checkmark
	{
		GxCanvas canvas(16, 16);
		canvas.Clear(c.textColor.Alpha(0));

		GxVec2f verts[6] = {
			GxVec2f(4,6), GxVec2f(6,10), GxVec2f(11,2),
			GxVec2f(14,4), GxVec2f(6,14), GxVec2f(2,8),
		};
		for(int i=0; i<6; ++i) verts[i].Add(+0.5f,+0.5f);
		canvas.SetColor(c.textColor);
		canvas.Polygon(verts, 6);

		d.checkmark.SetTexture(canvas.CreateTexture());
		d.checkmark.SetOrigin(0.5f, 0.5f);
	}

	// Create arrow icon
	{
		GxCanvas canvas(8, 8);

		canvas.Clear(c.textColor.Alpha(0));
		canvas.SetColor(c.textColor);
		GxVec2f v[3] = {GxVec2f(1,3), GxVec2f(7,3), GxVec2f(4,6)};
		canvas.Polygon(v, 3);

		d.arrow.SetTexture(canvas.CreateTexture());
		d.arrow.SetOrigin(0.5f, 0.5f);
	}

	// Create grab icon
	{
		GxCanvas canvas(8, 8);

		canvas.SetColor(c.textColor);
		canvas.Rect(0, 1, 8, 2);
		canvas.Rect(0, 3, 8, 4);
		canvas.Rect(0, 5, 8, 6);

		d.grab.SetTexture(canvas.CreateTexture());
		d.grab.SetOrigin(0.5f, 0.5f);
	}

	// Create close icon
	{
		GxCanvas canvas(16, 16);

		GxColor cclear = c.bgPanel;
		cclear.a = 0;

		// Close button
		canvas.Clear(cclear);
		canvas.SetColor(c.textShadow);
		canvas.Line(4, 4.5, 14, 14.5, 2);
		canvas.Line(4, 14.5, 14, 4.5, 2);
		canvas.SetColor(c.textColor);
		canvas.Line(3, 3.5, 13, 13.5, 2);
		canvas.Line(3, 13.5, 13, 3.5, 2);

		d.close.SetTexture(canvas.CreateTexture());
		d.close.SetOrigin(0.5f, 0.5f);
	}

	// Create dock bar
	{
		float bar = 20;
		GxCanvas canvas(16, (int)bar);

		canvas.SetColor(c.frameOutline);
		canvas.Rect(0, 0, 16, bar);
		canvas.SetColor(c.dockBg[0], c.dockBg[1]);
		canvas.Rect(1, 1, 15, bar-1);

		d.dockBar.SetTexture(canvas.CreateTexture());
		d.dockBar.SetBorderSize(6, 6);
	}

	// Create floating dock frame
	{
		float bar = 20;
		GxCanvas canvas(64, 64 + (int)bar);

		for(int i=0; i<4; ++i)
		{
			canvas.SetOuterGlow((float)(14 - i*4));
			canvas.SetColor(0,0,0,32+i*24);
			canvas.Rect(20, 20, 48, 48 + bar, 1);
		}
		canvas.SetOuterGlow(0);

		canvas.SetColor(c.frameOutline);
		canvas.Rect(16, 16, 48, 48 + bar);
		canvas.SetColor(c.dockBg[1]);
		canvas.Rect(17, 17, 47, 47 + bar);
		canvas.SetColor(c.dockBg[0], c.dockBg[1]);
		canvas.Rect(17, 17, 47, 17 + bar);
		canvas.SetColor(c.frameOutline);
		canvas.Rect(18, 16 + bar, 46, 46 + bar);
		canvas.SetColor(c.bgPanel);
		canvas.Rect(19, 17 + bar, 45, 45 + bar);

		d.dockFrame.SetTexture(canvas.CreateTexture());
		d.dockFrame.SetBorderSize(24, 24 + bar, 24, 24);
	}

	// Create hue map
	{
		GxCanvas canvas(256, 256);

		float x = 0, w = 256 / 6;
		for(int i=0; i<6; ++i)
		{
			GxColor a = hueSlice[i];
			GxColor b = hueSlice[i + 1];
			GxColor c = GxColor(128, 128, 128);
			if(i == 5) w = 256 - x;
			canvas.SetColor(a, b, c, c);
			canvas.Rect(x, 0, x+w, 256);
			x += w;
		}

		d.hueMap = canvas.CreateTexture();
	}

	// Create direction disc
	{
		GxCanvas canvas(48, 48);
		canvas.Clear(c.bgPanel.Alpha(0));

		canvas.SetColor(c.frameOutEmboss[0], c.frameOutEmboss[1]);
		canvas.Circle(24, 24, 24);
		canvas.SetColor(c.frameOutline);
		canvas.Circle(24, 24, 23);
		canvas.SetColor(c.bgBar);
		canvas.Circle(24, 24, 22);

		d.dirDisc = canvas.CreateTexture();
	}
}

}; // anonymous namespace

// ===================================================================================
// GxStyle
// ===================================================================================

GxStyle::GxStyle(Scheme scheme)
{
	switch(scheme)
	{
	case BLACK:
		GetBlackScheme(c);
		break;
	case WHITE:
		GetWhiteScheme(c);
		break;
	};
	CreateStyleData(c, d);
}

GxStyle::GxStyle(const Colors& scheme)
{
	c = scheme;
	CreateStyleData(c, d);
}

void GxStyle::SetFont(const GxFont& font)
{
	d.text[0].font = font;
	d.text[1].font = font;
}

void GxStyle::Label(const GxRecti& r, GxTextAlignH h, GxTextAlignV v, const GxString& text, bool lock)
{
	if(r.w > 2 && r.h > 2)
	{
		GxDraw::Get()->PushScissorRect(r.x, r.y, r.w, r.h);
		GxText& settings = d.text[lock ? 1 : 0];

		settings.SetAlign(h, v);
		const int x = (h == GX_TA_LEFT) ? 2 : ((h == GX_TA_CENTER) ? r.w/2 : r.w-2);
		const int y = (v == GX_TA_TOP ) ? 2 : ((v == GX_TA_MIDDLE) ? r.h/2 : r.h-2);
		settings.maxWidth = r.w - 2;
		settings.Draw(r.x + x, r.y + y - 2, text);
		settings.SetMaxWidth();

		GxDraw::Get()->PopScissorRect();
	}
}

void GxStyle::Button(const GxRecti& r, float hl, bool lock)
{
	if(hl == 0.f || lock)
	{
		d.button[0].DrawH(r.x, r.y, r.w, r.h);
	}
	else
	{
		const int a = GxClamp((int)(hl * 255.f), 0, 255);
		d.button[0].DrawH(r.x, r.y, r.w, r.h);
		d.button[1].SetColor(GxColor(255, 255, 255, a));
		d.button[1].DrawH(r.x, r.y, r.w, r.h);
		d.button[1].SetColor(255);
	}
}

void GxStyle::Radio(const GxRecti& r, float hl, bool lock, bool selected)
{
	if(hl == 0.f || lock)
	{
		d.radio[0].Draw(r.x + r.w/2, r.y + r.h/2);
	}
	else
	{
		const int a = GxClamp((int)(hl * 255.f), 0, 255);
		d.radio[0].Draw(r.x + r.w/2, r.y + r.h/2);
		d.radio[1].SetColor(GxColor(255, 255, 255, a));
		d.radio[1].Draw(r.x + r.w/2, r.y + r.h/2);
		d.radio[1].SetColor(255);
	}

	if(selected)
		d.radio[2].Draw(r.x + r.w/2, r.y + r.h/2);
}

void GxStyle::Tab(const GxRecti& r, bool hl, bool active)
{
	int i = active ? 1 : (hl ? 2 : 0);
	d.tab[i].DrawH(r.x, r.y, r.w, r.h);
}

void GxStyle::Bar(const GxRecti& r)
{
	d.bar.DrawH(r.x, r.y, r.w, r.h);
}

void GxStyle::Field(const GxRecti& r)
{
	d.field.DrawH(r.x, r.y, r.w, r.h);
}

void GxStyle::Box(const GxRecti& r)
{
	d.box.DrawH(r.x, r.y, r.w, r.h);
}

}; // namespace style
}; // namespace guix