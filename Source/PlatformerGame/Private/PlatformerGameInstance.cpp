// Fill out your copyright notice in the Description page of Project Settings.

#include "PlatformerGame.h"

#include "RuyiSDK/RuyiSDKManager.h"

#include "PlatformerGameInstance.h"

void UPlatformerGameInstance::Shutdown()
{
	UE_LOG(LogPlatformer, Log, TEXT("URuyiGameInstance::Shutdown  !!!!!!!!!!!!!!!!!!!!!!"));

	if (FRuyiSDKManager::Instance()->IsSDKReady())
	{
		FRuyiSDKManager::Instance()->SDK()->Subscriber->Unsubscribe("service/inputmanager_internal");
	}

	Super::Shutdown();
}

