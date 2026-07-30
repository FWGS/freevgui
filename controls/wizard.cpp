// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#include "controls/wizard.h"

using namespace vgui;

WizardPanel::WizardPanel( int x, int y, int wide, int tall ) : Panel( x, y, wide, tall )
{
}

void WizardPanel::fireFinishedActionSignal()
{
}

void WizardPanel::fireCancelledActionSignal()
{
}

void WizardPanel::firePageChangedActionSignal()
{
}

void WizardPanel::performLayout()
{
}

void WizardPanel::setCurrentWizardPage( WizardPage *page )
{
}

void WizardPanel::addFinishedActionSignal( ActionSignal *s )
{
}

void WizardPanel::addCancelledActionSignal( ActionSignal *s )
{
}

void WizardPanel::addPageChangedActionSignal( ActionSignal *s )
{
}

void WizardPanel::doBack()
{
}

void WizardPanel::doNext()
{
}

void WizardPanel::getCurrentWizardPageTitle( char *buf, int bufLen )
{
}

WizardPanel::WizardPage *WizardPanel::getCurrentWizardPage()
{
	return NULL;
}

WizardPanel::WizardPage::WizardPage() : Panel()
{
}

WizardPanel::WizardPage::WizardPage( int x, int y ) : Panel( x, y, 0, 0 )
{
}

void WizardPanel::WizardPage::fireSwitchingToBackPageSignals()
{
}

void WizardPanel::WizardPage::fireSwitchingToNextPageSignals()
{
}

void WizardPanel::WizardPage::init()
{
}

void WizardPanel::WizardPage::setBackWizardPage( WizardPage *page )
{
}

void WizardPanel::WizardPage::setNextWizardPage( WizardPage *page )
{
}

WizardPanel::WizardPage *WizardPanel::WizardPage::getBackWizardPage()
{
	return NULL;
}

WizardPanel::WizardPage *WizardPanel::WizardPage::getNextWizardPage()
{
	return NULL;
}

bool WizardPanel::WizardPage::isBackButtonEnabled()
{
	return false;
}

bool WizardPanel::WizardPage::isNextButtonEnabled()
{
	return false;
}

bool WizardPanel::WizardPage::isFinishedButtonEnabled()
{
	return false;
}

bool WizardPanel::WizardPage::isCancelButtonEnabled()
{
	return false;
}

void WizardPanel::WizardPage::setBackButtonEnabled( bool state )
{
}

void WizardPanel::WizardPage::setNextButtonEnabled( bool state )
{
}

void WizardPanel::WizardPage::setFinishedButtonEnabled( bool state )
{
}

void WizardPanel::WizardPage::setCancelButtonEnabled( bool state )
{
}

bool WizardPanel::WizardPage::isBackButtonVisible()
{
	return false;
}

bool WizardPanel::WizardPage::isNextButtonVisible()
{
	return false;
}

bool WizardPanel::WizardPage::isFinishedButtonVisible()
{
	return false;
}

bool WizardPanel::WizardPage::isCancelButtonVisible()
{
	return false;
}

void WizardPanel::WizardPage::setBackButtonVisible( bool state )
{
}

void WizardPanel::WizardPage::setNextButtonVisible( bool state )
{
}

void WizardPanel::WizardPage::setFinishedButtonVisible( bool state )
{
}

void WizardPanel::WizardPage::setCancelButtonVisible( bool state )
{
}

void WizardPanel::WizardPage::getBackButtonText( char *buf, int bufLen )
{
}

void WizardPanel::WizardPage::getNextButtonText( char *buf, int bufLen )
{
}

void WizardPanel::WizardPage::getFinishedButtonText( char *buf, int bufLen )
{
}

void WizardPanel::WizardPage::getCancelButtonText( char *buf, int bufLen )
{
}

void WizardPanel::WizardPage::setBackButtonText( const char *text )
{
}

void WizardPanel::WizardPage::setNextButtonText( const char *text )
{
}

void WizardPanel::WizardPage::setFinishedButtonText( const char *text )
{
}

void WizardPanel::WizardPage::setCancelButtonText( const char *text )
{
}

void WizardPanel::WizardPage::setWantedFocus( Panel *panel )
{
}

Panel *WizardPanel::WizardPage::getWantedFocus()
{
	return NULL;
}

void WizardPanel::WizardPage::addSwitchingToBackPageSignal( ActionSignal *s )
{
}

void WizardPanel::WizardPage::addSwitchingToNextPageSignal( ActionSignal *s )
{
}

void WizardPanel::WizardPage::setTitle( const char *title )
{
}

void WizardPanel::WizardPage::getTitle( char *buf, int bufLen )
{
}
