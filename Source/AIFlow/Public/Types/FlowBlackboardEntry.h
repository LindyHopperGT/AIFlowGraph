// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "Templates/SubclassOf.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "FlowBlackboardEntry.generated.h"

// Forward Declarations
class UBlackboardEntry;
class UBlackboardKeyType;
class UBlackboardComponent;

// Similar to FAnimNodeFunctionRef, providing a FName-based BlackboardEntry binding
//  that is resolved at runtime
USTRUCT(BlueprintType)
struct FFlowBlackboardEntry
{
	GENERATED_BODY()

	// For GET_MEMBER_NAME_CHECKED access
	friend class FFlowBlackboardEntryCustomization;

public:

	const FName& GetKeyName() const { return KeyName; }

	bool operator ==(const FFlowBlackboardEntry& Other) const { return KeyName == Other.KeyName; }
	bool operator !=(const FFlowBlackboardEntry& Other) const { return !(*this == Other); }

public:

	// The blackboard Key's name
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Blackboard)
	FName KeyName = NAME_None;

#if WITH_EDITORONLY_DATA
public:
	// array of allowed types with additional properties (e.g. uobject's base class) 
	UPROPERTY(EditDefaultsOnly, Instanced, Category = Blackboard)
	TArray<TObjectPtr<UBlackboardKeyType>> AllowedTypes;
#endif // WITH_EDITORONLY_DATA
};

UCLASS()
class UFlowBlackboardFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Auto-cast function to convert the FlowBlackboardEntry into an FName in Blueprint 
	// (so we don't need to Break the struct every time we want the key)
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Convert to Name", CompactNodeTitle = "->", BlueprintAutocast), Category = Blackboard)
	static FName AutoConvert_FlowBlackboardEntryKeyToName(const FFlowBlackboardEntry& FlowBlackboardEntry);
};