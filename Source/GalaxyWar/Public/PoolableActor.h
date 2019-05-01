// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolableActor.generated.h"

class APoolMaster;

UCLASS()
class GALAXYWAR_API APoolableActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APoolableActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
		uint32 bIsActorEnabled;

	APoolMaster * PMaster;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Init(APoolMaster * poolMaster, bool bDisable);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void ResetActor();
	virtual void ResetActor_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void DisableActor();
	virtual void DisableActor_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void ActorSpawned();
	virtual void ActorSpawned_Implementation();

	bool IsActorEnabled();
};