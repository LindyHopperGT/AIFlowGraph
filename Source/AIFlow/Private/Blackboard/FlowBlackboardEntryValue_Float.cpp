// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/FlowBlackboardEntryValue_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowLogChannels.h"
#include "Nodes/FlowNode.h"
#include "Types/FlowDataPinProperties.h"
#include "Types/FlowDataPinResults.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowBlackboardEntryValue_Float)

#if WITH_EDITOR
void UFlowBlackboardEntryValue_Float::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject))
	{
		if (Key.AllowedTypes.IsEmpty())
		{
			Key.AllowedTypes.Add(NewObject<UBlackboardKeyType_Float>(this));
		}
	}
}

FString UFlowBlackboardEntryValue_Float::GetEditorValueString() const
{
	return FString::Printf(TEXT("%f"), FloatValue);
}

FText UFlowBlackboardEntryValue_Float::BuildNodeConfigText() const
{
	return FText::FromString(FString::Printf(TEXT("Set %s to \"%f\""), *Key.GetKeyName().ToString(), FloatValue));
}
#endif // WITH_EDITOR

bool UFlowBlackboardEntryValue_Float::TryProvideFlowDataPinProperty(const bool bIsInputPin, TInstancedStruct<FFlowDataPinProperty>& OutFlowDataPinProperty) const
{
	if (bIsInputPin)
	{
		OutFlowDataPinProperty.InitializeAs<FFlowDataPinInputProperty_Float>(FloatValue);
	}
	else
	{
		OutFlowDataPinProperty.InitializeAs<FFlowDataPinOutputProperty_Float>(FloatValue);
	}

	return true;
}

bool UFlowBlackboardEntryValue_Float::TrySetValueFromInputDataPin(const FName& PinName, UFlowNode& PinOwnerFlowNode)
{
	const FFlowDataPinResult_Float FlowDataPinResult = PinOwnerFlowNode.TryResolveDataPinAsFloat(PinName);

	if (FlowDataPinResult.Result == EFlowDataPinResolveResult::Success)
	{
		FloatValue = static_cast<float>(FlowDataPinResult.Value);

		return true;
	}

	return false;
}

void UFlowBlackboardEntryValue_Float::SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const
{
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsFloat(Key.GetKeyName(), FloatValue);
	}
}

EBlackboardCompare::Type UFlowBlackboardEntryValue_Float::CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const
{
	if (!IsValid(BlackboardComponent))
	{
		UE_LOG(LogAIFlow, Error, TEXT("Cannot CompareKeyValues without a Blackboard!"));
		return EBlackboardCompare::NotEqual;
	}

	const float OtherValueAsFloat = BlackboardComponent->GetValueAsFloat(OtherKeyName);

	if (FMath::IsNearlyEqual(FloatValue, OtherValueAsFloat))
	{
		return EBlackboardCompare::Equal;
	}
	else
	{
		return EBlackboardCompare::NotEqual;
	}
}

TSubclassOf<UBlackboardKeyType> UFlowBlackboardEntryValue_Float::GetSupportedBlackboardKeyType() const
{
	return UBlackboardKeyType_Float::StaticClass();
}

bool UFlowBlackboardEntryValue_Float::TryGetNumericalValuesForArithmeticOperation(int32* OutIntValue, float* OutFloatValue) const
{
	if (OutIntValue)
	{
		*OutIntValue = FMath::FloorToInt32(FloatValue);
	}

	if (OutFloatValue)
	{
		*OutFloatValue = FloatValue;
	}

	return true;
}
