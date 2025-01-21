// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Nodes/FlowNode_GetBlackboardValues.h"
#include "Blackboard/FlowBlackboardEntryValue.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowAsset.h"
#include "AIFlowTags.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"
#include "Blackboard/FlowBlackboardEntryValue_Enum.h"
#include "InstancedStruct.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowNode_GetBlackboardValues)

FName UFlowNode_GetBlackboardValues::INPIN_SpecificActor;

UFlowNode_GetBlackboardValues::UFlowNode_GetBlackboardValues()
	: Super()
{
#if WITH_EDITOR
	NodeDisplayStyle = FlowNodeStyle::Blackboard;

	// TODO (gtaylor) Still needs a bit more testing and refinement, when time allows.
	Category = TEXT("Experimental");
#endif

	InputPins.Empty();
	OutputPins.Empty();

	INPIN_SpecificActor = GET_MEMBER_NAME_CHECKED(ThisClass, SpecificActor);
}

AActor* UFlowNode_GetBlackboardValues::TryResolveActorForBlackboard() const
{
	// TODO (gtaylor) Cache this result for better lookup perf + an exec pin to provoke a recache operation

	// First check the specific actor pin
	const FFlowDataPinResult_Object PinResult = TryResolveDataPinAsObject(INPIN_SpecificActor);

	if (PinResult.Result == EFlowDataPinResolveResult::Success)
	{
		AActor* ResolvedSpecificActor = Cast<AActor>(PinResult.Value);
		if (IsValid(ResolvedSpecificActor))
		{
			return ResolvedSpecificActor;
		}

		LogError(TEXT("Specific actor could not be resolved to an actor."), EFlowOnScreenMessageType::Temporary);

		return nullptr;
	}

	// Default to the Flow graph's actor.  
	AActor* ActorOwner = GetFlowAsset()->TryFindActorOwner();
	return ActorOwner;
}

UBlackboardComponent* UFlowNode_GetBlackboardValues::GetBlackboardComponentToApplyTo() const
{
	TSubclassOf<UBlackboardComponent> BlackboardComponentClass = UBlackboardComponent::StaticClass();
	if (UAIFlowAsset* AIFlowAsset = Cast<UAIFlowAsset>(GetFlowAsset()))
	{
		BlackboardComponentClass = AIFlowAsset->GetBlackboardComponentClass();
	}

	AActor* ActorSourceForBlackboard = TryResolveActorForBlackboard();
	if (!IsValid(ActorSourceForBlackboard))
	{
		return nullptr;
	}

	constexpr UFlowInjectComponentsManager* InjectComponentsManager = nullptr;

	UBlackboardComponent* BlackboardComponent =
		FAIFlowActorBlackboardHelper::FindOrAddBlackboardComponentOnActor(
			*ActorSourceForBlackboard,
			InjectComponentsManager,
			BlackboardComponentClass,
			SpecificBlackboardAsset,
			SpecificBlackboardSearchRule,
			EActorBlackboardInjectRule::DoNotInjectIfMissing);

	return BlackboardComponent;
}

bool UFlowNode_GetBlackboardValues::TryFindPropertyByRemappedPinName(const FName& RemappedPinName, const FProperty*& OutFoundProperty, TInstancedStruct<FFlowDataPinProperty>& OutFoundInstancedStruct, EFlowDataPinResolveResult& InOutResult) const
{
	if (RemappedPinName == INPIN_SpecificActor)
	{
		// If finding the property for SpecificActor, we cannot look on our blackboard, 
		// otherwise we will end up recursing infinitely (because it will lookup the specific actor...)
		return Super::TryFindPropertyByRemappedPinName(RemappedPinName, OutFoundProperty, OutFoundInstancedStruct, InOutResult);
	}

	// The GetBlackboardValues node stores its properties in instanced structs in an array, so look there first

	UBlackboardComponent* BlackboardComponent = GetBlackboardComponentToApplyTo();

	if (IsValid(BlackboardComponent) && IsValid(BlackboardComponent->GetBlackboardAsset()))
	{
		for (const FFlowBlackboardEntry& BlackboardEntry : BlackboardEntries)
		{
			if (BlackboardEntry.KeyName != RemappedPinName)
			{
				continue;
			}

			UBlackboardKeyType* BlackboardKeyType = GetBlackboardKeyTypeFromBlackboardKeyName(BlackboardComponent->GetBlackboardAsset(), BlackboardEntry.KeyName);

			const bool bProvidedValueFromBlackboard = 
				FAIFlowActorBlackboardHelper::TryProvideFlowDataPinPropertyFromBlackboardEntry(
					RemappedPinName,
					BlackboardKeyType,
					BlackboardComponent,
					OutFoundInstancedStruct);

			if (bProvidedValueFromBlackboard)
			{
				return true;
			}
			else
			{
				LogError(FString::Printf(TEXT("Could not get value for pin name %s, on blackboard %s"), *RemappedPinName.ToString(), *BlackboardComponent->GetName()), EFlowOnScreenMessageType::Temporary);

				return false;
			}
		}
	}

	return Super::TryFindPropertyByRemappedPinName(RemappedPinName, OutFoundProperty, OutFoundInstancedStruct, InOutResult);
}

