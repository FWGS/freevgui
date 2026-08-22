// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "panel.h"
#include "vgui_internal.h"
#include "input.h"
#include "surface.h"
#include "signals.h"
#include "app.h"
#include "buildgroup.h"
#include "controls/treefolder.h"
#include "controls/label.h"
#include "layout.h"

using namespace vgui;

Panel::Panel(int x, int y, int w, int h)
{
	init( x, y, w, h );
}

Panel::Panel() : Panel( 0, 0, 64, 64 ) {}

void Panel::setPos( int x, int y )
{
	origin[0] = x;
	origin[1] = y;
}

void Panel::getPos(int &x, int &y)
{
	x = origin[0];
	y = origin[1];
}

void Panel::setSize( int w, int h )
{
	w = Q_max( w, minimumSize[0] );
	h = Q_max( h, minimumSize[1] );
	size[0] = w;
	size[1] = h;
}

void Panel::getSize( int &w, int &h )
{
	w = size[0];
	h = size[1];
}

void Panel::setBounds( int x, int y, int w, int h )
{
	setPos( x, y );
	setSize( w, h );
}

void Panel::getBounds( int &x, int &y, int &w, int &h )
{
	x = origin[0];
	y = origin[1];
	w = size[0];
	h = size[1];
}

int Panel::getWide()
{
	return size[0];
}

int Panel::getTall()
{
	return size[1];
}

Panel *Panel::getParent()
{
	return parent;
}

void Panel::setVisible(bool state)
{
	visible = state;
}

bool Panel::isVisible()
{
	return visible;
}

bool Panel::isVisibleUp()
{
	for( Panel *p = this; p; p = p->getParent() )
	{
		if( p->visible == false )
			return false;
	}
	return true;
}

void Panel::repaint()
{
	pendingRepaint = true;

	if( surfaceBase )
	{
		surfaceBase->pendingSwap = true;
		surfaceBase->invalidate( this );
	}

	for( int i = 0; i < repaintSignals.getCount(); i++ )
		repaintSignals[i]->panelRepainted( this );
}

void Panel::repaintAll()
{
	surfaceBase->getPanel()->repaint();
}

void Panel::getAbsExtents(int &x0, int &y0, int &x1, int &y1)
{
	x0 = screenOrigin[0];
	y0 = screenOrigin[1];
	x1 = x0 + size[0];
	y1 = y0 + size[1];
}

void Panel::getClipRect(int &x0, int &y0, int &x1, int &y1)
{
	x0 = clipRect[0];
	y0 = clipRect[1];
	x1 = clipRect[2];
	y1 = clipRect[3];
}

void Panel::setParent(Panel *newParent)
{
	if( parent )
		parent->removeChild( this );

	if( !newParent )
		return;

	parent = newParent;
	parent->children.putElement( this );
	setSurfaceBaseTraverse( parent->surfaceBase );
}

void Panel::addChild(Panel *child)
{
	if( child->parent )
		child->parent->removeChild( child );

	child->parent = this;
	children.putElement( child );
	child->setSurfaceBaseTraverse( surfaceBase );
}

void Panel::insertChildAt(Panel *child, int i)
{
	if( children.hasElement( child ))
		return;

	if( child->parent )
		child->parent->removeChild( child );

	child->parent = this;
	children.insertElementAt( child, i );
	child->setSurfaceBaseTraverse( surfaceBase );
}

void Panel::removeChild(Panel *child)
{
	child->parent = nullptr;
	child->surfaceBase = nullptr;
	children.removeElement( child );
}

bool Panel::wasMousePressed( MouseCode code )
{
	return getApp()->wasMousePressed( code, this );
}

bool Panel::wasMouseDoublePressed( MouseCode code )
{
	return getApp()->wasMouseDoublePressed( code, this );
}

bool Panel::isMouseDown( MouseCode code )
{
	return getApp()->isMouseDown( code, this );
}

bool Panel::wasMouseReleased( MouseCode code )
{
	return getApp()->wasMouseReleased( code, this );
}

bool Panel::wasKeyPressed( KeyCode code )
{
	return getApp()->wasKeyPressed( code, this );
}

