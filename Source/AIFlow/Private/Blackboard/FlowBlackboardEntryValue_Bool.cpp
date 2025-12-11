// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/FlowBlackboardEntryValue_Bool.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowLogChannels.h"
#include "Nodes/FlowNode.h"
#include "Types/FlowDataPinValuesStandard.h"
#include "Types/FlowDataPinResults.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FlowBlackboardEntryValue_Bool)

#if WITH_EDITOR
void UFlowBlackboardEntryValue_Bool::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject))
	{
		if (Key.AllowedTypes.IsEmpty())
		{
			Key.AllowedTypes.Add(NewObject<UBlackboardKeyType_Bool>(this));
		}
	}
}

FString UFlowBlackboardEntryValue_Bool::GetEditorValueString() const
{
	return bBoolValue ? TEXT("true") : TEXT("false");
}

FText UFlowBlackboardEntryValue_Bool::BuildNodeConfigText() const
{
	return FText::FromString(FString::Printf(TEXT("Set %s to \"%s\""), *Key.GetKeyName().ToString(), *GetEditorValueString()));
}
#endif // WITH_EDITOR

bool UFlowBlackboardEntryValue_Bool::TryProvideFlowDataPinProperty(TInstancedStruct<FFlowDataPinValue>& OutFlowDataPinProperty) const
{
	OutFlowDataPinProperty.InitializeAs<FFlowDataPinValue_Bool>(bBoolValue);
	return true;
}

bool UFlowBlackboardEntryValue_Bool::TryProvideFlowDataPinPropertyFromBlackboardEntry(
	const FName& BlackboardKeyName,
	const UBlackboardKeyType& BlackboardKeyType,
	UBlackboardComponent* OptionalBlackboardComponent,
	TInstancedStruct<FFlowDataPinValue>& OutFlowDataPinProperty) const
{
	return
		TryProvideFlowDataPinPropertyFromBlackboardEntryTemplate<UBlackboardKeyType_Bool, FFlowDataPinValue_Bool>(
			BlackboardKeyName,
			BlackboardKeyType,
			OptionalBlackboardComponent,
			OutFlowDataPinProperty);
}

bool UFlowBlackboardEntryValue_Bool::TrySetValueFromInputDataPin(const FName& PinName, UFlowNode& PinOwnerFlowNode)
{
	const EFlowDataPinResolveResult ResolveResult = PinOwnerFlowNode.TryResolveDataPinValue<FFlowPinType_Bool>(PinName, bBoolValue);
	return FlowPinType::IsSuccess(ResolveResult);
}

void UFlowBlackboardEntryValue_Bool::SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const
{
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsBool(Key.GetKeyName(), bBoolValue);
	}
}

EBlackboardCompare::Type UFlowBlackboardEntryValue_Bool::CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const
{
	if (!IsValid(BlackboardComponent))
	{
		UE_LOG(LogAIFlow, Error, TEXT("Cannot CompareKeyValues without a Blackboard!"));
		return EBlackboardCompare::NotEqual;
	}

	const bool bOtherKeyValue = BlackboardComponent->GetValueAsBool(OtherKeyName);

	if (bBoolValue == bOtherKeyValue)
	{
		return EBlackboardCompare::Equal;
	}
	else
	{
		return EBlackboardCompare::NotEqual;
	}
}

TSubclassOf<UBlackboardKeyType> UFlowBlackboardEntryValue_Bool::GetSupportedBlackboardKeyType() const
{
	return UBlackboardKeyType_Bool::StaticClass();
}

