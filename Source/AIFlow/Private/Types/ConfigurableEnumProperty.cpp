// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Types/ConfigurableEnumProperty.h"

// UConfigurableEnumPropertyFunctionLibrary Implementation

uint8 UConfigurableEnumPropertyFunctionLibrary::AutoConvert_ConfigurableEnumPropertyToEnum(const FConfigurableEnumProperty& EnumValue)
{
	const uint64 EnumValueAsInt = EnumValue.EnumClass->GetValueByName(EnumValue.Value);

	// At least For Now(tm) Blueprint Enums want to be uint8's
	return static_cast<uint8>(EnumValueAsInt);
}
