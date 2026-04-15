// Fill out your copyright notice in the Description page of Project Settings.


#include "SurveyCharacter.h"

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
	WidgetInteraction->InteractionSource = EWidgetInteractionSource::World;
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
		if (const auto* WidgetComp = TargetMonitorActor->FindComponentByClass<UWidgetComponent>())
		{
			MonitorWidget = Cast<USurveyMonitorWidget>(WidgetComp->GetWidget());
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
		if (MouseDeltaX != 0.0f || MouseDeltaY != 0.0f)
		{
			// 현재 위치에서 이동량을 먼저 계산합니다.
			FVector CurrentLocation = MouseMesh->GetRelativeLocation();
			CurrentLocation.X += MouseDeltaY * MouseSpeed;
			CurrentLocation.Y += MouseDeltaX * MouseSpeed;
			
			// 초기 위치(InitialMouseLocation)를 기준으로 범위를 제한합니다.
			CurrentLocation.X = FMath::Clamp(CurrentLocation.X, InitialMouseLocation.X + MouseMinBounds.X, InitialMouseLocation.X + MouseMaxBounds.X);
			CurrentLocation.Y = FMath::Clamp(CurrentLocation.Y, InitialMouseLocation.Y + MouseMinBounds.Y, InitialMouseLocation.Y + MouseMaxBounds.Y);
			
			// 제한된 최종 위치를 적용합니다.
			MouseMesh->SetRelativeLocation(CurrentLocation);

			// --- 모니터 위젯 업데이트 추가 (비율 매핑 방식) ---
			if (MonitorWidget && WidgetInteraction)
			{
				// 1. 현재 마우스가 자신의 가동 범위 내에서 어디에 위치하는지 0~1 사이 비율로 계산합니다.
				// MouseMinBounds ~ MouseMaxBounds 범위를 0.0 ~ 1.0으로 정규화(Normalize)합니다.
				// (현재위치 - 하한선) / (상한선 - 하한선)
				
				float AlphaY = (CurrentLocation.X - (InitialMouseLocation.X + MouseMinBounds.X)) / (MouseMaxBounds.X - MouseMinBounds.X);
				float AlphaX = (CurrentLocation.Y - (InitialMouseLocation.Y + MouseMinBounds.Y)) / (MouseMaxBounds.Y - MouseMinBounds.Y);

				// 2. 정규화된 값을 모니터 해상도에 곱합니다.
				// 모니터 위젯의 좌표계 방향에 맞춰 부호를 반전시킵니다. (마우스가 위로 가면 위젯의 Y가 작아짐)
				// AlphaY=0(최하단) -> UI의 Height, AlphaY=1(최상단) -> 0
				FVector2D UI_CursorPos;
				UI_CursorPos.X = AlphaX * MonitorRes.X;
				UI_CursorPos.Y = (1.0f - AlphaY) * MonitorRes.Y; 

				MonitorWidget->UpdateCursorPosition(UI_CursorPos);
				
				if (const auto* WidgetComp = TargetMouseActor->FindComponentByClass<UWidgetComponent>())
				{
					FTransform WidgetTransform = WidgetComp->GetComponentTransform();
					FVector2D DrawSize = WidgetComp->GetDrawSize();
					
					float LocalX = UI_CursorPos.X - (DrawSize.X * 0.5f);
					float LocalY = UI_CursorPos.Y - (DrawSize.Y * 0.5f);
					
					FVector LocalPos(0.0f, LocalX, -LocalY);
					
					FVector WorldInteractionPos = WidgetTransform.TransformPosition(LocalPos);
					
					WidgetInteraction->SetWorldLocation(WorldInteractionPos);
					WidgetInteraction->SetWorldRotation(WidgetTransform.GetRotation());
				}
			}
		}
	

		if (auto* AnimInstance = Cast<USurveyCharacterAnimInstance>(BodyMesh->GetAnimInstance()))
		{
			// 1. 마우스 소켓의 월드 트랜스폼을 가져옵니다.
			const FTransform WorldTargetTransform = MouseMesh->GetSocketTransform(FName("HandGrip"));

			// 2. 캐릭터(BodyMesh)를 기준으로 하는 상대적 트랜스폼(Component Space)으로 변환합니다.
			const FTransform RelativeTargetTransform = WorldTargetTransform.GetRelativeTransform(BodyMesh->GetComponentTransform());
		       
			// 3. 변환된 로컬 좌표계를 AnimBP로 넘겨줍니다.
			AnimInstance->RightHandTarget = RelativeTargetTransform; 
			AnimInstance->bEnableHandIK = true;
		}
		
		if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::SpaceBar))
		{
			if (MonitorWidget)
			{
				MonitorWidget->ShowNextSurvey();
			}
		}
	}
}

