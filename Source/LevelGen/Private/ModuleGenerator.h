#pragma once

#include "GameFramework/Actor.h"

#include "ModuleGenerator.generated.h"

class ALevelModule;

USTRUCT()
struct FModuleInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere)
	TArray<FName> Tags;

	FModuleInfo()
	{
		Mesh = NULL;
	}

	inline bool operator==(const FModuleInfo& Other) const
	{
		return Mesh == Other.Mesh && Tags == Other.Tags;
	}

	bool MeshIsValid() const
	{
		if (!Mesh) return false;
		return Mesh->IsValidLowLevel();
	}
};


USTRUCT()
struct FExitQueueItem
{
	GENERATED_USTRUCT_BODY()

	ALevelModule* NewModule;
	AExitInfo* OldExit;
	AExitInfo* NewExit;

	FVector StartingLocation;
	FVector TargetLocation;
	FVector FinalLocation;

	FRotator StartingRotation;
	FRotator DeltaRotation;

	bool bAComplete;
	bool bBComplete;
	bool bCComplete;

	FExitQueueItem()
		: NewModule(nullptr)
		, OldExit(nullptr)
		, NewExit(nullptr)
		, StartingLocation(FVector::ZeroVector)
		, TargetLocation(FVector::ZeroVector)
		, FinalLocation(FVector::ZeroVector)
		, StartingRotation(FRotator::ZeroRotator)
		, DeltaRotation(FRotator::ZeroRotator)
		, bAComplete(false)
		, bBComplete(false)
		, bCComplete(false)
	{}

	FExitQueueItem(ALevelModule* inNewModule, AExitInfo* inOldExit, AExitInfo* inNewExit, FVector inStartingLocation, FVector inTargetLocation, FVector inFinalLocation, FRotator inStartingRotation, FRotator inDeltaRotation,
				   bool inbAComplete, bool inbBComplete, bool inbCComplete)
		: NewModule(inNewModule)
		, OldExit(inOldExit)
		, NewExit(inNewExit)
		, StartingLocation(inStartingLocation)
		, TargetLocation(inTargetLocation)
		, FinalLocation(inFinalLocation)
		, StartingRotation(inStartingRotation)
		, DeltaRotation(inDeltaRotation)
		, bAComplete(false)
		, bBComplete(false)
		, bCComplete(false)
	{}


	FORCEINLINE bool operator!() const
	{
		return (!OldExit || !NewExit);
	}

	bool IsValid() const
	{
		return OldExit && NewExit;
		/*if (!OldExit || !NewExit) return false;
		return OldExit->IsValidLowLevel() || NewExit->IsValidLowLevel();*/
	}
};

UCLASS()
class AModuleGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	AModuleGenerator();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	void GenerateLevel();

	ALevelModule* SpawnModule(FModuleInfo Info);

	FModuleInfo RandPrefab(FName Tag, TArray<FModuleInfo> Pool);

	void GetNewTransform(FExitQueueItem* Item /*AExitInfo* OldExit, AExitInfo* NewExit*/);

	void LerpTranslation(float DeltaTime);

	void ProcessExitQueue(float DeltaTime);

	void CallWithDelay(float Delay, FExitQueueItem* Item);

	void ApplyModuleTransform(ALevelModule* Module, FVector Pos, FRotator Rot);

	float CalcAngle(AActor* Actor);

	ALevelModule* AddModules();

	ALevelModule* ActiveModule;

	UPROPERTY(EditAnywhere)
	TArray<FModuleInfo> ModulePrefabs;

	UPROPERTY(EditAnywhere)
	FModuleInfo InitialModuleInfo;

	ALevelModule* InitialModule;

	TArray<AExitInfo*> CurrentExits;

	//TArray<FName> CompatibleTags;

	int32 Iterations;

	UPROPERTY(EditAnywhere)
	int32 NumIterations;

	TQueue<ALevelModule*, EQueueMode::Spsc> ModuleTranslateQueue;
	TQueue<FExitQueueItem*, EQueueMode::Spsc> ExitConnectQueue;

	bool bLockQueue;

	UPROPERTY(EditAnywhere)
	float RotationRate;

	UPROPERTY(EditAnywhere)
	float MovementRate;
};