bool Panel::isKeyDown( KeyCode code )
{
	return getApp()->isKeyDown( code, this );
}

bool Panel::wasKeyTyped( KeyCode code )
{
	return getApp()->wasKeyTyped( code, this );
}

bool Panel::wasKeyReleased( KeyCode code )
{
	return getApp()->wasKeyReleased( code, this );
}

void Panel::addInputSignal(InputSignal *is)
{
	inputSignals.putElement( is );
}

void Panel::removeInputSignal(InputSignal *is)
{
	inputSignals.removeElement( is );
}

void Panel::addRepaintSignal(RepaintSignal *rs)
{
	repaintSignals.putElement( rs );
}

void Panel::removeRepaintSignal(RepaintSignal *rs)
{
	repaintSignals.removeElement( rs );
}

bool Panel::isWithin(int x, int y)
{
	screenToLocal( x, y );

	return x >= 0 && x < size[0] && y >= 0 && y < size[1];
}

Panel *Panel::isWithinTraverse(int x, int y)
{
	if( visible && isWithin( x, y ))
	{
		for( int i = children.getCount() - 1; i >= 0; i-- )
		{
			Panel *p = children[i]->isWithinTraverse( x, y );
			if( p )
				return p;
		}

		return this;
	}

	return nullptr;
}

void Panel::localToScreen(int &x, int &y)
{
	x += screenOrigin[0];
	y += screenOrigin[1];
}

void Panel::screenToLocal(int &x, int &y)
{
	x -= screenOrigin[0];
	y -= screenOrigin[1];
}

void Panel::setCursor(Cursor *newCursor)
{
	cursor = newCursor;
	schemeCursor = static_cast<Scheme::SchemeCursor>( Cursor::DC_USER );
}

void Panel::setCursor(Scheme::SchemeCursor scu)
{
	cursor = nullptr;
	schemeCursor = scu;
}

Cursor *Panel::getCursor()
{
	if( getApp()->getCursorOveride() )
		return getApp()->getCursorOveride();

	if( schemeCursor == static_cast<Scheme::SchemeCursor>( Cursor::DC_USER ))
		return cursor;

	return getApp()->getScheme()->getCursor( schemeCursor );
}

void Panel::setMinimumSize(int w, int h)
{
	minimumSize[0] = w;
	minimumSize[1] = h;
}

void Panel::getMinimumSize(int &w, int &h)
{
	w = minimumSize[0];
	h = minimumSize[1];
}

void Panel::requestFocus()
{
	getApp()->requestFocus( this );
}

bool Panel::hasFocus()
{
	return getApp()->getFocus() == this;
}

int Panel::getChildCount()
{
	return children.getCount();
}

Panel *Panel::getChild(int i)
{
	if( i >= 0 && i < children.getCount())
		return children[i];
	return nullptr;
}

void Panel::setLayout(Layout *newLayout)
{
	layout = newLayout;
	invalidateLayout( false );
}

void Panel::invalidateLayout(bool layoutNow)
{
	pendingLayout = true;

	if( layoutNow )
		internalPerformLayout();
}

void Panel::setFocusNavGroup( FocusNavGroup *fng )
{
	focusNavGroup = fng;
	if( fng )
		fng->addPanel( this );
}

void Panel::requestFocusPrev()
{
	if( focusNavGroup )
		focusNavGroup->requestFocusPrev();
}

void Panel::requestFocusNext()
{
	if( focusNavGroup )
		focusNavGroup->requestFocusNext();
}

void Panel::addFocusChangeSignal(FocusChangeSignal *fcs)
{
	focusChangeSignals.putElement( fcs );
}

bool Panel::isAutoFocusNavEnabled()
{
	return autoFocusNavEnabled;
}

void Panel::setAutoFocusNavEnabled(bool enable)
{
	autoFocusNavEnabled = enable;
}

void Panel::setBorder(Border *newBorder)
{
	border = newBorder;
}

void Panel::setPaintBorderEnabled(bool enable)
{
	paintBorderEnabled = enable;
}

void Panel::setPaintBackgroundEnabled(bool enable)
{
	paintBackgroundEnabled = enable;
}

