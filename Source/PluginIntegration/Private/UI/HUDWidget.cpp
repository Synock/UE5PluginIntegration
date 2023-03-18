// Copyright 2022 Maximilien (Synock) Guislain


#include "UI/HUDWidget.h"

#include "Character/PluginIntegrationCharacter.h"
#include "Items/InventoryItemBag.h"
#include "UI/BagWidget.h"

void UHUDWidget::HandleBag(EBagSlot InputBagSlot, UBagWidget* Widget)
{
	check(Widget);

	if (const APluginIntegrationCharacter* Character = Cast<APluginIntegrationCharacter>(GetOwningPlayerPawn()))
	{
		const UInventoryItemEquipable* BagItem = Character->GetEquippedItem(
			UInventoryComponent::GetInventorySlotFromBagSlot(InputBagSlot));
		check(BagItem);

		const UInventoryItemBag* ActualBagItem = Cast<UInventoryItemBag>(BagItem);
		check(ActualBagItem);

		Widget->InitBagData(BagItem->Name, ActualBagItem->BagWidth,  ActualBagItem->BagHeight,  ActualBagItem->BagSize, InputBagSlot);
	}
}
