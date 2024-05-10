// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Types/ConfigurableEnumProperty.h"

// FConfigurableEnumProperty Implementation

#if WITH_EDITOR

void FConfigurableEnumProperty::PostEditChangedEnumName()
{
	// See UBlackboardKeyType_Enum::PostEditChangeProperty()

	if (!EnumName.IsEmpty())
	{
		EnumClass = UClass::TryFindTypeSlow<UEnum>(EnumName, EFindFirstObjectOptions::ExactClass);

		if (EnumClass != nullptr && !UBlackboardKeyType_EnumFriendMaker::ValidateEnumForPublic(*EnumClass))
		{
			EnumClass = nullptr;
		}
	}
}

bool UBlackboardKeyType_EnumFriendMaker::ValidateEnumForPublic(const UEnum& EnumType)
{
	return UBlackboardKeyType_Enum::ValidateEnum(EnumType);
}
#endif // WITH_EDITOR

// UConfigurableEnumPropertyFunctionLibrary Implementation

uint8 UConfigurableEnumPropertyFunctionLibrary::AutoConvert_ConfigurableEnumPropertyToEnum(const FConfigurableEnumProperty& EnumValue)
{
	const uint64 EnumValueAsInt = EnumValue.EnumClass->GetValueByName(EnumValue.Value);

	// At least For Now(tm) Blueprint Enums want to be uint8's
	return static_cast<uint8>(EnumValueAsInt);
}
