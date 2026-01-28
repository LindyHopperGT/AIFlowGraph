// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "UObject/Object.h"
#include "Types/FlowNamedDataPinProperty.h"

#include "AIFlowBlackboardWatchData.generated.h"

/**
* Display mode for Blackboard watch entries.
*
* - DisplayEntriesAsText:
*     Show each entry's value as a plain text string, derived from
*     FFlowDataPinValue::TryConvertValuesToString.
*
* - DisplayEntriesAsFlowPinValues:
*     Show the full FFlowDataPinValue property/editor instead.
*/
UENUM()
enum class EAIFlowBlackboardDisplayMode : uint8
{
	/** Display entries as a stringified value (default). */
	DisplayEntriesAsText			UMETA(DisplayName = "Text"),

	/** Display entries as full Flow Data Pin Values (legacy / detailed view). */
	DisplayEntriesAsFlowPinValues	UMETA(DisplayName = "Flow Pin Values"),
};

/**
* Transient container for blackboard watch data.
*
* Holds a dynamic array of Flow Named Data Pin Properties, which are populated
* from the active Blackboard at debug time and displayed via a Details View.
*/
UCLASS()
class AIFLOWEDITOR_API UAIFlowBlackboardWatchData : public UObject
{
	GENERATED_BODY()

public:
	// Dynamic list of watched blackboard entries, as Flow Named Data Pin Properties.
	// Using VisibleAnywhere so the Details View can display them without implying
	// that they are part of the saved asset.
	UPROPERTY(VisibleAnywhere, Category = "BlackboardWatch", meta = (TitleProperty = "Name"))
	TArray<FFlowNamedDataPinProperty> NamedProperties;

	// Controls how NamedProperties should be presented to the user.
	// This is primarily consumed by the details customization and/or
	// the owning watch widget.
	UPROPERTY()
	EAIFlowBlackboardDisplayMode DisplayMode = EAIFlowBlackboardDisplayMode::DisplayEntriesAsText;
};