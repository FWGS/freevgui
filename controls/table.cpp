// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "table.h"
#include "vgui_internal.h"
#include "signals.h"

#define MAKE_COLUMN_EXTENTS( a, b ) (( b ) + (( a ) << 12))
#define UNPACK_COLUMN_EXTENTS( data, a, b ) (( a ) = ( data	) >> 12 & 0xfff, ( b ) = ( data ) & 0xfff )

namespace vgui {
class FooVGuiTablePanelHandler : public ChangeSignal, public InputSignal, public RepaintSignal
{
public:
	FooVGuiTablePanelHandler( TablePanel *table ) : _table( table ) { }

	virtual void valueChanged( Panel *p ) override
	{
		HeaderPanel *hp = dynamic_cast<HeaderPanel *>( p );

		if( hp )
		{
			int count = _table->getColumnCount();

			for( int i = 0; i < hp->getSectionCount() && i < count; i++ )
			{
				int x, y;
				hp->getSectionExtents( i, x, y );
				_table->setColumnExtents( i, x, y );
			}
		}
	}

	virtual void cursorMoved( int, int, Panel * ) override {}
	virtual void cursorEntered( Panel * ) override {}
	virtual void cursorExited( Panel * ) override {}

	virtual void mousePressed( MouseCode code, Panel *p ) override
	{
		_table->privateMousePressed( code, p );
	}

	virtual void mouseDoublePressed( MouseCode code, Panel *p ) override
	{
		_table->privateMouseDoublePressed( code, p );
	}

	virtual void mouseReleased( MouseCode, Panel * ) override {}
	virtual void mouseWheeled( int, Panel * ) override {}
	virtual void keyPressed( KeyCode, Panel * ) override {}

	virtual void keyTyped( KeyCode code, Panel *p ) override
	{
		_table->privateKeyTyped( code, p );
	}

	virtual void keyReleased( KeyCode, Panel * ) override {}
	virtual void keyFocusTicked( Panel * ) override {}

