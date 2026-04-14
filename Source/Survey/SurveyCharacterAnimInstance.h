// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SurveyCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SURVEY_API USurveyCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bEnableHandIK = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FTransform RightHandTarget;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FTransform LeftHandTarget ;
};
