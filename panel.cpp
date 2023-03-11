// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "panel.h"
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

void Panel::setPos(int x, int y)
{
	Vector2Set( _pos, x, y );
}

void Panel::getPos(int &x, int &y)
{
	x = _pos[0];
	y = _pos[1];
}

void Panel::setSize(int w, int h)
{
	w = Q_max( w, _minimumSize[0] );
	h = Q_max( h, _minimumSize[1] );
	Vector2Set( _size, w, h );
}

void Panel::getSize(int &w, int &h)
{
	w = _size[0];
	h = _size[1];
}

void Panel::setBounds(int x, int y, int w, int h)
{
	setPos( x, y );
	setSize( w, h );
}

void Panel::getBounds(int &x, int &y, int &w, int &h)
{
	x = _pos[0];
	y = _pos[1];
	w = _size[0];
	h = _size[1];
}

int Panel::getWide()
{
	return _size[0];
}

int Panel::getTall()
{
	return _size[1];
}

Panel *Panel::getParent()
{
	return _parent;
}

void Panel::setVisible(bool state)
{
	_visible = state;
}

bool Panel::isVisible()
{
	return _visible;
}

bool Panel::isVisibleUp()
{
	for( Panel *parent = this; parent; parent = parent->getParent() )
	{
		if( parent->_visible == false )
			return false;
	}
	return true;
}

void Panel::repaint()
{
	_needsRepaint = true;

	if( _surfaceBase )
	{
		_surfaceBase->_needsSwap = true;
		_surfaceBase->invalidate( this );
	}

	for( int i = 0; i < _repaintSignalDar.getCount(); i++ )
		_repaintSignalDar[i]->panelRepainted( this );
}

void Panel::repaintAll()
{
	_surfaceBase->getPanel()->repaint();
}

void Panel::getAbsExtents(int &x0, int &y0, int &x1, int &y1)
{
	x0 = _loc[0];
	y0 = _loc[1];
	x1 = x0 + _size[0];
	y1 = y0 + _size[1];
}

void Panel::getClipRect(int &x0, int &y0, int &x1, int &y1)
{
	x0 = _clipRect[0];
	y0 = _clipRect[1];
	x1 = _clipRect[2];
	y1 = _clipRect[3];
}

void Panel::setParent(Panel *newParent)
{
	if( _parent )
		_parent->removeChild( this );

	if( !newParent )
		return;

	_parent = newParent;
	_parent->_childDar.putElement( this );
	setSurfaceBaseTraverse( _parent->_surfaceBase );
}

void Panel::addChild(Panel *child)
{
	if( child->_parent )
		child->_parent->removeChild( child );

	child->_parent = this;
	_childDar.putElement( child );
	child->setSurfaceBaseTraverse( _surfaceBase );
}

void Panel::insertChildAt(Panel *child, int i)
{
	if( _childDar.hasElement( child ))
		return;

	if( child->_parent )
		child->_parent->removeChild( child );

	child->_parent = this;
	_childDar.insertElementAt( child, i );
	child->setSurfaceBaseTraverse( _surfaceBase );
}

void Panel::removeChild(Panel *child)
{
	child->_parent = nullptr;
	child->_surfaceBase = nullptr;
	_childDar.removeElement( child );
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
	_inputSignalDar.putElement( is );
}

void Panel::removeInputSignal(InputSignal *is)
{
	_inputSignalDar.removeElement( is );
}

void Panel::addRepaintSignal(RepaintSignal *rs)
{
	_repaintSignalDar.putElement( rs );
}

void Panel::removeRepaintSignal(RepaintSignal *rs)
{
	_repaintSignalDar.removeElement( rs );
}

bool Panel::isWithin(int x, int y)
{
	screenToLocal( x, y );

	return x >= 0 && x < _size[0] && y > 0 && y < _size[1];
}

Panel *Panel::isWithinTraverse(int x, int y)
{
	if( _visible && isWithin( x, y ))
	{
		for( int i = 0; i < _childDar.getCount(); i++ )
		{
			Panel *p = _childDar[i]->isWithinTraverse( x, y );
			if( p )
				return p;
		}
	}

	return nullptr;
}

void Panel::localToScreen(int &x, int &y)
{
	x += _loc[0];
	y += _loc[1];
}

void Panel::screenToLocal(int &x, int &y)
{
	x -= _loc[0];
	y -= _loc[1];
}

