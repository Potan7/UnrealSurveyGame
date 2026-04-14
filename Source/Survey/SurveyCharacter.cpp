// Fill out your copyright notice in the Description page of Project Settings.


#include "SurveyCharacter.h"

#include "SurveyCharacterAnimInstance.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASurveyCharacter::ASurveyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
	
	FaceCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FaceCamera"));
	FaceCamera->SetupAttachment(Root);
	
	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ASurveyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (!BodyMesh) return;
	
	TArray<USkeletalMeshComponent*> AllSkinnedMeshes;
	GetComponents<USkeletalMeshComponent>(AllSkinnedMeshes);

	// 2. 반복문을 돌면서 Leader Pose를 설정합니다.
	for (USkeletalMeshComponent* ClothesComp : AllSkinnedMeshes)
	{
		// 주의: 자기 자신(메인 몸체)이나 1인칭 전용 팔(Mesh1P)은 동기화하면 안 됩니다.
		if (ClothesComp != BodyMesh)
		{
			ClothesComp->SetLeaderPoseComponent(BodyMesh);
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
	
	if (!TargetMouseActor) return;
	if (USkeletalMeshComponent* MouseMesh = TargetMouseActor->FindComponentByClass<USkeletalMeshComponent>())
	{
		if (MouseMesh && (MouseDeltaX != 0.0f || MouseDeltaY != 0.0f))
		{
			constexpr float MouseSpeed = 2.0f;
			const FVector MouseMoveDelta = FVector(MouseDeltaY, MouseDeltaX, 0.0f) * MouseSpeed;
		
			MouseMesh->AddRelativeLocation(MouseMoveDelta);
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
	}
}

// Called to bind functionality to input
void ASurveyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

