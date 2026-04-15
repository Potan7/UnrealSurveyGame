// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SurveyMonitorWidget.generated.h"

USTRUCT(BlueprintType)
struct FSurveyData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Question;	// 질문 내용
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EventTrigger;	// 질문 시 발생할 이벤트
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EventDelay;	// 이벤트 발생 지연 시간
	
};

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
	
	UFUNCTION(BlueprintCallable, Category = "Monitor")
	void ShowNextSurvey();
	
protected:
	virtual void NativeConstruct() override;
	void UpdateMonitorUI(const FSurveyData& SurveyData);

	UPROPERTY(meta = (BindWidget))
	class UImage* Mouse;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Question;

	UPROPERTY()
	class UCanvasPanelSlot* MouseSlot;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* SurveyDataTable;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Data")
	int32 SurveyIndex = 0;
	
	TArray<FName> SurveyRowNames;
};
