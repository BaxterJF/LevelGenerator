#pragma once

#include "GameFramework/Actor.h"
#include "Engine/TargetPoint.h"
#include "Engine.h"

#include "ExitInfo.generated.h"

UCLASS()
class AExitInfo : public ATargetPoint
{
	GENERATED_BODY()
	
public:	
	AExitInfo();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	/** Root component to give the whole actor a transform. */
	USceneComponent* TransformComponent;

	UPROPERTY(EditAnywhere)
	bool bConnected;

	UPROPERTY(EditAnywhere)
	TArray<FName> CompatibleTags;
};
