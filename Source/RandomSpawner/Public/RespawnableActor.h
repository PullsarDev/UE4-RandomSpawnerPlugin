// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnerInterface.h"
#include "RespawnableActor.generated.h"


UCLASS()
class RANDOMSPAWNER_API ARespawnableActor : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ObjectMesh;

	AActor* Spawner;

public:
	// Sets default values for this actor's properties
	ARespawnableActor();

	FORCEINLINE void SetSpawner(AActor* SpawnerRef) { Spawner = SpawnerRef; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;


};