void Panel::setPaintEnabled(bool enable)
{
	paintEnabled = enable;
}

void Panel::getInset(int &left, int &top, int &right, int &bottom)
{
	if( border )
		border->getInset( left, top, right, bottom );
	else
		left = top = right = bottom = 0;
}

void Panel::getPaintSize( int &w, int &h )
{
	if( border )
	{
		int left, top, right, bottom;
		border->getInset( left, top, right, bottom );

		w = size[0] - ( left + right );
		h = size[1] - ( top + bottom );
	}
	else
	{
		w = size[0];
		h = size[1];
	}
}

void Panel::setPreferredSize(int w, int h)
{
	preferredSize[0] = w;
	preferredSize[1] = h;
}

void Panel::getPreferredSize(int &w, int &h)
{
	w = preferredSize[0];
	h = preferredSize[1];
}

SurfaceBase *Panel::getSurfaceBase()
{
	return surfaceBase;
}

bool Panel::isEnabled()
{
	return enabled;
}

void Panel::setEnabled(bool enable)
{
	enabled = enable;
}

void Panel::setBuildGroup( BuildGroup *bg, const char *panelPersistanceName )
{
	buildGroup = bg;
	buildGroup->panelAdded( this, panelPersistanceName );
}

bool Panel::isBuildGroupEnabled()
{
	if( buildGroup )
		return buildGroup->isEnabled();
	return false;
}

void Panel::removeAllChildren()
{
	children.removeAll();
}

void Panel::repaintParent()
{
	if( parent )
		parent->repaint();
}

Panel *Panel::createPropertyPanel()
{
	TreeFolder *root = new TreeFolder( "Properties" );
	TreeFolder *folder = new TreeFolder( "Panel" );

	folder->addChild( new Label( "setPos" ));
	folder->addChild( new Label( "setSize" ));
	folder->addChild( new Label( "setBorder" ));
	folder->addChild( new Label( "setLayout" ));
	root->addChild( folder );

	return root;
}

void Panel::getPersistanceText(char *buf, int len)
{
	int x, y, w, h;
	getBounds( x, y, w, h );
	snprintf( buf, len, "->setBounds(%d, %d, %d, %d);\n", x, y, w, h );
}

void Panel::applyPersistanceText(const char *)
{

}

void Panel::setFgColor(Scheme::SchemeColor sc)
{
	foregroundColor.setColor( sc );
}

void Panel::setBgColor(Scheme::SchemeColor sc)
{
	backgroundColor.setColor( sc );
}

void Panel::setFgColor(int r, int g, int b, int a)
{
	foregroundColor.setColor( r, g, b, a );
}

void Panel::setBgColor(int r, int g, int b, int a)
{
	backgroundColor.setColor( r, g, b, a );
}

void Panel::getFgColor(int &r, int &g, int &b, int &a)
{
	foregroundColor.getColor( r, g, b, a );
}

void Panel::getBgColor(int &r, int &g, int &b, int &a)
{
	backgroundColor.getColor( r, g, b, a );
}

void Panel::setBgColor(Color c)
{
	backgroundColor = c;
}

void Panel::setFgColor(Color c)
{
	foregroundColor = c;
}

void Panel::getBgColor(Color &c)
{
	c = backgroundColor;
}

void Panel::getFgColor(Color &c)
{
	c = foregroundColor;
}

void Panel::setAsMouseCapture( bool set )
{
	getApp()->setMouseCapture( set ? this : nullptr );
}

void Panel::setAsMouseArena( bool )
{
	getApp()->setMouseArena( this );
}

App *Panel::getApp()
{
	return App::getInstance();
}

void Panel::getVirtualSize(int &w, int &h)
{
	getSize( w, h );
}

void Panel::setLayoutInfo(LayoutInfo *li)
{
	layoutInfo = li;
}

LayoutInfo *Panel::getLayoutInfo()
{
	return layoutInfo;
}

bool Panel::isCursorNone()
{
	Cursor *c = getCursor();

	if( c )
		return c->getDefaultCursor( ) == Cursor::DC_NONE;
	return true;
}

