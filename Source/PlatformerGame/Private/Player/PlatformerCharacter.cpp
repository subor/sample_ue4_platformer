// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "PlatformerGame.h"
#include "PlatformerCharacter.h"
#include "Player/PlatformerPlayerMovementComp.h"
#include "PlatformerGameMode.h"
#include "PlatformerClimbMarker.h"
#include "PlatformerPlayerController.h"
#include "Animation/AnimInstance.h"

APlatformerCharacter::APlatformerCharacter(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPlatformerPlayerMovementComp>(ACharacter::CharacterMovementComponentName))
{
	MinSpeedForHittingWall = 200.0f;
	GetMesh()->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPoseAndRefreshBones;
}

void APlatformerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// setting initial rotation
	SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
	UE_LOG(LogPlatformer, Log, TEXT("APlatformerCharacter::PostInitializeComponents PostInitializeComponents !!!"));

	if (FRuyiSDKManager::Instance()->IsSDKReady())
	{
		UE_LOG(LogPlatformer, Log, TEXT("APlatformerCharacter::PostInitializeComponents Subscribe !!!"));
		
		//FRuyiSDKManager::Instance()->SDK()->Subscriber->Subscribe("service/inputmanager_internal");
		FRuyiSDKManager::Instance()->SDK()->Subscriber->Subscribe("service/user_service_external");
		FRuyiSDKManager::Instance()->SDK()->Subscriber->AddMessageHandler(this, &APlatformerCharacter::InputStateChangeHandler);
	} else 
	{
		UE_LOG(LogPlatformer, Log, TEXT("APlatformerCharacter::PostInitializeComponents sdk not ready !!!"));
	}
}

