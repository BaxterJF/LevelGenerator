#include "LevelGen.h"
#include "ExitInfo.h"

AExitInfo::AExitInfo()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicateMovement = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;
	NetUpdateFrequency = 1.f;
	NetPriority = 3.0f;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TransformComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TransformComponent"));
	RootComponent = TransformComponent;

	bHidden = false;

	bConnected = false;

	CompatibleTags.Add(FName(TEXT("A")));
	CompatibleTags.Add(FName(TEXT("B")));
	/*CompatibleTags.Add(FName(TEXT("C")));
	CompatibleTags.Add(FName(TEXT("D")));*/
}

void AExitInfo::BeginPlay()
{
	Super::BeginPlay();

}

void AExitInfo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector PosA = GetActorLocation() + GetActorForwardVector() * 100;
	DrawDebugLine(
		GetWorld(),
		GetActorLocation(),
		PosA,
		FColor(255, 0, 0),
		false,
		0,
		0,
		3
	);
	DrawDebugSphere(
		GetWorld(),
		GetActorLocation(),
		16,
		12,
		FColor(0, 0, 255),
		false,
		0
	);
	DrawDebugSphere(
		GetWorld(),
		PosA,
		8,
		12,
		FColor(0, 255, 0),
		false,
		0
	);

	/*if (GetAttachParentActor())
	{
		FVector PosB = GetAttachParentActor()->GetActorLocation() + GetAttachParentActor()->GetActorForwardVector() * 100;
		DrawDebugLine(
			GetWorld(),
			GetAttachParentActor()->GetActorLocation(),
			PosB,
			FColor(0, 0, 255),
			false,
			0,
			0,
			3
		);
		DrawDebugSphere(
			GetWorld(),
			GetAttachParentActor()->GetActorLocation(),
			16,
			12,
			FColor(0, 0, 255),
			false,
			0
		);
		DrawDebugSphere(
			GetWorld(),
			PosB,
			8,
			12,
			FColor(0, 0, 255),
			false,
			0
		);
	}*/
}