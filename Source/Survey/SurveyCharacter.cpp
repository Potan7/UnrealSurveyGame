// Fill out your copyright notice in the Description page of Project Settings.

#include "SurveyCharacter.h"

#include "EnhancedInputComponent.h"
#include "SurveyCharacterAnimInstance.h"
#include "SurveyGameMode.h"
#include "SurveyMonitorWidget.h"
#include "SurveySaveGame.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASurveyCharacter::ASurveyCharacter()
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;
    
    FaceCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FaceCamera"));
    FaceCamera->SetupAttachment(RootComponent);
    
    BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
    BodyMesh->SetupAttachment(RootComponent);
    
    WidgetInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteraction"));
    WidgetInteraction->SetupAttachment(RootComponent);
    
    // 물리 레이캐스트 대신 Custom 방식으로 변경
    WidgetInteraction->InteractionSource = EWidgetInteractionSource::Custom;
    WidgetInteraction->InteractionDistance = 100.0f;
}

void ASurveyCharacter::OnGameStarted()
{
	
	UE_LOG(LogTemp, Warning, TEXT("Game Started: Returning control to character and starting monitor fade-out"));
	bIsGameStarted = true;
	// 카메라가 자신으로 돌아온 뒤 시작
	// 모니터를 점점 활성화 시키고
	// 입력을 활성화
	
}

// Called when the game starts or when spawned
void ASurveyCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    if (BodyMesh)
    {
       TArray<USkeletalMeshComponent*> AllSkinnedMeshes;
       GetComponents<USkeletalMeshComponent>(AllSkinnedMeshes);

       // 2. 반복문을 돌면서 Leader Pose를 설정합니다.
       for (USkeletalMeshComponent* ClothesComp : AllSkinnedMeshes)
       {
          // 자기 자신은 제외
          if (ClothesComp != BodyMesh)
          {
             ClothesComp->SetLeaderPoseComponent(BodyMesh);
          }
       }
    }
    
    if (TargetMouseActor)
    {
       MouseMesh = TargetMouseActor->FindComponentByClass<USkeletalMeshComponent>();
       if (MouseMesh)
       {
          // 게임 시작 시 마우스의 현재 상대 위치를 기준점으로 저장합니다.
          InitialMouseLocation = MouseMesh->GetRelativeLocation();
       }
    }
    
    if (TargetMonitorActor)
    {
       MonitorWidgetComponent = TargetMonitorActor->FindComponentByClass<UWidgetComponent>();
       if (MonitorWidgetComponent)
       {
          MonitorWidget = Cast<USurveyMonitorWidget>(MonitorWidgetComponent->GetWidget());
          if (MonitorWidget)
          {
             MonitorWidget->Character = this;
          }
       }
    }
	
	if (UGameplayStatics::DoesSaveGameExist(TEXT("Slot1"), 0))
	{
		if (const USurveySaveGame* SaveData = Cast<USurveySaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("Slot1"), 0)))
		{
			if (SaveData->bHasCleared)
			{
				if (BodyMesh)
				{
					BodyMesh->SetVisibility(false, true);
				}
				UE_LOG(LogTemp, Warning, TEXT("Save data loaded: Player has cleared the survey. Hiding character mesh."));
			}
		}
		
	}
	
	if (ASurveyGameMode* Gm = GetWorld()->GetAuthGameMode<ASurveyGameMode>())
	{
		Gm->OnGameStart.AddDynamic(this, &ASurveyCharacter::OnGameStarted);
	}
}

void ASurveyCharacter::StartForcedSequence(const TArray<FVector2D>& TargetUIPositions, float Speed)
{
    ForcedTargets = TargetUIPositions;
    CurrentTargetIndex = 0;
    bIsForcedMoving = true;
    bWaitAfterClick = false;
    ForcedMoveTimer = 0.0f;
    CurrentForcedSpeed = Speed;
}

