#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SCR_Screen.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FScreenCloseEvent);

UCLASS()
class NOXMAGIC_API USCR_Screen : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FScreenCloseEvent ScreenCloseEvent;

protected:
	UFUNCTION(BlueprintCallable)
	void Close();

	UFUNCTION(BlueprintImplementableEvent)
	void OnClose();
};
