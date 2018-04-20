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
		
		FRuyiSDKManager::Instance()->SDK()->Subscriber->Subscribe("service/inputmanager_internal");
		FRuyiSDKManager::Instance()->SDK()->Subscriber->AddMessageHandler(this, &APlatformerCharacter::InputStateChangeHandler);
	} else 
	{
		UE_LOG(LogPlatformer, Log, TEXT("APlatformerCharacter::PostInitializeComponents sdk not ready !!!"));
	}
}

void APlatformerCharacter::InputStateChangeHandler(std::string topic, apache::thrift::TBase* msg)
{
	//FString fTopic = UTF8_TO_TCHAR(topic.c_str());
	//UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler DDDDDDDDDDDDDD topic:%s"), *fTopic);

	//auto idsc = dynamic_cast<Ruyi::SDK::InputManager::InputActionTriggered*>(msg);
	auto idsc = dynamic_cast<Ruyi::SDK::InputManager::InputDeviceStateChanged*>(msg);

	if (idsc == NULL) 
	{
		return;
	}
	
	//UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler header:%d, x360:%d, dgamepad:%d, djoystick:%d, dkeyboard:%d, dmouse:%d, ruyicontroller:%d"), idsc->__isset.header, idsc->__isset.x360, idsc->__isset.dgamepad, idsc->__isset.djoystick, idsc->__isset.dkeyboard, idsc->__isset.dmouse, idsc->__isset.ruyicontroller);

	//could be multiple input source so I use "if" not "else if"
	if (idsc->__isset.header == 1)
	{
		//UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler header:%d"), idsc->__isset.header);
	}
	if (idsc->__isset.x360 == 1)
	{
		int64_t PacketNumber = idsc->x360.PacketNumber;
		//FString fDeviceId = UTF8_TO_TCHAR(idsc->x360.DeviceId.c_str());
		int16_t LeftThumbDeadZone = idsc->x360.LeftThumbDeadZone;
		int16_t RightThumbDeadZone = idsc->x360.RightThumbDeadZone;
		int32_t Buttons = idsc->x360.Buttons;
		int8_t LeftTrigger = idsc->x360.LeftTrigger;
		int8_t RightTrigger = idsc->x360.RightTrigger;
		int16_t LeftThumbX = idsc->x360.LeftThumbX;
		int16_t LeftThumbY = idsc->x360.LeftThumbY;
		int16_t RightThumbX = idsc->x360.RightThumbX;
		int16_t RightThumbY = idsc->x360.RightThumbY;
		bool isPressed = true;
		bool isReleased = false;
		//StartJump
		if ((Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::DPadUp == Buttons) && isPressed)
		{
			OnStartJump();
		}
		//StopJump
		if ((Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::DPadUp == Buttons) && isReleased)
		{
			OnStopJump();
		}
		//OnStartSlide
		if ((Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::DPadDown == Buttons) && isPressed)
		{
			OnStartSlide();
		}
		//OnStopSlide
		if ((Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::DPadDown == Buttons) && isReleased)
		{
			OnStopSlide();
		}
		//InputLeft
		if ((Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::DPadLeft == Buttons) && isPressed)
		{
			InputLeft();
		}
		//InputRight
		if ((Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::DPadRight == Buttons) && isPressed)
		{
			InputRight();
		}
		//InputOK
		if (Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::B == Buttons && isPressed)
		{
			InputOK();
		}
		//InGameMenu
		if (Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::Start == Buttons && isPressed)
		{
			APlatformerPlayerController* MyPC = Cast<APlatformerPlayerController>(Controller);
			if (MyPC)
			{
				MyPC->OnToggleInGameMenu();
			}
		}
		//StartGame
		if (Ruyi::SDK::GlobalInputDefine::GamepadButtonFlags::A == Buttons && isPressed)
		{
			//this one listened in blueprints
			//So if you wanna call the input logic in blueprints but still get the input from Ruyi SDK
			//We recommand you listener all the input in c++ from Ruyi SDK, then call in blueprints
		}
		UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler x360 PacketNumber:%d, LeftThumbDeadZone:%d, RightThumbDeadZone:%d"), PacketNumber, LeftThumbDeadZone, RightThumbDeadZone);
		UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler x360 Buttons:%d, LeftTrigger:%d, RightTrigger:%d"), Buttons, LeftTrigger, RightTrigger);
		UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler x360 LeftThumbX:%d, LeftThumbY:%d, RightThumbX:%d, RightThumbY:%d"), LeftThumbX, LeftThumbY, RightThumbX, RightThumbY);
	}
	if (idsc->__isset.dgamepad == 1)
	{
		UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler dgamepad:%d"), idsc->__isset.dgamepad);
	}
	if (idsc->__isset.djoystick == 1)
	{
		UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler djoystick:%d"), idsc->__isset.djoystick);
	}
	if (idsc->__isset.dkeyboard == 1)
	{
		char key = idsc->dkeyboard.Key;
		bool isPressed = idsc->dkeyboard.IsPressed;
		bool isReleased = idsc->dkeyboard.IsReleased;

		//UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler key:%d, isPressed:%d, isReleased:%d"), key, isPressed, isReleased);

		//StartJump
		if ( (Ruyi::SDK::GlobalInputDefine::Key::W == key || Ruyi::SDK::GlobalInputDefine::Key::Up == key) && isPressed)
		{
			OnStartJump();
		}
		//StopJump
		if ((Ruyi::SDK::GlobalInputDefine::Key::W == key || Ruyi::SDK::GlobalInputDefine::Key::Up == key) && isReleased)
		{
			OnStopJump();
		}
		//OnStartSlide
		if ( (Ruyi::SDK::GlobalInputDefine::Key::S == key || Ruyi::SDK::GlobalInputDefine::Key::Down == key) && isPressed )
		{
			OnStartSlide();
		}
		//OnStopSlide
		if ((Ruyi::SDK::GlobalInputDefine::Key::S == key || Ruyi::SDK::GlobalInputDefine::Key::Down == key) && isReleased)
		{
			OnStopSlide();
		}
		//InputLeft
		if ((Ruyi::SDK::GlobalInputDefine::Key::A == key || Ruyi::SDK::GlobalInputDefine::Key::Left == key) && isPressed)
		{
			InputLeft();
		}
		//InputRight
		if ((Ruyi::SDK::GlobalInputDefine::Key::D == key || Ruyi::SDK::GlobalInputDefine::Key::Right == key) && isPressed)
		{
			InputRight();
		}
		//InputOK
		if (Ruyi::SDK::GlobalInputDefine::Key::Return == key && isPressed)
		{
			InputOK();
		}
		//InGameMenu
		if (Ruyi::SDK::GlobalInputDefine::Key::Escape == key && isPressed)
		{
			APlatformerPlayerController* MyPC = Cast<APlatformerPlayerController>(Controller);
			if (MyPC) 
			{
				MyPC->OnToggleInGameMenu();
			}
		}
		//StartGame
		if (Ruyi::SDK::GlobalInputDefine::Key::Return == key && isPressed)
		{
			//this one listened in blueprints
			//So if you wanna call the input logic in blueprints but still get the input from Ruyi SDK
			//We recommand you listener all the input in c++ from Ruyi SDK, then call in blueprints
		}
	}
	if (idsc->__isset.dmouse == 1)
	{
		//UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler dmouse:%d"), idsc->__isset.dmouse);
	} 
	if (idsc->__isset.ruyicontroller == 1)
	{
		int64_t PacketId = idsc->ruyicontroller.PacketId;
		int32_t ChannelId = idsc->ruyicontroller.ChannelId;
		//FString fDeviceId = UTF8_TO_TCHAR(idsc->ruyicontroller.DeviceId.c_str());
		int32_t KeyPress = idsc->ruyicontroller.KeyPress;
		int8_t AnalogL2 = idsc->ruyicontroller.AnalogL2;
		int8_t AnalogR2 = idsc->ruyicontroller.AnalogR2;
		int8_t AnalogLeftJoyX = idsc->ruyicontroller.AnalogLeftJoyX;
		int8_t AnalogLeftJoyY = idsc->ruyicontroller.AnalogLeftJoyY;
		int8_t AnalogRightJoyX = idsc->ruyicontroller.AnalogRightJoyX;
		int8_t AnalogRightJoyY = idsc->ruyicontroller.AnalogRightJoyY;
		//tempoary, ruyi controller not support isPressd/isReleased
		bool isPressed = true;
		bool isReleased = false;
		//StartJump
		if ( Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonUp == KeyPress && isPressed)
		{
			OnStartJump();
		}
		//StopJump
		if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonUp == KeyPress && isReleased)
		{
			OnStopJump();
		}
		//OnStartSlide
		if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonDown == KeyPress  && isPressed)
		{
			OnStartSlide();
		}
		//OnStopSlide
		if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonDown == KeyPress && isReleased)
		{
			OnStopSlide();
		}
		//InputLeft
		if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonLeft == KeyPress && isPressed)
		{
			InputLeft();
		}
		//InputRight
		if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonRight == KeyPress && isPressed)
		{
			InputRight();
		}
		//InputOK
		if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonStart == KeyPress && isPressed)
		{
			InputOK();
		}
		//InGameMenu
		if (Ruyi::SDK::GlobalInputDefine::RuyiControllerKey::eButtonHome == KeyPress && isPressed)
		{
			APlatformerPlayerController* MyPC = Cast<APlatformerPlayerController>(Controller);
			if (MyPC)
			{
				MyPC->OnToggleInGameMenu();
			}
		}
		UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler ruyicontroller PacketId:%d, ChannelId:%d"), PacketId, ChannelId);
		UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler ruyicontroller KeyPress:%d, AnalogL2:%d, AnalogR2:%d"), KeyPress, AnalogL2, AnalogR2);
		UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler ruyicontroller AnalogLeftJoyX:%d, AnalogLeftJoyY:%d, AnalogRightJoyX:%d, AnalogRightJoyY:%d"), AnalogLeftJoyX, AnalogLeftJoyY, AnalogRightJoyX, AnalogRightJoyY);
	}


	//if (NULL == msg) return;

	//FString deviceId = UTF8_TO_TCHAR(msg->->deviceId.c_str());
	//FString name = UTF8_TO_TCHAR(idsc->name.c_str());
	
	//UE_LOG(LogPlatformer, Log, TEXT("InputActionTriggered deviceId:%s name:%s"), *deviceId, *name);
	//UE_LOG(LogPlatformer, Log, TEXT("InputStateChangeHandler EESSSSS"));
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