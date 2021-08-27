// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomActorSpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "RespawnableActor.h"

// Sets default values
ARandomActorSpawner::ARandomActorSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = Root;
	Bound = CreateDefaultSubobject<UBoxComponent>(TEXT("Bound Component"));
	Bound->SetupAttachment(Root);
	Bound->SetCollisionProfileName("NoCollision", true);
	Bound->SetGenerateOverlapEvents(false); 
	Bound->SetCollisionEnabled(ECollisionEnabled::NoCollision); 

	TraceZ = 1500;
	Density = 50;
	bAlignToNormal = true;
	bCurvedBrach = false;
	ScaleRange = FVector2D(1, 1);
	RespawnTime = 30;
	RespawnedAmount = 0;

	if (HasAuthority())
	{
		bReplicates = true;
	}

	bIsCoundowning = false;
}

void ARandomActorSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

// Called when the game starts or when spawned
void ARandomActorSpawner::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();

	if (!HasAuthority() || !ActorToSpawn || !World) { return; }

	for (uint8 i = 0; i < Density; i++)
	{
		SpawnActor();
	}

	if (!bRespawn) { Destroy(); } 
}

void ARandomActorSpawner::SpawnActor()
{
	FVector Start;
	FVector End;
	GetTraceStartEnd(Start, End);

	FHitResult Hit;
	TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);

	const bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(World, Start, End, ObjectTypes, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, false, FColor::Yellow, FColor::Red, 15.0f);

	if (bHit)
	{
		float Scaler = FMath::RandRange(ScaleRange.X, ScaleRange.Y); 

		FTransform SpawnTransform = FTransform(GetRandomRotation(Hit.ImpactNormal).Quaternion(), Hit.ImpactPoint, FVector::OneVector * Scaler); 

		AActor* LastSpawned = (World->SpawnActor<AActor>(ActorToSpawn, SpawnTransform)); 
		SpawnedActors.Add(LastSpawned);
		
		ARespawnableActor* Respawnable = Cast<ARespawnableActor>(LastSpawned);
		if (Respawnable) {Respawnable->SetSpawner(this);
		}
	}
}

void ARandomActorSpawner::GetTraceStartEnd(FVector& Start, FVector& End)
{

	FVector Origin = Bound->Bounds.Origin;			
	FVector BoxExtent = Bound->Bounds.BoxExtent;	

	FVector RandomPoint = FMath::RandPointInBox(FBox(Origin - BoxExtent, Origin + BoxExtent)); 

	Start = FVector(RandomPoint.X, RandomPoint.Y, TraceZ); 

	End = Start + (GetActorUpVector() * -1 * (TraceZ + 1000)); 
}


FRotator ARandomActorSpawner::GetRandomRotation(const FVector& ImpactNormal)
{
	FRotator A;
	if (bCurvedBrach) 
	{
		A = FRotator(FMath::RandRange(-3, 3), FMath::RandRange(0, 360), FMath::RandRange(-3, 3)); 
	}
	else
	{
		A = FRotator(0, FMath::RandRange(0, 360), 0); 
	}


	FRotator B = FRotationMatrix::MakeFromZY(bAlignToNormal ? ImpactNormal : GetActorUpVector(), FVector(0, 1, 0)).Rotator(); 

	FQuat AQuat = FQuat(A);
	FQuat BQuat = FQuat(B);
	return FRotator(BQuat * AQuat); 
}

void ARandomActorSpawner::SpawnedDestroyed(AActor* Destroyed)
{
	if (SpawnedActors.Contains(Destroyed)) { SpawnedActors.Remove(Destroyed); }

	if (bRespawn)
	{
		RespawnedAmount++;
		CountdownForRespawn();
	}
}


void ARandomActorSpawner::CountdownForRespawn()
{
	if (!bIsCoundowning)
	{
		bIsCoundowning = true;
		GetWorldTimerManager().SetTimer(RespawnHandle, this, &ARandomActorSpawner::Respawn, RespawnTime, false);
	}
}

void ARandomActorSpawner::Respawn()
{
	GetWorldTimerManager().ClearTimer(RespawnHandle);
	bIsCoundowning = false;
	SpawnActor();
	RespawnedAmount--;
	if (RespawnedAmount > 0)
	{
		CountdownForRespawn();
	}
}

