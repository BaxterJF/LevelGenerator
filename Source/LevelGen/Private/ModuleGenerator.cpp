#include "LevelGen.h"
#include "LevelModule.h"
#include "ModuleGenerator.h"

DEFINE_LOG_CATEGORY_STATIC(Generator, Log, All);

AModuleGenerator::AModuleGenerator()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicateMovement = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	bNetLoadOnClient = true;
	NetUpdateFrequency = 1.f;
	NetPriority = 3.0f;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	NumIterations = 5;
	Iterations = 0;

	ActiveModule = NULL;
	RotationRate = 100.f;
	MovementRate = 800.f;
}

void AModuleGenerator::BeginPlay()
{
	Super::BeginPlay();

	InitialModule = SpawnModule(InitialModuleInfo);

	GenerateLevel();

	FTimerHandle Handle;
	//GetWorldTimerManager().SetTimer(Handle, this, &AModuleGenerator::GenerateLevel, 1.0f, false);
}

void AModuleGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LerpTranslation(DeltaTime);
	ProcessExitQueue(DeltaTime);
}

void AModuleGenerator::ProcessExitQueue(float DeltaTime)
{
	if (ExitConnectQueue.IsEmpty()) {return;}

	FExitQueueItem* Item;
	ExitConnectQueue.Peek(Item);

	if (!Item->IsValid()) {return;}

	if (!bLockQueue && Item && Item->IsValid())
	{
		GetNewTransform(Item);

		bLockQueue = true;
	}

	if (bLockQueue && !Item->NewExit->bConnected)
	{
		//Item->NewExit->bConnected = true;

		if (!Item->bAComplete)
		{
			FVector CurrentModuleLocation = Item->NewModule->GetActorLocation();
			float SizeA = (CurrentModuleLocation - Item->TargetLocation).Size();
			if (SizeA > 1.f)
			{
				Item->NewModule->SetActorLocation(FMath::VInterpConstantTo(CurrentModuleLocation, Item->TargetLocation, DeltaTime, MovementRate));
				return;
			}
			else
			{
				/*FVector Location = Actor->GetActorLocation();
				FRotator Rotation = Actor->GetActorRotation();
				FVector VectorInActorLocalSpace = Rotation.UnrotateVector(WorldSpaceVector - Location);*/
				FVector OldLoc = Item->OldExit->GetActorLocation();
				FVector NewLoc = Item->NewExit->GetActorLocation();
				FVector Angle = OldLoc - NewLoc;
				FRotator StartingRotation = Item->NewModule->GetActorRotation();
				//FRotator DeltaRotation = StartingRotation - FRotationMatrix::MakeFromX(Angle).Rotator();
				FRotator Rotation = StartingRotation - FRotationMatrix::MakeFromX(Angle).Rotator();

				//Item->DeltaRotation = FRotator( FQuat(Rotation) * FQuat(StartingRotation) );

				Item->bAComplete = true;
			}
		}

		if (!Item->bBComplete)
		{
			FRotator CurrentModuleRotation = Item->NewModule->GetActorRotation();
			//FRotator SizeB = CurrentModuleRotation - (CurrentModuleRotation + Item->DeltaRotation);
			//SizeB.Normalize();
			float YawA = FMath::Abs<float>(CurrentModuleRotation.GetNormalized().Yaw);
			float YawB = FMath::Abs<float>(Item->DeltaRotation.GetNormalized().Yaw);

			/*if (!FMath::IsNearlyEqual(YawA, YawB, KINDA_SMALL_NUMBER))
			{
				Item->NewModule->SetActorRelativeRotation(FMath::RInterpConstantTo(CurrentModuleRotation, Item->DeltaRotation, DeltaTime, RotationRate));

				return;
			}
			else
			{
				Item->FinalLocation = (Item->OldExit->GetActorLocation() - Item->NewExit->GetActorLocation()) + Item->NewModule->GetActorLocation();
				Item->bBComplete = true;
			}*/
			FRotator DeltaRotation = FRotator::ZeroRotator;
			DeltaRotation.Yaw = CalcAngle(Item->NewExit->GetActorForwardVector() * -1) - CalcAngle(Item->OldExit->GetActorForwardVector());

			UE_LOG(Generator, Log, TEXT("CalcAngle() - Yaw: %f)"), DeltaRotation.Yaw);

			Item->NewModule->SetActorRelativeRotation(DeltaRotation);

			Item->FinalLocation = (Item->OldExit->GetActorLocation() - Item->NewExit->GetActorLocation()) + Item->NewModule->GetActorLocation();
			Item->bBComplete = true;
		}

		if (!Item->bCComplete)
		{
			FVector CurrentModuleLocation = Item->NewModule->GetActorLocation();
			float SizeC = (CurrentModuleLocation - Item->FinalLocation).Size();
			if (SizeC > 1.f)
			{
				Item->NewModule->SetActorLocation(FMath::VInterpConstantTo(CurrentModuleLocation, Item->FinalLocation, DeltaTime, MovementRate));
				return;
			}
			else
			{
				Item->bCComplete = true;

				Item->NewExit->bConnected = true;
				Item->OldExit->bConnected = true;

				bLockQueue = false;

				FExitQueueItem* Q;
				ExitConnectQueue.Dequeue(Q);

				if (ExitConnectQueue.IsEmpty())
				{
					GenerateLevel();
				}
			}
		}
	}
}

