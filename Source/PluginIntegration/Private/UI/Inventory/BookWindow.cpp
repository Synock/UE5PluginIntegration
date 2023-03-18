// Copyright 2023 Maximilien (Synock) Guislain


#include "UI/Inventory/BookWindow.h"

#include "Components/TextBlock.h"
#include "UI/InventoryBookWidget.h"

void UBookWindow::InitUI(UInventoryBookWidget* TextWidget, UTextBlock* TitleWidget)
{
	InnerWidget = TextWidget;
	TitleBlock = TitleWidget;
}

void UBookWindow::Close()
{
	RemoveFromParent();
}

void UBookWindow::SetText(const FText& TextToDisplay)
{
	if(!InnerWidget)
		return;
	InnerWidget->SetText(TextToDisplay);
}

void UBookWindow::SetTitle(const FText& TitleToDisplay)
{
	if(!TitleBlock)
		return;
	TitleBlock->SetText(TitleToDisplay);
}
