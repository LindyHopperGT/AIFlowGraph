// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "UObject/Interface.h"
#include "BehaviorTree/BlackboardAssetProvider.h"

#include "FlowBlackboardAssetProvider.generated.h"

class UBlackboardData;
class IPropertyHandle;

// Extension of the IBlackboardAssetProvider interface for AIFlow blackboard asset providers.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UFlowBlackboardAssetProvider : public UBlackboardAssetProvider
{
	GENERATED_BODY()
};

class IFlowBlackboardAssetProvider : public IBlackboardAssetProvider
{
	GENERATED_BODY()

public:

#if WITH_EDITOR
	// Alternate GetBlackboardAsset for editor-use that will allow the supplier to determine which UBlackboardData to return, 
	// based on the PropertyHandle being serviced.
	AIFLOW_API virtual UBlackboardData* GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const { return GetBlackboardAsset(); }
#endif
};


