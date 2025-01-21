// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "AIFlowActorBlackboardHelper.h"
#include "Interfaces/FlowPerSpawnedActorInterface.h"
#include "FlowNodeAddOn_InjectComponentsBase.h"

#include "FlowNodeAddOn_ConfigureSpawnedActorBlackboard.generated.h"

class UFlowInjectComponentsManager;

/**
 * Set blackboard values to the values defined in the Entries array
 */
UCLASS(DisplayName = "Configure Spawned Actor Blackboard")
class AIFLOW_API UFlowNodeAddOn_ConfigureSpawnedActorBlackboard : public UFlowNodeAddOn_InjectComponentsBase
{
	GENERATED_BODY()

public:

	UFlowNodeAddOn_ConfigureSpawnedActorBlackboard();

	// IFlowPerSpawnedActorInterface
	virtual void FinishedSpawningActor_Implementation(AActor* SpawnedActor, UFlowNodeBase* SpawningNodeOrAddOn) override;
	// --

	// UFlowNodeBase
	virtual void UpdateNodeConfigText_Implementation() override;
	// --

	// IBlackboardAssetProvider
	virtual UBlackboardData* GetBlackboardAsset() const override;
	// --

	// IFlowBlackboardAssetProvider
#if WITH_EDITOR
	virtual UBlackboardData* GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const override;
#endif // WITH_EDITOR
	// --

protected:

	virtual UBlackboardComponent* TryEnsureBlackboardComponentToApplyTo(AActor* SpawnedActor, UFlowNodeBase* SpawningNodeOrAddOn);

protected:

	// Specify an explicit blackboard asset to write to
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Expected Blackboard Asset for Actors", meta = (DisplayPriority = 1))
	UBlackboardData* ExpectedBlackboardData = nullptr;

	// Blackboard properties to set on every spawned actor
	UPROPERTY(EditAnywhere, Category = Configuration, meta = (DisplayPriority = 3))
	FAIFlowConfigureBlackboardOption EntriesForEveryActor;

	// Where to search for the desired blackboard: the Actor, their Controller (if Pawn) or both.
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Blackboard Component Search Rule", meta = (DisplayPriority = 2))
	EActorBlackboardSearchRule SearchRule = EActorBlackboardSearchRule::ActorAndController;

	// Injection rule, if desired blackboard is not found
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Blackboard Injection Rule", meta = (DisplayPriority = 2))
	EActorBlackboardInjectRule InjectRule = EActorBlackboardInjectRule::InjectOntoActorIfMissing;

	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Per-Actor Options Assignment Method", meta = (DisplayPriority = 4))
	EPerActorOptionsAssignmentMethod PerActorOptionsAssignmentMethod = EPerActorOptionsAssignmentMethod::InOrderWithWrapping;

	// Configured blackboard entry option sets to apply to actors according to the application method
	UPROPERTY(EditAnywhere, Category = Configuration, DisplayName = "Per-Actor Options", meta = (DisplayPriority = 4))
	TArray<FAIFlowConfigureBlackboardOption> PerActorOptions;

	// Helper struct that shared functionality for manipulating Actor blackboards
	UPROPERTY(EditAnywhere, Category = Configuration, meta = (ShowOnlyInnerProperties, DisplayPriority = 4))
	FAIFlowActorBlackboardHelper ActorBlackboardHelper;
};