void Panel::setCursor(Cursor *cursor)
{
	_cursor = cursor;
	_schemeCursor = static_cast<Scheme::SchemeCursor>( Cursor::DC_USER );
}

void Panel::setCursor(Scheme::SchemeCursor scu)
{
	_cursor = nullptr;
	_schemeCursor = scu;
}

Cursor *Panel::getCursor()
{
	if( getApp()->getCursorOveride() )
		return getApp()->getCursorOveride();

	if( _schemeCursor == static_cast<Scheme::SchemeCursor>( Cursor::DC_USER ))
		return _cursor;

	return getApp()->getScheme()->getCursor( _schemeCursor );
}

void Panel::setMinimumSize(int w, int h)
{
	Vector2Set( _minimumSize, w, h );
}

void Panel::getMinimumSize(int &w, int &h)
{
	w = _minimumSize[0];
	h = _minimumSize[1];
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
	return _childDar.getCount();
}

Panel *Panel::getChild(int i)
{
	if( i >= 0 && i < _childDar.getCount())
		return _childDar[i];
	return nullptr;
}

void Panel::setLayout(Layout *layout)
{
	_layout = layout;
	invalidateLayout( false );
}

void Panel::invalidateLayout(bool layoutNow)
{
	_needsLayout = true;

	if( layoutNow )
		internalPerformLayout();
}

void Panel::setFocusNavGroup( FocusNavGroup *fng )
{
	_focusNavGroup = fng;
	if( fng )
		fng->addPanel( this );
}

void Panel::requestFocusPrev()
{
	if( _focusNavGroup )
		_focusNavGroup->requestFocusPrev();
}

void Panel::requestFocusNext()
{
	if( _focusNavGroup )
		_focusNavGroup->requestFocusNext();
}

void Panel::addFocusChangeSignal(FocusChangeSignal *fcs)
{
	_focusChangeSignalDar.putElement( fcs );
}

bool Panel::isAutoFocusNavEnabled()
{
	return _autoFocusNavEnabled;
}

void Panel::setAutoFocusNavEnabled(bool enable)
{
	_autoFocusNavEnabled = enable;
}

void Panel::setBorder(Border *border)
{
	_border = border;
}

void Panel::setPaintBorderEnabled(bool enable)
{
	_paintBorderEnabled = enable;
}

void Panel::setPaintBackgroundEnabled(bool enable)
{
	_paintBackgroundEnabled = enable;
}

void Panel::setPaintEnabled(bool enable)
{
	_paintEnabled = enable;
}

void Panel::getInset(int &left, int &top, int &right, int &bottom)
{
	if( _border )
		_border->getInset( left, top, right, bottom );
	else
		left = top = right = bottom = 0;
}

void Panel::getPaintSize(int &w, int &h)
{
	if( _border )
	{
		int left, top, right, bottom;
		_border->getInset( left, top, right, bottom );

		w = _size[0] - ( left + right );
		h = _size[1] - ( top + bottom );
	}
	else
	{
		w = _size[0];
		h = _size[1];
	}
}

void Panel::setPreferredSize(int w, int h)
{
	Vector2Set( _preferredSize, w, h );
}

void Panel::getPreferredSize(int &w, int &h)
{
	w = _preferredSize[0];
	h = _preferredSize[1];
}

SurfaceBase *Panel::getSurfaceBase()
{
	return _surfaceBase;
}

bool Panel::isEnabled()
{
	return _enabled;
}

void Panel::setEnabled(bool enable)
{
	_enabled = enable;
}

void Panel::setBuildGroup( BuildGroup *bg, const char *panelPersistanceName )
{
	_buildGroup = bg;
	_buildGroup->panelAdded( this, panelPersistanceName );
}

bool Panel::isBuildGroupEnabled()
{
	if( _buildGroup )
		return _buildGroup->isEnabled();
	return false;
}

void Panel::removeAllChildren()
{
	_childDar.removeAll();
}

void Panel::repaintParent()
{
	if( _parent )
		_parent->repaint();
}

