// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include <initializer_list>
#include "controls/wizard.h"

using namespace vgui;

WizardPanel::WizardPanel( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
	_currentWizardPage = nullptr;

	_backButton = new Button( "Back", 20, 100 );
	_backButton->addActionSignal( makeActionHandler([this]( Panel * )
	{
		doBack();
	}));
	_backButton->setParent( this );

	_nextButton = new Button( "Next", 80, 100 );
	_nextButton->addActionSignal( makeActionHandler([this]( Panel * )
	{
		doNext();
	}));
	_nextButton->setParent( this );

	_finishedButton = new Button( "Finished", 120, 100 );
	_finishedButton->setParent( this );

	_cancelButton = new Button( "Cancel", 180, 100 );
	_cancelButton->setParent( this );
}

void WizardPanel::fireFinishedActionSignal()
{
	_finishedButton->fireActionSignal();
}

void WizardPanel::fireCancelledActionSignal()
{
	_cancelButton->fireActionSignal();
}

void WizardPanel::firePageChangedActionSignal()
{
	for( auto signal : _pageChangedActionSignalDar )
		signal->actionPerformed( this );
}

void WizardPanel::performLayout()
{
	int wide, tall;

	getPaintSize( wide, tall );

	_backButton->setVisible( false );
	_nextButton->setVisible( false );
	_finishedButton->setVisible( false );
	_cancelButton->setVisible( false );

	if( !_currentWizardPage )
		return;

	WizardPage *page = _currentWizardPage;
	char buf[256];

	page->getCancelButtonText( buf, sizeof( buf ));
	_cancelButton->setText( buf );
	_cancelButton->setEnabled( page->isCancelButtonEnabled());
	_cancelButton->setVisible( page->isCancelButtonVisible());

	page->getFinishedButtonText( buf, sizeof( buf ));
	_finishedButton->setText( buf );
	_finishedButton->setEnabled( page->isFinishedButtonEnabled());
	_finishedButton->setVisible( page->isFinishedButtonVisible());

	page->getNextButtonText( buf, sizeof( buf ));
	_nextButton->setText( buf );
	_nextButton->setEnabled( page->isNextButtonEnabled());
	_nextButton->setVisible( page->isNextButtonVisible());

	page->getBackButtonText( buf, sizeof( buf ));
	_backButton->setText( buf );
	_backButton->setEnabled( page->isBackButtonEnabled());
	_backButton->setVisible( page->isBackButtonVisible());

	// button row pinned bottom-right, right-to-left
	// hidden buttons keep their slot but do not advance the cursor
	int gap = 2;
	int bTall = _backButton->getTall();
	int rowY = tall - bTall - gap;

	_currentWizardPage->setBounds( 2, 2, wide - 4, tall - bTall - 8 );

	int x = wide - gap * 2;
	for( Button *btn: { _cancelButton, _finishedButton, _nextButton, _backButton })
	{
		btn->setPos( x - btn->getWide(), rowY );

		if( btn->isVisible( ))
			x -= btn->getWide() + gap * 2;
	}
}

void WizardPanel::setCurrentWizardPage( WizardPage *page )
{
	if( _currentWizardPage )
		removeChild( _currentWizardPage );

	_currentWizardPage = page;

	if( page )
	{
		page->setParent( this );

		Panel *focus = page->getWantedFocus();
		if( focus )
			focus->requestFocus();
	}

	firePageChangedActionSignal();
	invalidateLayout( true );
}

void WizardPanel::addFinishedActionSignal( ActionSignal *s )
{
	_finishedButton->addActionSignal( s );
}

void WizardPanel::addCancelledActionSignal( ActionSignal *s )
{
	_cancelButton->addActionSignal( s );
}

void WizardPanel::addPageChangedActionSignal( ActionSignal *s )
{
	_pageChangedActionSignalDar.addElement( s );
}

void WizardPanel::doBack()
{
	if( _currentWizardPage )
	{
		_currentWizardPage->fireSwitchingToBackPageSignals();
		setCurrentWizardPage( _currentWizardPage->getBackWizardPage());
	}
}

void WizardPanel::doNext()
{
	if( _currentWizardPage )
	{
		_currentWizardPage->fireSwitchingToNextPageSignals();
		setCurrentWizardPage( _currentWizardPage->getNextWizardPage());
	}
}

void WizardPanel::getCurrentWizardPageTitle( char *buf, int bufLen )
{
	if( _currentWizardPage )
		_currentWizardPage->getTitle( buf, bufLen );
}

WizardPanel::WizardPage *WizardPanel::getCurrentWizardPage()
{
	return _currentWizardPage;
}

WizardPanel::WizardPage::WizardPage() : Panel( 0, 0, 64, 64 )
{
	init();
}

WizardPanel::WizardPage::WizardPage( int wide, int tall ) : Panel( 0, 0, wide, tall )
{
	init();
}

void WizardPanel::WizardPage::fireSwitchingToBackPageSignals()
{
	for( auto signal : _switchingToBackPageSignalDar )
		signal->actionPerformed( this );
}

void WizardPanel::WizardPage::fireSwitchingToNextPageSignals()
{
	for( auto signal : _switchingToNextPageSignalDar )
		signal->actionPerformed( this );
}

