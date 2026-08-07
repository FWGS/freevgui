// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#ifndef VGUI_LABEL_H
#define VGUI_LABEL_H

#include "panel.h"
#include "signals.h"
#include "image.h"

namespace vgui
{
class CLASSEXPORT Label : public Panel
{
public:
	enum Alignment {
		TOPLEFT = 0, TOP,    TOPRIGHT,
		LEFT,        CENTER, RIGHT,
		BOTTOMLEFT,  BOTTOM, BOTTOMRIGHT
	};

	Label( const char *str );
	Label( const char *str, int x, int y );
	Label( const char *str, int x, int y, int w, int h );
	Label( int len, const char *str, int x, int y, int w, int h );
	virtual void setImage( Image *image );
	virtual void setText( int len, const char *str );
	virtual void setText( const char *str, ... );
	virtual void setFont( Scheme::SchemeFont sf );
	virtual void setFont( Font *f );
	virtual void getTextSize( int &w, int &h );
	virtual void getContentSize( int &w, int &h );
	virtual void setTextAlignment( Alignment alignment );
	virtual void setContentAlignment( Alignment alignment );
	virtual Panel *createPropertyPanel() override;
	virtual void setFgColor( int r, int g, int b, int a ) override;
	virtual void setFgColor( Scheme::SchemeColor scheme_color ) override;
	virtual void setContentFitted( bool fit );

protected:
	virtual void computeAlignment(
			int &tx0, int &ty0, int &tx1, int &ty1,
			int &ix0, int &iy0, int &ix1, int &iy1,
			int &minX, int &minY, int &maxX, int &maxY );

	virtual void paint() override;

	virtual void recomputeMinimumSize();

	bool _textEnabled, _imageEnabled, _contentFitted;
	Alignment _textAlignment, _contentAlignment;
	TextImage *_textImage;
	Image *_image;

private:
	void init( int len, const char *str, bool textFitted );
};
CHECK_STRUCT_SIZE( Label, 208 );

class CLASSEXPORT IntLabel : public Label, public IntChangeSignal
{
public:
	IntLabel( int value, int x, int y, int wide, int tall );
	virtual void setValue( int value );
	virtual void intChanged( int value, Panel *p ) override;
protected:
	virtual void paintBackground() override;

	int _value;
};
CHECK_STRUCT_SIZE( IntLabel, 216 );
}

#endif // VGUI_LABEL_H
