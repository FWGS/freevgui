// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "table.h"
#include "vgui_internal.h"
#include "signals.h"

#define MAKE_COLUMN_EXTENTS( a, b ) (( b ) + (( a ) << 12))
#define UNPACK_COLUMN_EXTENTS( data, a, b ) (( a ) = ( data ) >> 12 & 0xfff, ( b ) = ( data ) & 0xfff )

namespace vgui {
class TablePanelSignalsHandler : public ChangeSignal, public InputSignalAdapter, public RepaintSignal
{
	TablePanel *table;

public:
	TablePanelSignalsHandler( TablePanel *table ) : table( table ) { }

	virtual void valueChanged( Panel *p ) override
	{
		HeaderPanel *hp = dynamic_cast<HeaderPanel *>( p );

		if( !hp )
			return;

		int count = table->getColumnCount();

		for( int i = 0; i < hp->getSectionCount() && i < count; i++ )
		{
			int x, y;
			hp->getSectionExtents( i, x, y );
			table->setColumnExtents( i, x, y );
		}
	}

	virtual void mousePressed( MouseCode code, Panel *p ) override
	{
		table->privateMousePressed( code, p );
	}

	virtual void mouseDoublePressed( MouseCode code, Panel *p ) override
	{
		table->privateMouseDoublePressed( code, p );
	}

	virtual void keyTyped( KeyCode code, Panel *p ) override
	{
		table->privateKeyTyped( code, p );
	}

	virtual void panelRepainted( Panel * ) override
	{
		table->repaint();
	}
};

TablePanel::TablePanel( int x, int y, int w, int h, int columnCount ) : Panel( x, y, w, h ),
	selectedCell{ -1, -1 }, mouseOverCell{ 0, 0 }, editableCell{ -1, -1 }, virtualSize{ w, h },
	columnSelectionEnabled( false ), rowSelectionEnabled( true ), cellSelectionEnabled( true ), cellEditingEnabled( true ),
	editableCellPanel( nullptr ), fakeInputPanel( new Panel())
{
	setGridSize( 2, 2 );
	setGridVisible( false, false );
	setColumnCount( columnCount );
	setFgColor( Color( Scheme::SC_BLACK ));
	addInputSignal( new TablePanelSignalsHandler( this ));
}

void TablePanel::setCellEditingEnabled( bool enable )
{
	cellEditingEnabled = enable;
}

void TablePanel::setColumnCount( int count )
{
	columns.ensureCapacity( count );
	columns.setCount( count );
}

void TablePanel::setGridVisible( bool h, bool v )
{
	// shipped bug: horizontal is stored into both bytes, vertical is ignored
	gridVisible[0] = h;
	gridVisible[1] = h;
}

void TablePanel::setGridSize( int w, int h )
{
	gridWide = w;
	gridTall = h;
}

int TablePanel::getColumnCount()
{
	return columns.getCount();
}

void TablePanel::setColumnExtents( int column, int a, int b )
{
	columns.setElementAt( MAKE_COLUMN_EXTENTS( a, b ), column );
	repaint();
}

void TablePanel::setSelectedCell( int column, int row )
{
	if( selectedCell[0] != column || selectedCell[1] != row )
	{
		repaint();
		stopCellEditing();
	}
	selectedCell[0] = column;
	selectedCell[1] = row;
}

void TablePanel::getSelectedCell( int &column, int &row )
{
	column = selectedCell[0];
	row = selectedCell[1];
}

void TablePanel::setHeaderPanel( HeaderPanel *hp )
{
	hp->addChangeSignal( new TablePanelSignalsHandler( this ));
	hp->fireChangeSignal();
	repaint();
}

void TablePanel::setColumnSelectionEnabled( bool enable )
{
	columnSelectionEnabled = enable;
	repaint();
}

void TablePanel::setRowSelectionEnabled( bool enable )
{
	rowSelectionEnabled = enable;
	repaint();
}

void TablePanel::setCellSectionEnabled( bool enable )
{
	cellSelectionEnabled = enable;
	repaint();
}

void TablePanel::setEditableCell( int column, int row )
{
	if( editableCell[0] != column || editableCell[1] != row )
	{
		stopCellEditing();
		editableCellPanel = startCellEditing( column, row );
		if( editableCellPanel )
			editableCellPanel->setParent( this );
	}
	editableCell[0] = column;
	editableCell[1] = row;
}

void TablePanel::stopCellEditing()
{
	if( editableCellPanel )
		editableCellPanel->setParent( nullptr );

	editableCell[0] = -1;
	editableCell[1] = -1;
	editableCellPanel = nullptr;
}

void TablePanel::getVirtualSize( int &w, int &h )
{
	w = virtualSize[0];
	h = virtualSize[1];
}

Panel *TablePanel::isWithinTraverse( int x, int y )
{
	Panel *const p = Panel::isWithinTraverse( x, y );
	if( p != this )
		return p;

	int grid_left_half = gridWide / 2 - 1;
	int grid_right_half = gridWide - grid_left_half;

	for( int i = 0; i < columns.getCount(); i++ )
	{
		int x_left, x_right, y_top = gridTall;

		UNPACK_COLUMN_EXTENTS( columns[i], x_left, x_right );

		x_left += grid_left_half;
		x_right -= grid_right_half;

		for( int j = 0; j < getRowCount(); j++ )
		{
			Panel *withinPanel;

			fakeInputPanel->setParent( this );
			fakeInputPanel->setBounds( x_left, y_top, x_right - x_left, getCellTall( j ));
			fakeInputPanel->solveTraverse();

			withinPanel = fakeInputPanel->isWithinTraverse( x, y );

			fakeInputPanel->setParent( nullptr );

			if( withinPanel == fakeInputPanel )
			{
				mouseOverCell[0] = i;
				mouseOverCell[1] = j;
				return p;
			}

			y_top += gridTall + getCellTall( j );
		}
	}

	return p;
}

void TablePanel::privateMousePressed( MouseCode, Panel * )
{
	if( !cellEditingEnabled )
		return;

	setSelectedCell( mouseOverCell[0], mouseOverCell[1] );
	requestFocus();
}

void TablePanel::privateMouseDoublePressed( MouseCode, Panel * )
{
	if( !cellEditingEnabled )
		return;

	setSelectedCell( mouseOverCell[0], mouseOverCell[1] );

	int column, row;
	getSelectedCell( column, row );
	setEditableCell( column, row );
}

void TablePanel::privateKeyTyped( KeyCode code, Panel * )
{
	if( !cellEditingEnabled )
		return;

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
	default:
		break;
	}
}

