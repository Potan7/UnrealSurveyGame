// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurveyGameMode.h"
#include "SurveyMainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SURVEY_API USurveyMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FGameStartDelegate OnStartButtonClicked;
protected:
	UFUNCTION()
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* StartButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* SettingsButton;
	
	UPROPERTY(meta = (BindWidget))
	class UButton* ExitButton;
	
	UFUNCTION()
	void HandleStartButtonClicked();
	
	UFUNCTION()
	void HandleSettingsButtonClicked();
	
	UFUNCTION()
	void HandleExitButtonClicked();
};