void Panel::solveTraverse()
{
	if( visible )
	{
		solve();
		if( pendingLayout )
			internalPerformLayout();
		solve();
		for( int i = 0; i < children.getCount(); i++ )
			children[i]->solveTraverse();
	}
}

void Panel::paintTraverse()
{
	paintTraverse( pendingRepaint );
}

void Panel::setSurfaceBaseTraverse(SurfaceBase *sb)
{
	surfaceBase = sb;

	for( int i = 0; i < children.getCount(); i++ )
		children[i]->setSurfaceBaseTraverse( sb );
}

void Panel::performLayout()
{
}

void Panel::internalPerformLayout()
{
	pendingLayout = false;

	layout ? layout->performLayout( this ) : performLayout();

	repaint();
}

void Panel::drawSetColor( Scheme::SchemeColor sc )
{
	int r, g, b, a;
	getApp()->getScheme()->getColor( sc, r, g, b, a );
	drawSetColor( r, g, b, a );
}

void Panel::drawSetColor( int r, int g, int b, int a )
{
	surfaceBase->drawSetColor( r, g, b, a );
}

void Panel::drawFilledRect( int x0, int y0, int x1, int y1 )
{
	surfaceBase->drawFilledRect( x0, y0, x1, y1 );
}

void Panel::drawOutlinedRect( int x0, int y0, int x1, int y1 )
{
	surfaceBase->drawOutlinedRect( x0, y0, x1, y1 );
}

void Panel::drawSetTextFont( Scheme::SchemeFont sf )
{
	drawSetTextFont( getApp()->getScheme()->getFont( sf ));
}

void Panel::drawSetTextFont( Font *f )
{
	surfaceBase->drawSetTextFont( f );
}

void Panel::drawSetTextColor( Scheme::SchemeColor sc )
{
	int r, g, b, a;
	getApp()->getScheme()->getColor( sc, r, g, b, a );
	drawSetTextColor( r, g, b, a );
}

void Panel::drawSetTextColor( int r, int g, int b, int a )
{
	surfaceBase->drawSetTextColor( r, g, b, a );
}

void Panel::drawSetTextPos( int x, int y )
{
	surfaceBase->drawSetTextPos( x, y );
}

void Panel::drawPrintText( const char *str, int len )
{
	surfaceBase->drawPrintText( str, len );
}

void Panel::drawPrintText( int x, int y, const char *str, int len )
{
	surfaceBase->drawSetTextPos( x, y );
	surfaceBase->drawPrintText( str, len );
}

void Panel::drawPrintChar( char ch )
{
	surfaceBase->drawPrintText( &ch, 1 );
}

void Panel::drawPrintChar( int x, int y, char ch )
{
	surfaceBase->drawSetTextPos( x, y );
	surfaceBase->drawPrintText( &ch, 1 );
}

void Panel::drawSetTextureRGBA( int id, const char *rgba, int w, int h )
{
	if( surfaceBase )
		surfaceBase->drawSetTextureRGBA( id, rgba, w, h );
}

void Panel::drawSetTexture( int id )
{
	surfaceBase->drawSetTexture( id );
}

void Panel::drawTexturedRect( int x0, int y0, int x1, int y1 )
{
	surfaceBase->drawTexturedRect( x0, y0, x1, y1 );
}

void Panel::solve()
{
	screenOrigin[0] = origin[0];
	screenOrigin[1] = origin[1];

	if( parent )
	{
		int inset[4];
		parent->getInset( inset[0], inset[1], inset[2], inset[3] );

		screenOrigin[0] = screenOrigin[0] + parent->screenOrigin[0];
		screenOrigin[1] = screenOrigin[1] + parent->screenOrigin[1];
		screenOrigin[0] = screenOrigin[0] + inset[0];
		screenOrigin[1] = screenOrigin[1] + inset[1];
	}

	clipRect[0] = screenOrigin[0];
	clipRect[1] = screenOrigin[1];
	clipRect[2] = screenOrigin[0] + size[0];
	clipRect[3] = screenOrigin[1] + size[1];

	if( parent )
	{
		clipRect[0] = Q_max( clipRect[0], parent->clipRect[0] );
		clipRect[1] = Q_max( clipRect[1], parent->clipRect[1] );

		clipRect[2] = Q_min( clipRect[2], parent->clipRect[2] );
		clipRect[3] = Q_min( clipRect[3], parent->clipRect[3] );
	}
}