void APlatformerCharacter::InputStateChangeHandler(std::string topic, apache::thrift::TBase* msg)
{
	FString fTopic = UTF8_TO_TCHAR(topic.c_str());
	//UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler DDDDDDDDDDDDDD topic:%s"), *fTopic);

	//auto idsc = dynamic_cast<Ruyi::SDK::InputManager::InputActionTriggered*>(msg);
	//auto idsc = dynamic_cast<Ruyi::SDK::InputManager::InputDeviceStateChanged*>(msg);
	auto idsc = dynamic_cast<Ruyi::SDK::UserServiceExternal::InputActionEvent*>(msg);
	
	if (idsc == NULL) 
	{
		return;
	}
	
	//userId is the id of the current logged account
	FString fUserId = UTF8_TO_TCHAR(idsc->userId.c_str());
	FString fAction = UTF8_TO_TCHAR(idsc->action.c_str());

	//std::vector<TriggerKeys>  Triggers is the numbers of current input from any device
	//TriggerKeys 
	//DeviceType: to identify your input device
	//Key: the key of your input device
	//NewValue/OldValue:  could be three value:0,1,2.  1 means press Down 2 means release 0 not define yet
	//NewValue is the current key state, if your press down, NewValue will be 1, when you release, NewValue will be 2, OldValue will be 1	
	//action:the Key value of config file.

	//you can just judge the input by "action" value of Triggers structure. The value of "action" can be modified
	//in system config file of the game package.
	int triggerNumber = idsc->Triggers.size();

	UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler userId:%s action:%s triggers Num:%d"), *fUserId, *fAction, triggerNumber);
	std::for_each(idsc->Triggers.begin(), idsc->Triggers.end(), [&](Ruyi::SDK::UserServiceExternal::TriggerKeys& key) 
	{
		UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler deviceType:%d, key:%d, newValue:%d, oldValue:%d"), key.DeviceType, key.Key, key.NewValue, key.OldValue);

		if ((1 == key.NewValue) && (0 == idsc->action.compare("GamePad_Up")))
		{
			OnStartJump();
		}
		if ((2 == key.NewValue) && (0 == idsc->action.compare("GamePad_Up")))
		{
			OnStopJump();
		}

		if ((1 == key.NewValue) && (0 == idsc->action.compare("GamePad_Down")))
		{
			OnStartSlide();
		}
		if ((2 == key.NewValue) && (0 == idsc->action.compare("GamePad_Down")))
		{
			OnStopSlide();
		}

		if ((1 == key.NewValue) && (0 == idsc->action.compare("GamePad_Left")))
		{
			InputLeft();
		}
		
		if ((1 == key.NewValue) && (0 == idsc->action.compare("GamePad_Right")))
		{
			InputRight();
		}

		if ((1 == key.NewValue) && (0 == idsc->action.compare("GamePad_X")))
		{
			APlatformerPlayerController* MyPC = Cast<APlatformerPlayerController>(Controller);
			if (MyPC)
			{
				MyPC->OnToggleInGameMenu();
			}
		}

		/*
		if (Ruyi::SDK::GlobalInputDefine::RuyiInputDeviceType::Keyboard == key.DeviceType)
		{
			//StartJump
			if ((Ruyi::SDK::GlobalInputDefine::Key::W == key.Key || Ruyi::SDK::GlobalInputDefine::Key::Up == key.Key) && (1 == key.NewValue))
			{
				OnStartJump();
			}
			//StopJump
			if ((Ruyi::SDK::GlobalInputDefine::Key::W == key.Key || Ruyi::SDK::GlobalInputDefine::Key::Up == key.Key) && (2 == key.NewValue))
			{
				OnStopJump();
			}
			//OnStartSlide
			if ((Ruyi::SDK::GlobalInputDefine::Key::S == key.Key || Ruyi::SDK::GlobalInputDefine::Key::Down == key.Key) && (1 == key.NewValue))
			{
				OnStartSlide();
			}
			//OnStopSlide
			if ((Ruyi::SDK::GlobalInputDefine::Key::S == key.Key || Ruyi::SDK::GlobalInputDefine::Key::Down == key.Key) && (2 == key.NewValue))
			{
				OnStopSlide();
			}
			//InputLeft
			if ((Ruyi::SDK::GlobalInputDefine::Key::A == key.Key || Ruyi::SDK::GlobalInputDefine::Key::Left == key.Key) && (1 == key.NewValue))
			{
				InputLeft();
			}
			//InputRight
			if ((Ruyi::SDK::GlobalInputDefine::Key::D == key.Key || Ruyi::SDK::GlobalInputDefine::Key::Right == key.Key) && (1 == key.NewValue))
			{
				InputRight();
			}
			//InputOK
			if (Ruyi::SDK::GlobalInputDefine::Key::Return == key.Key && (1 == key.NewValue))
			{
				InputOK();
			}
			//InGameMenu
			if (Ruyi::SDK::GlobalInputDefine::Key::Escape == key.Key && (1 == key.NewValue))
			{
				APlatformerPlayerController* MyPC = Cast<APlatformerPlayerController>(Controller);
				if (MyPC)
				{
					MyPC->OnToggleInGameMenu();
				}
			}
			//StartGame
			if (Ruyi::SDK::GlobalInputDefine::Key::Return == key.Key && (1 == key.NewValue))
			{
				//this one listened in blueprints
				//So if you wanna call the input logic in blueprints but still get the input from Ruyi SDK
				//We recommand you listener all the input in c++ from Ruyi SDK, then call in blueprints
			}
		} 
		else if (Ruyi::SDK::GlobalInputDefine::RuyiInputDeviceType::XB360 == key.DeviceType)
		{
			//StartJump
			if ((Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::DPadUp == key.Key) && (1 == key.NewValue))
			{
				OnStartJump();
			}
			//StopJump
			if ((Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::DPadUp == key.Key) && (2 == key.NewValue))
			{
				OnStopJump();
			}
			//OnStartSlide
			if ((Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::DPadDown == key.Key) && (1 == key.NewValue))
			{
				OnStartSlide();
			}
			//OnStopSlide
			if ((Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::DPadDown == key.Key) && (2 == key.NewValue))
			{
				OnStopSlide();
			}
			//InputLeft
			if ((Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::DPadLeft == key.Key) && (1 == key.NewValue))
			{
				InputLeft();
			}
			//InputRight
			if ((Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::DPadRight == key.Key) && (1 == key.NewValue))
			{
				InputRight();
			}
			//InputOK
			if (Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::B == key.Key && (1 == key.NewValue))
			{
				InputOK();
			}
			//InGameMenu
			if (Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::Start == key.Key && (1 == key.NewValue))
			{
				APlatformerPlayerController* MyPC = Cast<APlatformerPlayerController>(Controller);
				if (MyPC)
				{
					MyPC->OnToggleInGameMenu();
				}
			}
			//StartGame
			if (Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::A == key.Key && (1 == key.NewValue))
			{
				//this one listened in blueprints
				//So if you wanna call the input logic in blueprints but still get the input from Ruyi SDK
				//We recommand you listener all the input in c++ from Ruyi SDK, then call in blueprints
			}

		} 
		else if (Ruyi::SDK::GlobalInputDefine::RuyiInputDeviceType::RuyiController == key.DeviceType)
		{
			//StartJump
			//if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonUp == key.Key && (1 == key.NewValue))
			{
				OnStartJump();
			}
			//StopJump
			if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonUp == key.Key && (2 == key.NewValue))
			{
				OnStopJump();
			}
			//OnStartSlide
			if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonDown == key.Key  && (1 == key.NewValue))
			{
				OnStartSlide();
			}
			//OnStopSlide
			if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonDown == key.Key && (2 == key.NewValue))
			{
				OnStopSlide();
			}
			//InputLeft
			if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonLeft == key.Key && (1 == key.NewValue))
			{
				InputLeft();
			}
			//InputRight
			if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonRight == key.Key && (1 == key.NewValue))
			{
				InputRight();
			}
			//InputOK
			if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonStart == key.Key && (1 == key.NewValue))
			{
				InputOK();
			}
			//InGameMenu
			if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonHome == key.Key && (1 == key.NewValue))
			{
				APlatformerPlayerController* MyPC = Cast<APlatformerPlayerController>(Controller);
				if (MyPC)
				{
					MyPC->OnToggleInGameMenu();
				}
			}
		} else {}
		*/
	});
}

