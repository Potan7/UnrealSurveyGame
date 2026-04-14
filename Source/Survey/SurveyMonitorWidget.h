// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurveyMonitorWidget.generated.h"

/**
 * 
 */
UCLASS()
class SURVEY_API USurveyMonitorWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Monitor")
	void UpdateCursorPosition(FVector2D NewPosition) const;
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UImage* Mouse;

	UPROPERTY()
	class UCanvasPanelSlot* MouseSlot;
	
};
