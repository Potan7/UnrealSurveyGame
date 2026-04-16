// Fill out your copyright notice in the Description page of Project Settings.


#include "SurveyMonitorWidget.h"
#include "SurveyCharacter.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

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
	
	if (WidgetSwitch)
	{
		WidgetSwitch->SetActiveWidgetIndex(0);
	}
	
	if (BeginButton)
	{
		BeginButton->OnClicked.AddDynamic(this, &USurveyMonitorWidget::ShowNextSurvey);
	}

	SurveyIndex = -1;
	SetTitleScreen();
}

void USurveyMonitorWidget::UpdateMonitorUI(const FSurveyData& SurveyData)
{
	if (Question)
	{
		const FText SurveyText = FText::FromStringTable(
			TEXT("/Game/Data/ST_Question.ST_Question"), FTextKey(SurveyData.Question.ToString()));
		Question->SetText(SurveyText);
		CurrentEventTrigger = SurveyData.EventTrigger;

		UE_LOG(LogTemp, Warning, TEXT("Updated Question: %s, Event Trigger: %s"), *SurveyData.Question.ToString(),
		       *SurveyData.EventTrigger.ToString());

		// Mouse 이벤트: 한 프레임 뒤에(지오메트리 갱신 후) 강제 이동 시작
		if (CurrentEventTrigger == TEXT("Mouse"))
		{
			if (Character)
			{
				Character->SetMouseInput(false);
				if (NextButton)
				{
					NextButton->SetVisibility(ESlateVisibility::HitTestInvisible);
				}

				// 람다를 사용하여 다음 프레임에 실행 (정확한 좌표 계산을 위해)
				GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
				{
					if (Character)
					{
						TArray<FVector2D> Targets;
						Targets.Add(GetButtonPosition(4));
						Targets.Add(GetNextButtonPosition());
						Character->StartForcedSequence(Targets, 6.0f);
					}
				});
			}
		}
		else if (CurrentEventTrigger == TEXT("Keyboard"))
		{
			if (WidgetSwitch)
			{
				WidgetSwitch->SetActiveWidgetIndex(1);
				const FText WriteText = FText::FromStringTable("/Game/Data/ST_UI.ST_UI", TEXT("You"));
				
				StartAutoTyping(WriteText.ToString());
			}
		}
		else if (CurrentEventTrigger == TEXT("END"))
		{
			if (WidgetSwitch)
			{
				WidgetSwitch->SetActiveWidgetIndex(2);
			}
			
			GetWorld()->GetTimerManager().SetTimer(TypingTimerHandle, this, &USurveyMonitorWidget::EndingBegin, 2.5f, false);
		}
	}
}

void USurveyMonitorWidget::SetTitleScreen()
{
	if (WidgetSwitch)
	{
		WidgetSwitch->SetActiveWidgetIndex(3);
	}
	if (Question)
	{
		Question->SetText(FText::FromStringTable(TEXT("/Game/Data/ST_Question.ST_Question"), TEXT("Title")));
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

void USurveyMonitorWidget::StartAutoTyping(const FString& InMessage)
{
    if (!TextBox) return;
    
    TargetString = InMessage;
    CurrentString = TEXT("");
    TypeIndex = 0;
    
    TextBox->SetText(FText::GetEmpty());
    GetWorld()->GetTimerManager().SetTimer(TypingTimerHandle, this, &USurveyMonitorWidget::TypeNextCharacter, 0.8f, false);
}

void USurveyMonitorWidget::TypeNextCharacter()
{
	if (TypeIndex == 0 && Character)
	{
		TArray<FVector2D> Targets;
		FVector2D NextPos = GetNextButtonPosition() + FVector2D(0.0f, -100.0f); // Next 버튼 바로 아래로 이동
		Targets.Add(NextPos);
		Character->StartForcedSequence(Targets, 1);
	}
	
    if (TypeIndex < TargetString.Len())
    {
       CurrentString.AppendChar(TargetString[TypeIndex]);
       TypeIndex++;
       
       if (TextBox)
       {
          // 글자와 함께 커서(|) 표시
          TextBox->SetText(FText::FromString(CurrentString + TEXT("|")));
       }

       // 1. 글자를 친 후 커서가 켜진 채로 유지되는 시간
       const float ShowCursorDelay = FMath::RandRange(0.4f, 0.7f);
        
       // 2. 지정된 시간 후에 커서를 끄는 함수 호출
       GetWorld()->GetTimerManager().SetTimer(TypingTimerHandle, this, &USurveyMonitorWidget::HideCursorAndScheduleNext, ShowCursorDelay, false);
    }
}

void USurveyMonitorWidget::HideCursorAndScheduleNext()
{
    if (TextBox)
    {
        // 커서(|) 숨김
        TextBox->SetText(FText::FromString(CurrentString));
    }

    // 아직 칠 글자가 남았다면
    if (TypeIndex < TargetString.Len())
    {
        // 커서가 꺼진 상태를 짧게 유지 (깜빡! 하는 느낌) 후 다음 글자 타이핑
        const float HideCursorDelay = FMath::RandRange(0.1f, 0.2f);
        GetWorld()->GetTimerManager().SetTimer(TypingTimerHandle, this, &USurveyMonitorWidget::TypeNextCharacter, HideCursorDelay, false);
    }
    else
    {
        // 모든 타이핑이 끝났을 때 (마지막 깜빡임 연출)
        // 현재 꺼진 상태 -> 0.3초 대기 -> 켜짐 -> 0.4초 대기 -> 최종 꺼짐 -> Next 이동
        GetWorld()->GetTimerManager().SetTimer(TypingTimerHandle, [this]()
        {
            if (TextBox)
            {
                TextBox->SetText(FText::FromString(CurrentString + TEXT("|")));
            }
        	
        	if (NextButton)
        	{
        		NextButton->SetVisibility(ESlateVisibility::Visible);
        	}
            
            GetWorld()->GetTimerManager().SetTimer(TypingTimerHandle, [this]()
            {
                if (TextBox)
                {
                    TextBox->SetText(FText::FromString(CurrentString));
                }
            	
            	GetWorld()->GetTimerManager().SetTimer(TypingTimerHandle, [this]()
            	{
            		if (Character)
            		{
            			Character->SetMouseInput(true);
            			TArray<FVector2D> Targets;
            			Targets.Add(GetNextButtonPosition());
            			Character->StartForcedSequence(Targets, 4);
            		}
            	}, 1.5f, false);
            }, 0.4f, false);
            
        }, 0.3f, false);
    }
}

void USurveyMonitorWidget::EndingBegin()
{
	if (EndingWidgetClass)
	{
		auto* EndingWidget = CreateWidget<UUserWidget>(GetWorld(), EndingWidgetClass);
		EndingWidget->AddToViewport(100);
	}
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
	if (!SurveyDataTable || SurveyRowNames.Num() == 0)
	{
		return;
	}
	
	if (SurveyIndex == -1 && WidgetSwitch)
	{
		WidgetSwitch->SetActiveWidgetIndex(0);
	}

	SurveyIndex++;
	if (SurveyIndex >= SurveyRowNames.Num())
	{
		return;
	}

	const FName SurveyRowName = SurveyRowNames[SurveyIndex];
	if (const FSurveyData* SurveyData = SurveyDataTable->FindRow<FSurveyData>(
		SurveyRowName, TEXT("Next Survey Context")))
	{
		UpdateMonitorUI(*SurveyData);
	}

	SelectedIndex = -1;
	RefreshButtonVisuals();
}
