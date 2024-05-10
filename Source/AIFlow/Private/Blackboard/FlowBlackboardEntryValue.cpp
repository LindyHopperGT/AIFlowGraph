// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Blackboard/FlowBlackboardEntryValue.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType.h"

// UFlowBlackboardEntryValue Implementation

UBlackboardData* UFlowBlackboardEntryValue::GetBlackboardAsset() const
{
	IBlackboardAssetProvider* OuterProvider = Cast<IBlackboardAssetProvider>(GetOuter());
	if (OuterProvider)
	{
		return OuterProvider->GetBlackboardAsset();
	}

	return nullptr;
}

#if WITH_EDITOR
bool UFlowBlackboardEntryValue::TryReconfigureFromBlackboardKeyType(const UBlackboardKeyType& KeyType)
{
	TSubclassOf<UBlackboardKeyType> SupportedKeyClass = GetSupportedBlackboardKeyType();
	check(SupportedKeyClass && KeyType.IsA(SupportedKeyClass));
	check(Key.AllowedTypes.IsEmpty() || Key.AllowedTypes.Num() == 1);

	if (Key.AllowedTypes.Num() == 1 && Key.AllowedTypes[0]->IsA(SupportedKeyClass))
	{
		// No change to the supported class
		return false;
	}
	else
	{
		// Reconfigure the AllowedTypes to match the passed-in KeyType
		Key.AllowedTypes.Reset();

		UBlackboardKeyType* InstancedKeyType = NewObject<UBlackboardKeyType>(this, KeyType.GetClass(), NAME_None, RF_NoFlags, const_cast<UBlackboardKeyType*>(&KeyType));

		Key.AllowedTypes.Add(InstancedKeyType);

		return true;
	}
}

TSubclassOf<UFlowBlackboardEntryValue> UFlowBlackboardEntryValue::GetFlowBlackboardEntryValueClassForKeyType(TSubclassOf<UBlackboardKeyType> KeyTypeClass)
{
	if (!KeyTypeClass)
	{
		return nullptr;
	}

	TArray<UClass*> Subclasses;
	GetDerivedClasses(UFlowBlackboardEntryValue::StaticClass(), Subclasses);

	for (UClass* Subclass : Subclasses)
	{
		const UFlowBlackboardEntryValue* TypedSubclassCDO = Cast<UFlowBlackboardEntryValue>(Subclass->GetDefaultObject());

		TSubclassOf<UBlackboardKeyType> SupportedType = TypedSubclassCDO->GetSupportedBlackboardKeyType();
		if (SupportedType && KeyTypeClass->IsChildOf(SupportedType))
		{
			// NOTE (gtaylor) Taking the first class found that supports the KeyTypeClass.
			//  We could, instead, keep searching and resolve between multiple possible choices with a "best fit" algorithm of some sort.
			//  But that's overkill at this point, since we don't have any expectation that that is a case we will need to support.

			return Subclass;
		}
	}

	return nullptr;
}

#endif // WITH_EDITOR