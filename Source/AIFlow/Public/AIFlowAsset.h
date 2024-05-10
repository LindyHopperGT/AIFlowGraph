// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "FlowAsset.h"
#include "Interfaces/FlowBlackboardInterface.h"

#include "BehaviorTree/BlackboardAssetProvider.h"

#include "AIFlowAsset.generated.h"

// Forward Declarations
class UBlackboardData;
class UBlackboardComponent;

/**
 * Flow Asset subclass to add AI utility (specifically blackboard) capabilities
 */
UCLASS(BlueprintType, DisplayName = "AI Flow Asset")
class AIFLOW_API UAIFlowAsset
	: public UFlowAsset
	, public IBlackboardAssetProvider
	, public IFlowBlackboardInterface
{
	GENERATED_UCLASS_BODY()

public:

	//Begin UFlowAsset
	virtual void InitializeInstance(const TWeakObjectPtr<UObject> InOwner, UFlowAsset* InTemplateAsset) override;
	//End UFlowAsset

	//~Begin IFlowBlackboardInterface
	virtual UBlackboardComponent* GetBlackboardComponent() const override;
	//~End IFlowBlackboardInterface

	//~Begin IBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAsset() const override;
	//~End IBlackboardAssetProvider

protected:

	virtual void EnsureBlackboardComponent();
	virtual void SetKeySelfOnBlackboardComponent(UBlackboardComponent* BlackboardComp) const;

protected:

	// Blackboard asset for this FlowAsset
	UPROPERTY(EditAnywhere, Category = "AI Flow Asset")
	TObjectPtr<UBlackboardData> BlackboardAsset = nullptr;

	// Cached blackboard component (on the owning actor)
	UPROPERTY(Transient)
	TWeakObjectPtr<UBlackboardComponent> BlackboardComponent = nullptr;
};
