// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "BehaviorTree/Blackboard/BlackboardKeyType_Enum.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"

#include "ConfigurableEnumProperty.generated.h"

// Similar to FAnimNodeFunctionRef, providing a configurable enum property
USTRUCT(BlueprintType)
struct FConfigurableEnumProperty
{
	GENERATED_BODY()

	// For GET_MEMBER_NAME_CHECKED access
	friend class FConfigurableEnumPropertyCustomization;

public:

#if WITH_EDITOR
	// NOTE!  Must call this whenever EnumName property changes in editor to support C++ enums.
	void PostEditChangedEnumName();
#endif // WITH_EDITOR

	// The selected enum Value
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Blackboard)
	FName Value = NAME_None;

	// Class for this enum
	UPROPERTY(EditAnywhere, Category = Blackboard)
	UEnum* EnumClass = nullptr;

#if WITH_EDITORONLY_DATA
	// name of enum defined in c++ code, will take priority over asset from EnumType property
	//  (this is a work-around because EnumClass cannot find C++ Enums, 
	//   so you need to type the name of the enum in here, manually)
	// See also: UBlackboardKeyType_Enum::PostEditChangeProperty()
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FString EnumName;
#endif // WITH_EDITORONLY_DATA
};

UCLASS()
class UConfigurableEnumPropertyFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Auto-cast function to convert the FConfigurableEnumProperty into an Enum in Blueprint 
	// (so we don't need to Break the struct every time we want the key)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Convert to Enum", CompactNodeTitle = "->", BlueprintAutocast), Category = Blackboard)
	static uint8 AutoConvert_ConfigurableEnumPropertyToEnum(const FConfigurableEnumProperty& ConfigurableEnumProperty);
};

UCLASS(hidedropdown)
class UBlackboardKeyType_EnumFriendMaker : public UBlackboardKeyType_Enum
{
	GENERATED_BODY()

	// Subclass only to give access to ValidateEnum() function to Outsiders
public:
#if WITH_EDITOR
	static bool ValidateEnumForPublic(const UEnum& EnumType);
#endif
};

