// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Private/FootIKRuntimePrivatePCH.h"
#include "Classes/DummyClassForFootIKModule.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeDummyClassForFootIKModule() {}
// Cross Module References
	FOOTIKRUNTIME_API UClass* Z_Construct_UClass_UDummyClassForFootIKModule_NoRegister();
	FOOTIKRUNTIME_API UClass* Z_Construct_UClass_UDummyClassForFootIKModule();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	UPackage* Z_Construct_UPackage__Script_FootIKRuntime();
// End Cross Module References
	void UDummyClassForFootIKModule::StaticRegisterNativesUDummyClassForFootIKModule()
	{
	}
	UClass* Z_Construct_UClass_UDummyClassForFootIKModule_NoRegister()
	{
		return UDummyClassForFootIKModule::StaticClass();
	}
	UClass* Z_Construct_UClass_UDummyClassForFootIKModule()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_UObject,
				(UObject* (*)())Z_Construct_UPackage__Script_FootIKRuntime,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "IncludePath", "DummyClassForFootIKModule.h" },
				{ "ModuleRelativePath", "Classes/DummyClassForFootIKModule.h" },
				{ "ToolTip", "@TODO: This class is a junk class, temporarily required due to a bug in the UnrealHeaderTool" },
			};
#endif
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<UDummyClassForFootIKModule>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&UDummyClassForFootIKModule::StaticClass,
				DependentSingletons, ARRAY_COUNT(DependentSingletons),
				0x00000080u,
				nullptr, 0,
				nullptr, 0,
				nullptr,
				&StaticCppClassTypeInfo,
				nullptr, 0,
				METADATA_PARAMS(Class_MetaDataParams, ARRAY_COUNT(Class_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUClass(OuterClass, ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UDummyClassForFootIKModule, 1359750083);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UDummyClassForFootIKModule(Z_Construct_UClass_UDummyClassForFootIKModule, &UDummyClassForFootIKModule::StaticClass, TEXT("/Script/FootIKRuntime"), TEXT("UDummyClassForFootIKModule"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UDummyClassForFootIKModule);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
