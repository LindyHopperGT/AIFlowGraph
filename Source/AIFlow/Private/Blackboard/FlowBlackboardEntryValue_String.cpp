// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/FlowBlackboardEntryValue_String.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_String.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowLogChannels.h"
#include "Nodes/FlowNode.h"
#include "Types/FlowDataPinValuesStandard.h"
#include "Types/FlowDataPinResults.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowBlackboardEntryValue_String)

#if WITH_EDITOR
void UFlowBlackboardEntryValue_String::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject))
	{
		if (Key.AllowedTypes.IsEmpty())
		{
			Key.AllowedTypes.Add(NewObject<UBlackboardKeyType_String>(this));
		}
	}
}

FText UFlowBlackboardEntryValue_String::BuildNodeConfigText() const
{
	return FText::FromString(FString::Printf(TEXT("Set %s to \"%s\""), *Key.GetKeyName().ToString(), *GetEditorValueString()));
}
#endif // WITH_EDITOR

bool UFlowBlackboardEntryValue_String::TryProvideFlowDataPinProperty(TInstancedStruct<FFlowDataPinValue>& OutFlowDataPinProperty) const
{
	OutFlowDataPinProperty.InitializeAs<FFlowDataPinValue_String>(StringValue);
	return true;
}

bool UFlowBlackboardEntryValue_String::TryProvideFlowDataPinPropertyFromBlackboardEntry(
	const FName& BlackboardKeyName,
	const UBlackboardKeyType& BlackboardKeyType,
	UBlackboardComponent* OptionalBlackboardComponent,
	TInstancedStruct<FFlowDataPinValue>& OutFlowDataPinProperty) const
{
	return
		TryProvideFlowDataPinPropertyFromBlackboardEntryTemplate<UBlackboardKeyType_String, FFlowDataPinValue_String>(
			BlackboardKeyName,
			BlackboardKeyType,
			OptionalBlackboardComponent,
			OutFlowDataPinProperty);
}

bool UFlowBlackboardEntryValue_String::TrySetValueFromInputDataPin(const FName& PinName, UFlowNode& PinOwnerFlowNode)
{
	const EFlowDataPinResolveResult ResolveResult = PinOwnerFlowNode.TryResolveDataPinValue<FFlowPinType_String>(PinName, StringValue);
	return FlowPinType::IsSuccess(ResolveResult);
}

void UFlowBlackboardEntryValue_String::SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const
{
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsString(Key.GetKeyName(), StringValue);
	}
}

EBlackboardCompare::Type UFlowBlackboardEntryValue_String::CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const
{
	if (!IsValid(BlackboardComponent))
	{
		UE_LOG(LogAIFlow, Error, TEXT("Cannot CompareKeyValues without a Blackboard!"));
		return EBlackboardCompare::NotEqual;
	}

	const FString OtherValueAsString = BlackboardComponent->GetValueAsString(OtherKeyName);

	if (StringValue == OtherValueAsString)
	{
		return EBlackboardCompare::Equal;
	}
	else
	{
		return EBlackboardCompare::NotEqual;
	}
}

TSubclassOf<UBlackboardKeyType> UFlowBlackboardEntryValue_String::GetSupportedBlackboardKeyType() const
{
	return UBlackboardKeyType_String::StaticClass();
}
