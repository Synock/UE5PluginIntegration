// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DialogActor.h"

#include "Interfaces/MerchantInterface.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ADialogActor::ADialogActor()
{
	PrimaryActorTick.bCanEverTick = false;

	//if(HasAuthority())
	{
		Dialog = CreateDefaultSubobject<UDialogComponent>("DialogComponent");
		Dialog->SetNetAddressable(); // Make DSO components net addressable
		Dialog->SetIsReplicated(true); // Enable replication by default
	}

}

//----------------------------------------------------------------------------------------------------------------------

void ADialogActor::BeginPlay()
{
	Super::BeginPlay();

}
//----------------------------------------------------------------------------------------------------------------------

float ADialogActor::GetRelation(AActor* RelationWithActor) const
{
	return 0.5;
}

//----------------------------------------------------------------------------------------------------------------------

UDialogComponent* ADialogActor::GetDialogComponent() const
{
	return Dialog;
}

//----------------------------------------------------------------------------------------------------------------------

FString ADialogActor::GetRelationString(float Relation) const
{
	return "Indifferent";
}

//----------------------------------------------------------------------------------------------------------------------

void ADialogActor::InitDialog()
{
	if (HasDialog())
	{
		Dialog->InitDialogFromID(DialogMetaBundleID);
		if (!Dialog->IsValid())
		{
			FTimerHandle Handle;
			GetWorld()->GetTimerManager().SetTimer(Handle, this, &ADialogActor::InitDialog, 1.0, false, 1.0);
		}
	}
}

bool ADialogActor::CanTrade() const
{
	return Cast<IMerchantInterface>(this) != nullptr;
}

//----------------------------------------------------------------------------------------------------------------------

bool ADialogActor::CanTrain() const
{
	return IDialogInterface::CanTrain();
}

//----------------------------------------------------------------------------------------------------------------------

bool ADialogActor::CanBank() const
{
	return Banker;
}

//----------------------------------------------------------------------------------------------------------------------

void ADialogActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADialogActor, Dialog);
}