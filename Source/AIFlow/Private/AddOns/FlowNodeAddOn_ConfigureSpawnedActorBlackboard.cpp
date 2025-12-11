// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "AddOns/FlowNodeAddOn_ConfigureSpawnedActorBlackboard.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blackboard/FlowBlackboardEntryValue.h"
#include "Types/FlowInjectComponentsManager.h"
#include "AIFlowAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNodeAddOn_ConfigureSpawnedActorBlackboard)

UFlowNodeAddOn_ConfigureSpawnedActorBlackboard::UFlowNodeAddOn_ConfigureSpawnedActorBlackboard()
	: Super()
{
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
			&PerActorOptions);
	}

	Super::FinishedSpawningActor_Implementation(SpawnedActor, SpawningNodeOrAddOn);
}

UBlackboardComponent* UFlowNodeAddOn_ConfigureSpawnedActorBlackboard::TryEnsureBlackboardComponentToApplyTo(AActor* SpawnedActor, UFlowNodeBase* SpawningNodeOrAddOn)
{
	if (!IsValid(SpawnedActor))
	{
		return nullptr;
	}

	const bool bMayInjectComponent = EActorBlackboardInjectRule_Classifiers::NeedsInjectComponentsManager(InjectRule);
	if (bMayInjectComponent)
	{
		EnsureInjectComponentsManager();
	}

	// Get the BlackboardComponentClass to use from the FlowAsset
	TSubclassOf<UBlackboardComponent> BlackboardComponentClass = UBlackboardComponent::StaticClass();
	UAIFlowAsset* AIFlowAsset = Cast<UAIFlowAsset>(GetFlowAsset());
	if (IsValid(AIFlowAsset))
	{
		BlackboardComponentClass = AIFlowAsset->GetBlackboardComponentClass();
	}

	// Find or add the blackboard component
	UBlackboardComponent* BlackboardComponent = 
		FAIFlowActorBlackboardHelper::FindOrAddBlackboardComponentOnActor(
			*SpawnedActor,
			InjectComponentsManager,
			BlackboardComponentClass,
			ExpectedBlackboardData,
			SearchRule,
			InjectRule);

	// Start monitoring the actor, if we (potentially) injected a blackboard component
	if (bMayInjectComponent)
	{
		// Inform subclasses that we are monitoring this Actor
		OnStartMonitoringActor(*SpawnedActor);
	}

	return BlackboardComponent;
}

void UFlowNodeAddOn_ConfigureSpawnedActorBlackboard::UpdateNodeConfigText_Implementation()
{
#if WITH_EDITOR
	FTextBuilder TextBuilder;

	for (const UFlowBlackboardEntryValue* Entry : EntriesForEveryActor.Entries)
	{
		if (IsValid(Entry))
		{
			TextBuilder.AppendLine(Entry->BuildNodeConfigText());
		}
	}

	FAIFlowActorBlackboardHelper::AppendBlackboardOptions(PerActorOptions, TextBuilder);

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

