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
	
	const FName& GetCurrentEventTrigger() const { return CurrentEventTrigger; } 
	const int& GetSelectedIndex() const { return SelectedIndex; }

	UFUNCTION(BlueprintCallable, Category = "Monitor")
	FVector2D GetButtonPosition(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Monitor")
	FVector2D GetNextButtonPosition();
	
	UPROPERTY(BlueprintReadWrite, Category = "Monitor")
	class ASurveyCharacter* Character;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ending")
	TSubclassOf<UUserWidget> EndingWidgetClass;
	
	
protected:
	virtual void NativeConstruct() override;
	void UpdateMonitorUI(const FSurveyData& SurveyData);

	UPROPERTY(meta = (BindWidget))
	class UImage* Mouse;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Question;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* NextButton;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UWidgetSwitcher* WidgetSwitch;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UEditableTextBox* TextBox;

	UPROPERTY()
	class UCanvasPanelSlot* MouseSlot;
	
	// 현재 이벤트 트리거
	FName CurrentEventTrigger;
	
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
	
	// 강제 타이핑 이벤트용
	
	// 타이핑할 문장
	FString TargetString;
	// 현재까지 타이핑한 문장
	FString CurrentString;
	// 인덱스
	int32 TypeIndex = 0;
	// 타이머 핸들
	FTimerHandle TypingTimerHandle;
	
	void StartAutoTyping(const FString& InMessage);
	void TypeNextCharacter();
	void HideCursorAndScheduleNext();
	
	// 엔딩 용
	void EndingBegin();
};
