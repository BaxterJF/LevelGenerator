#pragma once

#include "GameFramework/Actor.h"
#include "ExitInfo.h"
#include "ModuleGenerator.h"

#include "LevelModule.generated.h"

UCLASS()
class ALevelModule : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelModule();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	virtual void OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult);

	void Initialize(AModuleGenerator* Gen, FModuleInfo Info);

	TArray<AExitInfo*> GetExits();

	TArray<UStaticMeshSocket*> GetSockets();

	UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* MeshComp;

	/** Root component to give the whole actor a transform. */
	USceneComponent* TransformComponent;

	//URotatingMovementComponent* RotatingMovement;

	UPROPERTY(EditAnywhere)
	TArray<AExitInfo*> Exits;

	FVector TargetLocation;
	FRotator DeltaRotation;
};