	virtual void panelRepainted( Panel * ) override
	{
		_table->repaint();
	}

private:
	TablePanel *_table;
};

TablePanel::TablePanel( int x, int y, int w, int h, int columnCount ) :
	Panel( x, y, w, h )
{
	setGridSize( 2, 2 );
	setGridVisible( false, false );
	setColumnCount( columnCount );
	setFgColor( Color( Scheme::SC_BLACK ));
	Vector2Set( _selectedCell, -1, -1 );
	Vector2Set( _mouseOverCell, 0, 0 );
	Vector2Set( _editableCell, -1, -1 );
	Vector2Set( _virtualSize, w, h );
	_columnSelectionEnabled = false;
	_rowSelectionEnabled = true;
	_cellEditingEnabled = true;
	_editableCellPanel = nullptr;
	_fakeInputPanel = new Panel();
	addInputSignal( new FooVGuiTablePanelHandler( this ));
}

void TablePanel::setCellEditingEnabled(bool enable)
{
	_cellEditingEnabled = enable;
}

void TablePanel::setColumnCount(int count)
{
	_columnDar.ensureCapacity( count );
	_columnDar.setCount( count );
}

void TablePanel::setGridVisible(bool h, bool v)
{
	Vector2Set( _gridVisible, h, v );
}

void TablePanel::setGridSize(int w, int h)
{
	_gridWide = w;
	_gridTall = h;
}

int TablePanel::getColumnCount()
{
	return _columnDar.getCount();
}

void TablePanel::setColumnExtents(int column, int a, int b)
{
	_columnDar.setElementAt( column, MAKE_COLUMN_EXTENTS( a, b ));
	repaint();
}

void TablePanel::setSelectedCell(int column, int row)
{
	if( _selectedCell[0] != column || _selectedCell[1] != row )
	{
		repaint();
		stopCellEditing();
	}
	Vector2Set( _selectedCell, column, row );
}

void TablePanel::getSelectedCell(int &column, int &row)
{
	column = _selectedCell[0];
	row = _selectedCell[1];
}

void TablePanel::setHeaderPanel( HeaderPanel *hp )
{
	hp->addChangeSignal( new FooVGuiTablePanelHandler( this ));
	hp->fireChangeSignal();
	repaint();
}

void TablePanel::setColumnSelectionEnabled(bool enable)
{
	_columnSelectionEnabled = enable;
	repaint();
}

void TablePanel::setRowSelectionEnabled(bool enable)
{
	_rowSelectionEnabled = enable;
	repaint();
}

void TablePanel::setCellSectionEnabled(bool enable)
{
	_cellSelectionEnabled = enable;
	repaint();
}

void TablePanel::setEditableCell(int column, int row)
{
	if( _editableCell[0] != column || _editableCell[1] != row )
	{
		stopCellEditing();
		_editableCellPanel = startCellEditing( column, row );
		if( _editableCellPanel )
			_editableCellPanel->setParent( this );
	}
	Vector2Set( _editableCell, column, row );
}

void TablePanel::stopCellEditing()
{
	if( _editableCellPanel )
	{
		_editableCellPanel->setParent( nullptr );
	}
	Vector2Set( _editableCell, -1, -1 );
	_editableCellPanel = nullptr;
}

void TablePanel::getVirtualSize(int &w, int &h)
{
	w = _virtualSize[0];
	h = _virtualSize[1];
}

Panel *TablePanel::isWithinTraverse( int x, int y )
{
	Panel *const p = Panel::isWithinTraverse( x, y );
	if( p == this )
	{
		int grid_left_half = _gridWide / 2 - 1;
		int grid_right_half = _gridWide - grid_left_half;

		for( int i = 0; i < _columnDar.getCount(); i++ )
		{
			int x_left, x_right, y_top = _gridTall;

			UNPACK_COLUMN_EXTENTS( _columnDar[i], x_left, x_right );

			x_left += grid_left_half;
			x_right -= grid_right_half;

			for( int j = 0; j < getRowCount(); j++ )
			{
				Panel *withinPanel;

				_fakeInputPanel->setParent( this );
				_fakeInputPanel->setBounds( x_left, y_top, x_right - x_left, getCellTall( j ));
				_fakeInputPanel->solveTraverse();

				withinPanel = _fakeInputPanel->isWithinTraverse( x, y );

				_fakeInputPanel->setParent( nullptr );

				if( withinPanel == _fakeInputPanel )
				{
					Vector2Set( _mouseOverCell, i, j );
					return p;
				}

				y_top += _gridTall + getCellTall( j );
			}
		}
	}

	return p;
}

void TablePanel::privateMousePressed(MouseCode, Panel *)
{
	if( _cellEditingEnabled )
	{
		setSelectedCell( _mouseOverCell[0], _mouseOverCell[1] );
		requestFocus();
	}
}

void TablePanel::privateMouseDoublePressed(MouseCode, Panel *)
{
	if( _cellEditingEnabled )
	{
		setSelectedCell( _mouseOverCell[0], _mouseOverCell[1] );

		int column, row;
		getSelectedCell( column, row );
		setEditableCell( column, row );
	}
}

void TablePanel::privateKeyTyped(KeyCode code, Panel *)
{
	if( _cellEditingEnabled )
	{
		int column, row;
		getSelectedCell( column, row );

		switch( code )
		{
		case KEY_UP:
			row = Q_max( 0, row - 1 );
			setSelectedCell( column, row );
			break;
		case KEY_DOWN:
			row++;
			setSelectedCell( column, row );
			break;
		case KEY_LEFT:
			column = Q_max( 0, column - 1 );
			setSelectedCell( column, row );
			break;
		case KEY_RIGHT:
			column++;
			setSelectedCell( column, row );
			break;
		case KEY_ENTER:
			setEditableCell( column, row );
			break;
		}
	}
}

void TablePanel::paint()
{
	int grid_left_half = _gridWide / 2 - 1;
	int grid_right_half = _gridWide - grid_left_half;

	int wide, tall;
	getPaintSize( wide, tall );
	int maxX = wide;

	int r, g, b, a;
	{
		Color fg_color;
		getFgColor( fg_color );
		fg_color.getColor( r, g, b, a );
	}

	if( _gridVisible[1] )
	{
		maxX = 0;
		for( int i = 0; i < _columnDar.getCount(); i++ )
		{
			int unused;
			UNPACK_COLUMN_EXTENTS( _columnDar[i], unused, maxX );

			int x_left = maxX - grid_right_half, x_right = maxX + grid_left_half;

			drawSetColor( r, g, b, a );
			drawFilledRect( x_left, 0, x_right, tall );
		}
	}

	if( _gridVisible[0] )
	{
		for( int i = 0, top = 0; i < getRowCount(); i++, top += getCellTall( i ) + _gridTall )
		{
			int bottom = top + _gridTall;
			drawSetColor( r, g, b, a );
			drawFilledRect( 0, top, maxX, bottom );
		}
	}

	Vector2Set( _virtualSize, 0, 0 );

	for( int i = 0; i < _columnDar.getCount(); i++ )
	{
		int x_left, x_right;

		UNPACK_COLUMN_EXTENTS( _columnDar[i], x_left, x_right );

		x_left += grid_left_half;
		x_right -= grid_right_half;

		if( _virtualSize[0] < x_right )
			_virtualSize[0] = x_right;

		int y_top = _gridTall;

		bool columnSelected = _columnSelectionEnabled && _selectedCell[0] == i;

		for( int j = 0; j < getRowCount(); j++ )
		{
			bool rowSelected = _rowSelectionEnabled && _selectedCell[1] == j;
			bool cellSelected = _cellSelectionEnabled && _selectedCell[0] == i && _selectedCell[1] == j;
			Panel *p;

			if( i == _editableCell[0] && j == _editableCell[1] )
			{
				if( _editableCellPanel )
				{
					_editableCellPanel->setBounds( x_left, y_top, x_right - x_left, getCellTall( j ));
					_editableCellPanel->repaint();
					_editableCellPanel->solveTraverse();
				}
			}
			else if(( p = getCellRenderer( i, j, columnSelected, rowSelected, cellSelected )))
			{
				p->setParent( this );
				p->setBounds( x_left, y_top, x_right - x_left, getCellTall( j ));
				p->repaint();
				p->solveTraverse();
				p->paintTraverse();
				p->setParent( nullptr );
			}

			y_top += _gridTall + getCellTall( j );
			if( _virtualSize[1] < y_top )
				_virtualSize[1] = y_top; // original code references x_left?
		}
	}
}
}