void AModuleGenerator::LerpTranslation(float DeltaTime)
{

	/*if (ModuleTranslateQueue.Peek(ActiveModule))
	{
		FVector Location = ActiveModule->GetActorLocation();
		FVector TargetLocation = ActiveModule->TargetLocation;

		if ((Location - ActiveModule->TargetLocation).Size() > 1.f)
		{
			ActiveModule->SetActorLocation(FMath::Lerp<FVector>(Location, TargetLocation, 0.05f));

			return;
		}
		else if (ActiveModule->GetActorRotation().Yaw - ActiveModule->GetActorRotation().Yaw + ActiveModule->DeltaRotation.Yaw > 1.f)
		{
			ActiveModule->SetActorRotation(FMath::Lerp<FRotator>(ActiveModule->GetActorRotation(), ActiveModule->DeltaRotation, 0.05f));

			return;
		}
		else
		{
			ALevelModule* M;
			ModuleTranslateQueue.Dequeue(M);
		}
	}*/
}

void AModuleGenerator::GenerateLevel()
{
	if (Iterations == 0)
	{
		CurrentExits = InitialModule->Exits;

		UE_LOG(Generator, Log, TEXT("//////////////////////////////"));
		UE_LOG(Generator, Log, TEXT("////// BEGIN GENERATOR //////"), Iterations);
		UE_LOG(Generator, Log, TEXT("//////////////////////////////"));
	}
	if (Iterations == NumIterations)
	{
		UE_LOG(Generator, Log, TEXT("////////////////////////////"));
		UE_LOG(Generator, Log, TEXT("////// END GENERATOR //////"), Iterations);
		UE_LOG(Generator, Log, TEXT("////////////////////////////"));

		return;
	}

	UE_LOG(Generator, Log, TEXT("============================================"));
	UE_LOG(Generator, Log, TEXT("===== GenerateLevel() - Iteration: %i ======"), Iterations + 1);
	UE_LOG(Generator, Log, TEXT("============================================"));

	//TArray<AExitInfo*> ValidExits;
	//for (AExitInfo* It : CurrentExits)
	//{
	//	TArray<FName> CompatibleTags = It->CompatibleTags;
	//	TArray<FModuleInfo> ModulePool = ModulePrefabs;
	//	ALevelModule* NewModule = NULL;
	//	AExitInfo* ExitToMatch = NULL;
	//	bool bSucceeded = false;
	//	while (ModulePool.Num() > 0 && !bSucceeded)
	//	{
	//		int32 Index = FMath::RandRange(0, CompatibleTags.Num() - 1);
	//		FName SelectedTag;
	//		if (CompatibleTags.IsValidIndex(Index))
	//		{
	//			SelectedTag = CompatibleTags[Index];
	//		}
	//
	//		FModuleInfo SelectedModule = RandPrefab(SelectedTag, ModulePool);
	//
	//		NewModule = SpawnModule(SelectedModule);
	//
	//		Index = FMath::RandRange(0, NewModule->CurrentExits.Num() - 1);
	//		if (NewModule->CurrentExits.IsValidIndex(Index))
	//		{
	//			ExitToMatch = NewModule->CurrentExits[Index];
	//		}
	//
	//		GetNewTransform(It, ExitToMatch);
	//
	//		/*TArray<AActor*> Overlaps;
	//		NewModule->GetOverlappingActors(Overlaps, ALevelModule::StaticClass());
	//		if (Overlaps.Num() > 0)
	//		{
	//			UE_LOG(Generator, Log, TEXT("Overlapped"));
	//
	//			ModulePool.Remove(SelectedModule);
	//			NewModule->Destroy(true);
	//		}
	//		else
	//		{
	//			bSucceeded = true;
	//		}*/	
	//		bSucceeded = true;
	//	}
	//
	//	if (NewModule)
	//	{
	//		for (AExitInfo* It : NewModule->GetExits())
	//		{
	//			if (It != ExitToMatch)
	//			{
	//				ValidExits.Add(It);
	//			}
	//		}
	//	}
	//}
	//CurrentExits = ValidExits;

	// try to prevent creation of new modules until old ones are finished
	ALevelModule* NewModule = AddModules();

	CurrentExits.Reset();

	if (NewModule->GetExits().Num() > 0)
	{
		for (AExitInfo* It : NewModule->GetExits())
		{
			CurrentExits.Add(It);
		}
	}
	else
	{
		UE_LOG(Generator, Log, TEXT("GenerateLevel(): Exit array is empty!"));
	}

	Iterations++;

	/*FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AModuleGenerator::GenerateLevel, 2.f, false);*/

	//GenerateLevel();
}

