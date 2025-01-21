// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#include "Types/FlowBlackboardEntry.h"

// UFlowBlackboardFunctionLibrary Implementation

FName UFlowBlackboardFunctionLibrary::AutoConvert_FlowBlackboardEntryKeyToName(const FFlowBlackboardEntry& FlowBlackboardEntry)
{
	return FlowBlackboardEntry.GetKeyName();
}

