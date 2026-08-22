// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "controls/configwizard.h"

using namespace vgui;

ConfigWizard::ConfigWizard( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
	setBorder( new LineBorder());

	treeFolder = new TreeFolder( "DonkeyFoo" );
	treeFolder->setParent( this );
	treeFolder->setBorder( new LoweredBorder());

	client = new Panel( 80, 30, 64, 64 );
	client->setParent( this );
	client->setBorder( new LineBorder());

	okButton = new Button( "Ok", 0, 0, 60, 20 );
	okButton->setParent( this );

	cancelButton = new Button( "Cancel", 0, 0 );
	cancelButton->setParent( this );

	applyButton = new Button( "Apply", 0, 0 );
	applyButton->setParent( this );

	helpButton = new Button( "Help", 0, 0 );
	helpButton->setParent( this );
}

void ConfigWizard::setSize( int wide, int tall )
{
	Panel::setSize( wide, tall );
	getPaintSize( wide, tall );

	treeFolder->setBounds( 10, 10, 160, tall - 60 );
	client->setBounds( 180, 10, wide - 190, tall - 60 );

	// fixed 70px stride from the right edge
	okButton->setPos( wide - 290, tall - 25 );
	cancelButton->setPos( wide - 220, tall - 25 );
	applyButton->setPos( wide - 150, tall - 25 );
	helpButton->setPos( wide - 80, tall - 25 );
}

Panel *ConfigWizard::getClient()
{
	return client;
}

TreeFolder *ConfigWizard::getFolder()
{
	return treeFolder;
}
