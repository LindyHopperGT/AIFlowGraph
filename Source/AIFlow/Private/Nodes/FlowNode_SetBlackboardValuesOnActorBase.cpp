// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/FlowNode_SetBlackboardValuesOnActorBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FlowAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNode_SetBlackboardValuesOnActorBase)

UFlowNode_SetBlackboardValuesOnActorBase::UFlowNode_SetBlackboardValuesOnActorBase()
	: Super()
{
}

TArray<AActor*> UFlowNode_SetBlackboardValuesOnActorBase::TryResolveActorsForBlackboard() const
{
	// Default to the Flow graph's actor.  
	// (Not very satisfactory, because the UFlowNode_SetBlackboardValues node does this simpler,
	// but we expect subclasses to replace this function).

	TArray<AActor*> ResolvedActors;
	if (AActor* FoundActor = GetFlowAsset()->TryFindActorOwner())
	{
		ResolvedActors.Reserve(1);
		ResolvedActors.Add(FoundActor);
	}

	return ResolvedActors;
}

TArray<UBlackboardComponent*> UFlowNode_SetBlackboardValuesOnActorBase::GetBlackboardComponentsToApplyTo() const
{
	TArray<UBlackboardComponent*> BlackboardComponents = 
		FAIFlowActorBlackboardHelper::FindOrAddBlackboardComponentOnActors(
			TryResolveActorsForBlackboard(),
			ExpectedBlackboardData,
			SearchRule,
			InjectRule);

	return BlackboardComponents;
}