void APlatformerCharacter::InputConnectionChangeHandler(std::string topic, apache::thrift::TBase* msg)
{
	FString fTopic = UTF8_TO_TCHAR(topic.c_str());

	UE_LOG(LogPlatformer, Log, TEXT("InputConnectionChangeHandler EESSSSS topic:%s"), *fTopic);

}

void APlatformerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//you can use UE4 engine input system, or you can use input system from sdk.
	return;
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlatformerCharacter::OnStartJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlatformerCharacter::OnStopJump);
	PlayerInputComponent->BindAction("Slide", IE_Pressed, this, &APlatformerCharacter::OnStartSlide);
	PlayerInputComponent->BindAction("Slide", IE_Released, this, &APlatformerCharacter::OnStopSlide);
	PlayerInputComponent->BindAction("InputLeft", IE_Released, this, &APlatformerCharacter::InputLeft);
	PlayerInputComponent->BindAction("InputRight", IE_Released, this, &APlatformerCharacter::InputRight);
	PlayerInputComponent->BindAction("InputOK", IE_Released, this, &APlatformerCharacter::InputOK);
}

bool APlatformerCharacter::IsSliding() const
{
	UPlatformerPlayerMovementComp* MoveComp = Cast<UPlatformerPlayerMovementComp>(GetCharacterMovement());
	return MoveComp && MoveComp->IsSliding();
}

void APlatformerCharacter::CheckJumpInput(float DeltaTime)
{
	if (bPressedJump)
	{
		UPlatformerPlayerMovementComp* MoveComp = Cast<UPlatformerPlayerMovementComp>(GetCharacterMovement());
		if (MoveComp && MoveComp->IsSliding())
		{
			MoveComp->TryToEndSlide();
			return;
		}
	}

	Super::CheckJumpInput(DeltaTime);
}

void APlatformerCharacter::Tick(float DeltaSeconds)
{
	// decrease anim position adjustment
	if (!AnimPositionAdjustment.IsNearlyZero())
	{
		AnimPositionAdjustment = FMath::VInterpConstantTo(AnimPositionAdjustment, FVector::ZeroVector, DeltaSeconds, 400.0f);
		GetMesh()->SetRelativeLocation(GetBaseTranslationOffset() + AnimPositionAdjustment);
	}

	if (ClimbToMarker)
	{
		// correction in case climb marker is moving
		const FVector AdjustDelta = ClimbToMarker->GetComponentLocation() - ClimbToMarkerLocation;
		if (!AdjustDelta.IsZero())
		{
			SetActorLocation(GetActorLocation() + AdjustDelta, false);
			ClimbToMarkerLocation += AdjustDelta;
		}
	}

	Super::Tick(DeltaSeconds);

}

