// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PlatformerGame.h"
#include "PlatformerOptions.h"
#include "PlatformerGameUserSettings.h"


#define LOCTEXT_NAMESPACE "PlatformerGame.HUD.Menu"

// @note Construct isn't a good name as it can be confused with Slate interface
void FPlatformerOptions::MakeMenu(TWeakObjectPtr<APlayerController> _PCOwner)
{
	PCOwner = _PCOwner;
	UserSettings = NULL;

	TArray<FText> ResolutionList;
	TArray<FText> OnOffList;
	TArray<FText> VolumeList;

	// Build an array of resolutions available
	for (int32 i = 0; i < PlatformerResCount; i++)
	{
		ResolutionList.Add(FText::Format(FText::FromString("{0}x{1}"), FText::AsNumber(PlatformerResolutions[i].X), FText::AsNumber(PlatformerResolutions[i].Y)));
	}

	OnOffList.Add(LOCTEXT("Off","OFF"));
	OnOffList.Add(LOCTEXT("On","ON"));

	// Volume 0-10
	for (int32 i = 0; i < 11; i++)
	{
		VolumeList.Add(FText::AsNumber(i));
	}

	MenuTitle = LOCTEXT("Options", "OPTIONS");
	SoundVolumeOption = AddMenuItemWithOptions<FPlatformerOptions>(LOCTEXT("SoundVolume", "SOUND VOLUME"), VolumeList, this, &FPlatformerOptions::SoundVolumeOptionChanged);
#if PLATFORM_DESKTOP
	VideoResolutionOption = AddMenuItemWithOptions<FPlatformerOptions>(LOCTEXT("Resolution", "RESOLUTION"), ResolutionList, this, &FPlatformerOptions::VideoResolutionOptionChanged);
	FullScreenOption = AddMenuItemWithOptions<FPlatformerOptions>(LOCTEXT("FullScreen", "FULL SCREEN"), OnOffList, this, &FPlatformerOptions::FullScreenOptionChanged);
#endif
	
	// Setup some handlers for misc actions.
	SetAcceptHandler<FPlatformerOptions>(this, &FPlatformerOptions::ApplySettings);
	SetCancelHandler<FPlatformerOptions>(this, &FPlatformerOptions::DiscardSettings);
	SetOnOpenHandler<FPlatformerOptions>(this, &FPlatformerOptions::UpdateOptions);

	AddMenuItem(LOCTEXT("AcceptChanges", "ACCEPT CHANGES"), this, &FPlatformerOptions::OnAcceptSettings);
	AddMenuItem(LOCTEXT("DiscardChanges", "DISCARD CHANGES"), this, &FPlatformerOptions::OnDiscardSettings);

	UserSettings = CastChecked<UPlatformerGameUserSettings>(GEngine->GetGameUserSettings());
	SoundVolumeOpt = UserSettings->GetSoundVolume();

#if PLATFORM_DESKTOP
	// Sanitize resolution, this guarantees we'll only use a supported resolution.
	// Only do this on desktop, on other platforms, this can resize the actual GSystemResolution, which we don't want -
	// we'd want to use other methods like r.MobileContentScaleFactor or r.ScreenPercentage. This crashes IOS
	int32 SupportedResolutionIndex = GetCurrentResolutionIndex(UserSettings->GetScreenResolution());
	ResolutionOpt = PlatformerResolutions[SupportedResolutionIndex];
#endif

	bFullScreenOpt = UserSettings->GetFullscreenMode();
}



void FPlatformerOptions::ApplySettings()
{
	UserSettings->SetSoundVolume(SoundVolumeOpt);
	UserSettings->SetScreenResolution(ResolutionOpt);
	UserSettings->SetFullscreenMode(bFullScreenOpt);
	UserSettings->ApplySettings(false);
}

void FPlatformerOptions::DiscardSettings()
{
	RevertChanges(); 
}

void FPlatformerOptions::RevertChanges()
{
	UpdateOptions();
}

void FPlatformerOptions::OnAcceptSettings()
{	
	RootMenuPageWidget->MenuGoBack(false);
}

void FPlatformerOptions::OnDiscardSettings()
{
	RootMenuPageWidget->MenuGoBack(true);
}

int32 FPlatformerOptions::GetCurrentResolutionIndex(FIntPoint CurrentRes)
{
	int32 Result = 0; // return first valid resolution if match not found
	for (int32 i = 0; i < PlatformerResCount; i++)
	{
		if (PlatformerResolutions[i] == CurrentRes)
		{
			Result = i;
			break;
		}
	}
	return Result;
}

void FPlatformerOptions::UpdateOptions()
{
	//grab the user settings
	UserSettings = CastChecked<UPlatformerGameUserSettings>(GEngine->GetGameUserSettings());
#if PLATFORM_DESKTOP
	VideoResolutionOption->SelectedMultiChoice = GetCurrentResolutionIndex(UserSettings->GetScreenResolution());
	FullScreenOption->SelectedMultiChoice = UserSettings->GetFullscreenMode() != EWindowMode::Windowed ? 1 : 0;
#endif
	SoundVolumeOption->SelectedMultiChoice = FMath::TruncToInt(UserSettings->GetSoundVolume() * 10.0f);
}

void FPlatformerOptions::VideoResolutionOptionChanged(TSharedPtr<FGameMenuItem> MenuItem, int32 MultiOptionIndex)
{
	ResolutionOpt = PlatformerResolutions[MultiOptionIndex];
}

void FPlatformerOptions::FullScreenOptionChanged(TSharedPtr<FGameMenuItem> MenuItem, int32 MultiOptionIndex)
{
	static auto CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.FullScreenMode"));
	auto FullScreenMode = CVar->GetValueOnGameThread() == 1 ? EWindowMode::WindowedFullscreen : EWindowMode::Fullscreen;
	bFullScreenOpt = MultiOptionIndex == 0 ? EWindowMode::Windowed : FullScreenMode;
}

void FPlatformerOptions::SoundVolumeOptionChanged(TSharedPtr<FGameMenuItem> MenuItem, int32 MultiOptionIndex)
{
	SoundVolumeOpt = MultiOptionIndex / 10.0f;
}

#undef LOCTEXT_NAMESPACE