void ASurveyCharacter::UpdateForcedMovement(float DeltaTime)
{
    if (!bIsForcedMoving || CurrentTargetIndex >= ForcedTargets.Num())
    {
        bIsForcedMoving = false;
        return;
    }

    if (bWaitAfterClick)
    {
        ForcedMoveTimer += DeltaTime;
        if (ForcedMoveTimer > 0.05f) // 클릭 후 0.05초 대기
        {
            bWaitAfterClick = false;
            ForcedMoveTimer = 0.0f;
            CurrentTargetIndex++;
        }
        return;
    }

    const FVector2D TargetUIPos = ForcedTargets[CurrentTargetIndex];
    
    // 현재 마우스 위치를 UI 좌표로 변환하여 보간에 사용
    const FVector CurrentLocation = MouseMesh->GetRelativeLocation();
    const float AlphaY = (CurrentLocation.X - (InitialMouseLocation.X + MouseMinBounds.X)) / (MouseMaxBounds.X - MouseMinBounds.X);
    const float AlphaX = (CurrentLocation.Y - (InitialMouseLocation.Y + MouseMinBounds.Y)) / (MouseMaxBounds.Y - MouseMinBounds.Y);

    FVector2D CurrentUIPos;
    CurrentUIPos.X = AlphaX * MonitorRes.X;
    CurrentUIPos.Y = (1.0f - AlphaY) * MonitorRes.Y;

    if (FVector2D::Distance(CurrentUIPos, TargetUIPos) < 10.0f)
    {
        // 대상 도달 시 마우스 메시 위치를 목표값으로 정확히 맞춤
        MoveMouseToUIPosition(TargetUIPos);

        // 중요: 클릭 직전에 인터랙션 위치를 최신화하여 시뮬레이션 클릭이 정확한 버튼을 누르게 함
        if (WidgetInteraction && MonitorWidgetComponent)
        {
            const FVector TargetLocation = MouseMesh->GetRelativeLocation();
            const float AY = (TargetLocation.X - (InitialMouseLocation.X + MouseMinBounds.X)) / (MouseMaxBounds.X - MouseMinBounds.X);
            const float AX = (TargetLocation.Y - (InitialMouseLocation.Y + MouseMinBounds.Y)) / (MouseMaxBounds.Y - MouseMinBounds.Y);

            const FVector2D DrawSize = MonitorWidgetComponent->GetDrawSize();
            const FVector2D Pivot = MonitorWidgetComponent->GetPivot();
            const float LY = (Pivot.X - AX) * DrawSize.X;
            const float LZ = (AY - Pivot.Y) * DrawSize.Y;

            const FVector LPos(0.0f, LY, LZ);
            const FVector WPos = MonitorWidgetComponent->GetComponentTransform().TransformPosition(LPos);
            
            FHitResult ClickHit;
            ClickHit.bBlockingHit = true;
            ClickHit.Component = MonitorWidgetComponent;
            ClickHit.ImpactPoint = WPos;
            ClickHit.Location = WPos;
            WidgetInteraction->SetCustomHitResult(ClickHit);
        }
        
        SimulateClick();
        bWaitAfterClick = true;
        ForcedMoveTimer = 0.0f;
    }
    else
    {
        // 부드럽게 이동 (설정된 속도 사용)
        FVector2D NextUIPos = FMath::Vector2DInterpTo(CurrentUIPos, TargetUIPos, DeltaTime, CurrentForcedSpeed);
        MoveMouseToUIPosition(NextUIPos);
    }
}

void ASurveyCharacter::MoveMouseToUIPosition(FVector2D UIPos)
{
    if (!MouseMesh) return;

    float AlphaX = UIPos.X / MonitorRes.X;
    float AlphaY = 1.0f - (UIPos.Y / MonitorRes.Y);
    
    FVector NewLocation = InitialMouseLocation;
    NewLocation.X = AlphaY * (MouseMaxBounds.X - MouseMinBounds.X) + (InitialMouseLocation.X + MouseMinBounds.X);
    NewLocation.Y = AlphaX * (MouseMaxBounds.Y - MouseMinBounds.Y) + (InitialMouseLocation.Y + MouseMinBounds.Y);
    
    MouseMesh->SetRelativeLocation(NewLocation);
}

