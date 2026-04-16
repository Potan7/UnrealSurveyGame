 // Fill out your copyright notice in the Description page of Project Settings.


#include "SurveyMonitorWidget.h"
#include "SurveyCharacter.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"

 void USurveyMonitorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 이미지가 로드된 상태에서 해당 슬롯을 미리 캐스팅해서 저장해둡니다.
	if (Mouse)
	{
		MouseSlot = Cast<UCanvasPanelSlot>(Mouse->Slot);
	}
	
	if (SurveyDataTable)
	{
		SurveyRowNames = SurveyDataTable->GetRowNames();
	}
 	
 	if (NextButton)
 	{
 		NextButton->SetVisibility(ESlateVisibility::Hidden);
 		NextButton->OnClicked.AddDynamic(this, &USurveyMonitorWidget::ShowNextSurvey);
 	}
 	
 	SurveyIndex = -1;
 	ShowNextSurvey();
}

 void USurveyMonitorWidget::UpdateMonitorUI(const FSurveyData& SurveyData)
 {
	if (Question)
	{
		const FText SurveyText = FText::FromStringTable(TEXT("/Game/Data/ST_Question.ST_Question"), FTextKey(SurveyData.Question.ToString()));
		Question->SetText(SurveyText);
		CurrentEventTrigger = SurveyData.EventTrigger;
		
		UE_LOG(LogTemp, Warning, TEXT("Updated Question: %s, Event Trigger: %s"), *SurveyData.Question.ToString(), *SurveyData.EventTrigger.ToString());
	}
 }

 void USurveyMonitorWidget::HandleButtonClick(const int32 ClickedIndex)
 {
	if (Character && Character->IsForcedMoving())
	{
		SelectedIndex = ClickedIndex;
		RefreshButtonVisuals();
		return;
	}

	if (CurrentEventTrigger == TEXT("Agree"))
	{
		if (Character)
		{
			if (NextButton)
			{
				NextButton->SetVisibility(ESlateVisibility::HitTestInvisible);
			}

			TArray<FVector2D> Targets;
			if (ClickedIndex != 4)
			{
				Targets.Add(GetButtonPosition(4));
			}
			
			Targets.Add(GetNextButtonPosition());
			
			Character->StartForcedSequence(Targets);

			if (ClickedIndex == 4)
			{
				SelectedIndex = 4;
				RefreshButtonVisuals();
			}
			return;
		}
	}

 	SelectedIndex = ClickedIndex;
 	RefreshButtonVisuals();
 	
    // UE_LOG(LogTemp, Warning, TEXT("Button %d clicked, SelectedIndex set to %d"), ClickedIndex, SelectedIndex);
 }

FVector2D USurveyMonitorWidget::GetButtonPosition(int32 Index)
{
	if (ButtonWidgets.IsValidIndex(Index) && ButtonWidgets[Index])
	{
		const FGeometry& WidgetGeometry = ButtonWidgets[Index]->GetCachedGeometry();
		const FGeometry& MyGeometry = GetCachedGeometry();
		
		// 전체 위젯(Canvas) 기준 로컬 좌표로 변환
		FVector2D LocalPos = MyGeometry.AbsoluteToLocal(WidgetGeometry.GetAbsolutePosition());
		// 버튼의 중심점 계산
		return LocalPos + (WidgetGeometry.GetLocalSize() * 0.5f);
	}
	return FVector2D::ZeroVector;
}

FVector2D USurveyMonitorWidget::GetNextButtonPosition()
{
	if (NextButton)
	{
		const FGeometry& WidgetGeometry = NextButton->GetCachedGeometry();
		const FGeometry& MyGeometry = GetCachedGeometry();
		
		FVector2D LocalPos = MyGeometry.AbsoluteToLocal(WidgetGeometry.GetAbsolutePosition());
		return LocalPos + (WidgetGeometry.GetLocalSize() * 0.5f);
	}
	return FVector2D::ZeroVector;
}


 void USurveyMonitorWidget::UpdateCursorPosition(const FVector2D NewPosition) const
{
	// 미리 저장된 슬롯이 유효한지만 체크하고 바로 위치를 설정합니다.
	if (MouseSlot)
	{
		MouseSlot->SetPosition(NewPosition);
	}
}

 void USurveyMonitorWidget::ShowNextSurvey()
 {
	if (!SurveyDataTable || SurveyRowNames.Num() == 0) return;
	
	SurveyIndex++;
	if (SurveyIndex >= SurveyRowNames.Num())
	{
		return;
	}
 	
	const FName SurveyRowName = SurveyRowNames[SurveyIndex];
	if (const FSurveyData* SurveyData = SurveyDataTable->FindRow<FSurveyData>(SurveyRowName, TEXT("Next Survey Context")))
	{
		UpdateMonitorUI(*SurveyData);
	}
 	
 	SelectedIndex = -1;
 	RefreshButtonVisuals();
 }
