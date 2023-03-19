// Copyright 2023 Maximilien (Synock) Guislain

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UObject/Object.h"
#include "LootWindow.generated.h"

class ULootScreenWidget;
/**
 *
 */
UCLASS()
class PLUGININTEGRATION_API ULootWindow : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(BlueprintReadWrite)
	ULootScreenWidget* LootWidgetPointer = nullptr;

public:

	ULootScreenWidget* GetLootWidget() const {return LootWidgetPointer;}
};
