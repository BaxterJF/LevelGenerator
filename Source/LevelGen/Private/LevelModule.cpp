#include "LevelGen.h"
#include "LevelModule.h"
#include "ExitInfo.h"
#include "Engine/StaticMeshSocket.h"

DEFINE_LOG_CATEGORY_STATIC(Module, Log, All);

ALevelModule::ALevelModule()
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

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	/*RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	RotatingMovement->RotationRate = FRotator(0, 0, 0);
	RotatingMovement->PivotTranslation = FVector(0, 0, 0);*/

	TargetLocation = FVector::ZeroVector;
	DeltaRotation = FRotator::ZeroRotator;
}

void ALevelModule::BeginPlay()
{
	Super::BeginPlay();

	/*if (GetRootComponent())
	{
		GetRootComponent()->bAbsoluteRotation = true;
	}*/

	//Setup();

	FTimerHandle Handle;
	//GetWorldTimerManager().SetTimer(Handle, this, &ALevelModule::Setup, 1.0f, false);
}

void ALevelModule::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*TArray<AActor*> Overlaps;
	GetOverlappingActors(Overlaps);
	if (Overlaps.Num() > 0)
	{
		UE_LOG(Module, Log, TEXT("Overlapped"));
	}*/
	
	FRotator r = MeshComp->GetSocketTransform(TEXT("A"), RTS_World).Rotator();
	//UE_LOG(Module, Log, TEXT("%s Rot: %f"), *this->GetName(), r.Yaw);

	/*DrawDebugLine(
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
		FColor(255, 0, 0),
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
	);*/
}

void ALevelModule::Initialize(AModuleGenerator* Gen, FModuleInfo Info)
{
	if (Info.Mesh)
	{
		//UStaticMeshComponent* CollisionTest = NewObject<UStaticMeshComponent>(this);

		MeshComp->SetStaticMesh(Info.Mesh);
		//CollisionTest->SetStaticMesh(Info.Mesh);
		//CollisionTest->SetRelativeScale3D(FVector(0.8, 0.8, 0.8));
		Tags = Info.Tags;

		/*CollisionTest->bGenerateOverlapEvents = true;
		CollisionTest->SetCollisionProfileName(TEXT("OverlapAll"));
		CollisionTest->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CollisionTest->SetMobility(EComponentMobility::Movable);
		CollisionTest->SetVisibility(false);
		CollisionTest->RegisterComponentWithWorld(GetWorld());
		CollisionTest->AttachToComponent(MeshComp, FAttachmentTransformRules::KeepWorldTransform);*/

		//CollisionTest->OnComponentBeginOverlap.AddDynamic(this, &ALevelModule::OnOverlapBegin);
		//CollisionTest->ReregisterComponent();
		//SetActorEnableCollision(true);
	}
	else
	{
		UE_LOG(Module, Log, TEXT("Initialize(): Invalid Mesh from FModuleInfo"));
	}

	for (UStaticMeshSocket* Socket : GetSockets())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		//SpawnParams.Template;

		AExitInfo* Exit = GetWorld()->SpawnActor<AExitInfo>(AExitInfo::StaticClass(), SpawnParams);

		Exits.Add(Exit);

		Exit->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, Socket->SocketName);
		//Socket->AttachActor(Exit, MeshComp);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	//SpawnParams.Template;

	/*AExitInfo* ExitA = GetWorld()->SpawnActor<AExitInfo>(AExitInfo::StaticClass(), SpawnParams);

	Exits.AddUnique(ExitA);
	ExitA->GetRootComponent()->AttachTo(MeshComp, TEXT("A"), EAttachLocation::SnapToTarget);
	ExitA->SetActorRotation(MeshComp->GetSocketTransform(TEXT("A"), RTS_World).Rotator());

	AExitInfo* ExitB = GetWorld()->SpawnActor<AExitInfo>(AExitInfo::StaticClass(), SpawnParams);

	Exits.AddUnique(ExitB);
	ExitB->GetRootComponent()->AttachTo(MeshComp, TEXT("B"), EAttachLocation::SnapToTarget);
	ExitB->SetActorRotation(MeshComp->GetSocketTransform(TEXT("B"), RTS_World).Rotator());*/

	FTimerHandle Handle;
	//GetWorldTimerManager().SetTimer(Handle, Gen, &AModuleGenerator::GenerateLevel, 0.5f, false);
	//Gen->GenerateLevel();
}

TArray<UStaticMeshSocket*> ALevelModule::GetSockets()
{
	TArray<UStaticMeshSocket*> Sockets;

	if (MeshComp && MeshComp->StaticMesh)
	{
		Sockets = MeshComp->StaticMesh->Sockets;
	}

	if (Sockets.Num() == 0)
	{
		UE_LOG(Module, Log, TEXT("GetSockets(): Returned empty array"));
	}

	return Sockets;
}

TArray<AExitInfo*> ALevelModule::GetExits()
{
	return Exits;
}

void ALevelModule::OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepHitResult)
{

}