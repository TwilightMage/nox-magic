#pragma once

#include "CoreMinimal.h"
#include "SCR_Screen.h"
#include "SCR_Settings.generated.h"

UENUM(BlueprintType)
enum class ESettingsScreen : uint8
{
	Gameplay,
	Sound,
	Graphics,
	Video,

	MAX		UMETA(Hidden)
};

UCLASS(Abstract)
class NOXMAGIC_API USCR_Settings : public USCR_Screen
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ShowScreen(ESettingsScreen NewScreen);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	UNamedSlot* GetScreenSlot();

	virtual void NativeConstruct() override;

private:
	UPROPERTY(BlueprintReadOnly, Meta = (AllowPrivateAccess = True))
	ESettingsScreen CurrentScreen;

	UPROPERTY()
	class USCR_SettingsScreen* CurrentScreenObject;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Meta = (AllowPrivateAccess = True))
	TMap<ESettingsScreen, TSubclassOf<class USCR_SettingsScreen>> ScreenClassMap;
};
