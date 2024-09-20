// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AddOns/FlowNodeAddOn_ConfigureSpawnedActorBlackboard.h"
#include "BehaviorTree/BlackboardComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNodeAddOn_ConfigureSpawnedActorBlackboard)

UFlowNodeAddOn_ConfigureSpawnedActorBlackboard::UFlowNodeAddOn_ConfigureSpawnedActorBlackboard()
	: Super()
{
#if WITH_EDITOR
	NodeStyle = EFlowNodeStyle::Default;
	Category = TEXT("Per-Spawned Actor");
#endif
}

void UFlowNodeAddOn_ConfigureSpawnedActorBlackboard::FinishedSpawningActor_Implementation(AActor* SpawnedActor, UFlowNodeBase* SpawningNodeOrAddOn)
{
	UBlackboardComponent* BlackboardComponent = TryEnsureBlackboardComponentToApplyTo(SpawnedActor, SpawningNodeOrAddOn);

	if (IsValid(BlackboardComponent))
	{
		ActorBlackboardHelper.ApplyBlackboardOptionsToBlackboardComponent(
			*BlackboardComponent,
			PerActorOptionsAssignmentMethod,
			EntriesForEveryActor,
			PerActorOptions);
	}
}

UBlackboardComponent* UFlowNodeAddOn_ConfigureSpawnedActorBlackboard::TryEnsureBlackboardComponentToApplyTo(AActor* SpawnedActor, UFlowNodeBase* SpawningNodeOrAddOn)
{
	if (!IsValid(SpawnedActor))
	{
		return nullptr;
	}

	UBlackboardComponent* BlackboardComponent = 
		FAIFlowActorBlackboardHelper::FindOrAddBlackboardComponentOnActor(
			*SpawnedActor,
			ExpectedBlackboardData,
			SearchRule,
			InjectRule);

	return BlackboardComponent;
}

void UFlowNodeAddOn_ConfigureSpawnedActorBlackboard::UpdateNodeConfigText_Implementation()
{
#if WITH_EDITOR
	FTextBuilder TextBuilder;

	FAIFlowActorBlackboardHelper::AppendBlackboardOptions(EntriesForEveryActor, PerActorOptions, TextBuilder);

	SetNodeConfigText(TextBuilder.ToText());
#endif // WITH_EDITOR
}

UBlackboardData* UFlowNodeAddOn_ConfigureSpawnedActorBlackboard::GetBlackboardAsset() const
{
	// For the purposes of this function, we will substitute the expected blackboard rather than our own.
	// (so that the blackboard entry keys are sourced from the ExpectedBlackboardData)
	if (ExpectedBlackboardData)
	{
		return ExpectedBlackboardData;
	}

	return Super::GetBlackboardAsset();
}

#if WITH_EDITOR
UBlackboardData* UFlowNodeAddOn_ConfigureSpawnedActorBlackboard::GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const
{
	// For the purposes of this function, we will substitute the expected blackboard rather than our own.
	// (so that the blackboard entry keys are sourced from the ExpectedBlackboardData)
	if (ExpectedBlackboardData)
	{
		return ExpectedBlackboardData;
	}

	return Super::GetBlackboardAssetForPropertyHandle(PropertyHandle);
}
#endif // WITH_EDITOR
