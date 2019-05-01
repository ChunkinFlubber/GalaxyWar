// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolableActor.h"
#include "PoolMaster.h"

// Sets default values
APoolableActor::APoolableActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APoolableActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APoolableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APoolableActor::Init(APoolMaster * poolMaster, bool bDisable)
{
	PMaster = poolMaster;
	if (bDisable)
	{
		DisableActor();
	}
}

void APoolableActor::ResetActor_Implementation()
{
	FTransform origin;
	origin.SetIdentity();
	SetActorTransform(origin);
}

void APoolableActor::DisableActor_Implementation()
{
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	bIsActorEnabled = false;
	PMaster->ReturnActor(this);
}

void APoolableActor::ActorSpawned_Implementation()
{
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	bIsActorEnabled = true;
}

bool APoolableActor::IsActorEnabled()
{
	return bIsActorEnabled;
}

