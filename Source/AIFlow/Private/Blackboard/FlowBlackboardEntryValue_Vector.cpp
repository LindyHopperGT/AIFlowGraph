// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/FlowBlackboardEntryValue_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIFlowLogChannels.h"

#if WITH_EDITOR
void UFlowBlackboardEntryValue_Vector::PostInitProperties()
{
	Super::PostInitProperties();

	if (!HasAnyFlags(RF_ArchetypeObject | RF_ClassDefaultObject))
	{
		if (Key.AllowedTypes.IsEmpty())
		{
			Key.AllowedTypes.Add(NewObject<UBlackboardKeyType_Vector>(this));
		}
	}
}

FText UFlowBlackboardEntryValue_Vector::BuildNodeConfigText() const
{
	return FText::FromString(FString::Printf(TEXT("Set %s to %s"), *Key.GetKeyName().ToString(), *GetEditorValueString()));
}
#endif // WITH_EDITOR

void UFlowBlackboardEntryValue_Vector::SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const
{
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsVector(Key.GetKeyName(), VectorValue);
	}
}

EBlackboardCompare::Type UFlowBlackboardEntryValue_Vector::CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const
{
	if (!IsValid(BlackboardComponent))
	{
		UE_LOG(LogAIFlow, Error, TEXT("Cannot CompareKeyValues without a Blackboard!"));
		return EBlackboardCompare::NotEqual;
	}

	const FVector OtherValueAsVector = BlackboardComponent->GetValueAsVector(OtherKeyName);

	if (VectorValue.Equals(OtherValueAsVector))
	{
		return EBlackboardCompare::Equal;
	}
	else
	{
		return EBlackboardCompare::NotEqual;
	}
}

TSubclassOf<UBlackboardKeyType> UFlowBlackboardEntryValue_Vector::GetSupportedBlackboardKeyType() const
{
	return UBlackboardKeyType_Vector::StaticClass();
}
