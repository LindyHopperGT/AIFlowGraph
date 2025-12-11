// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/FlowNode_EnsureActorHasBlackboard.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Types/FlowInjectComponentsManager.h"
#include "AIFlowAsset.h"
#include "AIFlowTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNode_EnsureActorHasBlackboard)

const FName UFlowNode_EnsureActorHasBlackboard::OUTPIN_Success("Success");
const FName UFlowNode_EnsureActorHasBlackboard::OUTPIN_Failed("Failed");

UFlowNode_EnsureActorHasBlackboard::UFlowNode_EnsureActorHasBlackboard()
	: Super()
{
#if WITH_EDITOR
	NodeDisplayStyle = FlowNodeStyle::Blackboard;
	Category = TEXT("Blackboard");
#endif

	OutputPins.Reset();
	OutputPins.Add(FFlowPin(OUTPIN_Success));
	OutputPins.Add(FFlowPin(OUTPIN_Failed));
}

void UFlowNode_EnsureActorHasBlackboard::ExecuteInput(const FName& PinName)
{
	// Use the SpecificActor if provided, otherwise use the Flow Owner Actor
	TObjectPtr<UObject> ResolvedObject = nullptr;
	const EFlowDataPinResolveResult ResolveResult = TryResolveDataPinValue<FFlowPinType_Object>(GET_MEMBER_NAME_CHECKED(ThisClass, SpecificActor), ResolvedObject);
	AActor* ResolvedActor = Cast<AActor>(ResolvedObject);

	if (!IsValid(ResolvedActor))
	{
		ResolvedActor = TryGetRootFlowActorOwner();

		if (!IsValid(ResolvedActor))
		{
			LogError(TEXT("Could not inject a blackboard onto a null actor"));

			return;
		}
	}

	UBlackboardComponent* BlackboardComponent = EnsureActorHasBlackboard(*ResolvedActor);

	constexpr bool bFinish = true;
	if (IsValid(BlackboardComponent))
	{
		TriggerOutput(OUTPIN_Success);
	}
	else
	{
		TriggerOutput(OUTPIN_Failed);
	}
}

UBlackboardComponent* UFlowNode_EnsureActorHasBlackboard::EnsureActorHasBlackboard(AActor& ResolvedActor)
{
	const bool bMayInjectComponent = EActorBlackboardInjectRule_Classifiers::NeedsInjectComponentsManager(InjectRule);
	if (bMayInjectComponent)
	{
		EnsureInjectComponentsManager();
	}

	TSubclassOf<UBlackboardComponent> BlackboardComponentClass = SpecificBlackboardComponentClass;
	if (!IsValid(BlackboardComponentClass))
	{
		// If no specific one provided, the BlackboardComponentClass to use from the FlowAsset
		UAIFlowAsset* AIFlowAsset = Cast<UAIFlowAsset>(GetFlowAsset());
		if (IsValid(AIFlowAsset))
		{
			BlackboardComponentClass = AIFlowAsset->GetBlackboardComponentClass();
		}
		else
		{
			BlackboardComponentClass = UBlackboardComponent::StaticClass();
		}
	}

	// Find or inject the blackboard
	return FAIFlowActorBlackboardHelper::FindOrAddBlackboardComponentOnActor(
		ResolvedActor,
		InjectComponentsManager,
		BlackboardComponentClass,
		SpecificBlackboardAsset,
		BlackboardSearchRule,
		InjectRule);
}

void UFlowNode_EnsureActorHasBlackboard::DeinitializeInstance()
{
	CleanupInjectComponentsManager();

	Super::DeinitializeInstance();
}

void UFlowNode_EnsureActorHasBlackboard::EnsureInjectComponentsManager()
{
	if (IsValid(InjectComponentsManager))
	{
		return;
	}

	InjectComponentsManager = NewObject<UFlowInjectComponentsManager>(this);

	InjectComponentsManager->InitializeRuntime();
	InjectComponentsManager->BeforeActorRemovedDelegate.AddDynamic(this, &ThisClass::OnBeforeActorRemoved);
}

void UFlowNode_EnsureActorHasBlackboard::CleanupInjectComponentsManager()
{
	if (IsValid(InjectComponentsManager))
	{
		InjectComponentsManager->ShutdownRuntime();
		InjectComponentsManager->BeforeActorRemovedDelegate.RemoveDynamic(this, &ThisClass::OnBeforeActorRemoved);
	}

	InjectComponentsManager = nullptr;
}

void UFlowNode_EnsureActorHasBlackboard::OnBeforeActorRemoved(AActor* RemovedActor)
{
	if (IsValid(RemovedActor))
	{
		OnStopMonitoringActor(*RemovedActor);
	}
}
