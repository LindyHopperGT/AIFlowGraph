// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/FlowNode_SetBlackboardValues.h"
#include "Blackboard/FlowBlackboardEntryValue.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNode_SetBlackboardValues)

UFlowNode_SetBlackboardValues::UFlowNode_SetBlackboardValues()
	: Super()
{
#if WITH_EDITOR
	NodeDisplayStyle = FlowNodeStyle::Blackboard;
	Category = TEXT("Blackboard");
#endif
}

void UFlowNode_SetBlackboardValues::ExecuteInput(const FName& PinName)
{
	Super::ExecuteInput(PinName);

	// Refresh EntriesForEveryActor from data pin input values

	for (UFlowBlackboardEntryValue* BlackboardEntry : EntriesForEveryActor.Entries)
	{
		if (!IsValid(BlackboardEntry))
		{
			LogError(TEXT("Found null BlackboardEntry.  This is unexpected."), EFlowOnScreenMessageType::Temporary);

			continue;
		}

		(void) BlackboardEntry->TrySetValueFromInputDataPin(BlackboardEntry->Key.KeyName, *this);
	}

	// Apply the values to the blackboards
	const TArray<UBlackboardComponent*> BlackboardComponents = GetBlackboardComponentsToApplyTo();
	if (!BlackboardComponents.IsEmpty())
	{
		for (UBlackboardComponent* BlackboardComponent : BlackboardComponents)
		{
			if (IsValid(BlackboardComponent))
			{
				ActorBlackboardHelper.ApplyBlackboardOptionsToBlackboardComponent(
					*BlackboardComponent,
					PerActorOptionsAssignmentMethod,
					EntriesForEveryActor,
					PerActorOptions);
			}
		}
	}
	else
	{
		LogError(TEXT("Cannot SetBlackboardValues without a Blackboard"));
	}

	constexpr bool bIsFinished = true;
	TriggerFirstOutput(bIsFinished);
}

TArray<UBlackboardComponent*> UFlowNode_SetBlackboardValues::GetBlackboardComponentsToApplyTo() const
{
	TArray<UBlackboardComponent*> BlackboardComponents;

	const FAIFlowCachedBlackboardReference CachedBlackboard(*this, SpecificBlackboardAsset, SpecificBlackboardSearchRule);

	if (CachedBlackboard.IsValid())
	{
		BlackboardComponents.Reserve(1);
		BlackboardComponents.Add(CachedBlackboard.BlackboardComponent);
	}

	return BlackboardComponents;
}

bool UFlowNode_SetBlackboardValues::TryFindPropertyByRemappedPinName(const FName& RemappedPinName, const FProperty*& OutFoundProperty, TInstancedStruct<FFlowDataPinProperty>& OutFoundInstancedStruct, EFlowDataPinResolveResult& InOutResult) const
{
	// The SetBlackboardValues node stores its properties in instanced structs in an array, so look there first

	for (const UFlowBlackboardEntryValue* BlackboardEntry : EntriesForEveryActor.Entries)
	{
		if (!IsValid(BlackboardEntry))
		{
			LogError(TEXT("Found null BlackboardEntry.  This is unexpected."), EFlowOnScreenMessageType::Temporary);

			continue;
		}

		if (BlackboardEntry->Key.KeyName == RemappedPinName)
		{
			const IFlowDataPinPropertyProviderInterface* FlowDataPinProviderInterface = CastChecked<IFlowDataPinPropertyProviderInterface>(BlackboardEntry);

			if (FlowDataPinProviderInterface->TryProvideFlowDataPinProperty(bAreEntriesForEveryActorInputPins, OutFoundInstancedStruct))
			{
				return true;
			}
			else
			{
				// Found the property name, but it could not provide the data for the data pin property
				break;
			}
		}
	}

	return Super::TryFindPropertyByPinName(RemappedPinName, OutFoundProperty, OutFoundInstancedStruct, InOutResult);
}

