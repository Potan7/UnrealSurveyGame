// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SurveyGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameStartDelegate);

/**
 * 
 */
UCLASS()
class SURVEY_API ASurveyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FGameStartDelegate OnGameStart;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	class TSubclassOf<class UUserWidget> SurveyMainMenuWidget;
	
	UPROPERTY()
	class UUserWidget* SurveyMainMenuWidgetInstance;
	
	UFUNCTION()
	void OnClickStartButton();
};
