// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/PluginIntegrationPlayerController.h"

#include "InventoryUtilities.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/PluginIntegrationCharacter.h"
#include "Engine/World.h"
#include "GameFramework/GameModeBase.h"
#include "Interfaces/GameModeChatInterface.h"

APluginIntegrationPlayerController::APluginIntegrationPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

//----------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// For edge cases where the PlayerState is repped before the Hero is possessed.
	CreateHUD();
}

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
		GetWorld()->GetTimerManager().SetTimer(SingleShotHandle, this, &APluginIntegrationPlayerController::CreateHUD, 1.0f,
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

	if(bInputPressed)
	{
		FollowTime += DeltaTime;

		// Look for the touch location
		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		if(bIsTouch)
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
		if(MyPawn)
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

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &APluginIntegrationPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &APluginIntegrationPlayerController::OnSetDestinationReleased);

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
	if(FollowTime <= ShortPressThreshold)
	{
		// We look for the location in the world where the player has pressed the input
		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		HitLocation = Hit.Location;

		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, HitLocation);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, HitLocation, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
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
	check(UIHUDWidget);//this would be bad news

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

	FInventoryItem LootedItem = UInventoryUtilities::GetItemFromID(ItemID, GetWorld());

	const FString FinalStringSolo = "You have looted a " + FString("<Item id=\"") + FString::FormatAsNumber(ItemID) +
		FString("\">") + LootedItem.Name + FString("</> ");
	Client_AddChatData(EChatColor::White, EMessageCategories::Misc, FinalStringSolo);
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::AddItemRewardNotification(int32 ItemID)
{
	if (ItemID <= 0)
		return;

	FInventoryItem LootedItem = UInventoryUtilities::GetItemFromID(ItemID, GetWorld());

	const FString FinalStringSolo = "You have received a " + FString("<Item id=\"") + FString::FormatAsNumber(ItemID) +
		FString("\">") + LootedItem.Name + FString("</> ");
	AddChatData(EChatColor::White, EMessageCategories::Misc, FinalStringSolo);
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::GetMOTD()
{
	Client_AddChatDataType(EGlobalMessageType::ServerAnnouncement, "Welcome to the Plugin Integration example. check out https://github.com/Synock/ for more info");
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

void APluginIntegrationPlayerController::Client_AddChatData_Implementation(EChatColor Color, EMessageCategories Category, const FString& Message)
{
	AddChatData(Color, Category, Message);
}

//------------------------------------------------------------------------------------------------------------------

void APluginIntegrationPlayerController::Client_AddChatDataType_Implementation(EGlobalMessageType Type,
	const FString& Message)
{
	AddChatDataType(Type, Message);
}
