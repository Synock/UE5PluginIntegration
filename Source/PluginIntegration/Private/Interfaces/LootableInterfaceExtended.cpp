// Copyright 2023 Maximilien (Synock) Guislain


#include "Interfaces/LootableInterfaceExtended.h"

#include "InventoryUtilities.h"
#include "Character/CharacterBase.h"
#include "Interfaces/PurseInterface.h"
#include "Player/PluginIntegrationPlayerController.h"
#include "Utilities/ChatUtilities.h"

void ILootableInterfaceExtended::StartLooting(AActor* Looter)
{
	if (GetIsBeingLooted())
		return;

	IPurseInterface* Purse = Cast<IPurseInterface>(Looter);
	if (!Purse)
		return;

	const ACharacterBase* Character = Cast<ACharacterBase>(Looter);
	if (!Character)
		return;

	APluginIntegrationPlayerController* Controller = Cast<APluginIntegrationPlayerController>(
		Character->GetController());
	if (!Controller)
		return;

	SetIsBeingLooted(true);

	const FCoinValue& CoinValue = GetCoinComponent()->GetPurseContent();
	const float TotalCoinValue = CoinValue.ToFloat();

	if (TotalCoinValue != 0.f)
	{
		if (Controller->GetGroupID() > 0 && Controller->GetGroupMembers().Num())
		{
			const float CoinPart = TotalCoinValue / Controller->GetGroupMembers().Num();

			for (APluginIntegrationCharacter* GroupMember : Controller->GetGroupMembers())
			{
				FCoinValue LocalCoinValue(CoinPart);
				GroupMember->ReceiveCoinAmount(LocalCoinValue);
				APluginIntegrationPlayerController* GroupMemberController = Cast<APluginIntegrationPlayerController>(
					GroupMember->GetController());
				UChatUtilities::AddChatData(GroupMemberController, EChatColor::White, EMessageCategories::World,
				                            "You received your share of " + UInventoryUtilities::GetCoinValueAsString(
					                            LocalCoinValue));
			}
		}
		else

		{
			Purse->ReceiveCoinAmount(CoinValue);
			UChatUtilities::AddChatData(Controller, EChatColor::White, EMessageCategories::World,
			                            "You have found " + UInventoryUtilities::GetCoinValueAsString(CoinValue));
			GetCoinComponent()->ClearPurse();
		}
	}
}