void Panel::paintTraverse( bool repaint )
{
	if( visible )
	{
		if( pendingRepaint )
			repaint = true;

		pendingRepaint = false;

		if( clipRect[2] <= clipRect[0] || clipRect[3] <= clipRect[1] )
			repaint = false;

		if( repaint && ( paintBorderEnabled || paintBackgroundEnabled || paintEnabled ))
		{
			surfaceBase->pushMakeCurrent( this, true );

			if( border && paintBorderEnabled )
				border->doPaint( this );

			if( paintBackgroundEnabled )
				paintBackground();

			if( paintEnabled )
				paint();

			surfaceBase->popMakeCurrent( this );
		}

		for( int i = 0; i < children.getCount(); i++ )
		{
			children[i]->paintTraverse( repaint );
		}

		if( repaint )
		{
			if( isBuildGroupEnabled() && hasFocus() )
			{
				surfaceBase->pushMakeCurrent( this, false );
				paintBuildOverlay();
				surfaceBase->popMakeCurrent( this );
			}
		}

		if( surfaceBase->pendingSwap && surfaceBase->getPanel() == this )
		{
			surfaceBase->pendingSwap = false;
			surfaceBase->swapBuffers();
		}
	}
}

void Panel::paintBackground()
{
	int r, g, b, a, w, h;

	getPaintSize( w, h );
	getBgColor( r, g, b, a );
	drawSetColor( r, g, b, a );
	drawFilledRect( 0, 0, w, h );
}

void Panel::paint()
{

}

void Panel::paintBuildOverlay()
{
	int w, h;

	getSize( w, h );
	drawSetColor( Scheme::SC_BLACK );
	drawFilledRect( 0,     0,     w, 2     );
	drawFilledRect( 0,     h - 2, w, h     );
	drawFilledRect( 0,     2,     2, h - 2 );
	drawFilledRect( w - 2, 2,     w, h - 2 );
}

void Panel::internalCursorMoved( int x, int y )
{
	if( !isCursorNone( ))
	{
		if( isBuildGroupEnabled( ))
		{
			buildGroup->cursorMoved( x, y, this );
			return;
		}

		screenToLocal( x, y );

		for( int i = 0; i < inputSignals.getCount(); i++ )
			inputSignals[i]->cursorMoved( x, y, this );
	}
}

void Panel::internalCursorEntered()
{
	if( !isCursorNone( ))
	{
		if( isBuildGroupEnabled( ))
			return;

		for( int i = 0; i < inputSignals.getCount(); i++ )
			inputSignals[i]->cursorEntered( this );
	}
}

void Panel::internalCursorExited()
{
	if( !isCursorNone( ))
	{
		if( isBuildGroupEnabled( ))
			return;

		for( int i = 0; i < inputSignals.getCount(); i++ )
			inputSignals[i]->cursorExited( this );
	}
}

void Panel::internalMousePressed( MouseCode code )
{
	if( !isCursorNone( ))
	{
		if( isBuildGroupEnabled( ))
		{
			buildGroup->mousePressed( code, this );
			return;
		}

		for( int i = 0; i < inputSignals.getCount(); i++ )
			inputSignals[i]->mousePressed( code, this );
	}
}

void Panel::internalMouseDoublePressed( MouseCode code )
{
	if( !isCursorNone( ))
	{
		if( isBuildGroupEnabled( ))
		{
			buildGroup->mouseDoublePressed( code, this );
			return;
		}

		for( int i = 0; i < inputSignals.getCount(); i++ )
			inputSignals[i]->mouseDoublePressed( code, this );
	}
}

void Panel::internalMouseReleased( MouseCode code )
{
	if( !isCursorNone( ))
	{
		if( isBuildGroupEnabled( ))
		{
			buildGroup->mouseReleased( code, this );
			return;
		}

		for( int i = 0; i < inputSignals.getCount(); i++ )
			inputSignals[i]->mouseReleased( code, this );
	}
}

