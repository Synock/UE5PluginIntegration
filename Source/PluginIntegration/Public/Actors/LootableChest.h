// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LootableActor.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractableInterface.h"
#include "LootableChest.generated.h"

UCLASS()
class PLUGININTEGRATION_API ALootableChest : public ALootableActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ALootableChest();

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UStaticMeshComponent* LidMesh;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float MaxInteractionDistance = 500.f;

	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_IsOpen)
	bool IsOpen = false;

	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void Interact(AActor* Interactor) override;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintCallableInteraction(AActor* Interactor);

	UFUNCTION()
	void OnRep_IsOpen();

	virtual void StartLooting(AActor* Looter) override;
	virtual void StopLooting(AActor* Looter) override;

};
