// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SilentMerchantActor.h"

#include "Actors/MerchantActor.h"
#include "Character/CharacterBase.h"
#include "Components/CoinComponent.h"
#include "Components/MerchantComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/PluginIntegrationPlayerController.h"

ASilentMerchantActor::ASilentMerchantActor()
{
	PrimaryActorTick.bCanEverTick = false;

	if (HasAuthority())
	{
		MerchantComponent = CreateDefaultSubobject<UMerchantComponent>("MerchantComponent");
		MerchantComponent->SetNetAddressable(); // Make DSO components net addressable
		MerchantComponent->SetIsReplicated(true); // Enable replication by default

		CashContent_Merchant = CreateDefaultSubobject<UCoinComponent>("CashContent_Merchant");
		CashContent_Merchant->SetNetAddressable(); // Make DSO components net addressable
		CashContent_Merchant->SetIsReplicated(true); // Enable replication by default
	}
}

// Called when the game starts or when spawned
void ASilentMerchantActor::BeginPlay()
{
	Super::BeginPlay();

}

UWorld* ASilentMerchantActor::GetMerchantWorldContext() const
{
	return GetWorld();
}

FString ASilentMerchantActor::GetMerchantName() const
{
	return MerchantName;
}

float ASilentMerchantActor::GetMerchantRatio() const
{
	return MerchantRatio;
}

UCoinComponent* ASilentMerchantActor::GetCoinComponent()
{
	return CashContent_Merchant;
}

UMerchantComponent* ASilentMerchantActor::GetMerchantComponent()
{
	return MerchantComponent;
}

UCoinComponent* ASilentMerchantActor::GetCoinComponentConst() const
{
	return CashContent_Merchant;
}

UMerchantComponent* ASilentMerchantActor::GetMerchantComponentConst() const
{
	return MerchantComponent;
}

FOnMerchantDynamicPoolChangedDelegate& ASilentMerchantActor::GetMerchantDispatcher() const
{
	return MerchantComponent->MerchantPoolDispatcher;
}

void ASilentMerchantActor::Interact(AActor* Interactor)
{
	const ACharacterBase* PlayerCharacter = Cast<ACharacterBase>(Interactor);
	if (!PlayerCharacter)
		return;

	APluginIntegrationPlayerController* PlayerController = Cast<APluginIntegrationPlayerController>(PlayerCharacter->GetController());
	if (!PlayerController)
		return;

	if (const float Distance = FVector::Distance(GetActorLocation(), Interactor->GetActorLocation()); Distance > MaxInteractionDistance)
		return;

	PlayerController->MerchantTrade(this);
}

//----------------------------------------------------------------------------------------------------------------------

void ASilentMerchantActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASilentMerchantActor, CashContent_Merchant);
	DOREPLIFETIME(ASilentMerchantActor, MerchantComponent);
	DOREPLIFETIME(ASilentMerchantActor, MerchantName);
	DOREPLIFETIME(ASilentMerchantActor, MerchantRatio);
}