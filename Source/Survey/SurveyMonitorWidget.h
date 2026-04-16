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
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* NextButton;

	UPROPERTY()
	class UCanvasPanelSlot* MouseSlot;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* SurveyDataTable;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Data")
	int32 SurveyIndex = 0;
	
	// 버튼 위젯들을 배열로 관리 (BP에서 할당)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monitor")
	TArray<UUserWidget*> ButtonWidgets;
	
	// 현재 선택된 인덱스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monitor")
	int32 SelectedIndex = -1;
	
	// 버튼이 클릭되었을 때 호출되는 함수
	UFUNCTION(BlueprintCallable, Category = "Monitor")
	void HandleButtonClick(const int32 ClickedIndex);
	
	// 모든 버튼의 비주얼을 새로고침하는 함수 (BP에서 구현)
	UFUNCTION(BlueprintImplementableEvent, Category = "Survey")
	void RefreshButtonVisuals();
	
	TArray<FName> SurveyRowNames;
};
