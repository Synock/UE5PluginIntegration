// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FinalDialogWindow.generated.h"

class UDialogWindow;

UCLASS()
class PLUGININTEGRATION_API UFinalDialogWindow : public UUserWidget
{
	GENERATED_BODY()
protected:

	UPROPERTY(BlueprintReadWrite)
	UDialogWindow* InnerWidget = nullptr;

public:

	UFUNCTION()
	void HideWindow();

	UFUNCTION(BlueprintCallable)
	void InitUI();

	UFUNCTION(BlueprintCallable)
	void DisplayPlainString(const FString& PlainString);
};
