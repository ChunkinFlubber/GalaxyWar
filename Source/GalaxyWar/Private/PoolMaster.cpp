// Fill out your copyright notice in the Description page of Project Settings.

#include "PoolMaster.h"
#include "SpaceShooterGameInstance.h"
#if !UE_BUILD_SHIPPING
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Engine/World.h"
#include "Runtime/Engine/Public/TimerManager.h"
#endif

void UPoolArray::Get(APoolableActor *& spawnedActor)
{
	spawnedActor = nullptr;
	if (ReadyQueue.IsEmpty())
	{
		AddAndUseItem(spawnedActor);
	}
	else
	{
		while (!IsValid(spawnedActor) && !ReadyQueue.IsEmpty())
		{
			ReadyQueue.Dequeue(spawnedActor);
		}
	}
	++CurrentlyActive;
}
int32 UPoolArray::GetActive()
{
	return CurrentlyActive;
}
void UPoolArray::Return(APoolableActor * actor)
{
	if (actor->IsActorEnabled())
	{
		actor->DisableActor();
		return;
	}
	ReadyQueue.Enqueue(actor);
	--CurrentlyActive;
	CurrentlyActive = (CurrentlyActive < 0) ? 0 : CurrentlyActive;
}

UPoolArray::UPoolArray()
{

}

void UPoolArray::Init(TSubclassOf<class APoolableActor> actorClass, APoolMaster * master, UWorld* world)
{
	World = world;
	PooledActors.Reserve(150);
	ActorClass = actorClass;
	Master = master;
	bIsDoneLoading = true;
	CurrentlyActive = 0;
}

UPoolArray::~UPoolArray()
{

}

void UPoolArray::AddItems(const int32 numberOfActors)
{
	if (World != nullptr)
	{
		NumberToSpawn += numberOfActors;
		int32 num = NumberToSpawn - NumberToRemove;
		NumberToSpawn = (num < 0) ? 0 : num;
		NumberToRemove = (num > 0) ? 0 : -num;
		if (bIsDoneLoading)
		{
			if (SpawningTimerHandle.IsValid())
			{
				World->GetTimerManager().UnPauseTimer(SpawningTimerHandle);
			}
			else
			{
				World->GetTimerManager().SetTimer(SpawningTimerHandle, this, &UPoolArray::SpawnItemTimerCall, 0.1f, true);
			}
		}
	}
}

void UPoolArray::AddAndUseItem(APoolableActor *& actor)
{
	if (World != nullptr)
	{
		actor = World->SpawnActor<APoolableActor>(ActorClass, FTransform::Identity);
		actor->Init(Master, false);
		PooledActors.Add(actor);
	}
}

void UPoolArray::RemoveItems(const int32 numberOfActors)
{
	if (World != nullptr)
	{
		NumberToRemove += numberOfActors;
		int32 num = NumberToSpawn - NumberToRemove;
		NumberToSpawn = (num < 0) ? 0 : num;
		NumberToRemove = (num > 0) ? 0 : -num;
		if (RemoveingTimerHandle.IsValid())
		{
			World->GetTimerManager().UnPauseTimer(RemoveingTimerHandle);
		}
		else
		{
			World->GetTimerManager().SetTimer(RemoveingTimerHandle, this, &UPoolArray::RemoveItemTimerCall, 0.01f, true);
		}
	}
}

void UPoolArray::SpawnItemTimerCall()
{
	if (NumberToSpawn == 0)
	{
		bIsDoneLoading = true;
		World->GetTimerManager().PauseTimer(SpawningTimerHandle);
		return;
	}

	int32 NTS = (10 > NumberToSpawn) ? NumberToSpawn : 10;
	NumberToSpawn -= NTS;
	APoolableActor * actor = nullptr;
	for (int32 i = 0; i < NTS; i++)
	{
		actor = World->SpawnActor<APoolableActor>(ActorClass, FTransform::Identity);
		PooledActors.Add(actor);
		//Initializes actor and adds it to queue
		actor->SetOwner(Master);
		actor->Init(Master, true);
	}

	if (NumberToSpawn == 0)
	{
		bIsDoneLoading = true;
		World->GetTimerManager().PauseTimer(SpawningTimerHandle);
	}
}

