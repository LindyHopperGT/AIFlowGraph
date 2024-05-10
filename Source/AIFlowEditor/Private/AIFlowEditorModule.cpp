// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AIFlowEditorModule.h"

#include "Asset/AssetTypeActions_AIFlowAsset.h"
#include "DetailCustomizations/FlowBlackboardEntryCustomization.h"
#include "DetailCustomizations/ConfigurableEnumPropertyCustomization.h"
#include "Graph/FlowGraphSettings.h"

#include "AssetToolsModule.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "AIFlowEditorModule"

EAssetTypeCategories::Type FAIFlowEditorModule::FlowAssetCategory = static_cast<EAssetTypeCategories::Type>(0);

void FAIFlowEditorModule::StartupModule()
{
	RegisterAssets();

	RegisterDetailCustomizations();
}

void FAIFlowEditorModule::ShutdownModule()
{
	UnregisterDetailCustomizations();

	UnregisterAssets();
}

void FAIFlowEditorModule::RegisterAssets()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	// try to merge asset category with a built-in one
	{
		const FText AssetCategoryText = UFlowGraphSettings::Get()->FlowAssetCategoryName;

		// Find matching built-in category
		if (!AssetCategoryText.IsEmpty())
		{
			TArray<FAdvancedAssetCategory> AllCategories;
			AssetTools.GetAllAdvancedAssetCategories(AllCategories);
			for (const FAdvancedAssetCategory& ExistingCategory : AllCategories)
			{
				if (ExistingCategory.CategoryName.EqualTo(AssetCategoryText))
				{
					FlowAssetCategory = ExistingCategory.CategoryType;
					break;
				}
			}
		}

		if (FlowAssetCategory == EAssetTypeCategories::None)
		{
			FlowAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Flow")), AssetCategoryText);
		}
	}

	const TSharedRef<IAssetTypeActions> AIFlowAssetActions = MakeShareable(new FAssetTypeActions_AIFlowAsset());
	RegisteredAssetActions.Add(AIFlowAssetActions);
	AssetTools.RegisterAssetTypeActions(AIFlowAssetActions);
}

void FAIFlowEditorModule::UnregisterAssets()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (const TSharedRef<IAssetTypeActions>& TypeAction : RegisteredAssetActions)
		{
			AssetTools.UnregisterAssetTypeActions(TypeAction);
		}
	}

	RegisteredAssetActions.Empty();
}

void FAIFlowEditorModule::RegisterCustomClassLayout(const TSubclassOf<UObject> Class, const FOnGetDetailCustomizationInstance DetailLayout)
{
	if (Class)
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomClassLayout(Class->GetFName(), DetailLayout);

		CustomClassLayouts.Add(Class->GetFName());
	}
}

void FAIFlowEditorModule::RegisterCustomStructLayout(const UScriptStruct& Struct, const FOnGetPropertyTypeCustomizationInstance DetailLayout)
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomPropertyTypeLayout(Struct.GetFName(), DetailLayout);

		CustomStructLayouts.Add(Struct.GetFName());
	}
}

void FAIFlowEditorModule::RegisterDetailCustomizations()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

		RegisterCustomStructLayout(*FConfigurableEnumProperty::StaticStruct(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FConfigurableEnumPropertyCustomization::MakeInstance));
		RegisterCustomStructLayout(*FFlowBlackboardEntry::StaticStruct(), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FFlowBlackboardEntryCustomization::MakeInstance));

		PropertyModule.NotifyCustomizationModuleChanged();
	}
}

void FAIFlowEditorModule::UnregisterDetailCustomizations()
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

		for (auto It = CustomClassLayouts.CreateConstIterator(); It; ++It)
		{
			if (It->IsValid())
			{
				PropertyModule.UnregisterCustomClassLayout(*It);
			}
		}

		for (auto It = CustomStructLayouts.CreateConstIterator(); It; ++It)
		{
			if (It->IsValid())
			{
				PropertyModule.UnregisterCustomPropertyTypeLayout(*It);
			}
		}

		PropertyModule.NotifyCustomizationModuleChanged();
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAIFlowEditorModule, AIFlowEditor)
