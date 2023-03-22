// Copyright 2023 Maximilien (Synock) Guislain


#include "UI/Dialog/FinalDialogWindow.h"
#include "DialogAndQuestPlugin/Public/UI/DialogWindow.h"

void UFinalDialogWindow::HideWindow()
{
	SetVisibility(ESlateVisibility::Hidden);
}

//----------------------------------------------------------------------------------------------------------------------

void UFinalDialogWindow::InitUI()
{
	if(InnerWidget)
		InnerWidget->OnExit.AddUniqueDynamic(this, &UFinalDialogWindow::HideWindow);
}

//----------------------------------------------------------------------------------------------------------------------

void UFinalDialogWindow::DisplayPlainString(const FString& PlainString)
{
	InnerWidget->DisplayPlainString(PlainString);
}
