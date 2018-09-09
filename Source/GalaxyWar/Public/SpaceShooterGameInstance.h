// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Runtime/Engine/Classes/Engine/StreamableManager.h"
#include "SpaceShooterGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GALAXYWAR_API USpaceShooterGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	
	FStreamableManager StreamableManager;

	virtual void Init() override;
	
};
