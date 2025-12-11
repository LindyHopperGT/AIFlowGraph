// Copyright https://github.com/MothCocoon/FlowGraph/graphs/contributors

#pragma once

#include "FlowBlackboardEntryValue.h"

#include "FlowBlackboardEntryValue_Object.generated.h"

// Selector enum to control the EditCondition for ObjectInstance/ObjectAsset switching 
UENUM()
enum class EObjectInstanceTypeSelector : uint8
{
	Unknown,
	Instanced,
	Asset,
};

/**
 * Configuration object for setting blackboard entries for UBlackboardKeyType_Object entries
 */
UCLASS(BlueprintType, DisplayName = "Object Blackboard Value")
class UFlowBlackboardEntryValue_Object : public UFlowBlackboardEntryValue
{
	GENERATED_BODY()

public:

	//~Begin UFlowBlackboardEntryValue
	virtual void SetOnBlackboardComponent(UBlackboardComponent* BlackboardComponent) const override;
	virtual EBlackboardCompare::Type CompareKeyValues(const UBlackboardComponent* BlackboardComponent, const FName& OtherKeyName) const override;
	virtual TSubclassOf<UBlackboardKeyType> GetSupportedBlackboardKeyType() const override;
	virtual bool TrySetValueFromInputDataPin(const FName& PinName, UFlowNode& PinOwnerFlowNode) override;
#if WITH_EDITOR
	virtual bool TryReconfigureFromBlackboardKeyType(const UBlackboardKeyType& KeyType) override;
	virtual FString GetEditorValueString() const override;
#endif // WITH_EDITOR
	//~End UFlowBlackboardEntryValue
	
	// IFlowDataPinPropertyProviderInterface
	virtual bool TryProvideFlowDataPinProperty(TInstancedStruct<FFlowDataPinValue>& OutFlowDataPinProperty) const override;
	// --

	virtual bool TryProvideFlowDataPinPropertyFromBlackboardEntry(
		const FName& BlackboardKeyName,
		const UBlackboardKeyType& BlackboardKeyType,
		UBlackboardComponent* OptionalBlackboardComponent,
		TInstancedStruct<FFlowDataPinValue>& OutFlowDataPinProperty) const override;

	UObject* GetObjectValue() const;
	void SetObjectValue(UObject* InValue);

#if WITH_EDITOR
public:
	//~Begin UFlowNodeBase
	virtual FText BuildNodeConfigText() const override;
	//~End UFlowNodeBase

	//~Begin UObject
	virtual void PostInitProperties() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~End UObject

protected:
	void EnsureObjectsAreCompatibleWithBaseClass();
	void RefreshObjectTypeFromBaseClass();
#endif // WITH_EDITOR

protected:
#if WITH_EDITORONLY_DATA
	// The BaseClass for the Object to set
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Configuration, meta = (AllowAbstract = "true"))
	TObjectPtr<UClass> BaseClass = nullptr;
#endif // WITH_EDITORONLY_DATA

	// If an inline Instanced object, use this property to configure it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = Configuration, meta = (DisplayAfter = Key, EditCondition = "ObjectTypeSelector == EObjectInstanceTypeSelector::Instanced"))
	TObjectPtr<UObject> ObjectInstance = nullptr;

	// If the object is to be created from an asset, use this property to configure it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Configuration, meta = (DisplayAfter = ObjectInstance, EditCondition = "ObjectTypeSelector == EObjectInstanceTypeSelector::Asset"))
	TObjectPtr<UObject> ObjectAsset = nullptr;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	EObjectInstanceTypeSelector ObjectTypeSelector = EObjectInstanceTypeSelector::Unknown;
#endif // WITH_EDITORONLY_DATA
};
