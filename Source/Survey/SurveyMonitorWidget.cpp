 // Fill out your copyright notice in the Description page of Project Settings.


#include "SurveyMonitorWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"

void USurveyMonitorWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 이미지가 로드된 상태에서 해당 슬롯을 미리 캐스팅해서 저장해둡니다.
	if (Mouse)
	{
		MouseSlot = Cast<UCanvasPanelSlot>(Mouse->Slot);
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
