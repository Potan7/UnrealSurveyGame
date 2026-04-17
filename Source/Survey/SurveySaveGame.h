// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SurveySaveGame.generated.h"

/**
 * 
 */
UCLASS()
class SURVEY_API USurveySaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	bool bHasCleared = false;
};
