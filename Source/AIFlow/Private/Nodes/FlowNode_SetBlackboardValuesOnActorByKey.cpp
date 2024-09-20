// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/FlowNode_SetBlackboardValuesOnActorByKey.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#if WITH_EDITOR
#include "PropertyHandle.h"
#endif // WITH_EDITOR

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNode_SetBlackboardValuesOnActorByKey)

UFlowNode_SetBlackboardValuesOnActorByKey::UFlowNode_SetBlackboardValuesOnActorByKey()
	: Super()
{
}

void UFlowNode_SetBlackboardValuesOnActorByKey::UpdateNodeConfigText_Implementation()
{
#if WITH_EDITOR

	FTextBuilder TextBuilder;

	if (!BlackboardActorKey.KeyName.IsNone())
	{
		TextBuilder.AppendLine(FString::Printf(TEXT("Actor Key: %s"), *BlackboardActorKey.KeyName.ToString()));
	}

	FAIFlowActorBlackboardHelper::AppendBlackboardOptions(EntriesForEveryActor, PerActorOptions, TextBuilder);

	SetNodeConfigText(TextBuilder.ToText());
#endif // WITH_EDITOR
}

TArray<AActor*> UFlowNode_SetBlackboardValuesOnActorByKey::TryResolveActorsForBlackboard() const
{
	TArray<AActor*> ResolvedActors;
	if (BlackboardActorKey.KeyName == NAME_None)
	{
		return ResolvedActors;
	}

	UBlackboardComponent* FlowBlackboardComponent = GetBlackboardComponent();
	if (!FlowBlackboardComponent)
	{
		return ResolvedActors;
	}

	if (AActor* FoundActor = Cast<AActor>(FlowBlackboardComponent->GetValueAsObject(BlackboardActorKey.KeyName)))
	{
		ResolvedActors.Reserve(1);
		ResolvedActors.Add(FoundActor);
	}
	
	return ResolvedActors;
}

#if WITH_EDITOR
void UFlowNode_SetBlackboardValuesOnActorByKey::PostInitProperties()
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

UBlackboardData* UFlowNode_SetBlackboardValuesOnActorByKey::GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const
{
	if (ExpectedBlackboardData && PropertyHandle)
	{
		// Use the ExpectedBlackboardData for all properties except the BlackboardActorKey,
		// which we should use the owning Flow Graph's blackboard instead.

		const FName PropertyName = PropertyHandle->GetProperty()->GetFName();
		if (PropertyName != GET_MEMBER_NAME_CHECKED(UFlowNode_SetBlackboardValuesOnActorByKey, BlackboardActorKey))
		{
			return ExpectedBlackboardData;
		}
	}

	return GetBlackboardAsset();
}
#endif // WITH_EDITOR