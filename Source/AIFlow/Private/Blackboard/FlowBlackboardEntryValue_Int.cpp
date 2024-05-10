// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/FlowBlackboardEntryValue_Int.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Int.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowLogChannels.h"

#if WITH_EDITOR
void UFlowBlackboardEntryValue_Int::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject))
	{
		if (Key.AllowedTypes.IsEmpty())
		{
			Key.AllowedTypes.Add(NewObject<UBlackboardKeyType_Int>(this));
		}
	}
}

FString UFlowBlackboardEntryValue_Int::GetEditorValueString() const
{
	return FString::FromInt(IntValue);
}

FText UFlowBlackboardEntryValue_Int::BuildNodeConfigText() const
{
	return FText::FromString(FString::Printf(TEXT("Set %s to %d"), *Key.GetKeyName().ToString(), IntValue));
}
#endif // WITH_EDITOR

void UFlowBlackboardEntryValue_Int::SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const
{
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsInt(Key.GetKeyName(), IntValue);
	}
}

EBlackboardCompare::Type UFlowBlackboardEntryValue_Int::CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const
{
	if (!IsValid(BlackboardComponent))
	{
		UE_LOG(LogAIFlow, Error, TEXT("Cannot CompareKeyValues without a Blackboard!"));
		return EBlackboardCompare::NotEqual;
	}

	const int32 OtherValueAsInt = BlackboardComponent->GetValueAsInt(OtherKeyName);

	if (IntValue == OtherValueAsInt)
	{
		return EBlackboardCompare::Equal;
	}
	else
	{
		return EBlackboardCompare::NotEqual;
	}
}

TSubclassOf<UBlackboardKeyType> UFlowBlackboardEntryValue_Int::GetSupportedBlackboardKeyType() const
{
	return UBlackboardKeyType_Int::StaticClass();
}

bool UFlowBlackboardEntryValue_Int::TryGetNumericalValuesForArithmeticOperation(int32* OutIntValue, float* OutFloatValue) const
{
	if (OutIntValue)
	{
		*OutIntValue = IntValue;
	}

	if (OutFloatValue)
	{
		*OutFloatValue = static_cast<float>(IntValue);
	}

	return true;
}