ALevelModule* AModuleGenerator::AddModules()
{
	ALevelModule* NewModule = NULL;
	for (AExitInfo* Exit : CurrentExits)
	{
		if (!Exit->bConnected)
		{
			int32 Index = FMath::RandRange(0, Exit->CompatibleTags.Num() - 1);
			FName ChosenTag;
			if (Exit->CompatibleTags.IsValidIndex(Index))
			{
				ChosenTag = Exit->CompatibleTags[Index];
			}
			else
			{
				UE_LOG(Generator, Log, TEXT("AddModules(): Invalid Tag index!"));
				break;
			}

			NewModule = SpawnModule(RandPrefab(ChosenTag, ModulePrefabs));

			if (NewModule)
			{
				Index = FMath::RandRange(0, NewModule->GetExits().Num() - 1);
				AExitInfo* NewExit = NULL;
				if (NewModule->GetExits().IsValidIndex(Index))
				{
					NewExit = NewModule->GetExits()[Index];
				}
				else
				{
					UE_LOG(Generator, Log, TEXT("AddModules(): Invalid Exit index!"));
					break;
				}

				FExitQueueItem* QueueItem = new FExitQueueItem();
				QueueItem->NewModule = NewModule;
				QueueItem->OldExit = Exit;
				QueueItem->NewExit = NewExit;

				ExitConnectQueue.Enqueue(QueueItem);

				//CallWithDelay(1.f, Exit, NewExit);
				//GetNewTransform(Exit, NewExit);
				//NewExit->bConnected = true;
			}
		}
	}

	return NewModule;
}

