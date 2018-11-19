// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "PlatformerGame.h"
#include "PlatformerPlayerController_Menu.h"
#include "PlatformerMainMenu.h"


APlatformerPlayerController_Menu::APlatformerPlayerController_Menu(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void APlatformerPlayerController_Menu::PostInitializeComponents() 
{
	Super::PostInitializeComponents();
	PlatformerMainMenu = MakeShareable(new FPlatformerMainMenu());
	PlatformerMainMenu->MakeMenu(this);
	PlatformerMainMenu->ShowRootMenu();
}

void APlatformerPlayerController_Menu::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PlatformerMainMenu.IsValid())
	{
		PlatformerMainMenu->DestroyRootMenu();
		PlatformerMainMenu = nullptr;
	}
}
