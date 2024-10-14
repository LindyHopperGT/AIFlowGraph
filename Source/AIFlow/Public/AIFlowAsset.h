// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "FlowAsset.h"
#include "Interfaces/FlowBlackboardAssetProvider.h"
#include "Interfaces/FlowBlackboardInterface.h"

#include "AIFlowAsset.generated.h"

// Forward Declarations
class UBlackboardData;
class UBlackboardComponent;
class UFlowInjectComponentsManager;

/**
 * Flow Asset subclass to add AI utility (specifically blackboard) capabilities
 */
UCLASS(BlueprintType, DisplayName = "AI Flow Asset")
class AIFLOW_API UAIFlowAsset
	: public UFlowAsset
	, public IFlowBlackboardAssetProvider
	, public IFlowBlackboardInterface
{
	GENERATED_UCLASS_BODY()

public:

	// UFlowAsset
	virtual void InitializeInstance(const TWeakObjectPtr<UObject> InOwner, UFlowAsset* InTemplateAsset) override;
	virtual void DeinitializeInstance() override;
	// --

	// IFlowBlackboardInterface
	virtual UBlackboardComponent* GetBlackboardComponent() const override;
	// --

	// IBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAsset() const override;
	// --

	static UBlackboardComponent* TryFindBlackboardComponentOnActor(AActor& Actor, UBlackboardData* OptionalBlackboardData);

protected:

	virtual void CreateAndRegisterBlackboardComponent();
	virtual void DestroyAndUnregisterBlackboardComponent();
	virtual void SetKeySelfOnBlackboardComponent(UBlackboardComponent* BlackboardComp) const;

	// Return the BlackboardData to use at runtime
	// (subclasses may want to instance this class For Reasons)
	virtual UBlackboardData* EnsureRuntimeBlackboardData() const { return BlackboardAsset; }

protected:

	// Blackboard asset for this FlowAsset
	UPROPERTY(EditAnywhere, Category = "AI Flow Asset")
	TObjectPtr<UBlackboardData> BlackboardAsset = nullptr;

	// Cached blackboard component (on the owning actor)
	UPROPERTY(Transient)
	TWeakObjectPtr<UBlackboardComponent> BlackboardComponent = nullptr;

	// Manager object to inject and remove blackboard components from the Flow owning Actor
	UPROPERTY(Transient)
	TObjectPtr<UFlowInjectComponentsManager> InjectComponentsManager = nullptr;

	// Subclass-configurable Blackboard component class to use
	UPROPERTY(Transient)
	TSubclassOf<UBlackboardComponent> BlackboardComponentClass;
};
