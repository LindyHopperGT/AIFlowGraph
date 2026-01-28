// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AIFlowEditorModule.h"

#include "DetailCustomizations/FlowBlackboardEntryCustomization.h"
#include "DetailCustomizations/ConfigurableEnumPropertyCustomization.h"
#include "DetailCustomizations/AIFlowBlackboardWatchDataCustomization.h"
#include "Blackboard/AIFlowBlackboardWatchData.h"
#include "AIFlowTags.h"
#include "Graph/FlowGraphSettings.h"

#include "FlowModule.h"
#include "AssetToolsModule.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

#define LOCTEXT_NAMESPACE "AIFlowEditorModule"

EAssetTypeCategories::Type FAIFlowEditorModule::FlowAssetCategory = static_cast<EAssetTypeCategories::Type>(0);

void FAIFlowEditorModule::StartupModule()
{
	TrySetFlowNodeDisplayStyleDefaults();

	RegisterAssets();

	RegisterDetailCustomizations();
}

void FAIFlowEditorModule::ShutdownModule()
{
	UnregisterDetailCustomizations();

	UnregisterAssets();
}

void FAIFlowEditorModule::TrySetFlowNodeDisplayStyleDefaults() const
{
	// Ensure the Flow module is loaded before accessing Flow graph settings.
	FModuleManager::LoadModuleChecked<FFlowModule>("Flow");

	if (UFlowGraphSettings* Settings = UFlowGraphSettings::Get())
	{
		Settings->TryAddDefaultNodeDisplayStyle(
			FFlowNodeDisplayStyleConfig(
				FlowNodeStyle::Blackboard,
				FLinearColor(-0.7f, 0.58f, 1.0f, 1.0f)));
	}
}

void FAIFlowEditorModule::RegisterAssets()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	// Try to merge the AI Flow asset category with an existing Flow asset category if possible.
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

		// Fallback: register our own category if no suitable one exists.
		if (FlowAssetCategory == EAssetTypeCategories::None)
		{
			FlowAssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Flow")), AssetCategoryText);
		}
	}
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
		RegisterCustomClassLayout(UAIFlowBlackboardWatchData::StaticClass(), FOnGetDetailCustomizationInstance::CreateStatic(&FAIFlowBlackboardWatchDataCustomization::MakeInstance));

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