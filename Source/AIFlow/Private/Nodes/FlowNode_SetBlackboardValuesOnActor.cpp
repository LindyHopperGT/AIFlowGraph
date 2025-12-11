// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/FlowNode_SetBlackboardValuesOnActor.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#if WITH_EDITOR
#include "PropertyHandle.h"
#endif // WITH_EDITOR

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNode_SetBlackboardValuesOnActor)

FName UFlowNode_SetBlackboardValuesOnActor::INPIN_SpecificActor;

UFlowNode_SetBlackboardValuesOnActor::UFlowNode_SetBlackboardValuesOnActor()
	: Super()
{
	INPIN_SpecificActor = GET_MEMBER_NAME_CHECKED(ThisClass, SpecificActor);
#if WITH_EDITOR
	bNodeDeprecated = true;
#endif
}

void UFlowNode_SetBlackboardValuesOnActor::UpdateNodeConfigText_Implementation()
{
#if WITH_EDITOR

	FTextBuilder TextBuilder;

	if (!BlackboardActorKey.KeyName.IsNone())
	{
		TextBuilder.AppendLine(FString::Printf(TEXT("Actor Key: %s"), *BlackboardActorKey.KeyName.ToString()));
	}

	AppendEntriesForEveryActor(TextBuilder);

	SetNodeConfigText(TextBuilder.ToText());
#endif // WITH_EDITOR
}

TArray<AActor*> UFlowNode_SetBlackboardValuesOnActor::TryResolveActorsForBlackboard() const
{
	TArray<AActor*> ResolvedActors;

	const bool bAddedSpecificActor = TryAddSpecificActor(ResolvedActors);
	const bool bAddedBlackboardKeyedActor = TryAddBlackboardKeyedActor(ResolvedActors);

	if (bAddedSpecificActor || bAddedBlackboardKeyedActor)
	{
		return ResolvedActors;
	}

	// Default to the flow actor owner, if not overridden 
	return Super::TryResolveActorsForBlackboard();
}

bool UFlowNode_SetBlackboardValuesOnActor::TryAddSpecificActor(TArray<AActor*>& InOutResolvedActors) const
{
	// Resolve the Specific Actor(s) Pin first and add it to the ResolvedActors array
	TArray<TObjectPtr<UObject>> PinSuppliedObjects;
	const EFlowDataPinResolveResult ResolveResult = TryResolveDataPinValues<FFlowPinType_Object>(INPIN_SpecificActor, PinSuppliedObjects);
	if (FlowPinType::IsSuccess(ResolveResult))
	{
		for (TObjectPtr<UObject> PinSuppliedObject : PinSuppliedObjects)
		{
			AActor* PinSuppliedActor = Cast<AActor>(PinSuppliedObject);

			if (IsValid(PinSuppliedActor))
			{
				InOutResolvedActors.Add(PinSuppliedActor);
			}
		}

		return true;
	}

	return false;
}

bool UFlowNode_SetBlackboardValuesOnActor::TryAddBlackboardKeyedActor(TArray<AActor*>& InOutResolvedActors) const
{
	// Add the actor specified by the given blackboard key
	if (BlackboardActorKey.KeyName == NAME_None)
	{
		return false;
	}

	UBlackboardComponent* FlowBlackboardComponent = GetBlackboardComponent();
	if (!FlowBlackboardComponent)
	{
		return false;
	}

	AActor* FoundActor = Cast<AActor>(FlowBlackboardComponent->GetValueAsObject(BlackboardActorKey.KeyName));
	if (!IsValid(FoundActor))
	{
		// Counts as true, because we intended to add an actor from the blackboard,
		// it was just a null actor
		return true;
	}

	InOutResolvedActors.Add(FoundActor);

	return true;
}

#if WITH_EDITOR
void UFlowNode_SetBlackboardValuesOnActor::PostInitProperties()
{
	Super::PostInitProperties();

	if (HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject))
	{
		return;
	}

	if (BlackboardActorKey.AllowedTypes.IsEmpty())
	{
		UBlackboardKeyType_Object* ActorObjectType = NewObject<UBlackboardKeyType_Object>(this);

		// Limit the BlackboardActorKey to AActor
		ActorObjectType->BaseClass = AActor::StaticClass();

		BlackboardActorKey.AllowedTypes.Add(ActorObjectType);
	}
}

UBlackboardData* UFlowNode_SetBlackboardValuesOnActor::GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const
{
	if (SpecificBlackboardAsset && PropertyHandle)
	{
		// Use the ExpectedBlackboardData for all properties except the BlackboardActorKey,
		// which we should use the owning Flow Graph's blackboard instead.

		const FName PropertyName = PropertyHandle->GetProperty()->GetFName();
		if (PropertyName != GET_MEMBER_NAME_CHECKED(UFlowNode_SetBlackboardValuesOnActor, BlackboardActorKey))
		{
			return SpecificBlackboardAsset;
		}
	}

	return GetBlackboardAsset();
}
#endif // WITH_EDITOR