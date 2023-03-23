// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/DialogInterface.h"
#include "DialogActor.generated.h"

UCLASS()
class PLUGININTEGRATION_API ADialogActor : public AActor, public IDialogInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADialogActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Dialog")
	TObjectPtr<UDialogComponent> Dialog = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialog")
	int32 DialogMetaBundleID = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialog")
	bool Banker = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Dialog")
	FString ActorName = "Dialog Actor";

public:

	//------------------------------------------------------------------------------------------------------------------
	// Dialog
	//------------------------------------------------------------------------------------------------------------------

	virtual float GetRelation(AActor* RelationWithActor) const override;

	virtual UDialogComponent* GetDialogComponent() const override;

	virtual FString GetRelationString(float Relation) const override;

	virtual bool HasDialog() const override {return Dialog != nullptr && DialogMetaBundleID != 0;}

	UFUNCTION(BlueprintCallable)
	void InitDialog();

	virtual bool CanTrade() const override;

	virtual bool CanTrain() const override;

	virtual bool CanBank() const override;

	virtual FText GetCharacterNameForDialog() const override {return FText::FromString(ActorName);}


};
