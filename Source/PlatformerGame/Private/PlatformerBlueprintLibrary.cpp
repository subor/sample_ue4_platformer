// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "PlatformerGame.h"
#include "PlatformerBlueprintLibrary.h"
#include "UI/PlatformerHUD.h"
#include "UI/Widgets/FPlatformerPicture.h"
#include "PlatformerGameMode.h"

#include "RuyiSDK/RuyiSDKManager.h"

UPlatformerBlueprintLibrary::UPlatformerBlueprintLibrary(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

APlatformerGameMode* GetGameFromContextObject(class UObject* WorldContextObject)
{
	UWorld* const MyWorld = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	check(MyWorld);

	APlatformerGameMode* const MyGame = MyWorld->GetAuthGameMode<APlatformerGameMode>();
	return MyGame;
}

void UPlatformerBlueprintLibrary::PrepareRace(class UObject* WorldContextObject)
{
	APlatformerGameMode* const MyGame = GetGameFromContextObject(WorldContextObject);
	if (MyGame)
	{
		MyGame->PrepareRound();
	}
}

bool UPlatformerBlueprintLibrary::FinishRace(class UObject* WorldContextObject)
{
	bool bHasWon = false;

	APlatformerGameMode* MyGame = GetGameFromContextObject(WorldContextObject);
	if (MyGame)
	{
		MyGame->FinishRound();
		bHasWon = MyGame->IsRoundWon();
	}

	return bHasWon;
}

void UPlatformerBlueprintLibrary::AllowToRestartRace(class UObject* WorldContextObject)
{
	APlatformerGameMode* MyGame = GetGameFromContextObject(WorldContextObject);
	if (MyGame)
	{
		MyGame->SetCanBeRestarted(true);
	}
}

void UPlatformerBlueprintLibrary::DecreaseRoundDuration(class UObject* WorldContextObject, float DeltaTime)
{
	APlatformerGameMode* MyGame = GetGameFromContextObject(WorldContextObject);
	if (MyGame)
	{
		const float Delta = FMath::Abs(DeltaTime);
		MyGame->ModifyRoundDuration(Delta, (DeltaTime > 0) ? false : true);
	}
}

float UPlatformerBlueprintLibrary::MarkCheckpointTime(class UObject* WorldContextObject, int32 CheckpointID)
{
	float DeltaTime = 0.0f;

	APlatformerGameMode* MyGame = GetGameFromContextObject(WorldContextObject);
	if (MyGame)
	{
		const float PrevBestTime = MyGame->GetBestCheckpointTime(CheckpointID);

		MyGame->SaveCheckpointTime(CheckpointID);

		if (PrevBestTime > 0)
		{
			const float CurrentTime = MyGame->GetCurrentCheckpointTime(CheckpointID);
			DeltaTime = (CurrentTime - PrevBestTime);
		}
	}

	return DeltaTime;
}

float UPlatformerBlueprintLibrary::GetCurrentCheckpointTime(class UObject* WorldContextObject, int32 CheckpointID)
{
	APlatformerGameMode* MyGame = GetGameFromContextObject(WorldContextObject);
	if (MyGame)
	{
		return MyGame->GetCurrentCheckpointTime(CheckpointID);
	}

	return -1.0f;
}

float UPlatformerBlueprintLibrary::GetBestCheckpointTime(class UObject* WorldContextObject, int32 CheckpointID)
{
	APlatformerGameMode* MyGame = GetGameFromContextObject(WorldContextObject);
	if (MyGame)
	{
		return MyGame->GetBestCheckpointTime(CheckpointID);
	}

	return -1.0f;
}

int32 UPlatformerBlueprintLibrary::GetLastCheckpoint(class UObject* WorldContextObject)
{
	APlatformerGameMode* MyGame = GetGameFromContextObject(WorldContextObject);
	if (MyGame)
	{
		return MyGame->GetNumCheckpoints() - 1;
	}

	return 0;
}

void UPlatformerBlueprintLibrary::DisplayMessage(class UObject* WorldContextObject, FString Message, float DisplayDuration, float PosX, float PosY, float TextScale, bool bRedBorder)
{
	APlayerController* LocalPC = GEngine->GetFirstLocalPlayerController(GEngine->GetWorldFromContextObjectChecked(WorldContextObject));
	APlatformerHUD* MyHUD = LocalPC ? Cast<APlatformerHUD>(LocalPC->GetHUD()) : NULL;
	if (MyHUD)
	{
		MyHUD->AddMessage(Message, DisplayDuration, PosX, PosY, TextScale/4, bRedBorder);
	}
}

void UPlatformerBlueprintLibrary::ShowHighscore(class UObject* WorldContextObject, TArray<float> Times, TArray<FString> Names)
{
	APlayerController* LocalPC = GEngine->GetFirstLocalPlayerController(GEngine->GetWorldFromContextObjectChecked(WorldContextObject));
	APlatformerHUD* MyHUD = LocalPC ? Cast<APlatformerHUD>(LocalPC->GetHUD()) : NULL;
	if (MyHUD)
	{
		MyHUD->ShowHighscore(MoveTemp(Times),MoveTemp(Names));
	}
}

void UPlatformerBlueprintLibrary::HideHighscore(class UObject* WorldContextObject)
{
	APlayerController* LocalPC = GEngine->GetFirstLocalPlayerController(GEngine->GetWorldFromContextObjectChecked(WorldContextObject));
	APlatformerHUD* MyHUD = LocalPC ? Cast<APlatformerHUD>(LocalPC->GetHUD()) : NULL;
	if (MyHUD)
	{
		MyHUD->HideHighscore();
	}
}

void UPlatformerBlueprintLibrary::SortHighscores(TArray<float> InTimes, TArray<FString> InNames, TArray<float>& OutTimes, TArray<FString>& OutNames, int32 MaxScores)
{
	// First sort the times and names
	for (auto iOuter = 0; iOuter < InTimes.Num(); iOuter++)
	{
		for (auto iInner = iOuter; iInner < InTimes.Num(); iInner++)
		{
			if (InTimes[iOuter] > InTimes[iInner])
			{
				InTimes.Swap(iOuter, iInner);
				InNames.Swap(iOuter, iInner);
			}
		}
	}
	// Copy the sorted array and cap the size
	OutTimes = MoveTemp(InTimes);
	OutNames = MoveTemp(InNames);
	OutTimes.RemoveAt(MaxScores, OutTimes.Num() - MaxScores);
	OutNames.RemoveAt(MaxScores, OutNames.Num() - MaxScores);
}

void UPlatformerBlueprintLibrary::ShowHighscorePrompt(class UObject* WorldContextObject)
{
	APlayerController* LocalPC = GEngine->GetFirstLocalPlayerController(GEngine->GetWorldFromContextObjectChecked(WorldContextObject));
	APlatformerHUD* MyHUD = LocalPC ? Cast<APlatformerHUD>(LocalPC->GetHUD()) : NULL;
	if (MyHUD)
	{
		MyHUD->ShowHighscorePrompt();
	}
}

FString UPlatformerBlueprintLibrary::DescribeTime(float TimeSeconds, bool bShowSign)
{
	const float AbsTimeSeconds = FMath::Abs(TimeSeconds);
	const bool bIsNegative = (TimeSeconds < 0);

	const int32 TotalSeconds = FMath::TruncToInt(AbsTimeSeconds) % 3600;
	const int32 NumMinutes = TotalSeconds / 60;
	const int32 NumSeconds = TotalSeconds % 60;

	const int32 NumMiliSeconds = FMath::TruncToInt(FMath::Fractional(AbsTimeSeconds) * 1000.0f);

	FString TimeDesc = FString::Printf(TEXT("%s%02d:%02d.%03d"), 
		bShowSign ? (bIsNegative ? TEXT("-") : TEXT("+")) : TEXT(""),
		NumMinutes, NumSeconds, NumMiliSeconds);

	return TimeDesc;
}

void UPlatformerBlueprintLibrary::ShowPicture(class UObject* WorldContextObject, UTexture2D* Picture, float FadeInTime, float ScreenCoverage, bool bKeepAspectRatio)
{
	UWorld* const MyWorld = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	APlatformerGameMode* const MyGameMode = MyWorld->GetAuthGameMode<APlatformerGameMode>();

	if (MyGameMode)
	{
		if (!MyGameMode->PlatformerPicture)
		{
			MyGameMode->PlatformerPicture = new FPlatformerPicture(MyWorld);
		}
		MyGameMode->PlatformerPicture->Show(Picture, FadeInTime, ScreenCoverage, bKeepAspectRatio);
	}
}

void UPlatformerBlueprintLibrary::HidePicture(class UObject* WorldContextObject, float FadeOutTime)
{
	UWorld* const MyWorld = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	APlatformerGameMode* const MyGameMode = MyWorld->GetAuthGameMode<APlatformerGameMode>();

	if (MyGameMode && MyGameMode->PlatformerPicture)
	{
		MyGameMode->PlatformerPicture->Hide(FadeOutTime);
	}
}

void UPlatformerBlueprintLibrary::ShowWaitingPanel(class UObject* WorldContextObject, bool isShow)
{
	APlayerController* LocalPC = GEngine->GetFirstLocalPlayerController(GEngine->GetWorldFromContextObjectChecked(WorldContextObject));
	APlatformerHUD* MyHUD = LocalPC ? Cast<APlatformerHUD>(LocalPC->GetHUD()) : NULL;
	if (MyHUD)
	{
		MyHUD->ShowWaitingPanel(isShow);
	}
}

#pragma region Ruyi SDK related

void UPlatformerBlueprintLibrary::RuyiSDKMangerSetHUD(class UObject* WorldContextObject, APlatformerHUD* HUD)
{
	FRuyiSDKManager::Instance()->MainHUD = HUD;
}

void UPlatformerBlueprintLibrary::RuyiSDKGetProfile(class UObject* WorldContextObject)
{
	FRuyiSDKManager::Instance()->StartRuyiSDKGetProfile();
}

void UPlatformerBlueprintLibrary::RuyiSDKPostScoreToLeadboard(class UObject* WorldContextObject, int score)
{
	FRuyiSDKManager::Instance()->m_Score = score;
	FRuyiSDKManager::Instance()->StartRuyiSDKPostScoreToLeadboard(score);
}

void UPlatformerBlueprintLibrary::RuyiSDKGetLeadboardPage(class UObject* WorldContextObject, int sortType, int startIndex, int endIndex)
{
	FRuyiSDKManager::Instance()->StartRuyiSDKGetLeadboardPage((Ruyi::SDK::BrainCloudApi::SortOrder::type)sortType, startIndex, endIndex);
}

void UPlatformerBlueprintLibrary::RuyiSDKFriendList() 
{
	FRuyiSDKManager::Instance()->StartRuyiSDKListFriends();
}

void UPlatformerBlueprintLibrary::RuyiSDKGetPartyInfo()
{
	FRuyiSDKManager::Instance()->StartRuyiSDKGetPartyInfo();
}

void UPlatformerBlueprintLibrary::RuyiSDKStartTelemetrySession() 
{
	FRuyiSDKManager::Instance()->StartRuyiSDKStartTelemetrySession();
}

void UPlatformerBlueprintLibrary::RuyiSDKUpdateVideo(class UObject* WorldContextObject, FString cloudFileName, FString localFilePath)
{
	FRuyiSDKManager::Instance()->StartRuyiSDKUpdateVideo(cloudFileName, localFilePath);
}

void UPlatformerBlueprintLibrary::RuyiSDKDownloadVideo(class UObject* WorldContextObject, FString cloudFileName)
{
	FRuyiSDKManager::Instance()->StartRuyiSDKDownloadVideo(cloudFileName);
}

void UPlatformerBlueprintLibrary::RuyiSDKAwardAchievement(class UObject* WorldContextObject, int score)
{
	FRuyiSDKManager::Instance()->StartRuyiSDKAwardAchievement(score);
}
#pragma endregion