Panel *Panel::createPropertyPanel()
{
#if 0
	TreeFolder *root = new TreeFolder( "Properties" );
	TreeFolder *folder = new TreeFolder( "Panel" );

	folder->addChild( new Label( "setPos" ));
	folder->addChild( new Label( "setSize" ));
	folder->addChild( new Label( "setBorder" ));
	folder->addChild( new Label( "setLayout" ));
	root->addChild( folder );

	return root;
#else
	vgui_printf( "%s: UNDONE\n", PRETTY_FUNCTION );
	return NULL;
#endif
}

void Panel::getPersistanceText(char *buf, int len)
{
	int x, y, w, h;
	getBounds( x, y, w, h );
	Q_snprintf( buf, len, "->setBounds(%d, %d, %d, %d);\n", x, y, w, h );
}

void Panel::applyPersistanceText(const char *)
{

}

void Panel::setFgColor(Scheme::SchemeColor sc)
{
	_fgColor.setColor( sc );
}

void Panel::setBgColor(Scheme::SchemeColor sc)
{
	_bgColor.setColor( sc );
}

void Panel::setFgColor(int r, int g, int b, int a)
{
	_fgColor.setColor( r, g, b, a );
}

void Panel::setBgColor(int r, int g, int b, int a)
{
	_bgColor.setColor( r, g, b, a );
}

void Panel::getFgColor(int &r, int &g, int &b, int &a)
{
	_fgColor.getColor( r, g, b, a );
}

void Panel::getBgColor(int &r, int &g, int &b, int &a)
{
	_bgColor.getColor( r, g, b, a );
}

void Panel::setBgColor(Color c)
{
	_fgColor = c;
}

void Panel::setFgColor(Color c)
{
	_bgColor = c;
}

void Panel::getBgColor(Color &c)
{
	c = _fgColor;
}

void Panel::getFgColor(Color &c)
{
	c = _bgColor;
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
	_layoutInfo = li;
}

LayoutInfo *Panel::getLayoutInfo()
{
	return _layoutInfo;
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
	if( _visible )
	{
		solve();
		if( _needsLayout )
			internalPerformLayout();
		solve();
		for( int i = 0; i < _childDar.getCount(); i++ )
			_childDar[i]->solveTraverse();
	}
}

void Panel::paintTraverse()
{
	paintTraverse( _needsRepaint );
}

void Panel::setSurfaceBaseTraverse(SurfaceBase *sb)
{
	_surfaceBase = sb;

	for( int i = 0; i < _childDar.getCount(); i++ )
		_childDar[i]->setSurfaceBaseTraverse( sb );
}

void Panel::performLayout()
{
}

void Panel::internalPerformLayout()
{
	_needsLayout = false;

	_layout ? _layout->performLayout( this ) : performLayout();

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
	_surfaceBase->drawSetColor( r, g, b, a );
}

void Panel::drawFilledRect( int x0, int y0, int x1, int y1 )
{
	_surfaceBase->drawFilledRect( x0, y0, x1, y1 );
}

void Panel::drawOutlinedRect( int x0, int y0, int x1, int y1 )
{
	_surfaceBase->drawFilledRect( x0, y0, x1, y1 );
}

void Panel::drawSetTextFont( Scheme::SchemeFont sf )
{
	drawSetTextFont( getApp()->getScheme()->getFont( sf ));
}

void Panel::drawSetTextFont( Font *f )
{
	_surfaceBase->drawSetTextFont( f );
}

void Panel::drawSetTextColor( Scheme::SchemeColor sc )
{
	int r, g, b, a;
	getApp()->getScheme()->getColor( sc, r, g, b, a );
	drawSetTextColor( r, g, b, a );
}

void Panel::drawSetTextColor( int r, int g, int b, int a )
{
	_surfaceBase->drawSetTextColor( r, g, b, a );
}

void Panel::drawSetTextPos( int x, int y )
{
	_surfaceBase->drawSetTextPos( x, y );
}

void Panel::drawPrintText( const char *str, int len )
{
	_surfaceBase->drawPrintText( str, len );
}

void Panel::drawPrintText( int x, int y, const char *str, int len )
{
	_surfaceBase->drawSetTextPos( x, y );
	_surfaceBase->drawPrintText( str, len );
}

void Panel::drawPrintChar( char ch )
{
	_surfaceBase->drawPrintText( &ch, 1 );
}

void Panel::drawPrintChar( int x, int y, char ch )
{
	_surfaceBase->drawSetTextPos( x, y );
	_surfaceBase->drawPrintText( &ch, 1 );
}