void APlatformerCharacter::PlayRoundFinished()
{
	APlatformerGameMode* MyGame = GetWorld()->GetAuthGameMode<APlatformerGameMode>();
	const bool bWon = MyGame && MyGame->IsRoundWon();
	
	PlayAnimMontage(bWon ? WonMontage : LostMontage);

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
}

void APlatformerCharacter::OnRoundFinished()
{
	// don't stop in mid air, will be continued from Landed() notify
	if (GetCharacterMovement()->MovementMode != MOVE_Falling)
	{
		PlayRoundFinished();
	}
}

void APlatformerCharacter::OnRoundReset()
{
	// reset animations
	if (GetMesh() && GetMesh()->AnimScriptInstance)
	{
		GetMesh()->AnimScriptInstance->Montage_Stop(0.0f);
	}

	// reset movement properties
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	bPressedJump = false;
	bPressedSlide = false;
}

void APlatformerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	APlatformerGameMode* MyGame = GetWorld()->GetAuthGameMode<APlatformerGameMode>();
	if (MyGame && MyGame->GetGameState() == EGameState::Finished)
	{
		PlayRoundFinished();
	}
}

void APlatformerCharacter::MoveBlockedBy(const FHitResult& Impact)
{
	const float ForwardDot = FVector::DotProduct(Impact.Normal, FVector::ForwardVector);
	if (GetCharacterMovement()->MovementMode != MOVE_None)
	{
		UE_LOG(LogPlatformer, Log, TEXT("Collision with %s, normal=(%f,%f,%f), dot=%f, %s"),
			*GetNameSafe(Impact.Actor.Get()),
			Impact.Normal.X, Impact.Normal.Y, Impact.Normal.Z,
			ForwardDot,
			*GetCharacterMovement()->GetMovementName());
	}

	if (GetCharacterMovement()->MovementMode == MOVE_Walking && ForwardDot < -0.9f)
	{
		UPlatformerPlayerMovementComp* MyMovement = Cast<UPlatformerPlayerMovementComp>(GetCharacterMovement());
		const float Speed = FMath::Abs(FVector::DotProduct(MyMovement->Velocity, FVector::ForwardVector));
		// if running or sliding: play bump reaction and jump over obstacle

		float Duration = 0.01f;
		if (Speed > MinSpeedForHittingWall)
		{
			Duration = PlayAnimMontage(HitWallMontage);
		}
		GetWorldTimerManager().SetTimer(TimerHandle_ClimbOverObstacle, this, &APlatformerCharacter::ClimbOverObstacle, Duration, false);
		MyMovement->PauseMovementForObstacleHit();
	}
	else if (GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		// if in mid air: try climbing to hit marker
		APlatformerClimbMarker* Marker = Cast<APlatformerClimbMarker>(Impact.Actor.Get());
		if (Marker)
		{
			ClimbToLedge(Marker);

			UPlatformerPlayerMovementComp* MyMovement = Cast<UPlatformerPlayerMovementComp>(GetCharacterMovement());
			MyMovement->PauseMovementForLedgeGrab();
		}
	}
}

void APlatformerCharacter::ResumeMovement()
{
	SetActorEnableCollision(true);

	// restore movement state and saved speed
	UPlatformerPlayerMovementComp* MyMovement = Cast<UPlatformerPlayerMovementComp>(GetCharacterMovement());
	MyMovement->RestoreMovement();

	ClimbToMarker = NULL;
}

