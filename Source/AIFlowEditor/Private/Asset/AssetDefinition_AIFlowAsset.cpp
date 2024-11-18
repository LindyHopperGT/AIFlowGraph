// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Asset/AssetDefinition_AIFlowAsset.h"

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

#undef LOCTEXT_NAMESPACE
