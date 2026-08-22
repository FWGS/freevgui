// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2024-2026 Alibek Omarov

#include <initializer_list>
#include "controls/wizard.h"

using namespace vgui;

WizardPanel::WizardPanel( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
	wizardPage = nullptr;

	backButton = new Button( "Back", 20, 100 );
	backButton->addActionSignal( makeActionHandler([this]( Panel * )
	{
		doBack();
	}));
	backButton->setParent( this );

	nextButton = new Button( "Next", 80, 100 );
	nextButton->addActionSignal( makeActionHandler([this]( Panel * )
	{
		doNext();
	}));
	nextButton->setParent( this );

	finishedButton = new Button( "Finished", 120, 100 );
	finishedButton->setParent( this );

	cancelButton = new Button( "Cancel", 180, 100 );
	cancelButton->setParent( this );
}

void WizardPanel::fireFinishedActionSignal()
{
	finishedButton->fireActionSignal();
}

void WizardPanel::fireCancelledActionSignal()
{
	cancelButton->fireActionSignal();
}

void WizardPanel::firePageChangedActionSignal()
{
	for( auto signal : pageChangedActionSignals )
		signal->actionPerformed( this );
}

void WizardPanel::performLayout()
{
	int wide, tall;

	getPaintSize( wide, tall );

	backButton->setVisible( false );
	nextButton->setVisible( false );
	finishedButton->setVisible( false );
	cancelButton->setVisible( false );

	if( !wizardPage )
		return;

	WizardPage *page = wizardPage;
	char buf[256];

	page->getCancelButtonText( buf, sizeof( buf ));
	cancelButton->setText( buf );
	cancelButton->setEnabled( page->isCancelButtonEnabled());
	cancelButton->setVisible( page->isCancelButtonVisible());

	page->getFinishedButtonText( buf, sizeof( buf ));
	finishedButton->setText( buf );
	finishedButton->setEnabled( page->isFinishedButtonEnabled());
	finishedButton->setVisible( page->isFinishedButtonVisible());

	page->getNextButtonText( buf, sizeof( buf ));
	nextButton->setText( buf );
	nextButton->setEnabled( page->isNextButtonEnabled());
	nextButton->setVisible( page->isNextButtonVisible());

	page->getBackButtonText( buf, sizeof( buf ));
	backButton->setText( buf );
	backButton->setEnabled( page->isBackButtonEnabled());
	backButton->setVisible( page->isBackButtonVisible());

	// button row pinned bottom-right, right-to-left
	// hidden buttons keep their slot but do not advance the cursor
	int gap = 2;
	int bTall = backButton->getTall();
	int rowY = tall - bTall - gap;

	wizardPage->setBounds( 2, 2, wide - 4, tall - bTall - 8 );

	int x = wide - gap * 2;
	for( Button *btn: { cancelButton, finishedButton, nextButton, backButton })
	{
		btn->setPos( x - btn->getWide(), rowY );

		if( btn->isVisible( ))
			x -= btn->getWide() + gap * 2;
	}
}

void WizardPanel::setCurrentWizardPage( WizardPage *page )
{
	if( wizardPage )
		removeChild( wizardPage );

	wizardPage = page;

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
	finishedButton->addActionSignal( s );
}

void WizardPanel::addCancelledActionSignal( ActionSignal *s )
{
	cancelButton->addActionSignal( s );
}

void WizardPanel::addPageChangedActionSignal( ActionSignal *s )
{
	pageChangedActionSignals.addElement( s );
}

void WizardPanel::doBack()
{
	if( wizardPage )
	{
		wizardPage->fireSwitchingToBackPageSignals();
		setCurrentWizardPage( wizardPage->getBackWizardPage());
	}
}

void WizardPanel::doNext()
{
	if( wizardPage )
	{
		wizardPage->fireSwitchingToNextPageSignals();
		setCurrentWizardPage( wizardPage->getNextWizardPage());
	}
}

void WizardPanel::getCurrentWizardPageTitle( char *buf, int bufLen )
{
	if( wizardPage )
		wizardPage->getTitle( buf, bufLen );
}

WizardPanel::WizardPage *WizardPanel::getCurrentWizardPage()
{
	return wizardPage;
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
	for( auto signal : switchingToBackPageSignals )
		signal->actionPerformed( this );
}

void WizardPanel::WizardPage::fireSwitchingToNextPageSignals()
{
	for( auto signal : switchingToNextPageSignals )
		signal->actionPerformed( this );
}

void WizardPanel::WizardPage::init()
{
	backWizardPage = nullptr;
	nextWizardPage = nullptr;
	backButtonText = nullptr;
	nextButtonText = nullptr;
	finishedButtonText = nullptr;
	cancelButtonText = nullptr;
	wantedFocus = nullptr;
	title = nullptr;

	backButtonEnabled = false;
	nextButtonEnabled = false;
	finishedButtonEnabled = false;
	cancelButtonEnabled = true;

	backButtonVisible = true;
	nextButtonVisible = true;
	finishedButtonVisible = true;
	cancelButtonVisible = true;

	setTitle( "Untitled" );
	setBackButtonText( "<< Back" );
	setNextButtonText( "Next >>" );
	setFinishedButtonText( "Finished" );
	setCancelButtonText( "Cancel" );
}