void WizardPanel::WizardPage::init()
{
	_backWizardPage = nullptr;
	_nextWizardPage = nullptr;
	_backButtonText = nullptr;
	_nextButtonText = nullptr;
	_finishedButtonText = nullptr;
	_cancelButtonText = nullptr;
	_wantedFocus = nullptr;
	_title = nullptr;

	_backButtonEnabled = false;
	_nextButtonEnabled = false;
	_finishedButtonEnabled = false;
	_cancelButtonEnabled = true;

	_backButtonVisible = true;
	_nextButtonVisible = true;
	_finishedButtonVisible = true;
	_cancelButtonVisible = true;

	setTitle( "Untitled" );
	setBackButtonText( "<< Back" );
	setNextButtonText( "Next >>" );
	setFinishedButtonText( "Finished" );
	setCancelButtonText( "Cancel" );
}

void WizardPanel::WizardPage::setBackWizardPage( WizardPage *page )
{
	_backWizardPage = page;
	_backButtonEnabled = page != nullptr;
}

void WizardPanel::WizardPage::setNextWizardPage( WizardPage *page )
{
	_nextWizardPage = page;
	_nextButtonEnabled = page != nullptr;
}

WizardPanel::WizardPage *WizardPanel::WizardPage::getBackWizardPage()
{
	return _backWizardPage;
}

WizardPanel::WizardPage *WizardPanel::WizardPage::getNextWizardPage()
{
	return _nextWizardPage;
}

bool WizardPanel::WizardPage::isBackButtonEnabled()
{
	return _backButtonEnabled;
}

bool WizardPanel::WizardPage::isNextButtonEnabled()
{
	return _nextButtonEnabled;
}

bool WizardPanel::WizardPage::isFinishedButtonEnabled()
{
	return _finishedButtonEnabled;
}

bool WizardPanel::WizardPage::isCancelButtonEnabled()
{
	return _cancelButtonEnabled;
}

void WizardPanel::WizardPage::setBackButtonEnabled( bool state )
{
	_backButtonEnabled = state;
}

void WizardPanel::WizardPage::setNextButtonEnabled( bool state )
{
	_nextButtonEnabled = state;
}

void WizardPanel::WizardPage::setFinishedButtonEnabled( bool state )
{
	_finishedButtonEnabled = state;
}

void WizardPanel::WizardPage::setCancelButtonEnabled( bool state )
{
	_cancelButtonEnabled = state;
}

bool WizardPanel::WizardPage::isBackButtonVisible()
{
	return _backButtonVisible;
}

bool WizardPanel::WizardPage::isNextButtonVisible()
{
	return _nextButtonVisible;
}

bool WizardPanel::WizardPage::isFinishedButtonVisible()
{
	return _finishedButtonVisible;
}

bool WizardPanel::WizardPage::isCancelButtonVisible()
{
	return _cancelButtonVisible;
}

void WizardPanel::WizardPage::setBackButtonVisible( bool state )
{
	_backButtonVisible = state;
}

void WizardPanel::WizardPage::setNextButtonVisible( bool state )
{
	_nextButtonVisible = state;
}

void WizardPanel::WizardPage::setFinishedButtonVisible( bool state )
{
	_finishedButtonVisible = state;
}

void WizardPanel::WizardPage::setCancelButtonVisible( bool state )
{
	_cancelButtonVisible = state;
}

void WizardPanel::WizardPage::getBackButtonText( char *buf, int bufLen )
{
	vgui_strcpy( buf, bufLen, _backButtonText );
}

void WizardPanel::WizardPage::getNextButtonText( char *buf, int bufLen )
{
	vgui_strcpy( buf, bufLen, _nextButtonText );
}

void WizardPanel::WizardPage::getFinishedButtonText( char *buf, int bufLen )
{
	vgui_strcpy( buf, bufLen, _finishedButtonText );
}

void WizardPanel::WizardPage::getCancelButtonText( char *buf, int bufLen )
{
	vgui_strcpy( buf, bufLen, _cancelButtonText );
}

void WizardPanel::WizardPage::setBackButtonText( const char *text )
{
	delete[] _backButtonText;
	_backButtonText = vgui_strdup( text );
}

void WizardPanel::WizardPage::setNextButtonText( const char *text )
{
	delete[] _nextButtonText;
	_nextButtonText = vgui_strdup( text );
}

void WizardPanel::WizardPage::setFinishedButtonText( const char *text )
{
	delete[] _finishedButtonText;
	_finishedButtonText = vgui_strdup( text );
}

void WizardPanel::WizardPage::setCancelButtonText( const char *text )
{
	delete[] _cancelButtonText;
	_cancelButtonText = vgui_strdup( text );
}

void WizardPanel::WizardPage::setWantedFocus( Panel *panel )
{
	_wantedFocus = panel;
}

Panel *WizardPanel::WizardPage::getWantedFocus()
{
	return _wantedFocus;
}

void WizardPanel::WizardPage::addSwitchingToBackPageSignal( ActionSignal *s )
{
	_switchingToBackPageSignalDar.putElement( s );
}

void WizardPanel::WizardPage::addSwitchingToNextPageSignal( ActionSignal *s )
{
	_switchingToNextPageSignalDar.putElement( s );
}

void WizardPanel::WizardPage::setTitle( const char *title )
{
	delete[] _title;
	_title = vgui_strdup( title );
}

void WizardPanel::WizardPage::getTitle( char *buf, int bufLen )
{
	vgui_strcpy( buf, bufLen, _title );
}
