// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Runtime/Core/Public/Containers/Queue.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolableActor.h"
#include "PoolMaster.generated.h"

class APoolMaster;

UCLASS()
class GALAXYWAR_API UPoolArray : public UObject
{
	GENERATED_BODY()
private:
	UWorld* World;
	APoolMaster * Master;
	
	FTimerHandle SpawningTimerHandle;
	
	FTimerHandle RemoveingTimerHandle;
	
	TSubclassOf<class APoolableActor> ActorClass;
	
	int32 NumberToSpawn;
	
	int32 NumberToRemove;
	
	int32 bIsDoneLoading;

	UPROPERTY()
	TArray<APoolableActor*> PooledActors;

	TQueue<APoolableActor*> ReadyQueue;

	void AddAndUseItem(APoolableActor *& actor);

public:

	UPoolArray();
	void Init(TSubclassOf<class APoolableActor> actorClass, APoolMaster * master, UWorld* world);
	~UPoolArray();

	//UPoolArray& operator=(const UPoolArray &other);

	void Get(APoolableActor *& spawnedActor);

	void Return(APoolableActor * actor);

	void AddItems(const int32 numberOfActors);
	void RemoveItems(const int32 numberOfActors);
	void SpawnItemTimerCall();
	void RemoveItemTimerCall();
};

UCLASS()
class GALAXYWAR_API APoolMaster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APoolMaster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	TMap<TSubclassOf<class APoolableActor>, UPoolArray*> Pool;

public:	
	// Called every frame
	virtual void Tick(float deltaTime) override;

	UFUNCTION( Category=Utility, BlueprintCallable )
	void SpawnObject(const TSubclassOf<class APoolableActor> actorClassToSpawn, const FTransform& transform, AActor* owningActor, APoolableActor *& objectSpawned);
	UFUNCTION(Category = Utility, BlueprintCallable)
	void RequestAddition(const TSubclassOf<class APoolableActor> actorClassToAdd, const int32 amountToAdd);
	UFUNCTION(Category = Utility, BlueprintCallable)
	void RequestRemoval(const TSubclassOf<class APoolableActor> actorClassToAdd, const int32 amountToAdd);

	void ReturnActor(APoolableActor * returningActor);
};