void WizardPanel::WizardPage::setBackWizardPage( WizardPage *page )
{
	backWizardPage = page;
	backButtonEnabled = page != nullptr;
}

void WizardPanel::WizardPage::setNextWizardPage( WizardPage *page )
{
	nextWizardPage = page;
	nextButtonEnabled = page != nullptr;
}

WizardPanel::WizardPage *WizardPanel::WizardPage::getBackWizardPage()
{
	return backWizardPage;
}

WizardPanel::WizardPage *WizardPanel::WizardPage::getNextWizardPage()
{
	return nextWizardPage;
}

bool WizardPanel::WizardPage::isBackButtonEnabled()
{
	return backButtonEnabled;
}

bool WizardPanel::WizardPage::isNextButtonEnabled()
{
	return nextButtonEnabled;
}

bool WizardPanel::WizardPage::isFinishedButtonEnabled()
{
	return finishedButtonEnabled;
}

bool WizardPanel::WizardPage::isCancelButtonEnabled()
{
	return cancelButtonEnabled;
}

void WizardPanel::WizardPage::setBackButtonEnabled( bool state )
{
	backButtonEnabled = state;
}

void WizardPanel::WizardPage::setNextButtonEnabled( bool state )
{
	nextButtonEnabled = state;
}

void WizardPanel::WizardPage::setFinishedButtonEnabled( bool state )
{
	finishedButtonEnabled = state;
}

void WizardPanel::WizardPage::setCancelButtonEnabled( bool state )
{
	cancelButtonEnabled = state;
}

bool WizardPanel::WizardPage::isBackButtonVisible()
{
	return backButtonVisible;
}

bool WizardPanel::WizardPage::isNextButtonVisible()
{
	return nextButtonVisible;
}

bool WizardPanel::WizardPage::isFinishedButtonVisible()
{
	return finishedButtonVisible;
}

bool WizardPanel::WizardPage::isCancelButtonVisible()
{
	return cancelButtonVisible;
}

void WizardPanel::WizardPage::setBackButtonVisible( bool state )
{
	backButtonVisible = state;
}

void WizardPanel::WizardPage::setNextButtonVisible( bool state )
{
	nextButtonVisible = state;
}

void WizardPanel::WizardPage::setFinishedButtonVisible( bool state )
{
	finishedButtonVisible = state;
}

void WizardPanel::WizardPage::setCancelButtonVisible( bool state )
{
	cancelButtonVisible = state;
}

void WizardPanel::WizardPage::getBackButtonText( char *buf, int bufLen )
{
	vgui_strcpy( buf, bufLen, backButtonText );
}

void WizardPanel::WizardPage::getNextButtonText( char *buf, int bufLen )
{
	vgui_strcpy( buf, bufLen, nextButtonText );
}

void WizardPanel::WizardPage::getFinishedButtonText( char *buf, int bufLen )
{
	vgui_strcpy( buf, bufLen, finishedButtonText );
}

void WizardPanel::WizardPage::getCancelButtonText( char *buf, int bufLen )
{
	vgui_strcpy( buf, bufLen, cancelButtonText );
}

void WizardPanel::WizardPage::setBackButtonText( const char *text )
{
	delete[] backButtonText;
	backButtonText = vgui_strdup( text );
}

void WizardPanel::WizardPage::setNextButtonText( const char *text )
{
	delete[] nextButtonText;
	nextButtonText = vgui_strdup( text );
}

void WizardPanel::WizardPage::setFinishedButtonText( const char *text )
{
	delete[] finishedButtonText;
	finishedButtonText = vgui_strdup( text );
}

void WizardPanel::WizardPage::setCancelButtonText( const char *text )
{
	delete[] cancelButtonText;
	cancelButtonText = vgui_strdup( text );
}

void WizardPanel::WizardPage::setWantedFocus( Panel *panel )
{
	wantedFocus = panel;
}

Panel *WizardPanel::WizardPage::getWantedFocus()
{
	return wantedFocus;
}

void WizardPanel::WizardPage::addSwitchingToBackPageSignal( ActionSignal *s )
{
	switchingToBackPageSignals.putElement( s );
}

void WizardPanel::WizardPage::addSwitchingToNextPageSignal( ActionSignal *s )
{
	switchingToNextPageSignals.putElement( s );
}

void WizardPanel::WizardPage::setTitle( const char *newTitle )
{
	delete[] title;
	title = vgui_strdup( newTitle );
}

void WizardPanel::WizardPage::getTitle( char *buf, int bufLen )
{
	vgui_strcpy( buf, bufLen, title );
}