void AModuleGenerator::GetNewTransform(FExitQueueItem* Item /*AExitInfo* OldExit, AExitInfo* NewExit*/)
{
	/*FExitQueueItem Item;
	ExitConnectQueue.Peek(Item);*/

	if (!Item || !Item->IsValid()) {return;}

	//ALevelModule* NewModule = Cast<ALevelModule>(Item->NewExit->GetOwner());
	//ALevelModule* OldModule = Cast<ALevelModule>(Item->OldExit->GetOwner());

	FVector ForwardVectorToMatch = Item->OldExit->GetActorForwardVector() * -1;
	FVector NewForwardVector = Item->NewExit->GetActorForwardVector();
	FVector OldExitLocation = Item->OldExit->GetActorLocation();
	FVector NewExitLocation = Item->NewExit->GetActorLocation();
	FVector Angle = OldExitLocation - NewExitLocation;
	FVector DeltaLocation = OldExitLocation - NewExitLocation;
	FRotator StartingRotation = Item->NewModule->GetActorRotation();
	FRotator DeltaRotation = FRotator::ZeroRotator;

	//DeltaRotation.Yaw = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardVectorToMatch, NewForwardVector)));

	//DeltaRotation.Yaw = CalcAngle(ForwardVectorToMatch) - CalcAngle(NewForwardVector);

	//DeltaRotation.Yaw = FRotationMatrix::MakeFromX(OldExitLocation - NewExitLocation).Rotator().Yaw;

	//UE_LOG(Generator, Log, TEXT("CalcAngle() - Yaw: %f)"), DeltaRotation.Yaw);

	Item->StartingLocation = Item->NewModule->GetActorLocation();
	Item->TargetLocation = OldExitLocation;
	Item->StartingRotation = StartingRotation;
	Item->DeltaRotation = DeltaRotation;

	if (Item->NewExit != NULL && Item->OldExit != NULL)
	{
		UE_LOG(Generator, Log, TEXT(
			"GetNewTransform(): Connected %s|-%s to %s|-%s"),
			*Item->NewExit->GetOwner()->GetName(), *Item->NewExit->GetAttachParentSocketName().ToString(),
			*Item->OldExit->GetOwner()->GetName(), *Item->OldExit->GetAttachParentSocketName().ToString()
		);
	}

	/*if (OldExit && NewExit)
	{*/
		/*ALevelModule* NewModule = Cast<ALevelModule>(NewExit->GetOwner());
		ALevelModule* OldModule = Cast<ALevelModule>(OldExit->GetOwner());*/

		/*FVector AttachPoint = OldExit->GetActorLocation();
		FRotator AttachRotation = OldExit->GetActorRotation();
		float AttachAngle = 180;
		FVector YawOnly = FVector(0.f, 1.f, 0.f);

		FVector RotateVect = YawOnly * AttachAngle;

		NewModule->SetActorLocation(AttachPoint);
		NewModule->SetActorRotation(RotateVect.Rotation() + AttachRotation);*/

		/*FVector ForwardVectorToMatch = OldExit->GetActorForwardVector() * -1;
		FVector NewForwardVector = NewExit->GetActorForwardVector();
		FVector OldExitLocation = OldExit->GetActorLocation();
		FVector NewExitLocation = NewExit->GetActorLocation();
		FVector Angle = OldExitLocation - NewExitLocation;
		FVector DeltaLocation = OldExitLocation - NewExitLocation;
		FRotator StartingRotation = NewModule->GetActorRotation();
		FRotator DeltaRotation = FRotator::ZeroRotator;
		DeltaRotation.Yaw = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardVectorToMatch, NewForwardVector)));
		FRotator FinalRotation = FRotationMatrix::MakeFromX(Angle).Rotator();*/

		//FRotator FinalRot = FRotator(0, OldExit->GetActorRotation().Yaw - DeltaRotation.Yaw, 0);
		//float Yaw = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ForwardVectorToMatch, NewForwardVector)));
		//DeltaRotation.Yaw = CalcAngle(ForwardVectorToMatch) - CalcAngle(NewForwardVector);

		/*FVector target = Planet->GetActorLocation();
		FVector start = Thruster->GetComponentLocation();
		FVector direction = -(target - start);
		FRotator targetRot = FRotationMatrix::MakeFromX(direction).Rotator();*/
		// set result to actor component
		//Thruster->SetWorldRotation(targetRot, true);

		//NewModule->TargetLocation = OldExitLocation;
		//NewModule->DeltaRotation = DeltaRotation;
		//ModuleTranslateQueue.Enqueue(NewModule);

		//NewModule->SetActorLocation(OldExitLocation);
		//NewModule->SetActorRotation(FinalRotation);
		//NewModule->AddActorWorldRotation(DeltaRotation, true);
		//NewModule->AddActorWorldOffset(OldExit->GetActorLocation() - NewExit->GetActorLocation(), true);

		/*FVector Pivot = NewExit->GetActorLocation();
		FVector Axis =  NewExit->GetActorUpVector();	

		FVector SafeAxis = Axis.GetSafeNormal();
		FRotator NewRot = FQuat(SafeAxis, FMath::DegreesToRadians(Yaw)).Rotator();

		FQuat AQuat = FQuat(NewModule->GetActorRotation());
		FQuat BQuat = FQuat(NewRot);

		FRotator FinalRotation = FRotator(BQuat*AQuat);
		FVector FinalPosition = (OldExit->GetActorLocation() - Pivot).RotateAngleAxis(Yaw, Axis) + Pivot;*/

		//NewModule->SetActorLocationAndRotation(FinalPosition, FinalRotation);
		//NewModule->SetActorRotation(FinalRotation);

		//ApplyModuleTransform(NewModule, FinalPosition, FinalRotation);

		/*UE_LOG(Generator, Log, TEXT(
			"GetNewTransform(): Connected %s|-%s to %s|-%s"),
			*NewExit->GetOwner()->GetName(), *NewExit->GetAttachParentSocketName().ToString(),
			*OldExit->GetOwner()->GetName(), *OldExit->GetAttachParentSocketName().ToString()
		);*/

		//UE_LOG(Generator, Log, TEXT("GetNewTransform(): OldFwd (%f, %f, %f) - NewFwd: (%f, %f, %f)"), FwdVectorToMatch.X, FwdVectorToMatch.Y, FwdVectorToMatch.Z, NewFwdVector.X, NewFwdVector.Y, NewFwdVector.Z);

		/*FVector PosB = NewExit->GetActorLocation() + NewExit->GetActorForwardVector() * 100;
		DrawDebugLine(
			GetWorld(),
			NewExit->GetActorLocation(),
			PosB,
			FColor(0, 0, 255),
			true,
			0,
			0,
			3
		);
		DrawDebugSphere(
			GetWorld(),
			NewExit->GetActorLocation(),
			16,
			12,
			FColor(0, 0, 255),
			true,
			0
		);
		DrawDebugSphere(
			GetWorld(),
			PosB,
			8,
			12,
			FColor(0, 0, 255),
			true,
			0
		);*/
	//}
}

