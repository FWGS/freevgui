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

	_messageLabel = new Label( text );
	_messageLabel->setParent( getClient());

	_okButton = new Button( "Ok", 10, 10 );
	_okButton->setParent( getClient());
	_okButton->addActionSignal( makeActionHandler([this]( Panel * )
	{
		fireActionSignal();
	}));

	int wide, tall;
	_messageLabel->getContentSize( wide, tall );
	setSize( wide + 100, tall + 100 );
}

void MessageBox::performLayout()
{
	int clientWide, clientTall;
	getClient()->getSize( clientWide, clientTall );

	int wide, tall;

	_messageLabel->getSize( wide, tall );
	_messageLabel->setPos( clientWide / 2 - wide / 2, clientTall / 2 - tall / 2 - 20 );

	_okButton->getSize( wide, tall );
	_okButton->setPos( clientWide / 2 - wide / 2, clientTall - tall - 10 );
}

void MessageBox::addActionSignal( ActionSignal *s )
{
	_actionSignalDar.putElement( s );
}

void MessageBox::fireActionSignal()
{
	for( auto signal: _actionSignalDar )
		signal->actionPerformed( this );
}
