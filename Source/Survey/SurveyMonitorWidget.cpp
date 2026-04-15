 // Fill out your copyright notice in the Description page of Project Settings.


#include "SurveyMonitorWidget.h"

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
 	
 	SurveyIndex = -1;
 	ShowNextSurvey();
}

 void USurveyMonitorWidget::UpdateMonitorUI(const FSurveyData& SurveyData)
 {
	if (Question)
	{
		const FText SurveyText = FText::FromStringTable(TEXT("/Game/Data/ST_Question.ST_Question"), FTextKey(SurveyData.Question.ToString()));
		Question->SetText(SurveyText);
		UE_LOG(LogTemp, Warning, TEXT("Updated Question: %s"), *SurveyData.Question.ToString());
	}
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
 }
