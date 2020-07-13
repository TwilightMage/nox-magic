#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SCR_Screen.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FScreenCloseEvent, class USCR_Screen*, Sender);

UCLASS(Abstract)
class NOXMAGIC_API USCR_Screen : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FScreenCloseEvent OnClose;

	UFUNCTION(BlueprintCallable)
	void Close();

protected:
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Close")
	void K2_OnClose();

	UFUNCTION(BlueprintImplementableEvent)
	class UWidgetAnimation* GetInAnim();

	UFUNCTION(BlueprintImplementableEvent)
	class UWidgetAnimation* GetOutAnim();

	UFUNCTION(BlueprintCallable, Meta = (DeterminesOutputType = Class, DisplayName = "Open Child Screen"))
	USCR_Screen* K2_OpenChildScreen(class UNamedSlot* ParentSlot, TSubclassOf<USCR_Screen> Class);

	template<typename ScreenT>
	ScreenT* OpenChildScreen(class UNamedSlot* ParentSlot, TSubclassOf<ScreenT> Class)
	{
		return Cast<ScreenT>(K2_OpenChildScreen(ParentSlot, Class));
	}

	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	TArray<USCR_Screen*> ChildrenScreens;

	UFUNCTION()
	void ChildScreenClosed(USCR_Screen* Sender);

	void CloseInternal();
};
