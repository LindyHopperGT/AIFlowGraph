// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Asset/AssetDefinition_AIFlowAsset.h"
#include "Asset/AIFlowAssetEditor.h"
#include "AIFlowAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AssetDefinition_AIFlowAsset)

#define LOCTEXT_NAMESPACE "AssetDefinition_AIFlowAsset"

FText UAssetDefinition_AIFlowAsset::GetAssetDisplayName() const
{
	return LOCTEXT("AssetTypeActions_AIFlowAsset", "AI Flow Asset");
}

TSoftClassPtr<UObject> UAssetDefinition_AIFlowAsset::GetAssetClass() const
{
	return UAIFlowAsset::StaticClass();
}

EAssetCommandResult UAssetDefinition_AIFlowAsset::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	for (UAIFlowAsset* AIFlowAsset : OpenArgs.LoadObjects<UAIFlowAsset>())
	{
		TSharedRef<FAIFlowAssetEditor> NewEditor(new FAIFlowAssetEditor());
		NewEditor->InitFlowAssetEditor(OpenArgs.GetToolkitMode(), OpenArgs.ToolkitHost, AIFlowAsset);
	}

	return EAssetCommandResult::Handled;
}

#undef LOCTEXT_NAMESPACE