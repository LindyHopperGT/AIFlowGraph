// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Asset/AssetDefinition_FlowAsset.h"
#include "AssetDefinition_AIFlowAsset.generated.h"

/**
 * AI Flow Asset - subclass of FlowAsset that adds support for AI Blackboards
 */
UCLASS()
class AIFLOWEDITOR_API UAssetDefinition_AIFlowAsset : public UAssetDefinition_FlowAsset
{
	GENERATED_BODY()

public:
	virtual FText GetAssetDisplayName() const override;
	virtual TSoftClassPtr<UObject> GetAssetClass() const override;
};
