// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "BoneControllers/AnimNode_SkeletalControlBase.h"
#include "AnimNode_FootPlacementIK.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct FOOTIKRUNTIME_API FAnimNode_FootPlacementIK : public FAnimNode_SkeletalControlBase
{
	GENERATED_USTRUCT_BODY()

	/** Name of bone to control. This is the main bone chain to modify from. **/
	UPROPERTY(EditAnywhere, Category=IK)
	FBoneReference IKBone;

	/** Effector Location. **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=JointTarget, meta=(PinShownByDefault))
	FVector JointTargetLocation = FVector::ZeroVector;

	/** Reference frame of Effector Location. */
	UPROPERTY(EditAnywhere, Category=JointTarget)
	TEnumAsByte<enum EBoneControlSpace> JointTargetLocationSpace = BCS_WorldSpace;

	/** If EffectorLocationSpace is a bone, this is the bone to use. **/
	UPROPERTY(EditAnywhere, Category=JointTarget)
	FName JointTargetSpaceBoneName;

	/** if set, node can stretch as much as possible, bone to catch effector location */
	UPROPERTY(EditAnywhere, Category=IK)
	uint32 bAllowStretching:1;

	/** limits for bone stretching */
	UPROPERTY(EditAnywhere, Category=IK)
	FVector2D StretchLimits = FVector2D::ZeroVector;

	/** Z offset from hit point, to correct effector location */
	UPROPERTY(EditAnywhere, Category=IK)
	float HitZOffset = 0.0f;

	/** time to blend in/out node's influence */
	UPROPERTY(EditAnywhere, Category=IK)
	float BlendTime = 0.2f;

	/** Internal use - activation time for node blending */
	float ActivationTime = 0.0f;

	// FAnimNode_Base interface
	virtual bool HasPreUpdate() const override { return true; }
	virtual void PreUpdate(const UAnimInstance* InAnimInstance) override;
	// End of FAnimNode_Base interface

	// FAnimNode_SkeletalControlBase interface
	virtual void EvaluateSkeletalControl_AnyThread(FComponentSpacePoseContext& Output, TArray<FBoneTransform>& OutBoneTransforms) override;
	virtual bool IsValidToEvaluate(const USkeleton* Skeleton, const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

	// Replace this when bitfields can have default member initializers
	FAnimNode_FootPlacementIK()
		: bAllowStretching(0)
	{
	}

private:
	enum EMyBlendState
	{
		STATE_UNKNOWN = 0,
		STATE_BLEND_IN = 1,
		STATE_BLEND_OUT = 2
	};

	/** state of our node (init, blend-in, blend-out) */
	EMyBlendState BlendState = STATE_UNKNOWN;

	/** effector location calculated dynamically */
	FVector EffectorLocation = FVector::ZeroVector;

	// FAnimNode_SkeletalControlBase interface
	virtual void InitializeBoneReferences(const FBoneContainer& RequiredBones) override;
	// End of FAnimNode_SkeletalControlBase interface

	/** calculate current effector location and blend alpha */
	void CalculateEffector(float DeltaTime, USkeletalMeshComponent* SkelComp, FCSPose<FCompactPose>& MeshBases);
};
