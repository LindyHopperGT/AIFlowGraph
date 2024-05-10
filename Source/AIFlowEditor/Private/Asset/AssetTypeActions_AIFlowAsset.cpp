// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AssetTypeActions_AIFlowAsset.h"
#include "AIFlowAsset.h"
#include "AIFlowEditorModule.h"
#include "FlowEditorModule.h"
#include "Graph/FlowGraphSettings.h"

#include "Toolkits/IToolkit.h"

// FAssetTypeActions_AIFlowAsset Implementation

#define LOCTEXT_NAMESPACE "AssetTypeActions_AIFlowAsset"

FText FAssetTypeActions_AIFlowAsset::GetName() const
{
	return LOCTEXT("AssetTypeActions_AIFlowAsset", "AI Flow Asset");
}

uint32 FAssetTypeActions_AIFlowAsset::GetCategories()
{
	return UFlowGraphSettings::Get()->bExposeFlowAssetCreation ? FAIFlowEditorModule::FlowAssetCategory : 0;
}

UClass* FAssetTypeActions_AIFlowAsset::GetSupportedClass() const
{
	return UAIFlowAsset::StaticClass();
}

void FAssetTypeActions_AIFlowAsset::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	const FFlowEditorModule* FlowModule = &FModuleManager::LoadModuleChecked<FFlowEditorModule>("FlowEditor");

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (UAIFlowAsset* FlowAsset = Cast<UAIFlowAsset>(*ObjIt))
		{
			FlowModule->CreateFlowAssetEditor(Mode, EditWithinLevelEditor, FlowAsset);
		}
	}
}

#undef LOCTEXT_NAMESPACE