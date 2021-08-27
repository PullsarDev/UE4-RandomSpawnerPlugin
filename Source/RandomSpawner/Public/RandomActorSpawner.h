// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnerInterface.h"
#include "RandomActorSpawner.generated.h"

class UBoxComponent;
class ASpawnableBase;
struct FTimerHandle;

UCLASS()
class RANDOMSPAWNER_API ARandomActorSpawner : public AActor ,public ISpawnerInterface

{
	GENERATED_BODY()
	
	UPROPERTY()
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Bound;

public:
	// Sets default values for this actor's properties
	ARandomActorSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;


	//SETUP

	UPROPERTY(EditAnywhere, Category = "Spawn Setup")
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Spawn Setup", Meta = (Tooltip = "Max spawn amount. The spawn amount may be less than that depending on the actor density in the environment.")) 
	uint8 Density;

	UPROPERTY(EditAnywhere, Category = "Spawn Setup", Meta = (DisplayName = "Respawn After Destroyed", Tooltip = "You can use this option if an actor can be destroyed and it is desired to spawn again.Thisoption can be used for classes that inherit from Respawnable actor"))
	uint8 bRespawn : 1;

	UPROPERTY(EditAnywhere, Category = "Spawn Setup", Meta = (DisplayName = "Respawn Countdown", Tooltip = "Cooldown until an actor spawns again after being destroyed", EditCondition = "bRespawn"))
	uint8 RespawnTime;

	UPROPERTY(EditAnywhere, Category = "Spawn Setup", Meta = (DisplayName = "Align To Surface"))
	uint8 bAlignToNormal : 1;

	UPROPERTY(EditAnywhere, Category = "Spawn Setup", Meta = (ClampMin = "0", ClampMax = "10", DisplayName = "Random Scale Range"))
	struct FVector2D ScaleRange;

	UPROPERTY(EditAnywhere, Category = "Spawn Setup", Meta = (Tooltip = "Gives up to 3 degrees of random roll and pitch angle.", DisplayName = "Curved Branch"))
	uint8 bCurvedBrach : 1;

	UPROPERTY(EditAnywhere, Category = "Spawn Setup|Trace Settings", meta = (Tooltip = "Peak starting point of the trace", ClampMax = "5000"))
	int32 TraceZ;

	FRotator GetRandomRotation(const FVector& ImpactNormal);

	UWorld* World;

private:
	void SpawnActor();

	void GetTraceStartEnd(FVector& Start, FVector& End);

	TArray<AActor*> SpawnedActors;

public:
	//virtual void Tick(float DeltaTime) override;

	virtual void SpawnedDestroyed(AActor* Destroyed);


//Respawn
private:
	uint8 RespawnedAmount;

	void CountdownForRespawn();

	FTimerHandle RespawnHandle;

	uint8 bIsCoundowning : 1;

	void Respawn();

};


