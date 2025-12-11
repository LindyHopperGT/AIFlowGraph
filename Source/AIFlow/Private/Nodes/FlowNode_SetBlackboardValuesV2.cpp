// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/FlowNode_SetBlackboardValuesV2.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowAsset.h"
#include "AIFlowTags.h"
#include "Types/FlowAutoDataPinsWorkingData.h"
#include "Types/FlowDataPinValue.h"
#include "Blackboard/FlowBlackboardEntryValue.h"

#if WITH_EDITOR
#include "PropertyHandle.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNode_SetBlackboardValuesV2)

FName UFlowNode_SetBlackboardValuesV2::INPIN_SpecificActors;

UFlowNode_SetBlackboardValuesV2::UFlowNode_SetBlackboardValuesV2()
{
#if WITH_EDITOR
	NodeDisplayStyle = FlowNodeStyle::Blackboard;
	Category = TEXT("Blackboard");
#endif

	INPIN_SpecificActors = GET_MEMBER_NAME_CHECKED(ThisClass, SpecificActors);
}

void UFlowNode_SetBlackboardValuesV2::ExecuteInput(const FName& PinName)
{
	UAIFlowNode::ExecuteInput(PinName);

	// Refresh EntriesForEveryActor from data pin input values
	for (UFlowBlackboardEntryValue* BlackboardEntry : EntriesForEveryActor.Entries)
	{
		if (!IsValid(BlackboardEntry))
		{
			LogError(TEXT("Found null BlackboardEntry.  This is unexpected."), EFlowOnScreenMessageType::Temporary);
			continue;
		}

		(void)BlackboardEntry->TrySetValueFromInputDataPin(BlackboardEntry->Key.KeyName, *this);
	}

	// Create the InjectComponentsManager sub-object if necessary
	const bool bMayInjectComponent = EActorBlackboardInjectRule_Classifiers::NeedsInjectComponentsManager(InjectRule);
	if (bMayInjectComponent)
	{
		EnsureInjectComponentsManager();
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
					&PerActorOptions);
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

void UFlowNode_SetBlackboardValuesV2::DeinitializeInstance()
{
	CleanupInjectComponentsManager();

	UAIFlowNode::DeinitializeInstance();
}

TArray<AActor*> UFlowNode_SetBlackboardValuesV2::TryResolveActorsForBlackboard() const
{
	TArray<AActor*> ResolvedActors;

	if (TryAddSpecificActors(ResolvedActors))
	{
		return ResolvedActors;
	}

	// Default to the flow actor owner
	AActor* Owner = GetFlowAsset()->TryFindActorOwner();
	if (IsValid(Owner))
	{
		ResolvedActors.Add(Owner);
	}

	return ResolvedActors;
}

bool UFlowNode_SetBlackboardValuesV2::TryAddSpecificActors(TArray<AActor*>& InOutResolvedActors) const
{
	TArray<TObjectPtr<UObject>> PinSuppliedObjects;
	const EFlowDataPinResolveResult ResolveResult = TryResolveDataPinValues<FFlowPinType_Object>(INPIN_SpecificActors, PinSuppliedObjects);

	if (FlowPinType::IsSuccess(ResolveResult))
	{
		bool bAddedActors = false;

		for (TObjectPtr<UObject> PinSuppliedObject : PinSuppliedObjects)
		{
			AActor* PinSuppliedActor = Cast<AActor>(PinSuppliedObject);
			if (IsValid(PinSuppliedActor))
			{
				InOutResolvedActors.Add(PinSuppliedActor);
				bAddedActors = true;
			}
			else
			{
				LogError(TEXT("Specific Actors pin supplied a null Actor, which was unexpected."), EFlowOnScreenMessageType::Temporary);
			}
		}

		return bAddedActors;
	}

	return false;
}

TArray<UBlackboardComponent*> UFlowNode_SetBlackboardValuesV2::GetBlackboardComponentsToApplyTo() const
{
	// TODO (gtaylor) Consider consolidating with UFlowNode_GetBlackboardValues::GetBlackboardComponentToApplyTo()
	UBlackboardData* DesiredBlackboardAsset = SpecificBlackboardAsset;

	TSubclassOf<UBlackboardComponent> BlackboardComponentClass = SpecificBlackboardComponentClass;
	if (UAIFlowAsset* AIFlowAsset = Cast<UAIFlowAsset>(GetFlowAsset()))
	{
		BlackboardComponentClass = AIFlowAsset->GetBlackboardComponentClass();

		// Default to the Flow Asset's default blackboard, if no SpecificBlackboardAsset was specified.
		if (!DesiredBlackboardAsset)
		{
			DesiredBlackboardAsset = AIFlowAsset->GetBlackboardAsset();
		}
	}

	if (!IsValid(BlackboardComponentClass))
	{
		BlackboardComponentClass = UBlackboardComponent::StaticClass();
	}

	const TArray<AActor*> ResolvedActors = TryResolveActorsForBlackboard();
	return FAIFlowActorBlackboardHelper::FindOrAddBlackboardComponentOnActors(
		ResolvedActors,
		InjectComponentsManager,
		BlackboardComponentClass,
		DesiredBlackboardAsset,
		SpecificBlackboardSearchRule,
		InjectRule);
}

bool UFlowNode_SetBlackboardValuesV2::TryFindPropertyByPinName(
	const UObject& PropertyOwnerObject,
	const FName& PinName,
	const FProperty*& OutFoundProperty,
	TInstancedStruct<FFlowDataPinValue>& OutFoundInstancedStruct) const
{
	for (const UFlowBlackboardEntryValue* BlackboardEntry : EntriesForEveryActor.Entries)
	{
		if (!IsValid(BlackboardEntry))
		{
			LogError(TEXT("Found null BlackboardEntry.  This is unexpected."), EFlowOnScreenMessageType::Temporary);
			continue;
		}

		if (BlackboardEntry->Key.KeyName == PinName)
		{
			const IFlowDataPinPropertyProviderInterface* FlowDataPinProviderInterface = CastChecked<IFlowDataPinPropertyProviderInterface>(BlackboardEntry);
			if (FlowDataPinProviderInterface->TryProvideFlowDataPinProperty(OutFoundInstancedStruct))
			{
				return true;
			}
			else
			{
				break;
			}
		}
	}

	return UAIFlowNode::TryFindPropertyByPinName(PropertyOwnerObject, PinName, OutFoundProperty, OutFoundInstancedStruct);
}

void UFlowNode_SetBlackboardValuesV2::EnsureInjectComponentsManager()
{
	if (IsValid(InjectComponentsManager))
	{
		return;
	}

	InjectComponentsManager = NewObject<UFlowInjectComponentsManager>(this);
	InjectComponentsManager->InitializeRuntime();
	InjectComponentsManager->BeforeActorRemovedDelegate.AddDynamic(this, &ThisClass::OnBeforeActorRemoved);
}

void UFlowNode_SetBlackboardValuesV2::CleanupInjectComponentsManager()
{
	if (IsValid(InjectComponentsManager))
	{
		InjectComponentsManager->ShutdownRuntime();
		InjectComponentsManager->BeforeActorRemovedDelegate.RemoveDynamic(this, &ThisClass::OnBeforeActorRemoved);
	}
	InjectComponentsManager = nullptr;
}

void UFlowNode_SetBlackboardValuesV2::OnBeforeActorRemoved(AActor* RemovedActor)
{
	if (IsValid(RemovedActor))
	{
		OnStopMonitoringActor(*RemovedActor);
	}
}

void UFlowNode_SetBlackboardValuesV2::UpdateNodeConfigText_Implementation()
{
#if WITH_EDITOR
	FTextBuilder TextBuilder;

	AppendEntriesForEveryActor(TextBuilder);

	FAIFlowActorBlackboardHelper::AppendBlackboardOptions(PerActorOptions, TextBuilder);

	SetNodeConfigText(TextBuilder.ToText());
#endif // WITH_EDITOR
}

#if WITH_EDITOR
void UFlowNode_SetBlackboardValuesV2::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	UAIFlowNode::PostEditChangeChainProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.PropertyChain.Num() == 0)
	{
		return;
	}

	auto& Property = PropertyChangedEvent.PropertyChain.GetActiveMemberNode()->GetValue();

	constexpr EPropertyChangeType::Type RelevantChangeTypesForReconstructionMask =
		EPropertyChangeType::Unspecified |
		EPropertyChangeType::ArrayAdd |
		EPropertyChangeType::ArrayRemove |
		EPropertyChangeType::ArrayClear |
		EPropertyChangeType::ValueSet |
		EPropertyChangeType::Redirected |
		EPropertyChangeType::ArrayMove;

	const uint32 PropertyChangedTypeFlags = (PropertyChangedEvent.ChangeType & RelevantChangeTypesForReconstructionMask);
	const bool bIsRelevantChangeTypeForReconstruction = PropertyChangedTypeFlags != 0;
	const bool bChangedInputProperties = Property->GetFName() == GET_MEMBER_NAME_CHECKED(ThisClass, EntriesForEveryActor);
	if (bIsRelevantChangeTypeForReconstruction && bChangedInputProperties)
	{
		OnReconstructionRequested.ExecuteIfBound();
	}
}

