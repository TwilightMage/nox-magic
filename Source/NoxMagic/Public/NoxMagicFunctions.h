

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NoxMagicFunctions.generated.h"

/**
 * 
 */
UCLASS()
class NOXMAGIC_API UNoxMagicFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "NoxMagic")
		static TArray<FString> StringToArray(FString input);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic")
		static void ReplaceInString(UPARAM(ref) FString& target, FString from, FString to);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic")
		static void RemoveFromString(UPARAM(ref) FString& target, FString remove);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic")
		static void TryParseInt(FString input, bool& success, int& result);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic")
		static void TryParseFloat(FString input, bool& success, float& result);

	UFUNCTION(Exec)
		static void SystemCall(FString args);

	UFUNCTION(BlueprintPure, Category = "NoxMagic", Exec)
		static FString UID();

	static TArray<uint8> SerializeObject(UObject* target);

	static void DeserializeObject(UObject* target, TArray<uint8> data);

	UFUNCTION(BlueprintPure, Category = "NoxMagic")
		static float RoundWithZeroes(float input, int zeroCount);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic")
		static void MakeDirty(UObject* target);
};
