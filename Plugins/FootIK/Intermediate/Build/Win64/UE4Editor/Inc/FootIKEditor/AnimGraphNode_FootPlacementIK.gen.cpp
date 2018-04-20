// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Private/FootIKEditorPrivatePCH.h"
#include "Classes/AnimGraphNode_FootPlacementIK.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAnimGraphNode_FootPlacementIK() {}
// Cross Module References
	FOOTIKEDITOR_API UClass* Z_Construct_UClass_UAnimGraphNode_FootPlacementIK_NoRegister();
	FOOTIKEDITOR_API UClass* Z_Construct_UClass_UAnimGraphNode_FootPlacementIK();
	ANIMGRAPH_API UClass* Z_Construct_UClass_UAnimGraphNode_SkeletalControlBase();
	UPackage* Z_Construct_UPackage__Script_FootIKEditor();
	FOOTIKRUNTIME_API UScriptStruct* Z_Construct_UScriptStruct_FAnimNode_FootPlacementIK();
// End Cross Module References
	void UAnimGraphNode_FootPlacementIK::StaticRegisterNativesUAnimGraphNode_FootPlacementIK()
	{
	}
	UClass* Z_Construct_UClass_UAnimGraphNode_FootPlacementIK_NoRegister()
	{
		return UAnimGraphNode_FootPlacementIK::StaticClass();
	}
	UClass* Z_Construct_UClass_UAnimGraphNode_FootPlacementIK()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_UAnimGraphNode_SkeletalControlBase,
				(UObject* (*)())Z_Construct_UPackage__Script_FootIKEditor,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "IncludePath", "AnimGraphNode_FootPlacementIK.h" },
				{ "ModuleRelativePath", "Classes/AnimGraphNode_FootPlacementIK.h" },
			};
#endif
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam NewProp_Node_MetaData[] = {
				{ "Category", "Settings" },
				{ "ModuleRelativePath", "Classes/AnimGraphNode_FootPlacementIK.h" },
			};
#endif
			static const UE4CodeGen_Private::FStructPropertyParams NewProp_Node = { UE4CodeGen_Private::EPropertyClass::Struct, "Node", RF_Public|RF_Transient|RF_MarkAsNative, 0x0010000000000001, 1, nullptr, STRUCT_OFFSET(UAnimGraphNode_FootPlacementIK, Node), Z_Construct_UScriptStruct_FAnimNode_FootPlacementIK, METADATA_PARAMS(NewProp_Node_MetaData, ARRAY_COUNT(NewProp_Node_MetaData)) };
			static const UE4CodeGen_Private::FPropertyParamsBase* const PropPointers[] = {
				(const UE4CodeGen_Private::FPropertyParamsBase*)&NewProp_Node,
			};
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<UAnimGraphNode_FootPlacementIK>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&UAnimGraphNode_FootPlacementIK::StaticClass,
				DependentSingletons, ARRAY_COUNT(DependentSingletons),
				0x00080080u,
				nullptr, 0,
				PropPointers, ARRAY_COUNT(PropPointers),
				nullptr,
				&StaticCppClassTypeInfo,
				nullptr, 0,
				METADATA_PARAMS(Class_MetaDataParams, ARRAY_COUNT(Class_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUClass(OuterClass, ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UAnimGraphNode_FootPlacementIK, 3768396104);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UAnimGraphNode_FootPlacementIK(Z_Construct_UClass_UAnimGraphNode_FootPlacementIK, &UAnimGraphNode_FootPlacementIK::StaticClass, TEXT("/Script/FootIKEditor"), TEXT("UAnimGraphNode_FootPlacementIK"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAnimGraphNode_FootPlacementIK);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
