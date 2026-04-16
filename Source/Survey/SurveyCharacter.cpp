// Fill out your copyright notice in the Description page of Project Settings.

#include "SurveyCharacter.h"

#include "EnhancedInputComponent.h"
#include "SurveyCharacterAnimInstance.h"
#include "SurveyMonitorWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/WidgetInteractionComponent.h"

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
       }
    }
}

// Called every frame
void ASurveyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    float MouseDeltaX = 0.0f;
    float MouseDeltaY = 0.0f;
    
    if (const auto* PC = Cast<APlayerController>(GetController()))
    {
       PC->GetInputMouseDelta(MouseDeltaX, MouseDeltaY);
    }

    if (MouseMesh)
    {
       // 1. 마우스 이동 처리 (마우스가 움직이고 있을 때만)
      if (!MouseInputDelta.IsNearlyZero())
      {
          FVector CurrentLocation = MouseMesh->GetRelativeLocation();
          
          // 향상된 입력의 Y는 위가 (+), X는 오른쪽이 (+)
          // 기존 코드의 축 로직에 맞춰 적용:
          CurrentLocation.X += MouseInputDelta.Y * MouseSpeed; 
          CurrentLocation.Y += MouseInputDelta.X * MouseSpeed;
          
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
       if (MonitorWidget && WidgetInteraction && MonitorWidgetComponent)
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
          AnimInstance->bEnableHandIK = true;
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
   if (WidgetInteraction)
   {
      WidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
   }
}

void ASurveyCharacter::OnMouseRelease(const FInputActionValue& Value)
{
   if (WidgetInteraction)
   {
      WidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
   }
}

void ASurveyCharacter::OnMouseMove(const FInputActionValue& Value)
{
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
