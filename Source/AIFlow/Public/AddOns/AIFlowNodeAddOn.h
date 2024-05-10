// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "AddOns/FlowNodeAddOn.h"
#include "BehaviorTree/BlackboardAssetProvider.h"
#include "Interfaces/FlowBlackboardInterface.h"

#include "AIFlowNodeAddOn.generated.h"

UCLASS(Abstract, EditInlineNew, Blueprintable)
class AIFLOW_API UAIFlowNodeAddOn
	: public UFlowNodeAddOn
	, public IBlackboardAssetProvider
	, public IFlowBlackboardInterface
{
	GENERATED_BODY()

public:

	//~Begin IFlowBlackboardInterface
	virtual UBlackboardComponent* GetBlackboardComponent() const override;
	//~End IFlowBlackboardInterface

	//~Begin IBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAsset() const override;
	//~End IBlackboardAssetProvider
};