#if WITH_EDITOR
void UFlowNode_GetBlackboardValues::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChainEvent)
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
	const bool bChangedOutputProperties = Property->GetFName() == GET_MEMBER_NAME_CHECKED(ThisClass, BlackboardEntries);
	if (bIsRelevantChangeTypeForReconstruction && bChangedOutputProperties)
	{
		OnReconstructionRequested.ExecuteIfBound();
	}
}

UBlackboardData* UFlowNode_GetBlackboardValues::GetBlackboardAssetForEditor() const
{
	if (IsValid(SpecificBlackboardAsset))
	{
		return SpecificBlackboardAsset;
	}

	return GetBlackboardAsset();
}

UBlackboardData* UFlowNode_GetBlackboardValues::GetBlackboardAssetForPropertyHandle(const TSharedPtr<IPropertyHandle>& PropertyHandle) const
{
	if (UBlackboardData* BlackboardAssetForEditor = GetBlackboardAssetForEditor())
	{
		return BlackboardAssetForEditor;
	}
	
	return Super::GetBlackboardAssetForPropertyHandle(PropertyHandle);
}

void UFlowNode_GetBlackboardValues::AutoGenerateDataPins(TMap<FName, FName>& InOutPinNameToBoundPropertyNameMap, TArray<FFlowPin>& InOutInputDataPins, TArray<FFlowPin>& InOutOutputDataPins) const
{
	const UBlackboardData* BlackboardAssetForEditor = GetBlackboardAssetForEditor();

	if (!IsValid(BlackboardAssetForEditor))
	{
		LogError(TEXT("Could not auto-generate pins: no blackboard asset found for use in the editor."), EFlowOnScreenMessageType::Temporary);

		return;
	}

	for (const FFlowBlackboardEntry& BlackboardEntry : BlackboardEntries)
	{
		const FName& PinName = BlackboardEntry.KeyName;
		if (PinName.IsNone())
		{
			continue;
		}
		
		// Source BlackboardKeyType from the expected blackboard
		UBlackboardKeyType* BlackboardKeyType = GetBlackboardKeyTypeFromBlackboardKeyName(BlackboardAssetForEditor, BlackboardEntry.KeyName);
		if (!BlackboardKeyType)
		{
			LogError(FString::Printf(TEXT("Could not auto-generate pins: blackboard asset could not provide a type for pin name %s."), *PinName.ToString()), EFlowOnScreenMessageType::Temporary);

			continue;
		}

		TInstancedStruct<FFlowDataPinProperty> InstancedFlowDataPinProperty;

		constexpr UBlackboardComponent* BlackboardComponent = nullptr;
		const bool bProvidedValueFromBlackboard =
			FAIFlowActorBlackboardHelper::TryProvideFlowDataPinPropertyFromBlackboardEntry(
				PinName,
				BlackboardKeyType,
				BlackboardComponent,
				InstancedFlowDataPinProperty);

		if (bProvidedValueFromBlackboard)
		{
			InOutPinNameToBoundPropertyNameMap.Add(PinName, PinName);

			FFlowPin NewFlowPin = FFlowDataPinProperty::CreateFlowPin(PinName, InstancedFlowDataPinProperty);
			InOutOutputDataPins.AddUnique(NewFlowPin);
		}
		else
		{
			LogError(FString::Printf(TEXT("Could not auto-generate pins: blackboard asset could not provide a value for pin name %s."), *PinName.ToString()), EFlowOnScreenMessageType::Temporary);
		}
	}
}

#endif // WITH_EDITOR