void Panel::drawSetTextureRGBA( int id, const char *rgba, int w, int h )
{
	if( _surfaceBase )
		_surfaceBase->drawSetTextureRGBA( id, rgba, w, h );
}

void Panel::drawSetTexture( int id )
{
	_surfaceBase->drawSetTexture( id );
}

void Panel::drawTexturedRect( int x0, int y0, int x1, int y1 )
{
	_surfaceBase->drawTexturedRect( x0, y0, x1, y1 );
}

void Panel::solve()
{
	Vector2Copy( _pos, _loc );

	if( _parent )
	{
		int inset[4];
		_parent->getInset( inset[0], inset[1], inset[2], inset[3] );

		Vector2Add( _loc, _parent->_loc, _loc );
		Vector2Add( _loc, inset, _loc );
	}

	Vector2Copy( _loc, _clipRect );
	Vector2Add( _loc, _size, &_clipRect[2] );

	if( _parent )
	{
		_clipRect[0] = Q_max( _clipRect[0], _parent->_clipRect[0] );
		_clipRect[1] = Q_max( _clipRect[1], _parent->_clipRect[1] );

		_clipRect[2] = Q_min( _clipRect[2], _parent->_clipRect[2] );
		_clipRect[3] = Q_min( _clipRect[3], _parent->_clipRect[3] );
	}
}

