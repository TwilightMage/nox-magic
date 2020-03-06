#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.h"
#include "Inventory.generated.h"

class ADrop;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NOXMAGIC_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventory();

	UFUNCTION(BlueprintCallable, Category = NoxMagic, Server, Reliable)
		void PickUpItem(ADrop* drop);

	UFUNCTION(BLueprintCallable, Category = NoxMagic)
		void AddItem(FItemContainer item);

	UPROPERTY(EditAnywhere)
		FLinearColor inventoryColor;

	UPROPERTY(EditAnywhere)
		TArray<FItemContainer> items;
};