void UFlowNode_SetBlackboardValuesV2::AutoGenerateDataPins(FFlowAutoDataPinsWorkingData& InOutWorkingData) const
{
	Super::AutoGenerateDataPins(InOutWorkingData);

	// TODO (gtaylor) Consider combining/merging with UFlowNode_GetBlackboardValues::AutoGenerateDataPins() version
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
		TInstancedStruct<FFlowDataPinValue> InstancedFlowDataPinProperty;

		if (FlowDataPinProviderInterface->TryProvideFlowDataPinProperty(InstancedFlowDataPinProperty))
		{
			const FFlowDataPinValue& FlowDataPinValuePtr = InstancedFlowDataPinProperty.Get<FFlowDataPinValue>();
			if (const FFlowPinType* FlowPinType = FFlowPinType::LookupPinType(FlowDataPinValuePtr.GetPinTypeName()))
			{
				FFlowPin NewFlowPin = FlowPinType->CreateFlowPinFromValueWrapper(PinName, FlowDataPinValuePtr);
				InOutWorkingData.AutoInputDataPinsNext.AddUnique(NewFlowPin);
			}
			else
			{
				LogError(FString::Printf(TEXT("Could not auto-generate pin %s: Could not find pin type %s."), *PinName.ToString(), *FlowDataPinValuePtr.GetPinTypeName().ToString()), EFlowOnScreenMessageType::Temporary);
			}
		}
	}
}

UBlackboardData* UFlowNode_SetBlackboardValuesV2::GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const
{
	if (SpecificBlackboardAsset)
	{
		return SpecificBlackboardAsset;
	}

	return GetBlackboardAsset();
}

void UFlowNode_SetBlackboardValuesV2::AppendEntriesForEveryActor(FTextBuilder& InOutTextBuilder) const
{
	for (const UFlowBlackboardEntryValue* Entry : EntriesForEveryActor.Entries)
	{
		if (IsValid(Entry) && !IsInputConnected(Entry->Key.KeyName))
		{
			InOutTextBuilder.AppendLine(Entry->BuildNodeConfigText());
		}
	}
}

UBlackboardData* UFlowNode_SetBlackboardValuesV2::GetBlackboardAssetForEditor() const
{
	if (IsValid(SpecificBlackboardAsset))
	{
		return SpecificBlackboardAsset;
	}

	return GetBlackboardAsset();
}
#endif