void Panel::paintTraverse( bool repaint )
{
	if( _visible )
	{
		if( _needsRepaint )
			repaint = true;

		_needsRepaint = false;

		if( _clipRect[2] <= _clipRect[0] || _clipRect[3] <= _clipRect[1] )
			repaint = false;

		if( repaint && ( _paintBorderEnabled || _paintBackgroundEnabled || _paintEnabled ))
		{
			_surfaceBase->pushMakeCurrent( this, true );

			if( _border && _paintBorderEnabled )
				_border->doPaint( this );

			if( _paintBackgroundEnabled )
				paintBackground();

			if( _paintEnabled )
				paint();

			_surfaceBase->popMakeCurrent( this );
		}

		for( int i = 0; i < _childDar.getCount(); i++ )
		{
			_childDar[i]->paintTraverse( repaint );
		}

		if( repaint )
		{
			if( isBuildGroupEnabled() && hasFocus() )
			{
				_surfaceBase->pushMakeCurrent( this, false );
				paintBuildOverlay();
				_surfaceBase->popMakeCurrent( this );
			}
		}

		if( _surfaceBase->_needsSwap && _surfaceBase->getPanel() == this )
		{
			_surfaceBase->_needsSwap = false;
			_surfaceBase->swapBuffers();
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
	drawFilledRect( h - 2, 2,     w, h - 2 );
}

void Panel::internalCursorMoved( int x, int y )
{
	if( !isCursorNone( ))
	{
		if( isBuildGroupEnabled( ))
		{
			_buildGroup->cursorMoved( x, y, this );
			return;
		}

		screenToLocal( x, y );

		for( int i = 0; i < _inputSignalDar.getCount(); i++ )
			_inputSignalDar[i]->cursorMoved( x, y, this );
	}
}

void Panel::internalCursorEntered()
{
	if( !isCursorNone( ))
	{
		if( isBuildGroupEnabled( ))
			return;

		for( int i = 0; i < _inputSignalDar.getCount(); i++ )
			_inputSignalDar[i]->cursorEntered( this );
	}
}

void Panel::internalCursorExited()
{
	if( !isCursorNone( ))
	{
		if( isBuildGroupEnabled( ))
			return;

		for( int i = 0; i < _inputSignalDar.getCount(); i++ )
			_inputSignalDar[i]->cursorExited( this );
	}
}

void Panel::internalMousePressed( MouseCode code )
{
	if( !isCursorNone( ))
	{
		if( isBuildGroupEnabled( ))
		{
			_buildGroup->mousePressed( code, this );
			return;
		}

		for( int i = 0; i < _inputSignalDar.getCount(); i++ )
			_inputSignalDar[i]->mousePressed( code, this );
	}
}

void Panel::internalMouseDoublePressed( MouseCode code )
{
	if( !isCursorNone( ))
	{
		if( isBuildGroupEnabled( ))
		{
			_buildGroup->mouseDoublePressed( code, this );
			return;
		}

		for( int i = 0; i < _inputSignalDar.getCount(); i++ )
			_inputSignalDar[i]->mouseDoublePressed( code, this );
	}
}

void Panel::internalMouseReleased( MouseCode code )
{
	if( !isCursorNone( ))
	{
		if( isBuildGroupEnabled( ))
		{
			_buildGroup->mouseReleased( code, this );
			return;
		}

		for( int i = 0; i < _inputSignalDar.getCount(); i++ )
			_inputSignalDar[i]->mouseReleased( code, this );
	}
}

void Panel::internalMouseWheeled( int y )
{
	if( !isBuildGroupEnabled( ))
	{
		for( int i = 0; i < _inputSignalDar.getCount(); i++ )
			_inputSignalDar[i]->mouseWheeled( y, this );
	}
}

void Panel::internalKeyPressed( KeyCode code )
{
	if( !isBuildGroupEnabled( ))
	{
		for( int i = 0; i < _inputSignalDar.getCount(); i++ )
			_inputSignalDar[i]->keyPressed( code, this );
	}
}

void Panel::internalKeyTyped( KeyCode code )
{
	if( !isBuildGroupEnabled( ))
	{
		for( int i = 0; i < _inputSignalDar.getCount(); i++ )
			_inputSignalDar[i]->keyTyped( code, this );
	}
}

void Panel::internalKeyReleased( KeyCode code )
{
	if( !isBuildGroupEnabled( ))
	{
		for( int i = 0; i < _inputSignalDar.getCount(); i++ )
			_inputSignalDar[i]->keyReleased( code, this );
	}
}

void Panel::internalKeyFocusTicked()
{
	if( !isBuildGroupEnabled( ))
	{
		for( int i = 0; i < _inputSignalDar.getCount(); i++ )
			_inputSignalDar[i]->keyFocusTicked( this );
	}

}

void Panel::internalFocusChanged( bool lost )
{
	if( !lost && _focusNavGroup )
		_focusNavGroup->setCurrentPanel( this );

	for( int i = 0; i < _focusChangeSignalDar.getCount(); i++ )
		_focusChangeSignalDar[i]->focusChanged( lost, this );
}

void Panel::internalSetCursor()
{
	if( isBuildGroupEnabled())
		_buildGroup->getCursor( this );

	if( _surfaceBase )
		_surfaceBase->setCursor( getCursor( ));
}

void Panel::init( int x, int y, int w, int h )
{
	Vector2Set( _loc, 0, 0 );
	Vector2Set( _pos, x, y );
	Vector2Set( _size, w, h );
	_needsRepaint = false;
	_parent = nullptr;
	_surfaceBase = nullptr;
	_visible = true;
	Vector2Set( _minimumSize, 0, 0 );
	_cursor = nullptr;
	_schemeCursor = static_cast<Scheme::SchemeCursor>( Cursor::DC_ARROW );
	_border = nullptr;
	_buildGroup = nullptr;
	_layoutInfo = nullptr;
	_layout = nullptr;
	_needsLayout = true;
	_focusNavGroup = nullptr;
	_enabled = true;
	_paintBorderEnabled = true;
	_paintBackgroundEnabled = true;
	_paintEnabled = true;
	setFgColor( Scheme::SC_BLACK );
	setBgColor( Scheme::SC_SECONDARY3 );
	setAutoFocusNavEnabled( true );
}

FocusNavGroup::FocusNavGroup() : _currentIndex( 0 ) {}

void FocusNavGroup::addPanel(Panel *panel)
{
	_panelDar.putElement( panel );
}

void FocusNavGroup::requestFocusPrev()
{
	if( _panelDar.getCount() == 0 )
		return;

	_currentIndex--;
	if( _currentIndex < 0 )
		_currentIndex = _panelDar.getCount() - 1;

	_panelDar[_currentIndex]->requestFocus();
}

void FocusNavGroup::requestFocusNext()
{
	if( _panelDar.getCount() == 0 )
		return;

	_currentIndex++;
	if( _currentIndex >= _panelDar.getCount() )
		_currentIndex = 0;

	_panelDar[_currentIndex]->requestFocus();

}

void FocusNavGroup::setCurrentPanel(Panel *panel)
{
	for( int i = 0; i < _panelDar.getCount(); i++ )
	{
		if( _panelDar[i] == panel )
		{
			_currentIndex = i;
			return;
		}
	}
}
