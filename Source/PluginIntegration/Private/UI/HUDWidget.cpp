// Copyright 2022 Maximilien (Synock) Guislain


#include "UI/HUDWidget.h"

#include "Character/PluginIntegrationCharacter.h"
#include "UI/BagWidget.h"

void UHUDWidget::HandleBag(EBagSlot InputBagSlot, UBagWidget* Widget)
{
	check(Widget);

	if (const APluginIntegrationCharacter* Character = Cast<APluginIntegrationCharacter>(GetOwningPlayerPawn()))
	{
		const FInventoryItem& BagItem = Character->GetEquippedItem(
			UInventoryComponent::GetInventorySlotFromBagSlot(InputBagSlot));

		check(BagItem.Bag);

		Widget->InitBagData(BagItem.Name, BagItem.BagWidth, BagItem.BagHeight, BagItem.BagSize, InputBagSlot);
	}
}