void APlatformerCharacter::ClimbOverObstacle()
{
	// climbing over obstacle:
	// - there are three animations matching with three types of predefined obstacle heights
	// - pawn is moved using root motion, ending up on top of obstacle as animation ends

	const FVector ForwardDir = GetActorForwardVector();
	const FVector TraceStart = GetActorLocation() + ForwardDir * 150.0f + FVector(0,0,1) * (GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 150.0f);
	const FVector TraceEnd = TraceStart + FVector(0,0,-1) * 500.0f;

	FCollisionQueryParams TraceParams(NAME_None, FCollisionQueryParams::GetUnknownStatId(), true);
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Pawn, TraceParams);

	if (Hit.bBlockingHit)
	{
		const FVector DestPosition = Hit.ImpactPoint + FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		const float ZDiff = DestPosition.Z - GetActorLocation().Z;
		UE_LOG(LogPlatformer, Log, TEXT("Climb over obstacle, Z difference: %f (%s)"), ZDiff,
			 (ZDiff < ClimbOverMidHeight) ? TEXT("small") : (ZDiff < ClimbOverBigHeight) ? TEXT("mid") : TEXT("big"));

		UAnimMontage* Montage = (ZDiff < ClimbOverMidHeight) ? ClimbOverSmallMontage : (ZDiff < ClimbOverBigHeight) ? ClimbOverMidMontage : ClimbOverBigMontage;
		
		// set flying mode since it needs Z changes. If Walking or Falling, we won't be able to apply Z changes
		// this gets reset in the ResumeMovement
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		SetActorEnableCollision(false);
		const float Duration = PlayAnimMontage(Montage);
		GetWorldTimerManager().SetTimer(TimerHandle_ResumeMovement, this, &APlatformerCharacter::ResumeMovement, Duration - 0.1f, false);
	}
	else
	{
		// shouldn't happen
		ResumeMovement();
	}
}

void APlatformerCharacter::ClimbToLedge(const APlatformerClimbMarker* MoveToMarker)
{
	ClimbToMarker = MoveToMarker ? MoveToMarker->FindComponentByClass<UStaticMeshComponent>() : NULL;
	ClimbToMarkerLocation = ClimbToMarker ? ClimbToMarker->GetComponentLocation() : FVector::ZeroVector;

	// place on top left corner of marker, but preserve current Y coordinate
	const FBox MarkerBox = MoveToMarker->GetMesh()->Bounds.GetBox();
	const FVector DesiredPosition(MarkerBox.Min.X, GetActorLocation().Y, MarkerBox.Max.Z);

	// climbing to ledge:
	// - pawn is placed on top of ledge (using ClimbLedgeGrabOffsetX to offset from grab point) immediately
	// - AnimPositionAdjustment modifies mesh relative location to smooth transition
	//   (mesh starts roughly at the same position, additional offset quickly decreases to zero in Tick)

	const FVector StartPosition = GetActorLocation();
	FVector AdjustedPosition = DesiredPosition;
	AdjustedPosition.X += (ClimbLedgeGrabOffsetX * GetMesh()->RelativeScale3D.X) - GetBaseTranslationOffset().X;
	AdjustedPosition.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	TeleportTo(AdjustedPosition, GetActorRotation(), false, true);

	AnimPositionAdjustment = StartPosition - (GetActorLocation() - (ClimbLedgeRootOffset * GetMesh()->RelativeScale3D));
	GetMesh()->SetRelativeLocation(GetBaseTranslationOffset() + AnimPositionAdjustment);

	const float Duration = PlayAnimMontage(ClimbLedgeMontage);
	GetWorldTimerManager().SetTimer(TimerHandle_ResumeMovement, this, &APlatformerCharacter::ResumeMovement, Duration - 0.1f, false);
}

void APlatformerCharacter::OnStartJump()
{
	APlatformerGameMode* const MyGame = GetWorld()->GetAuthGameMode<APlatformerGameMode>();
	APlatformerPlayerController* MyPC = Cast<APlatformerPlayerController>(Controller);
	if (MyPC)
	{
		//zzm
		//Add keyboard/gamepad operation logic when drawing HighscoreEntryPrompt
		EGameState::Type GameState = MyGame->GetGameState();

		if (GameState == EGameState::Finished)
		{
			APlatformerHUD* MyHUD = Cast<APlatformerHUD>(MyPC->GetHUD());
			if (MyHUD)
			{
				if (MyHUD->HighScoreName[MyHUD->CurrentLetter] < 'Z')
				{
					MyHUD->HighScoreName[MyHUD->CurrentLetter]++;
				}
			}
		}
		//zzm

		if (MyPC->TryStartingGame())
		{
			return;
		}

		if (!MyPC->IsMoveInputIgnored() &&
			MyGame && MyGame->IsRoundInProgress())
		{
			bPressedJump = true;
		}
	}

	
}

void APlatformerCharacter::OnStopJump()
{
	bPressedJump = false;
	StopJumping();
}

