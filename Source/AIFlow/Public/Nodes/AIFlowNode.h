// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Interfaces/FlowBlackboardAssetProvider.h"
#include "Nodes/FlowNode.h"

#include "Interfaces/FlowBlackboardInterface.h"

#include "AIFlowNode.generated.h"

/**
 * A base flow node class for Flow Nodes that access Blackboards
 */
UCLASS(Abstract, Blueprintable, BlueprintType, DisplayName = "AI Flow Node")
class AIFLOW_API UAIFlowNode
	: public UFlowNode
	, public IFlowBlackboardAssetProvider
	, public IFlowBlackboardInterface
{
	GENERATED_BODY()

public:

	// IFlowBlackboardInterface
	virtual UBlackboardComponent* GetBlackboardComponent() const override;
	// --

	// IBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAsset() const override;
	// --

	// UFlowNode
	virtual int32 GetRandomSeed() const override;
	// --

#if WITH_EDITOR
	// IFlowBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const override;
	// --
#endif // WITH_EDITOR
};
