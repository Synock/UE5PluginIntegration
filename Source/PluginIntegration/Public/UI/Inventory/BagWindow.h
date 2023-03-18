// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BagWindow.generated.h"


class UBagWidget;
/**
 *
 */
UCLASS()
class PLUGININTEGRATION_API UBagWindow : public UUserWidget
{
	GENERATED_BODY()
protected:

	UPROPERTY(BlueprintReadWrite)
	UBagWidget* BagWidgetPointer = nullptr;


public:

	UBagWidget* GetBagWidget() const {return BagWidgetPointer;}
};