void Panel::internalMouseWheeled( int y )
{
	if( !isBuildGroupEnabled( ))
	{
		for( int i = 0; i < inputSignals.getCount(); i++ )
			inputSignals[i]->mouseWheeled( y, this );
	}
}

void Panel::internalKeyPressed( KeyCode code )
{
	if( !isBuildGroupEnabled( ))
	{
		for( int i = 0; i < inputSignals.getCount(); i++ )
			inputSignals[i]->keyPressed( code, this );
	}
}

void Panel::internalKeyTyped( KeyCode code )
{
	if( autoFocusNavEnabled && focusNavGroup )
	{
		switch( code )
		{
		case KEY_TAB:
			if( isKeyDown( KEY_LSHIFT ) || isKeyDown( KEY_RSHIFT ))
				focusNavGroup->requestFocusPrev();
			else
				focusNavGroup->requestFocusNext();
			break;
		case KEY_UP:
			focusNavGroup->requestFocusPrev();
			break;
		case KEY_DOWN:
			focusNavGroup->requestFocusNext();
			break;
		default:
			break;
		}
	}

	if( !isBuildGroupEnabled( ))
	{
		for( int i = 0; i < inputSignals.getCount(); i++ )
			inputSignals[i]->keyTyped( code, this );
	}
}

void Panel::internalKeyReleased( KeyCode code )
{
	if( !isBuildGroupEnabled( ))
	{
		for( int i = 0; i < inputSignals.getCount(); i++ )
			inputSignals[i]->keyReleased( code, this );
	}
}

void Panel::internalKeyFocusTicked()
{
	if( !isBuildGroupEnabled( ))
	{
		for( int i = 0; i < inputSignals.getCount(); i++ )
			inputSignals[i]->keyFocusTicked( this );
	}

}

void Panel::internalFocusChanged( bool lost )
{
	if( !lost && focusNavGroup )
		focusNavGroup->setCurrentPanel( this );

	for( int i = 0; i < focusChangeSignals.getCount(); i++ )
		focusChangeSignals[i]->focusChanged( lost, this );
}

void Panel::internalSetCursor()
{
	if( isBuildGroupEnabled())
		buildGroup->getCursor( this );

	if( surfaceBase )
		surfaceBase->setCursor( getCursor( ));
}

void Panel::init( int x, int y, int w, int h )
{
	screenOrigin[0] = 0;
	screenOrigin[1] = 0;
	origin[0] = x;
	origin[1] = y;
	size[0] = w;
	size[1] = h;
	pendingRepaint = false;
	parent = nullptr;
	surfaceBase = nullptr;
	visible = true;
	minimumSize[0] = 0;
	minimumSize[1] = 0;
	cursor = nullptr;
	schemeCursor = static_cast<Scheme::SchemeCursor>( Cursor::DC_ARROW );
	border = nullptr;
	buildGroup = nullptr;
	layoutInfo = nullptr;
	layout = nullptr;
	pendingLayout = true;
	focusNavGroup = nullptr;
	enabled = true;
	paintBorderEnabled = true;
	paintBackgroundEnabled = true;
	paintEnabled = true;
	setFgColor( Scheme::SC_BLACK );
	setBgColor( Scheme::SC_SECONDARY3 );
	setAutoFocusNavEnabled( true );
}

FocusNavGroup::FocusNavGroup() : index( 0 ) {}

void FocusNavGroup::addPanel(Panel *panel)
{
	panels.putElement( panel );
}

void FocusNavGroup::requestFocusPrev()
{
	if( panels.getCount() == 0 )
		return;

	index--;
	if( index < 0 )
		index = panels.getCount() - 1;

	panels[index]->requestFocus();
}

void FocusNavGroup::requestFocusNext()
{
	if( panels.getCount() == 0 )
		return;

	index++;
	if( index >= panels.getCount() )
		index = 0;

	panels[index]->requestFocus();

}

void FocusNavGroup::setCurrentPanel(Panel *panel)
{
	for( int i = 0; i < panels.getCount(); i++ )
	{
		if( panels[i] == panel )
		{
			index = i;
			return;
		}
	}
}