void UFlowNode_GetBlackboardValues::UpdateNodeConfigText_Implementation()
{
#if WITH_EDITOR
	FTextBuilder TextBuilder;

	// TODO (gtaylor) Include some additional configuration notes here

	SetNodeConfigText(TextBuilder.ToText());
#endif // WITH_EDITOR
}

UBlackboardKeyType* UFlowNode_GetBlackboardValues::GetBlackboardKeyTypeFromBlackboardKeyName(const UBlackboardData* BlackboardAsset, const FName& KeyName)
{
	if (!IsValid(BlackboardAsset))
	{
		return nullptr;
	}

	const FBlackboard::FKey KeyID = BlackboardAsset->GetKeyID(KeyName);

	if (KeyID != FBlackboard::InvalidKey)
	{
		if (const FBlackboardEntry* BlackboardKey = BlackboardAsset->GetKey(KeyID))
		{
			return BlackboardKey->KeyType;
		}
	}

	return nullptr;
}

// Must implement TrySupplyDataPinAs... for every EFlowPinType
FLOW_ASSERT_ENUM_MAX(EFlowPinType, 16);

FFlowDataPinResult_Bool UFlowNode_GetBlackboardValues::TrySupplyDataPinAsBool_Implementation(const FName& PinName) const
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponentToApplyTo())
	{
		return FFlowDataPinResult_Bool(BlackboardComponent->GetValueAsBool(PinName));
	}

	return Super::TrySupplyDataPinAsBool_Implementation(PinName);
}

FFlowDataPinResult_Int UFlowNode_GetBlackboardValues::TrySupplyDataPinAsInt_Implementation(const FName& PinName) const
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponentToApplyTo())
	{
		return FFlowDataPinResult_Int(BlackboardComponent->GetValueAsInt(PinName));
	}

	return Super::TrySupplyDataPinAsInt_Implementation(PinName);
}

FFlowDataPinResult_Float UFlowNode_GetBlackboardValues::TrySupplyDataPinAsFloat_Implementation(const FName& PinName) const
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponentToApplyTo())
	{
		return FFlowDataPinResult_Float(BlackboardComponent->GetValueAsFloat(PinName));
	}

	return Super::TrySupplyDataPinAsFloat_Implementation(PinName);
}

FFlowDataPinResult_Name UFlowNode_GetBlackboardValues::TrySupplyDataPinAsName_Implementation(const FName& PinName) const
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponentToApplyTo())
	{
		return FFlowDataPinResult_Name(BlackboardComponent->GetValueAsName(PinName));
	}

	return Super::TrySupplyDataPinAsName_Implementation(PinName);
}

FFlowDataPinResult_String UFlowNode_GetBlackboardValues::TrySupplyDataPinAsString_Implementation(const FName& PinName) const
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponentToApplyTo())
	{
		return FFlowDataPinResult_String(BlackboardComponent->GetValueAsString(PinName));
	}

	return Super::TrySupplyDataPinAsString_Implementation(PinName);
}

FFlowDataPinResult_Enum UFlowNode_GetBlackboardValues::TrySupplyDataPinAsEnum_Implementation(const FName& PinName) const
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponentToApplyTo())
	{
		FFlowDataPinResult_Enum EnumResult = UFlowBlackboardEntryValue_Enum::TryBuildDataPinResultFromBlackboardEnumEntry(PinName, *BlackboardComponent);

		if (EnumResult.Result == EFlowDataPinResolveResult::Success)
		{
			return EnumResult;
		}
	}

	return Super::TrySupplyDataPinAsEnum_Implementation(PinName);
}

FFlowDataPinResult_Vector UFlowNode_GetBlackboardValues::TrySupplyDataPinAsVector_Implementation(const FName& PinName) const
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponentToApplyTo())
	{
		return FFlowDataPinResult_Vector(BlackboardComponent->GetValueAsVector(PinName));
	}

	return Super::TrySupplyDataPinAsVector_Implementation(PinName);
}

FFlowDataPinResult_Rotator UFlowNode_GetBlackboardValues::TrySupplyDataPinAsRotator_Implementation(const FName& PinName) const
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponentToApplyTo())
	{
		return FFlowDataPinResult_Rotator(BlackboardComponent->GetValueAsRotator(PinName));
	}

	return Super::TrySupplyDataPinAsRotator_Implementation(PinName);
}

