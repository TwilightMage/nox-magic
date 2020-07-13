#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NMHUD_Play.generated.h"

UENUM(BlueprintType)
enum class EScreenType : uint8
{
	Play,
	Pause,
	Inventory,
	Looting,
	Trade,

	MAX		UMETA(Hidden)
};

USTRUCT()
struct FScreenLayer
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class USCR_Screen> ScreenClass;

	UPROPERTY(EditDefaultsOnly)
	int Layer;
};

UCLASS()
class NOXMAGIC_API ANMHUD_Play : public AHUD
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SwitchPauseScreen(bool NewState);

	UFUNCTION(BlueprintCallable)
	void OpenInventoryScreen(bool ForceRefresh, FText Title, class UInventory* Inventory);

	UFUNCTION(BlueprintCallable)
	void CloseInventoryScreen();

	UFUNCTION(BlueprintCallable)
	void OpenLootingScreen(bool ForceRefresh, FText Title, class UInventory* Inventory, FText AnotherTitle, class UInventory* AnotherInventory);

	UFUNCTION(BlueprintCallable)
	void CloseLootingScreen();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = Classed, Meta = (AllowPrivateAccess = True))
	TMap<EScreenType, FScreenLayer> ScreenDefinitions;

	UPROPERTY()
	TMap<EScreenType, class USCR_Screen*> ScreenInstances;

	UFUNCTION(BlueprintCallable)
	class USCR_Screen* OpenScreen(EScreenType Screen, bool ForceRefresh = false);

	UFUNCTION(BlueprintCallable)
	void CloseScreen(EScreenType Screen);

	UFUNCTION()
	void PauseScreenClosed(class USCR_Screen* Sender);

	UFUNCTION()
	void ScreenClosed(class USCR_Screen* Sender);
};