#if WITH_EDITOR
void UFlowNode_SetBlackboardValues::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChainEvent)
{
	Super::PostEditChangeChainProperty(PropertyChainEvent);

	if (PropertyChainEvent.PropertyChain.Num() == 0)
	{
		return;
	}

	auto& Property = PropertyChainEvent.PropertyChain.GetActiveMemberNode()->GetValue();

	constexpr EPropertyChangeType::Type RelevantChangeTypesForReconstructionMask =
		EPropertyChangeType::Unspecified |
		EPropertyChangeType::ArrayAdd |
		EPropertyChangeType::ArrayRemove |
		EPropertyChangeType::ArrayClear |
		EPropertyChangeType::ValueSet |
		EPropertyChangeType::Redirected |
		EPropertyChangeType::ArrayMove;

	const uint32 PropertyChangedTypeFlags = (PropertyChainEvent.ChangeType & RelevantChangeTypesForReconstructionMask);
	const bool bIsRelevantChangeTypeForReconstruction = PropertyChangedTypeFlags != 0;
	const bool bChangedInputProperties = Property->GetFName() == GET_MEMBER_NAME_CHECKED(UFlowNode_SetBlackboardValues, EntriesForEveryActor);
	if (bIsRelevantChangeTypeForReconstruction && bChangedInputProperties)
	{
		OnReconstructionRequested.ExecuteIfBound();
	}
}

UBlackboardData* UFlowNode_SetBlackboardValues::GetBlackboardAssetForEditor() const
{
	if (IsValid(SpecificBlackboardAsset))
	{
		return SpecificBlackboardAsset;
	}

	return GetBlackboardAsset();
}

UBlackboardData* UFlowNode_SetBlackboardValues::GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const
{
	if (UBlackboardData* BlackboardAssetForEditor = GetBlackboardAssetForEditor())
	{
		return BlackboardAssetForEditor;
	}
	
	return Super::GetBlackboardAssetForPropertyHandle(PropertyHandle);
}

void UFlowNode_SetBlackboardValues::AutoGenerateDataPins(TMap<FName, FName>& InOutPinNameToBoundPropertyNameMap, TArray<FFlowPin>& InOutInputDataPins, TArray<FFlowPin>& InOutOutputDataPins) const
{
	for (const UFlowBlackboardEntryValue* BlackboardEntry : EntriesForEveryActor.Entries)
	{
		if (!IsValid(BlackboardEntry))
		{
			LogError(TEXT("Found null BlackboardEntry.  This is unexpected."), EFlowOnScreenMessageType::Temporary);

			continue;
		}

		const FName& PinName = BlackboardEntry->Key.KeyName;
		if (PinName.IsNone())
		{
			continue;
		}

		const IFlowDataPinPropertyProviderInterface* FlowDataPinProviderInterface = CastChecked<IFlowDataPinPropertyProviderInterface>(BlackboardEntry);
		TInstancedStruct<FFlowDataPinProperty> InstancedFlowDataPinProperty;

		if (FlowDataPinProviderInterface->TryProvideFlowDataPinProperty(bAreEntriesForEveryActorInputPins, InstancedFlowDataPinProperty))
		{
			InOutPinNameToBoundPropertyNameMap.Add(PinName, PinName);

			FFlowPin NewFlowPin = FFlowDataPinProperty::CreateFlowPin(PinName, InstancedFlowDataPinProperty);

			if (bAreEntriesForEveryActorInputPins)
			{
				InOutInputDataPins.AddUnique(NewFlowPin);
			}
			else
			{
				InOutOutputDataPins.AddUnique(NewFlowPin);
			}
		}
	}
}

void UFlowNode_SetBlackboardValues::AppendEntriesForEveryActor(FTextBuilder& InOutTextBuilder) const
{
	for (const UFlowBlackboardEntryValue* Entry : EntriesForEveryActor.Entries)
	{
		if (IsValid(Entry) && !IsInputConnected(Entry->Key.KeyName))
		{
			InOutTextBuilder.AppendLine(Entry->BuildNodeConfigText());
		}
	}
}

#endif // WITH_EDITOR

void UFlowNode_SetBlackboardValues::UpdateNodeConfigText_Implementation()
{
#if WITH_EDITOR
	FTextBuilder TextBuilder;

	AppendEntriesForEveryActor(TextBuilder);

	FAIFlowActorBlackboardHelper::AppendBlackboardOptions(PerActorOptions, TextBuilder);

	SetNodeConfigText(TextBuilder.ToText());
#endif // WITH_EDITOR
}