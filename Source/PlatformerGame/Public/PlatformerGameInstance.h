// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlatformerGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PLATFORMERGAME_API UPlatformerGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Shutdown() override;
};
