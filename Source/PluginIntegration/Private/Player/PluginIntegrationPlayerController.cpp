// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/PluginIntegrationPlayerController.h"

#include "InventoryUtilities.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/PluginIntegrationCharacter.h"
#include "Components/BankComponent.h"
#include "Components/LootPoolComponent.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/DialogInterface.h"
#include "Interfaces/GameModeChatInterface.h"
#include "Interfaces/LootableInterface.h"
#include "Interfaces/MerchantInterface.h"
#include "Items/InventoryItemBag.h"
#include "Net/UnrealNetwork.h"
#include <TimerManager.h>

#include "Components/KeyringComponent.h"
#include "Game/PluginIntegrationGameMode.h"

APluginIntegrationPlayerController::APluginIntegrationPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	if (HasAuthority())
	{

		StagingAreaCoin = CreateDefaultSubobject<UCoinComponent>("StagingAreaCoin");
		StagingAreaCoin->SetNetAddressable();
		StagingAreaCoin->SetIsReplicated(true);

		StagingAreaItems = CreateDefaultSubobject<UStagingAreaComponent>("StagingAreaItems");
		StagingAreaItems->SetNetAddressable();
		StagingAreaItems->SetIsReplicated(true);

		BankCoin = CreateDefaultSubobject<UCoinComponent>("BankCoin");
		BankCoin->SetNetAddressable();
		BankCoin->SetIsReplicated(true);

		BankComponent = CreateDefaultSubobject<UBankComponent>("BankComponent");
		BankComponent->SetNetAddressable();
		BankComponent->SetIsReplicated(true);
	}
}

//----------------------------------------------------------------------------------------------------------------------

TArray<APluginIntegrationCharacter*> APluginIntegrationPlayerController::GetGroupMembers()
{
	return {};
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// For edge cases where the PlayerState is repped before the Hero is possessed.
	CreateHUD();
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::CreateHUD()
{
	// Only create once
	if (UIHUDWidget)
		return;

	if (!UIHUDWidgetClass)
	{
		UE_LOG(LogTemp, Error,
		       TEXT("%s() Missing UIHUDWidgetClass. Please fill in on the Blueprint of the PlayerController."),
		       *FString(__FUNCTION__));
		return;
	}

	// Only create a HUD for local player
	if (!IsLocalPlayerController())
		return;

	//somehow, sometime the creation process is badly timed and we end up without HUD
	if (!GetCharacter())
	{
		FTimerHandle SingleShotHandle;
		GetWorld()->GetTimerManager().SetTimer(SingleShotHandle, this, &APluginIntegrationPlayerController::CreateHUD,
		                                       1.0f,
		                                       false, 1.0f);
		return;
	}

	UIHUDWidget = CreateWidget<UHUDWidget>(this, UIHUDWidgetClass);
	UIHUDWidget->AddToViewport();

	GetMOTD();
}

void APluginIntegrationPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (bInputPressed)
	{
		FollowTime += DeltaTime;

		// Look for the touch location
		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		if (bIsTouch)
		{
			GetHitResultUnderFinger(ETouchIndex::Touch1, ECC_Visibility, true, Hit);
		}
		else
		{
			GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		}
		HitLocation = Hit.Location;

		// Direct the Pawn towards that location
		APawn* const MyPawn = GetPawn();
		if (MyPawn)
		{
			FVector WorldDirection = (HitLocation - MyPawn->GetActorLocation()).GetSafeNormal();
			MyPawn->AddMovementInput(WorldDirection, 1.f, false);
		}
	}
	else
	{
		FollowTime = 0.f;
	}
}

void APluginIntegrationPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this,
	                           &APluginIntegrationPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this,
	                           &APluginIntegrationPlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &APluginIntegrationPlayerController::OnTouchPressed);
	InputComponent->BindTouch(EInputEvent::IE_Released, this, &APluginIntegrationPlayerController::OnTouchReleased);
}

void APluginIntegrationPlayerController::OnSetDestinationPressed()
{
	// We flag that the input is being pressed
	bInputPressed = true;
	// Just in case the character was moving because of a previous short press we stop it
	StopMovement();
}

