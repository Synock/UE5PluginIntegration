// Copyright 2023 Maximilien (Synock) Guislain


#include "Actors/LootableActor.h"
#include "Components/CoinComponent.h"
#include "Components/LootPoolComponent.h"
#include "Net/UnrealNetwork.h"


ALootableActor::ALootableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	//bAlwaysRelevant = true;

	if (HasAuthority())
	{
		LootPool = CreateDefaultSubobject<ULootPoolComponent>("LootPool");
		LootPool->SetNetAddressable();
		LootPool->SetIsReplicated(true);

		CoinContent = CreateDefaultSubobject<UCoinComponent>("CoinContent");
		CoinContent->SetNetAddressable();
		CoinContent->SetIsReplicated(true);
	}
}

//----------------------------------------------------------------------------------------------------------------------

void ALootableActor::BeginPlay()
{
	Super::BeginPlay();

}

//----------------------------------------------------------------------------------------------------------------------

UCoinComponent* ALootableActor::GetCoinComponent()
{
	return CoinContent;
}

//----------------------------------------------------------------------------------------------------------------------

ULootPoolComponent* ALootableActor::GetLootPoolComponent()
{
	return LootPool;
}

//----------------------------------------------------------------------------------------------------------------------

UCoinComponent* ALootableActor::GetCoinComponentConst() const
{
	return CoinContent;
}

//----------------------------------------------------------------------------------------------------------------------

ULootPoolComponent* ALootableActor::GetLootPoolComponentConst() const
{
	return LootPool;
}

//----------------------------------------------------------------------------------------------------------------------

bool ALootableActor::GetIsBeingLooted() const
{
	return IsBeingLooted;
}

//----------------------------------------------------------------------------------------------------------------------

void ALootableActor::SetIsBeingLooted(bool LootStatus)
{
	IsBeingLooted = LootStatus;
}

//----------------------------------------------------------------------------------------------------------------------

bool ALootableActor::GetIsDestroyable() const
{
	return CanBeDestroyed;
}

//----------------------------------------------------------------------------------------------------------------------

void ALootableActor::DestroyLootActor()
{
	Destroy();
}

//----------------------------------------------------------------------------------------------------------------------

FString ALootableActor::GetLootActorName() const
{
	return ActorName;
}

//----------------------------------------------------------------------------------------------------------------------

void ALootableActor::StartLooting(AActor* Looter)
{
	ILootableInterfaceExtended::StartLooting(Looter);
	BlueprintableStartLoot();
}

//----------------------------------------------------------------------------------------------------------------------

void ALootableActor::StopLooting(AActor* Looter)
{
	ILootableInterfaceExtended::StopLooting(Looter);
	BlueprintableStopLoot();
}

//----------------------------------------------------------------------------------------------------------------------

void ALootableActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALootableActor, CoinContent);
	DOREPLIFETIME(ALootableActor, LootPool);
	DOREPLIFETIME(ALootableActor, IsBeingLooted);
	DOREPLIFETIME(ALootableActor, ActorName);
	DOREPLIFETIME(ALootableActor, CanBeDestroyed);
}