FFlowDataPinResult_GameplayTag UFlowNode_GetBlackboardValues::TrySupplyDataPinAsGameplayTag_Implementation(const FName& PinName) const
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponentToApplyTo())
	{
		if (const UBlackboardKeyType* BlackboardKeyType = GetBlackboardKeyTypeFromBlackboardKeyName(BlackboardComponent->GetBlackboardAsset(), PinName))
		{
			// Because FGameplayTag blackboard support is provided by an extension plugin (that this plugin cannot access),
			// we need to use a more expensive lookup pathway using the UFlowBlackboardEntryValue subclass that 
			// supports the BlackboardKeyType of the key we want to lookup.

			TInstancedStruct<FFlowDataPinProperty> InstancedFlowDataPinProperty;

			const bool bProvidedValueFromBlackboard =
				FAIFlowActorBlackboardHelper::TryProvideFlowDataPinPropertyFromBlackboardEntry(
					PinName,
					BlackboardKeyType,
					BlackboardComponent,
					InstancedFlowDataPinProperty);

			if (const FFlowDataPinOutputProperty_GameplayTag* TagProperty = InstancedFlowDataPinProperty.GetPtr<FFlowDataPinOutputProperty_GameplayTag>())
			{
				return FFlowDataPinResult_GameplayTag(TagProperty->Value);
			}
		}
		else
		{
			LogError(FString::Printf(TEXT("Could supply data pin as gameplay tag, because the blackboard key %s could not be found on the blackboard %s."), *PinName.ToString(), *BlackboardComponent->GetName()), EFlowOnScreenMessageType::Temporary);
		}
	}

	return Super::TrySupplyDataPinAsGameplayTag_Implementation(PinName);
}

FFlowDataPinResult_GameplayTagContainer UFlowNode_GetBlackboardValues::TrySupplyDataPinAsGameplayTagContainer_Implementation(const FName& PinName) const
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponentToApplyTo())
	{
		if (const UBlackboardKeyType* BlackboardKeyType = GetBlackboardKeyTypeFromBlackboardKeyName(BlackboardComponent->GetBlackboardAsset(), PinName))
		{
			// Because FGameplayTagContainer blackboard support is provided by an extension plugin (that this plugin cannot access),
			// we need to use a more expensive lookup pathway using the UFlowBlackboardEntryValue subclass that 
			// supports the BlackboardKeyType of the key we want to lookup.

			TInstancedStruct<FFlowDataPinProperty> InstancedFlowDataPinProperty;

			const bool bProvidedValueFromBlackboard =
				FAIFlowActorBlackboardHelper::TryProvideFlowDataPinPropertyFromBlackboardEntry(
					PinName,
					BlackboardKeyType,
					BlackboardComponent,
					InstancedFlowDataPinProperty);

			if (const FFlowDataPinOutputProperty_GameplayTagContainer* TagContainerProperty = InstancedFlowDataPinProperty.GetPtr<FFlowDataPinOutputProperty_GameplayTagContainer>())
			{
				return FFlowDataPinResult_GameplayTagContainer(TagContainerProperty->Value);
			}
		}
		else
		{
			LogError(FString::Printf(TEXT("Could supply data pin as gameplay tag container, because the blackboard key %s could not be found on the blackboard %s."), *PinName.ToString(), *BlackboardComponent->GetName()), EFlowOnScreenMessageType::Temporary);
		}
	}

	return Super::TrySupplyDataPinAsGameplayTagContainer_Implementation(PinName);
}

FFlowDataPinResult_Object UFlowNode_GetBlackboardValues::TrySupplyDataPinAsObject_Implementation(const FName& PinName) const
{
	if (PinName == INPIN_SpecificActor)
	{
		return Super::TrySupplyDataPinAsObject_Implementation(PinName);
	}

	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponentToApplyTo())
	{
		return FFlowDataPinResult_Object(BlackboardComponent->GetValueAsObject(PinName));
	}

	return Super::TrySupplyDataPinAsObject_Implementation(PinName);
}

FFlowDataPinResult_Class UFlowNode_GetBlackboardValues::TrySupplyDataPinAsClass_Implementation(const FName& PinName) const
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponentToApplyTo())
	{
		return FFlowDataPinResult_Class(BlackboardComponent->GetValueAsClass(PinName));
	}

	return Super::TrySupplyDataPinAsClass_Implementation(PinName);
}