float AModuleGenerator::CalcAngle(AActor* Actor)
{
	FRotator Delta = FRotator::ZeroRotator;

	Delta.Yaw = FMath::RadiansToDegrees(acosf(FVector::DotProduct(FVector::ForwardVector, Vector))) * FMath::Sign(Vector.X);

	//Delta = FRotationMatrix::MakeFromX(FVector::ForwardVector - Vector).Rotator() * FMath::Sign(Vector.Y);

	return Delta.Yaw;
}

FModuleInfo AModuleGenerator::RandPrefab(FName Tag, TArray<FModuleInfo> Pool)
{
	TArray<FModuleInfo> CompatibleModules;

	for (FModuleInfo It : Pool)
	{
		for (FName T : It.Tags)
		{
			if (T == Tag)
			{
				CompatibleModules.Add(It);
			}
		}
	}

	int32 Index = FMath::RandRange(0, CompatibleModules.Num() - 1);
	FModuleInfo Result = InitialModuleInfo;

	if (CompatibleModules.IsValidIndex(Index))
	{
		Result = CompatibleModules[Index];
	}
	else
	{
		UE_LOG(Generator, Log, TEXT("RandPrefab(): Invalid Module index"));
	}

	return Result;
}

ALevelModule* AModuleGenerator::SpawnModule(FModuleInfo Info)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	//SpawnParams.Template;

	ALevelModule* Module = GetWorld()->SpawnActor<ALevelModule>(ALevelModule::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	Module->Initialize(this, Info);

	return Module;
}

void AModuleGenerator::CallWithDelay(float Delay, FExitQueueItem* Item)
{
	FTimerHandle UniqueHandle;
	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &AModuleGenerator::GetNewTransform, Item);
	GetWorldTimerManager().SetTimer(UniqueHandle, RespawnDelegate, Delay, false);
}

void AModuleGenerator::ApplyModuleTransform(ALevelModule* Module, FVector Pos, FRotator Rot)
{
	FRotator RoundedRot = FRotator(FMath::RoundToInt(Rot.Pitch), FMath::RoundToInt(Rot.Yaw), FMath::RoundToInt(Rot.Roll));
	FVector RoundedPos = FVector(FMath::RoundToInt(Pos.X), FMath::RoundToInt(Pos.Y), FMath::RoundToInt(Pos.Z));

	//Module->SetActorLocationAndRotation(Pos, Rot);
	//Module->SetActorRotation(Rot);

	Module->AddActorWorldRotation(Rot);
	Module->AddActorWorldOffset(Pos);

	UE_LOG(Generator, Log, TEXT("-------------------------------------------"));
	UE_LOG(Generator, Log, TEXT("ApplyModuleTransform(): %s"), *Module->GetName());
	UE_LOG(Generator, Log, TEXT("ApplyModuleTransform() - Rot: (%f, %f, %f)"), Rot.Pitch, Rot.Yaw, Rot.Roll);
	UE_LOG(Generator, Log, TEXT("ApplyModuleTransform() - Pos: (%f, %f, %f)"), Pos.X, Pos.Y, Pos.Z);
	UE_LOG(Generator, Log, TEXT("-------------------------------------------"));
}