void APlatformerCharacter::OnStartSlide()
{
	APlatformerGameMode* MyGame = GetWorld()->GetAuthGameMode<APlatformerGameMode>();
	APlatformerPlayerController* MyPC = Cast<APlatformerPlayerController>(Controller);
	if (MyPC)
	{
		//zzm
		//Add keyboard/gamepad operation logic when drawing HighscoreEntryPrompt
		EGameState::Type GameState = MyGame->GetGameState();

		if (GameState == EGameState::Finished)
		{
			APlatformerHUD* MyHUD = Cast<APlatformerHUD>(MyPC->GetHUD());
			if (MyHUD)
			{
				if (MyHUD->HighScoreName[MyHUD->CurrentLetter] > 'A')
				{
					MyHUD->HighScoreName[MyHUD->CurrentLetter]--;
				}
			}
		}
		//zzm

		if (MyPC->TryStartingGame())
		{
			return;
		}
		
		if (!MyPC->IsMoveInputIgnored() &&
			MyGame && MyGame->IsRoundInProgress())
		{
			bPressedSlide = true;
		}
	}
}

void APlatformerCharacter::OnStopSlide()
{
	bPressedSlide = false;
}

void APlatformerCharacter::PlaySlideStarted()
{
	if (SlideSound)
	{
		SlideAC = UGameplayStatics::SpawnSoundAttached(SlideSound, GetMesh());
	}
}

void APlatformerCharacter::PlaySlideFinished()
{
	if (SlideAC)
	{
		SlideAC->Stop();
		SlideAC = NULL;
	}
}

bool APlatformerCharacter::WantsToSlide() const
{
	return bPressedSlide;
}

float APlatformerCharacter::GetCameraHeightChangeThreshold() const
{
	return CameraHeightChangeThreshold;
}

void APlatformerCharacter::InputLeft() 
{
	APlatformerGameMode* MyGame = GetWorld()->GetAuthGameMode<APlatformerGameMode>();
	APlatformerPlayerController* MyPC = Cast<APlatformerPlayerController>(Controller);
	if (MyPC)
	{
		EGameState::Type GameState = MyGame->GetGameState();

		if (GameState == EGameState::Finished)
		{
			APlatformerHUD* MyHUD = Cast<APlatformerHUD>(MyPC->GetHUD());
			if (MyHUD)
			{
				UE_LOG(LogPlatformer, Log, TEXT("APlatformerCharacter::InputLeft CurrentLetter:%d"), MyHUD->CurrentLetter);

				if (MyHUD->CurrentLetter == 0)
				{
					MyHUD->CurrentLetter = 2;
				} else 
				{
					--MyHUD->CurrentLetter;
				}
			}
		}
	}
}

void APlatformerCharacter::InputRight() 
{
	APlatformerGameMode* MyGame = GetWorld()->GetAuthGameMode<APlatformerGameMode>();
	APlatformerPlayerController* MyPC = Cast<APlatformerPlayerController>(Controller);
	if (MyPC) 
	{
		EGameState::Type GameState = MyGame->GetGameState();

		if (GameState == EGameState::Finished)
		{
			APlatformerHUD* MyHUD = Cast<APlatformerHUD>(MyPC->GetHUD());
			if (MyHUD)
			{
				UE_LOG(LogPlatformer, Log, TEXT("APlatformerCharacter::InputLeft CurrentLetter:%d"), MyHUD->CurrentLetter);
			
				if (MyHUD->CurrentLetter >= 2)
				{
					MyHUD->CurrentLetter = 0;
				} else
				{
					++MyHUD->CurrentLetter;
				}
			}
		}
	}
}

void APlatformerCharacter::InputOK()
{
	APlatformerGameMode* MyGame = GetWorld()->GetAuthGameMode<APlatformerGameMode>();
	APlatformerPlayerController* MyPC = Cast<APlatformerPlayerController>(Controller);
	if (MyPC)
	{
		EGameState::Type GameState = MyGame->GetGameState();

		if (GameState == EGameState::Finished)
		{
			APlatformerHUD* MyHUD = Cast<APlatformerHUD>(MyPC->GetHUD());
			if (MyHUD)
			{
				MyHUD->EndHighscoreEntryPrompt();
			}
		}
	}
}