void ASurveyCharacter::SimulateClick()
{
    if (WidgetInteraction)
    {
        WidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
        WidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
    }
	
	if (MouseForceClickSound)
	{
		UGameplayStatics::PlaySound2D(this, MouseForceClickSound);
	}
}

// Called every frame
void ASurveyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsGameStarted && !bIsForcedMoving) return;

    if (bIsForcedMoving)
    {
        UpdateForcedMovement(DeltaTime);
    }

    if (MouseMesh && MonitorWidget)
    {
       // 1. 마우스 이동 처리 (마우스가 움직이고 있을 때만)
      const float TargetModifier = bIsSensitivityReduced ? 0.4f : 1.0f;    
      if (!MouseInputDelta.IsNearlyZero())
      {
          FVector CurrentLocation = MouseMesh->GetRelativeLocation();

          if (const FName& Event = MonitorWidget->GetCurrentEventTrigger(); Event == TEXT("Sensor"))
          {
             // UE_LOG(LogTemp, Warning, TEXT("Sensor event triggered: Reducing mouse sensitivity"));
             bIsSensitivityReduced = true;
          }
         
         CurrentModifier = FMath::FInterpTo(CurrentModifier, TargetModifier, DeltaTime, 1.2f);
          
          // 향상된 입력의 Y는 위가 (+), X는 오른쪽이 (+)
          // 기존 코드의 축 로직에 맞춰 적용:
          CurrentLocation.X += MouseInputDelta.Y * MouseSpeed * CurrentModifier; 
          CurrentLocation.Y += MouseInputDelta.X * MouseSpeed * CurrentModifier;
          
          CurrentLocation.X = FMath::Clamp(CurrentLocation.X, InitialMouseLocation.X + MouseMinBounds.X, InitialMouseLocation.X + MouseMaxBounds.X);
          CurrentLocation.Y = FMath::Clamp(CurrentLocation.Y, InitialMouseLocation.Y + MouseMinBounds.Y, InitialMouseLocation.Y + MouseMaxBounds.Y);
          
          MouseMesh->SetRelativeLocation(CurrentLocation);

          // 중요: 사용한 입력값은 초기화해줘야 멈췄을 때 마우스가 흐르지 않습니다.
          MouseInputDelta = FVector2D::ZeroVector;
      }
       
       // // 1. 마우스 이동 처리
       // if (MouseDeltaX != 0.0f || MouseDeltaY != 0.0f)
       // {
       //    FVector CurrentLocation = MouseMesh->GetRelativeLocation();
       //    CurrentLocation.X += MouseDeltaY * MouseSpeed;
       //    CurrentLocation.Y += MouseDeltaX * MouseSpeed;
       //    
       //    CurrentLocation.X = FMath::Clamp(CurrentLocation.X, InitialMouseLocation.X + MouseMinBounds.X, InitialMouseLocation.X + MouseMaxBounds.X);
       //    CurrentLocation.Y = FMath::Clamp(CurrentLocation.Y, InitialMouseLocation.Y + MouseMinBounds.Y, InitialMouseLocation.Y + MouseMaxBounds.Y);
       //    
       //    MouseMesh->SetRelativeLocation(CurrentLocation);
       // }

       // 2. 위젯 및 인터랙션 업데이트 (마우스 이동 여부와 관계없이 매 프레임 실행)
       if (WidgetInteraction && MonitorWidgetComponent)
       {
          const FVector CurrentLocation = MouseMesh->GetRelativeLocation();
          
          // 비율 계산
          const float AlphaY = (CurrentLocation.X - (InitialMouseLocation.X + MouseMinBounds.X)) / (MouseMaxBounds.X - MouseMinBounds.X);
          const float AlphaX = (CurrentLocation.Y - (InitialMouseLocation.Y + MouseMinBounds.Y)) / (MouseMaxBounds.Y - MouseMinBounds.Y);

          // 2D UI 커서 업데이트
          FVector2D UI_CursorPos;
          UI_CursorPos.X = AlphaX * MonitorRes.X;
          UI_CursorPos.Y = (1.0f - AlphaY) * MonitorRes.Y; 
          MonitorWidget->UpdateCursorPosition(UI_CursorPos);

          // 3D WidgetInteraction 업데이트를 위한 수학적 충돌 위치 계산
          const FVector2D DrawSize = MonitorWidgetComponent->GetDrawSize();
          const FVector2D Pivot = MonitorWidgetComponent->GetPivot();

          const float LocalY = (Pivot.X - AlphaX) * DrawSize.X;
          const float LocalZ = (AlphaY - Pivot.Y) * DrawSize.Y;

          const FVector LocalPos(0.0f, LocalY, LocalZ);
          const FVector WorldPos = MonitorWidgetComponent->GetComponentTransform().TransformPosition(LocalPos);
          const FVector RayStart = FaceCamera->GetComponentLocation() + (FaceCamera->GetForwardVector() * 10.0f);

          // 계산된 3D 월드 좌표(WorldPos)를 바탕으로 강제 충돌 결과(HitResult) 생성
          FHitResult CustomHit;
          CustomHit.bBlockingHit = true; 
          CustomHit.Component = MonitorWidgetComponent;
          CustomHit.ImpactPoint = WorldPos; 
          CustomHit.Location = WorldPos;
          CustomHit.TraceStart = RayStart;
          CustomHit.TraceEnd = WorldPos;

          // WidgetInteractionComponent에 강제로 충돌 정보 주입
          WidgetInteraction->SetCustomHitResult(CustomHit);
       }

       // 3. 애니메이션 IK 업데이트
       if (auto* AnimInstance = Cast<USurveyCharacterAnimInstance>(BodyMesh->GetAnimInstance()))
       {
          const FTransform WorldTargetTransform = MouseMesh->GetSocketTransform(FName("HandGrip"));
          const FTransform RelativeTargetTransform = WorldTargetTransform.GetRelativeTransform(BodyMesh->GetComponentTransform());
              
          AnimInstance->RightHandTarget = RelativeTargetTransform; 
          
       }
       
       // 4. 입력 처리
       if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::SpaceBar))
       {
          if (MonitorWidget)
          {
             MonitorWidget->ShowNextSurvey();
          }
       }
    }
}

