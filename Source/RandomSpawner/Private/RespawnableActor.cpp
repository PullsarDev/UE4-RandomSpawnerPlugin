// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnableActor.h"

// Sets default values
ARespawnableActor::ARespawnableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = Root;

	ObjectMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	ObjectMesh->SetupAttachment(Root);

	if (HasAuthority())
	{
		bReplicates = true;
	}
}

// Called when the game starts or when spawned
void ARespawnableActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARespawnableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ARespawnableActor::Destroyed()
{
	if (!Spawner) { return; } 

	bool bIsImplemented = Spawner->Implements<USpawnerInterface>();
	if (!bIsImplemented) { return; }
	Cast<ISpawnerInterface>(Spawner)->SpawnedDestroyed(this);
}
