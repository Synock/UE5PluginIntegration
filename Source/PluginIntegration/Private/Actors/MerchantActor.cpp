// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/MerchantActor.h"

#include "Components/CoinComponent.h"
#include "Components/MerchantComponent.h"
#include "Net/UnrealNetwork.h"

AMerchantActor::AMerchantActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;

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

UWorld* AMerchantActor::GetMerchantWorldContext() const
{
	return GetWorld();
}

//----------------------------------------------------------------------------------------------------------------------

FString AMerchantActor::GetMerchantName() const
{
	return MerchantName;
}

//----------------------------------------------------------------------------------------------------------------------

float AMerchantActor::GetMerchantRatio() const
{
	return MerchantRatio;
}

//----------------------------------------------------------------------------------------------------------------------

UCoinComponent* AMerchantActor::GetCoinComponent()
{
	return CashContent_Merchant;
}

//----------------------------------------------------------------------------------------------------------------------

UMerchantComponent* AMerchantActor::GetMerchantComponent()
{
	return MerchantComponent;
}

//----------------------------------------------------------------------------------------------------------------------

UCoinComponent* AMerchantActor::GetCoinComponentConst() const
{
	return CashContent_Merchant;
}

//----------------------------------------------------------------------------------------------------------------------

UMerchantComponent* AMerchantActor::GetMerchantComponentConst() const
{
	return MerchantComponent;
}

//----------------------------------------------------------------------------------------------------------------------

FOnMerchantDynamicPoolChangedDelegate& AMerchantActor::GetMerchantDispatcher() const
{
	return MerchantComponent->MerchantPoolDispatcher;
}

//----------------------------------------------------------------------------------------------------------------------

void AMerchantActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Here we list the variables we want to replicate + a condition if wanted
	DOREPLIFETIME(AMerchantActor, CashContent_Merchant);
	DOREPLIFETIME(AMerchantActor, MerchantComponent);
	DOREPLIFETIME(AMerchantActor, MerchantName);
	DOREPLIFETIME(AMerchantActor, MerchantRatio);
}