void ASurveyCharacter::OnMousePressed(const FInputActionValue& Value)
{
   if (!bIsGameStarted) return;

   if (bIsForcedMoving || !bEnableMouseInput)
   {
   	if (MouseClickDeniedSound)
   	{
   		UGameplayStatics::PlaySound2D(this, MouseClickDeniedSound);
   	}
   	return;
   }
   if (WidgetInteraction)
   {
      WidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
   }
	
	if (MouseClickSound)
	{
		UGameplayStatics::PlaySound2D(this, MouseClickSound);
	}
}

void ASurveyCharacter::OnMouseRelease(const FInputActionValue& Value)
{
   if (!bIsGameStarted || bIsForcedMoving || !bEnableMouseInput) return;
   if (WidgetInteraction)
   {
      WidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
   }
}

void ASurveyCharacter::OnMouseMove(const FInputActionValue& Value)
{
      if (!bIsGameStarted || bIsForcedMoving || !bEnableMouseInput) return;
      MouseInputDelta = Value.Get<FVector2D>();
}

void ASurveyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
   Super::SetupPlayerInputComponent(PlayerInputComponent);
   
   if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
   {
      EnhancedInputComponent->BindAction(MouseClickAction, ETriggerEvent::Started, this, &ASurveyCharacter::OnMousePressed);
      EnhancedInputComponent->BindAction(MouseClickAction, ETriggerEvent::Completed, this, &ASurveyCharacter::OnMouseRelease);
      
      EnhancedInputComponent->BindAction(MouseMoveAction, ETriggerEvent::Triggered, this, &ASurveyCharacter::OnMouseMove);
   }
   
}