void TablePanel::paint()
{
	int grid_left_half = gridWide / 2 - 1;
	int grid_right_half = gridWide - grid_left_half;

	int wide, tall;
	getPaintSize( wide, tall );
	int maxX = wide;

	int r, g, b, a;
	Color fg_color;
	getFgColor( fg_color );
	fg_color.getColor( r, g, b, a );

	if( gridVisible[1] )
	{
		maxX = 0;
		for( int i = 0; i < columns.getCount(); i++ )
		{
			int unused;
			UNPACK_COLUMN_EXTENTS( columns[i], unused, maxX );

			int x_left = maxX - grid_right_half, x_right = maxX + grid_left_half;

			drawSetColor( r, g, b, a );
			drawFilledRect( x_left, 0, x_right, tall );
		}
	}

	if( gridVisible[0] )
	{
		for( int i = 0, top = 0; i < getRowCount(); i++, top += getCellTall( i ) + gridTall )
		{
			int bottom = top + gridTall;
			drawSetColor( r, g, b, a );
			drawFilledRect( 0, top, maxX, bottom );
		}
	}

	virtualSize[0] = 0;
	virtualSize[1] = 0;

	for( int i = 0; i < columns.getCount(); i++ )
	{
		int x_left, x_right;

		UNPACK_COLUMN_EXTENTS( columns[i], x_left, x_right );

		x_left += grid_left_half;
		x_right -= grid_right_half;

		if( virtualSize[0] < x_right )
			virtualSize[0] = x_right;

		int y_top = gridTall;

		bool columnSelected = columnSelectionEnabled && selectedCell[0] == i;

		for( int j = 0; j < getRowCount(); j++ )
		{
			bool rowSelected = rowSelectionEnabled && selectedCell[1] == j;
			bool cellSelected = cellSelectionEnabled && selectedCell[0] == i && selectedCell[1] == j;
			Panel *p;

			if( i == editableCell[0] && j == editableCell[1] )
			{
				if( editableCellPanel )
				{
					editableCellPanel->setBounds( x_left, y_top, x_right - x_left, getCellTall( j ));
					editableCellPanel->repaint();
					editableCellPanel->solveTraverse();
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

			y_top += gridTall + getCellTall( j );
			// shipped bug: virtualSize[1] tracks x1 (a horizontal coord) instead of the running y
			if( virtualSize[1] < x_right )
				virtualSize[1] = x_right;
		}
	}
}
}
