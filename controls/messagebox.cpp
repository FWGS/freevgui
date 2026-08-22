// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include "controls/messagebox.h"

using namespace vgui;

MessageBox::MessageBox( const char *title, const char *text, int x, int y ) : Frame( x, y, 64, 64 )
{
	setTitle( title );

	setMenuButtonVisible( false );
	setTrayButtonVisible( false );
	setMinimizeButtonVisible( false );
	setMaximizeButtonVisible( false );
	setCloseButtonVisible( false );
	setSizeable( false );

	messageLabel = new Label( text );
	messageLabel->setParent( getClient());

	okButton = new Button( "Ok", 10, 10 );
	okButton->setParent( getClient());
	okButton->addActionSignal( makeActionHandler([this]( Panel * )
	{
		fireActionSignal();
	}));

	int wide, tall;
	messageLabel->getContentSize( wide, tall );
	setSize( wide + 100, tall + 100 );
}

void MessageBox::performLayout()
{
	int clientWide, clientTall;
	getClient()->getSize( clientWide, clientTall );

	int wide, tall;

	messageLabel->getSize( wide, tall );
	messageLabel->setPos( clientWide / 2 - wide / 2, clientTall / 2 - tall / 2 - 20 );

	okButton->getSize( wide, tall );
	okButton->setPos( clientWide / 2 - wide / 2, clientTall - tall - 10 );
}

void MessageBox::addActionSignal( ActionSignal *s )
{
	actionSignals.putElement( s );
}

void MessageBox::fireActionSignal()
{
	for( auto signal: actionSignals )
		signal->actionPerformed( this );
}
