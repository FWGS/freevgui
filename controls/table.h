// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#ifndef VGUI_TABLEPANEL_H
#define VGUI_TABLEPANEL_H

#include "panel.h"
#include "controls/header.h"

namespace vgui
{
class CLASSEXPORT TablePanel : public Panel
{
public:
	TablePanel( int x, int y, int w, int h, int columnCount );
	virtual void setCellEditingEnabled( bool enable );
	virtual void setColumnCount( int count );
	virtual void setGridVisible( bool h, bool v );
	virtual void setGridSize( int w, int h );
	virtual int getColumnCount();
	virtual void setColumnExtents( int column, int a, int b );
	virtual void setSelectedCell( int column, int row );
	virtual void getSelectedCell( int &column, int &row );
	virtual void setHeaderPanel( HeaderPanel* );
	virtual void setColumnSelectionEnabled( bool enable );
	virtual void setRowSelectionEnabled( bool enable );
	virtual void setCellSectionEnabled( bool enable );
	virtual void setEditableCell( int column, int row );
	virtual void stopCellEditing();
	virtual void getVirtualSize( int &w, int &h ) override;
	virtual int getRowCount() = 0;
	virtual int getCellTall( int ) = 0;
	virtual Panel *getCellRenderer( int, int, bool, bool, bool ) = 0;
	virtual Panel *startCellEditing( int, int ) = 0;
protected:
	virtual void paint() override;
	virtual Panel *isWithinTraverse( int, int ) override;

public:
	virtual void privateMousePressed( MouseCode, Panel* );
	virtual void privateMouseDoublePressed( MouseCode, Panel* );
	virtual void privateKeyTyped( KeyCode code, Panel* );

	Dar<int> columns;
	bool gridVisible[2];
	int gridWide, gridTall, selectedCell[2], mouseOverCell[2], editableCell[2];
	Panel *fakeInputPanel;
	bool columnSelectionEnabled, rowSelectionEnabled, cellSelectionEnabled;
	Panel *editableCellPanel;
	int virtualSize[2];
	bool cellEditingEnabled;
};
CHECK_STRUCT_SIZE( TablePanel, 260, 360, 360 );
}

#endif // VGUI_TABLEPANEL_H
