// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "controls/configwizard.h"

using namespace vgui;

ConfigWizard::ConfigWizard( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
	setBorder( new LineBorder());

	_treeFolder = new TreeFolder( "DonkeyFoo" );
	_treeFolder->setParent( this );
	_treeFolder->setBorder( new LoweredBorder());

	_client = new Panel( 80, 30, 64, 64 );
	_client->setParent( this );
	_client->setBorder( new LineBorder());

	_okButton = new Button( "Ok", 0, 0, 60, 20 );
	_okButton->setParent( this );

	_cancelButton = new Button( "Cancel", 0, 0 );
	_cancelButton->setParent( this );

	_applyButton = new Button( "Apply", 0, 0 );
	_applyButton->setParent( this );

	_helpButton = new Button( "Help", 0, 0 );
	_helpButton->setParent( this );
}

void ConfigWizard::setSize( int wide, int tall )
{
	Panel::setSize( wide, tall );
	getPaintSize( wide, tall );

	_treeFolder->setBounds( 10, 10, 160, tall - 60 );
	_client->setBounds( 180, 10, wide - 190, tall - 60 );

	// fixed 70px stride from the right edge
	_okButton->setPos( wide - 290, tall - 25 );
	_cancelButton->setPos( wide - 220, tall - 25 );
	_applyButton->setPos( wide - 150, tall - 25 );
	_helpButton->setPos( wide - 80, tall - 25 );
}

Panel *ConfigWizard::getClient()
{
	return _client;
}

TreeFolder *ConfigWizard::getFolder()
{
	return _treeFolder;
}