void APluginIntegrationPlayerController::OnSetDestinationReleased()
{
	// Player is no longer pressing the input
	bInputPressed = false;

	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We look for the location in the world where the player has pressed the input
		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		HitLocation = Hit.Location;

		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, HitLocation);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, HitLocation, FRotator::ZeroRotator,
		                                               FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}
}

void APluginIntegrationPlayerController::OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	bIsTouch = true;
	OnSetDestinationPressed();
}

void APluginIntegrationPlayerController::OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

//------------------------------------------------------------------------------------------------------------------

FString APluginIntegrationPlayerController::GetChatName() const
{
	return Name;
}

//------------------------------------------------------------------------------------------------------------------

IHUDChatInterface* APluginIntegrationPlayerController::GetChatHUD()
{
	check(UIHUDWidget); //this would be bad news

	return UIHUDWidget;
}

//------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::IsInGroup() const
{
	return GroupID > 0;
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::AddQuestProgressChatData()
{
	AddChatData(EChatColor::Yellow, EMessageCategories::World, "Your quest journal has been updated.");
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::AddNewQuestChatData()
{
	AddChatData(EChatColor::Yellow, EMessageCategories::World, "You begin a new quest.");
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::AddItemLootNotification(int32 ItemID)
{
	if (ItemID <= 0)
		return;

	UInventoryItemBase* LootedItem = UInventoryUtilities::GetItemFromID(ItemID, GetWorld());

	const FString FinalStringSolo = "You have looted a " + FString("<Item id=\"") + FString::FormatAsNumber(ItemID) +
		FString("\">") + LootedItem->Name + FString("</> ");
	Client_AddChatData(EChatColor::White, EMessageCategories::Misc, FinalStringSolo);
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::AddItemRewardNotification(int32 ItemID)
{
	if (ItemID <= 0)
		return;

	UInventoryItemBase* LootedItem = UInventoryUtilities::GetItemFromID(ItemID, GetWorld());

	const FString FinalStringSolo = "You have received a " + FString("<Item id=\"") + FString::FormatAsNumber(ItemID) +
		FString("\">") + LootedItem->Name + FString("</> ");
	AddChatData(EChatColor::White, EMessageCategories::Misc, FinalStringSolo);
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::GetMOTD()
{
	Client_AddChatDataType(EGlobalMessageType::ServerAnnouncement,
	                       "Welcome to the Plugin Integration example. Check out https://github.com/Synock/ for more info.");

	Client_AddChatDataType(EGlobalMessageType::ServerAnnouncement,
	                       "Press I to display Inventory");

	Client_AddChatDataType(EGlobalMessageType::ServerAnnouncement,
	                       "Press E to interact with stuff");
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::DialogInteract(AActor* Target)
{
	IDialogInterface* TargetInterface = Cast<IDialogInterface>(Target);
	if (TargetInterface && TargetInterface->HasDialog() && !UIHUDWidget->IsDialogDisplayed())
	{
		UIHUDWidget->Execute_CreateDialogWindow(UIHUDWidget, TargetInterface->GetDialogComponent(), Target);
	}
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::ForceDisplayTextInDialog_Implementation(const FString& TextString)
{
	UIHUDWidget->ForceDisplayTextInDialog(TextString);
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_TellSpeak_Implementation(const FString& TargetName,
                                                                         const FString& Message)
{
	if (IGameModeChatInterface* GameModeChat = Cast<IGameModeChatInterface>(GetWorld()->GetAuthGameMode()))
		GameModeChat->TellSpeak(this, TargetName, Message);
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_AuctionSpeak_Implementation(const FString& Message)
{
	if (IGameModeChatInterface* GameModeChat = Cast<IGameModeChatInterface>(GetWorld()->GetAuthGameMode()))
		GameModeChat->AuctionSpeak(this, Message);
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_GuildSpeak_Implementation(const FString& Message)
{
	if (IGameModeChatInterface* GameModeChat = Cast<IGameModeChatInterface>(GetWorld()->GetAuthGameMode()))
		GameModeChat->GuildSpeak(this, Message);
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_RaidSpeak_Implementation(const FString& Message)
{
	if (IGameModeChatInterface* GameModeChat = Cast<IGameModeChatInterface>(GetWorld()->GetAuthGameMode()))
		GameModeChat->RaidSpeak(this, Message);
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_GroupSpeak_Implementation(const FString& Message)
{
	if (IGameModeChatInterface* GameModeChat = Cast<IGameModeChatInterface>(GetWorld()->GetAuthGameMode()))
		GameModeChat->GroupSpeak(this, Message);
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_OOCSpeak_Implementation(const FString& Message)
{
	if (IGameModeChatInterface* GameModeChat = Cast<IGameModeChatInterface>(GetWorld()->GetAuthGameMode()))
		GameModeChat->OOCSpeak(this, Message);
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_ShoutSpeak_Implementation(const FString& Message)
{
	if (IGameModeChatInterface* GameModeChat = Cast<IGameModeChatInterface>(GetWorld()->GetAuthGameMode()))
		GameModeChat->ShoutSpeak(this, Message);
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_AreaSpeak_Implementation(const FString& Message, float Range)
{
	if (IGameModeChatInterface* GameModeChat = Cast<IGameModeChatInterface>(GetWorld()->GetAuthGameMode()))
		GameModeChat->AreaSpeak(this, Message, Range);
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Client_AddChatData_Implementation(
	EChatColor Color, EMessageCategories Category, const FString& Message)
{
	AddChatData(Color, Category, Message);
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Client_AddChatDataType_Implementation(EGlobalMessageType Type,
                                                                               const FString& Message)
{
	AddChatDataType(Type, Message);
}


//----------------------------------------------------------------------------------------------------------------------

UInventoryComponent* APluginIntegrationPlayerController::GetInventoryComponent()
{
	return GetMainPlayerCharacter()->GetInventoryComponent();
}

//----------------------------------------------------------------------------------------------------------------------

const UInventoryComponent* APluginIntegrationPlayerController::GetInventoryComponentConst() const
{
	return GetMainPlayerCharacter()->GetInventoryComponentConst();
}

//----------------------------------------------------------------------------------------------------------------------

UCoinComponent* APluginIntegrationPlayerController::GetCoinComponent()
{
	return GetMainPlayerCharacter()->GetPurseComponent();
}

//----------------------------------------------------------------------------------------------------------------------

const UCoinComponent* APluginIntegrationPlayerController::GetCoinComponentConst() const
{
	return GetMainPlayerCharacter()->GetPurseComponentConst();
}

//----------------------------------------------------------------------------------------------------------------------

AActor* APluginIntegrationPlayerController::GetInventoryOwningActor()
{
	return GetMainPlayerCharacter();
}

//----------------------------------------------------------------------------------------------------------------------

AActor const* APluginIntegrationPlayerController::GetInventoryOwningActorConst() const
{
	return GetMainPlayerCharacter();
}

//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::GetTransactionBoolean()
{
	return TransactionBoolean;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::SetTransactionBoolean(bool Value)
{
	TransactionBoolean = Value;
}

//----------------------------------------------------------------------------------------------------------------------

AActor* APluginIntegrationPlayerController::GetMerchantActor()
{
	return MerchantActor;
}

//----------------------------------------------------------------------------------------------------------------------

const AActor* APluginIntegrationPlayerController::GetMerchantActorConst() const
{
	return MerchantActor;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::SetMerchantActor(AActor* Actor)
{
	MerchantActor = Actor;
}

//----------------------------------------------------------------------------------------------------------------------

AActor* APluginIntegrationPlayerController::GetLootedActor()
{
	return LootedActor;
}

//----------------------------------------------------------------------------------------------------------------------

const AActor* APluginIntegrationPlayerController::GetLootedActorConst() const
{
	return LootedActor;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::SetLootedActor(AActor* Actor)
{
	LootedActor = Actor;
}

//----------------------------------------------------------------------------------------------------------------------

IInventoryHUDInterface* APluginIntegrationPlayerController::GetInventoryHUDInterface()
{
	return UIHUDWidget;
}

//----------------------------------------------------------------------------------------------------------------------

UObject* APluginIntegrationPlayerController::GetInventoryHUDObject()
{
	return UIHUDWidget;
}

//----------------------------------------------------------------------------------------------------------------------

UCoinComponent* APluginIntegrationPlayerController::GetStagingAreaCoin()
{
	return StagingAreaCoin;
}

//----------------------------------------------------------------------------------------------------------------------

UStagingAreaComponent* APluginIntegrationPlayerController::GetStagingAreaItems()
{
	return StagingAreaItems;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::OnRep_LootedActor()
{
	if (LootedActor)
	{
		UE_LOG(LogTemp, Log, TEXT("OnRep_LootedActor %s"), *LootedActor->GetName());
		GetInventoryHUDInterface()->Execute_DisplayLootScreen(GetInventoryHUDObject(), LootedActor);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Clearing loot action"));
		GetInventoryHUDInterface()->Execute_HideLootScreen(GetInventoryHUDObject());
	}
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::OnRep_MerchantActor()
{
	if (MerchantActor)
	{
		const IMerchantInterface* MerchantInterface = Cast<IMerchantInterface>(MerchantActor);

		UE_LOG(LogTemp, Log, TEXT("OnRep_MerchantActor %s"), *MerchantInterface->GetMerchantName());
		GetInventoryHUDInterface()->Execute_DisplayMerchantScreen(GetInventoryHUDObject(), Cast<AActor>(MerchantActor));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Clearing Merchant Action"));
		GetInventoryHUDInterface()->Execute_HideMerchantScreen(GetInventoryHUDObject());
	}
}
//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_PlayerRemoveKeyToInventory_Implementation(int32 KeyId)
{
	Internal_PlayerRemoveKeyToInventory(KeyId);
}
//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_PlayerRemoveKeyToInventory_Validate(int32 KeyId)
{
	return GetKeyring()->HasKey(KeyId);
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_PlayerAddKeyFromInventory_Implementation(int32 InTopLeft,
	EBagSlot InSlot, int32 InItemId)
{
	Internal_PlayerAddKeyFromInventory(InTopLeft, InSlot, InItemId);
}
//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_PlayerAddKeyFromInventory_Validate(int32 InTopLeft, EBagSlot InSlot,
	int32 InItemId)
{
	return PlayerGetItem(InTopLeft, InSlot) == InItemId;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_DropItemFromInventory_Implementation(int32 TopLeft, EBagSlot Slot,
                                                                                     FVector DropLocation)
{
	const int32 ItemID = PlayerGetItem(TopLeft, Slot);
	PlayerRemoveItem(TopLeft, Slot);
	Cast<APluginIntegrationGameMode>(GetWorld()->GetAuthGameMode())->SpawnItemFromActor(GetMainPlayerCharacter(), ItemID, DropLocation, true);
}

bool APluginIntegrationPlayerController::Server_DropItemFromInventory_Validate(int32 TopLeft, EBagSlot Slot,
	FVector DropLocation)
{
	return true;
}

void APluginIntegrationPlayerController::Server_DropItemFromEquipment_Implementation(EEquipmentSlot Slot,
                                                                                     FVector DropLocation)
{
	const int32 ItemID = GetEquipmentForInventory()->GetEquippedItem(Slot)->ItemID;
	GetEquipmentForInventory()->UnequipItem(Slot);
	Cast<APluginIntegrationGameMode>(GetWorld()->GetAuthGameMode())->SpawnItemFromActor(GetMainPlayerCharacter(), ItemID, DropLocation, true);
}

bool APluginIntegrationPlayerController::Server_DropItemFromEquipment_Validate(EEquipmentSlot Slot,
	FVector DropLocation)
{
	return true;
}
//----------------------------------------------------------------------------------------------------------------------

UCoinComponent* APluginIntegrationPlayerController::GetBankCoin() const
{
	return BankCoin;
}

//----------------------------------------------------------------------------------------------------------------------

UBankComponent* APluginIntegrationPlayerController::GetBankComponent() const
{
	return BankComponent;
}

//----------------------------------------------------------------------------------------------------------------------
// Merchant related functions -- Server
//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_MerchantTrade_Implementation(AActor* InputMerchantActor)
{
	if (InputMerchantActor->Implements<UMerchantInterface>())
	{
		MerchantActor = InputMerchantActor;
	}
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_StopMerchantTrade_Implementation()
{
	if (MerchantActor)
		MerchantActor = nullptr;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_PlayerBuyFromMerchant_Implementation(int32 ItemId,
	const FCoinValue& Price)
{
	auto Char = GetMainPlayerCharacter();
	IMerchantInterface* MerchantInterface = Cast<IMerchantInterface>(MerchantActor);

	//this can happen during race condition if two players buy out the same item at (almost) the same time
	if (!MerchantInterface->HasItem(ItemId))
		return;

	//Remove Coin from the player
	GetCoinComponent()->PayAndAdjust(Price);

	//Remove item from the merchant if needed
	MerchantInterface->RemoveItemAmountIfNeeded(ItemId);

	//Add item to the player
	EEquipmentSlot TriedSlot = EEquipmentSlot::Unknown;
	EBagSlot TriedBag = EBagSlot::Unknown;
	int32 InTopLeft = -1;

	if (PlayerTryAutoLootFunction(ItemId, TriedSlot, InTopLeft, TriedBag))
	{
		if (TriedSlot != EEquipmentSlot::Unknown)
		{
			Char->EquipItem(TriedSlot, ItemId);
		}
		else if (TriedBag != EBagSlot::Unknown)
		{
			PlayerAddItem(InTopLeft, TriedBag, ItemId);
		}
		else
			check(nullptr); //hopefully we never go here
	}

	//Add Coin to the merchant
	MerchantInterface->ReceiveCoin(Price);
}

//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_PlayerBuyFromMerchant_Validate(int32 ItemId,
                                                                               const FCoinValue& Price)
{
	if (!MerchantActor)
		return false;

	if (!PlayerCanPayAmount(Price)) //Player must have the asked price
		return false;
	//Merchant needs to have the item somewhere (or not, see race condition)

	//Player must have enough room

	return true;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_PlayerSellToMerchant_Implementation(
	EBagSlot OutSlot, int32 ItemId, int32 TopLeft,
	const FCoinValue& Price)
{
	IMerchantInterface* MerchantInterface = Cast<IMerchantInterface>(MerchantActor);
	if (!MerchantInterface->CanPayAmount(Price))
		return;

	//Player item is removed
	PlayerRemoveItem(TopLeft, OutSlot);

	//Merchant cash is reduced
	MerchantInterface->PayCoin(Price);

	//Merchant item is added
	MerchantInterface->AddDynamicItem(ItemId);

	//Player cash is increased
	GetCoinComponent()->AddCoins(Price);
}

//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_PlayerSellToMerchant_Validate(
	EBagSlot OutSlot, int32 ItemId, int32 TopLeft,
	const FCoinValue& Price)
{
	if (!MerchantActor)
		return false;

	//Player needs to have the item
	const int32 ActualItemID = PlayerGetItem(TopLeft, OutSlot);

	if (ActualItemID != ItemId)
		return false;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Loot related functions -- Server
//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_LootActor_Validate(AActor* InputLootedActor)
{
	if (LootedActor)
		return false;

	if (InputLootedActor == nullptr)
		return false;

	if (ILootableInterface* Lootable = Cast<ILootableInterface>(InputLootedActor); Lootable->GetIsBeingLooted())
		return false;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_LootActor_Implementation(AActor* InputLootedActor)
{
	if (!LootedActor)
	{
		if (ILootableInterface* LootInterface = Cast<ILootableInterface>(InputLootedActor))
		{
			LootedActor = InputLootedActor;
			LootInterface->StartLooting(GetPawn());
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_StopLooting_Implementation()
{
	if (LootedActor)
	{
		ILootableInterface* Lootable = Cast<ILootableInterface>(LootedActor);
		Lootable->StopLooting(GetPawn());
		LootedActor = nullptr;
	}
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_PlayerLootItem_Implementation(
	int32 InTopLeft, EBagSlot InSlot, int32 InItemId,
	int32 OutTopLeft)
{
	ILootableInterface* Loot = Cast<ILootableInterface>(LootedActor);
	Loot->RemoveItem(OutTopLeft);
	PlayerAddItem(InTopLeft, InSlot, InItemId);
}

//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_PlayerLootItem_Validate(int32 InTopLeft, EBagSlot InSlot,
                                                                        int32 InItemId,
                                                                        int32 OutTopLeft)
{
	const ILootableInterface* Loot = Cast<ILootableInterface>(LootedActor);
	const int32 ActualItemID = Loot->GetItemData(OutTopLeft);
	return ActualItemID == InItemId;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_PlayerEquipItemFromLoot_Implementation(
	int32 InItemId, EEquipmentSlot InSlot,
	int32 OutTopLeft)
{
	ILootableInterface* Loot = Cast<ILootableInterface>(LootedActor);
	Loot->RemoveItem(OutTopLeft);
	GetEquipmentForInventory()->EquipItem(InSlot, InItemId);
}

//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_PlayerEquipItemFromLoot_Validate(int32 InItemId, EEquipmentSlot InSlot,
	int32 OutTopLeft)
{
	//more check needed
	const ILootableInterface* Loot = Cast<ILootableInterface>(LootedActor);
	const int32 ActualItemID = Loot->GetItemData(OutTopLeft);
	return ActualItemID == InItemId;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_PlayerAutoLootAll_Implementation()
{
	if (!LootedActor)
		return;

	TArray<FMinimalItemStorage> CopyArray = Cast<ILootableInterface>(LootedActor)->GetLootPoolComponent()->
		GetBagConst();

	EEquipmentSlot TriedSlot = EEquipmentSlot::Unknown;
	EBagSlot TriedBag = EBagSlot::Unknown;
	int32 InTopLeft = -1;

	TArray<EEquipmentSlot> ForbiddenSlot;
	TArray<FMinimalItemStorage> ItemToAdd;

	for (const auto& Item : CopyArray)
	{
		if (!PlayerTryAutoLootFunction(Item.ItemID, TriedSlot, InTopLeft, TriedBag))
			break;

		if (TriedSlot != EEquipmentSlot::Unknown)
		{
			Server_PlayerEquipItemFromLoot(Item.ItemID, TriedSlot, Item.TopLeftID);
		}
		else if (TriedBag != EBagSlot::Unknown)
		{
			Server_PlayerLootItem(InTopLeft, TriedBag, Item.ItemID, Item.TopLeftID);
		}
	}
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_PlayerMoveItem_Implementation(
	int32 InTopLeft, EBagSlot InSlot, int32 InItemId,
	int32 OutTopLeft, EBagSlot OutSlot)
{
	PlayerRemoveItem(OutTopLeft, OutSlot);
	PlayerAddItem(InTopLeft, InSlot, InItemId);
}

//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_PlayerMoveItem_Validate(int32 InTopLeft, EBagSlot InSlot,
                                                                        int32 InItemId,
                                                                        int32 OutTopLeft, EBagSlot OutSlot)
{
	return PlayerGetItem(OutTopLeft, OutSlot) == InItemId;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_PlayerUnequipItem_Implementation(
	int32 InTopLeft, EBagSlot InSlot, int32 InItemId,
	EEquipmentSlot OutSlot)
{
	GetEquipmentForInventory()->HandleUnEquipmentEffect(
		OutSlot, GetEquipmentForInventory()->GetEquipmentComponent()->GetItemAtSlot(OutSlot));
	GetEquipmentForInventory()->UnequipItem(OutSlot);
	PlayerAddItem(InTopLeft, InSlot, InItemId);
}

//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_PlayerUnequipItem_Validate(
	int32 InTopLeft, EBagSlot InSlot, int32 InItemId,
	EEquipmentSlot OutSlot)
{
	const UInventoryItemEquipable* ConsideredItem = GetEquipmentForInventory()->GetEquippedItem(OutSlot);
	if (!ConsideredItem || ConsideredItem->ItemID <= 0)
		return false;

	if (const UInventoryItemBag* Bag = Cast<UInventoryItemBag>(ConsideredItem))
	{
		if (GetInventoryComponent()->GetBagConst(UInventoryComponent::GetBagSlotFromInventory(OutSlot)).Num() > 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Unequip item was unempty bag slot %d, item %d"), OutSlot, InItemId);
			return false;
		}
	}

	return ConsideredItem->ItemID == InItemId;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_PlayerEquipItemFromInventory_Implementation(
	int32 InItemId, EEquipmentSlot InSlot, int32 OutTopLeft, EBagSlot OutSlot)
{
	GetEquipmentForInventory()->EquipItem(InSlot, InItemId);
	PlayerRemoveItem(OutTopLeft, OutSlot);
}

//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_PlayerEquipItemFromInventory_Validate(
	int32 InItemId, EEquipmentSlot InSlot, int32 OutTopLeft, EBagSlot OutSlot)
{
	UE_LOG(LogTemp, Log, TEXT("Validating item equipment %d"), InItemId);

	if (InItemId <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Input item is invalid %d"), InItemId);
		return false;
	}

	if (PlayerGetItem(OutTopLeft, OutSlot) != InItemId)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find item in player inventory %d"), InItemId);
		return false;
	}

	const UInventoryItemEquipable* LocalItem = Cast<UInventoryItemEquipable>(
		UInventoryUtilities::GetItemFromID(InItemId, GetWorld()));

	if (!LocalItem || LocalItem->ItemID <= 0)
		return false;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------


void APluginIntegrationPlayerController::Server_PlayerSwapEquipment_Implementation(
	int32 DroppedItemId, EEquipmentSlot DroppedInSlot,
	int32 SwappedItemId,
	EEquipmentSlot DraggedOutSlot)
{
	const UInventoryItemEquipable* ItemToMove = GetEquipmentForInventory()->GetEquippedItem(DroppedInSlot);
	const UInventoryItemEquipable* DroppedItem = GetEquipmentForInventory()->GetEquippedItem(DraggedOutSlot);

	GetEquipmentForInventory()->GetEquipmentComponent()->RemoveItem(DroppedInSlot);
	GetEquipmentForInventory()->HandleUnEquipmentEffect(DroppedInSlot, ItemToMove);
	GetEquipmentForInventory()->GetEquipmentComponent()->EquipItem(DroppedItem, DroppedInSlot);
	GetEquipmentForInventory()->HandleEquipmentEffect(DroppedInSlot, DroppedItem);

	GetEquipmentForInventory()->GetEquipmentComponent()->RemoveItem(DraggedOutSlot);
	GetEquipmentForInventory()->HandleUnEquipmentEffect(DraggedOutSlot, DroppedItem);
	GetEquipmentForInventory()->GetEquipmentComponent()->EquipItem(ItemToMove, DraggedOutSlot);
	GetEquipmentForInventory()->HandleEquipmentEffect(DraggedOutSlot, ItemToMove);
}

//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_PlayerSwapEquipment_Validate(
	int32 DroppedItemId, EEquipmentSlot DroppedInSlot,
	int32 SwappedItemId,
	EEquipmentSlot DraggedOutSlot)
{
	UE_LOG(LogTemp, Log, TEXT("Validating equipment swap from %d item %d to %d item %d"), DraggedOutSlot, SwappedItemId,
	       DroppedInSlot, DroppedItemId);

	if (DroppedInSlot == EEquipmentSlot::Unknown)
		return false;

	if (DraggedOutSlot == EEquipmentSlot::Unknown)
		return false;

	if (GetEquipmentForInventory()->GetEquippedItem(DraggedOutSlot)->ItemID != DroppedItemId)
		return false;

	if (GetEquipmentForInventory()->GetEquippedItem(DroppedInSlot)->ItemID != SwappedItemId)
		return false;

	if (GetEquipmentForInventory()->GetEquippedItem(DraggedOutSlot)->TwoSlotsItem)
		return GetEquipmentForInventory()->GetEquippedItem(DroppedInSlot)->TwoSlotsItem;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_TransferCoinTo_Implementation(UCoinComponent* GivingComponent,
                                                                              UCoinComponent* ReceivingComponent,
                                                                              const FCoinValue& RemovedCoinValue,
                                                                              const FCoinValue& AddedCoinValue)
{
	GetMainPlayerCharacter()->TransferCoinsTo(GivingComponent, ReceivingComponent, RemovedCoinValue, AddedCoinValue);
}

//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_TransferCoinTo_Validate(UCoinComponent* GivingComponent,
                                                                        UCoinComponent* ReceivingComponent,
                                                                        const FCoinValue& RemovedCoinValue,
                                                                        const FCoinValue& AddedCoinValue)
{
	if (ReceivingComponent->GetOwner() != this && ReceivingComponent->GetOwner() != GetMainPlayerCharacter())
		return false;

	return true;
}


//----------------------------------------------------------------------------------------------------------------------


void APluginIntegrationPlayerController::Server_PlayerAutoEquipItem_Implementation(
	int32 InTopLeft, EBagSlot InSlot, int32 InItemId)
{
	EEquipmentSlot SlotToEquip;
	if (!PlayerTryAutoEquip(InItemId, SlotToEquip))
		return;

	Server_PlayerEquipItemFromInventory(InItemId, SlotToEquip, InTopLeft, InSlot);
}

//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_PlayerAutoEquipItem_Validate(
	int32 InTopLeft, EBagSlot InSlot, int32 InItemId)
{
	const int32 ActualItemID = PlayerGetItem(InTopLeft, InSlot);

	if (ActualItemID != InItemId)
		return false;

	return true;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_CancelStagingArea_Implementation()
{
	GetCoinComponent()->AddCoins(GetStagingAreaCoin()->GetPurseContent());
	GetStagingAreaCoin()->ClearPurse();

	for (const auto& Item : StagingAreaItems->GetStagingAreaItems())
	{
		EEquipmentSlot TriedSlot = EEquipmentSlot::Unknown;
		EBagSlot TriedBag = EBagSlot::Unknown;
		int32 InTopLeft = -1;

		if (!PlayerTryAutoLootFunction(Item, TriedSlot, InTopLeft, TriedBag))
		{
			UE_LOG(LogTemp, Error, TEXT("Cannot put item back"));
		}

		if (TriedSlot != EEquipmentSlot::Unknown)
		{
			GetEquipmentForInventory()->EquipItem(TriedSlot, Item);
		}
		else if (TriedBag != EBagSlot::Unknown)
		{
			PlayerAddItem(InTopLeft, TriedBag, Item);
		}
	}
	StagingAreaItems->ClearStagingArea();
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_TransferStagingToActor_Implementation(AActor* TargetActor)
{
	/*
	if (ANPC* NPC = Cast<ANPC>(TargetActor))
	{
		TArray<int32> RemainderItems;
		float RemainderCoins = 0.f;
		NPC->HandlePlayerGive(this, StagingAreaItems->GetStagingAreaItems(),
		                      GetStagingAreaCoin()->GetPurseContent().ToFloat(), RemainderItems, RemainderCoins);

		GetCoinComponent()->AddCoins(RemainderCoins);

		for (const auto& Item : RemainderItems)
		{
			EEquipmentSlot TriedSlot = EEquipmentSlot::Unknown;
			EBagSlot TriedBag = EBagSlot::Unknown;
			int32 InTopLeft = -1;

			if (!PlayerTryAutoLootFunction(Item, TriedSlot, InTopLeft, TriedBag))
			{
				UE_LOG(LogTemp, Error, TEXT("Cannot put item back"));
			}

			if (TriedSlot != EEquipmentSlot::Unknown)
			{
				GetEquipmentForInventory()->EquipItem(TriedSlot, Item);
			}
			else if (TriedBag != EBagSlot::Unknown)
			{
				PlayerAddItem(InTopLeft, TriedBag, Item);
			}
		}

		GetStagingAreaCoin()->ClearPurse();
		StagingAreaItems->ClearStagingArea();
	}*/
}

//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_TransferStagingToActor_Validate(AActor* TargetActor)
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_MoveEquipmentToStagingArea_Implementation(
	int32 InItemId, EEquipmentSlot OutSlot)
{
	StagingAreaItems->AddItemToStagingArea(InItemId);
	GetEquipmentForInventory()->UnequipItem(OutSlot);
}

//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_MoveEquipmentToStagingArea_Validate(
	int32 InItemId, EEquipmentSlot OutSlot)
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Server_MoveInventoryItemToStagingArea_Implementation(
	int32 InItemId, int32 OutTopLeft, EBagSlot OutSlot)
{
	StagingAreaItems->AddItemToStagingArea(InItemId);
	PlayerRemoveItem(OutTopLeft, OutSlot);
}

//----------------------------------------------------------------------------------------------------------------------

bool APluginIntegrationPlayerController::Server_MoveInventoryItemToStagingArea_Validate(
	int32 InItemId, int32 OutTopLeft, EBagSlot OutSlot)
{
	return true;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(APluginIntegrationPlayerController, LootedActor, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APluginIntegrationPlayerController, MerchantActor, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APluginIntegrationPlayerController, StagingAreaCoin, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APluginIntegrationPlayerController, StagingAreaItems, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APluginIntegrationPlayerController, BankComponent, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APluginIntegrationPlayerController, BankCoin, COND_OwnerOnly);
}
