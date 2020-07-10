#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemWidget.generated.h"

UCLASS()
class NOXMAGIC_API UItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void DisplayItem(class UInventoryItem* Item);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetIcon(class UTexture2D* Icon);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCount(int count);

protected:
	virtual void NativePreConstruct() override;
};
