// SPDX-License-Identifier: GPL-2.0-only
// Copyright (C) 2024 Alibek Omarov

#ifndef VGUI_WIZARD_H
#define VGUI_WIZARD_H

#include "panel.h"
#include "controls/button.h"

namespace vgui
{
class CLASSEXPORT WizardPanel : public Panel
{
public:
	WizardPanel( int, int, int, int );

	class WizardPage : public Panel
	{
	private:
		WizardPage* _backWizardPage;
		WizardPage* _nextWizardPage;
		bool _backButtonEnabled, _nextButtonEnabled, _finishedButtonEnabled,
			_cancelButtonEnabled, _backButtonVisible, _nextButtonVisible,
			_finishedButtonVisible, _cancelButtonVisible;
		char* _backButtonText;
		char* _nextButtonText;
		char* _finishedButtonText;
		char* _cancelButtonText;
		Dar<ActionSignal*> _switchingToBackPageSignalDar, _switchingToNextPageSignalDar;
		char* _title;
		Panel* _wantedFocus;

		virtual void fireSwitchingToBackPageSignals();
		virtual void fireSwitchingToNextPageSignals();
		virtual void init();
	public:
		WizardPage( WizardPage* );
		WizardPage( int, int );

		virtual void setBackWizardPage( WizardPage* );
		virtual void setNextWizardPage( WizardPage* );
		virtual WizardPage* getBackWizardPage();
		virtual WizardPage* getNextWizardPage();
		virtual bool isBackButtonEnabled();
		virtual bool isNextButtonEnabled();
		virtual bool isFinishedButtonEnabled();
		virtual bool isCancelButtonEnabled();
		virtual void setBackButtonEnabled( bool );
		virtual void setNextButtonEnabled( bool );
		virtual void setFinishedButtonEnabled( bool );
		virtual void setCancelButtonEnabled( bool );
		virtual bool isBackButtonVisible();
		virtual bool isNextButtonVisible();
		virtual bool isFinishedButtonVisible();
		virtual bool isCancelButtonVisible();
		virtual void setBackButtonVisible( bool );
		virtual void setNextButtonVisible( bool );
		virtual void setFinishedButtonVisible( bool );
		virtual void setCancelButtonVisible( bool );
		virtual void getBackButtonText( char*, int );
		virtual void getNextButtonText( char*, int );
		virtual void getFinishedButtonText( char*, int );
		virtual void getCancelButtonText( char *, int );
		virtual void setBackButtonText( const char* );
		virtual void setNextButtonText( const char* );
		virtual void setFinishedButtonText( const char* );
		virtual void setCancelButtonText( const char* );
		virtual void setWantedFocus( Panel* );
		virtual Panel* getWantedFocus();
		virtual void addSwitchingToBackPageSignal( ActionSignal* );
		virtual void addSwitchingToNextPageSignal( ActionSignal* );
		virtual void setTitle( const char* );
		virtual void getTitle( char*, int );
	};
private:
	Button* _backButton;
	Button* _nextButton;
	Button* _finishedButton;
	Button* _cancelButton;
	WizardPage* _currentWizardPage;
	Dar<vgui::ActionSignal*> _pageChangedActionSignalDar;

	virtual void fireFinishedActionSignal();
	virtual void fireCancelledActionSignal();
	virtual void firePageChangedActionSignal();
protected:
	virtual void performLayout() override;
public:
	virtual void setCurrentWizardPage( WizardPage* );
	virtual void addFinishedActionSignal( ActionSignal* );
	virtual void addCancelledActionSignal( ActionSignal* );
	virtual void addPageChangedActionSignal( ActionSignal* );
	virtual void doBack();
	virtual void doNext();
	virtual void getCurrentWizardPageTitle( char *, int );
	virtual WizardPage* getCurrentWizardPage();
};
CHECK_STRUCT_SIZE( WizardPanel::WizardPage, 252 );
CHECK_STRUCT_SIZE( WizardPanel, 220 );
}
#endif // VGUI_WIZARD_H
