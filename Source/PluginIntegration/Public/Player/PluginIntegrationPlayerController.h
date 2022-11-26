// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/PlayerChatInterface.h"
#include "UI/HUDWidget.h"
#include "PluginIntegrationPlayerController.generated.h"

/** Forward declaration to improve compiling times */
class UNiagaraSystem;

UCLASS()
class APluginIntegrationPlayerController : public APlayerController, public IPlayerChatInterface
{
	GENERATED_BODY()

public:

	APluginIntegrationPlayerController();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	//This is based more or less on the method showed in GASDocumentation (https://github.com/tranek/GASDocumentation)
	void CreateHUD();

protected:

	virtual void OnRep_PlayerState() override;

	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
	void OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location);
	void OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location);


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	TSubclassOf<UHUDWidget> UIHUDWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UHUDWidget* UIHUDWidget = nullptr;


	//------------------------------------------------------------------------------------------------------------------
	// Chat
	//------------------------------------------------------------------------------------------------------------------

	virtual FString GetChatName() const override;

	virtual IHUDChatInterface* GetChatHUD() override;

	UFUNCTION(Client, unreliable, Category = "Chat|Display")
	virtual void Client_AddChatData(EChatColor Color, EMessageCategories Category, const FString& Message) override;

	UFUNCTION(Client, unreliable, Category = "Chat|Display")
	virtual void Client_AddChatDataType(EGlobalMessageType Type, const FString& Message) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_AreaSpeak(const FString& Message, float Range = 500.) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_ShoutSpeak(const FString& Message) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_OOCSpeak(const FString& Message) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_GroupSpeak(const FString& Message) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_RaidSpeak(const FString& Message) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_GuildSpeak(const FString& Message) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_AuctionSpeak(const FString& Message) override;

	UFUNCTION(Server, reliable, Category = "Chat|Talk")
	virtual void Server_TellSpeak(const FString& TargetName, const FString& Message) override;

	virtual bool IsInGroup() const override;

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void AddQuestProgressChatData();

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void AddNewQuestChatData();

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void AddItemLootNotification(int32 ItemID);

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void AddItemRewardNotification(int32 ItemID);

	UFUNCTION(BlueprintCallable, Category="Chat")
	void GetMOTD();

private:
	bool bInputPressed; // Input is bring pressed
	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed

	FString Name = GetName();

	int32 GroupID = -1;
};