void UPoolArray::RemoveItemTimerCall()
{
	if (NumberToRemove == 0)
	{
		bIsDoneLoading = true;
		World->GetTimerManager().PauseTimer(RemoveingTimerHandle);
		return;
	}

	APoolableActor * actor = nullptr;
	int32 NTR = (3 > NumberToRemove) ? NumberToRemove : 3;
	for (int32 i = 0; i < NTR; i++)
	{
		actor = PooledActors.Pop(false);
		actor->Destroy();
		--NumberToRemove;
	}

	if (NumberToRemove == 0)
	{
		bIsDoneLoading = true;
		World->GetTimerManager().PauseTimer(RemoveingTimerHandle);
	}
}

// Sets default values
APoolMaster::APoolMaster()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	NumberActive = 0;
}

// Called when the game starts or when spawned
void APoolMaster::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APoolMaster::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

}

void APoolMaster::SpawnObject(const TSubclassOf<class APoolableActor> actorClass, const FTransform& transform, AActor* owningActor, APoolableActor *& objectSpawned)
{
	//First sreach the map and see if key exists
	if (Pool.Contains(actorClass))
	{
		//If something does exist add onto it
		Pool[actorClass]->Get(objectSpawned);
	}
	else //if not then create a new one
	{
		UPoolArray* newArray = NewObject<UPoolArray>();
		newArray->Init(actorClass, this, GetWorld());
		Pool.Add(actorClass, newArray);
		newArray->Get(objectSpawned);
	}
	if (objectSpawned != nullptr) //If the object exists
	{
		//Put it where it needs to be
		objectSpawned->SetActorTransform(transform);
		//Set the owner as the actor who called for the spawn
		objectSpawned->SetOwner(owningActor);
		//Tell the spawned object that it has been spawned
		objectSpawned->ActorSpawned();
	}

	++NumberActive;
	DebugScreen();
}

void APoolMaster::RequestAddition(const TSubclassOf<class APoolableActor> actorClass, const int32 amountToAdd)
{
	//First sreach the map and see if anything already exists
	if (Pool.Contains(actorClass))
	{
		//If something does exist add onto it
		Pool[actorClass]->AddItems(amountToAdd);
	}
	else //if not then create a new one
	{
		UPoolArray* newArray = NewObject<UPoolArray>();
		newArray->Init(actorClass, this, GetWorld());
		Pool.Add(actorClass, newArray);
		newArray->AddItems(amountToAdd);
	}
}

void APoolMaster::RequestRemoval(const TSubclassOf<class APoolableActor> actorClass, const int32 amountToRemove)
{
	//First sreach the map and see if anything exists
	if (Pool.Contains(actorClass))
	{
		//If something does exist remove the amount
		Pool[actorClass]->RemoveItems(amountToRemove);
	}
}

void APoolMaster::ReturnActor(APoolableActor * returningActor)
{
	UClass * actorClass = returningActor->GetClass();
	if (actorClass->IsChildOf(APoolableActor::StaticClass()))
	{
		//First sreach the map and see if anything exists
		if (Pool.Contains(actorClass))
		{
			returningActor->SetOwner(this);
			Pool[actorClass]->Return(returningActor);
		}
	}
	--NumberActive;
	if (NumberActive < 0)
	{
		NumberActive = 0;
	}

	DebugScreen();
}
void APoolMaster::DebugScreen()
{
	GEngine->AddOnScreenDebugMessage(0, 2, FColor::Red, FString("Number Of Active Pooled Objects: " + FString::FromInt(NumberActive)));

	int32 i = 1;
	for (const TPair<TSubclassOf<class APoolableActor>, UPoolArray*>& pair : Pool)
	{
		pair.Key;
		pair.Value;
		GEngine->AddOnScreenDebugMessage(i, 2, FColor::Red, FString(pair.Key.Get()->GetName() + "'s: " + FString::FromInt(pair.Value->GetActive())));
		++i;
	}
}