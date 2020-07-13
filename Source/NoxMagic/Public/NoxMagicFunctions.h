#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "NoxMagicFunctions.generated.h"

#define nameof(exp)		(#exp)

USTRUCT(BlueprintType)
struct FTextureRaw
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int width;

	UPROPERTY(BlueprintReadWrite)
	int height;

	UPROPERTY(BlueprintReadWrite)
	TArray<FColor> pixels;

	FTextureRaw()
	{
	width = 0;
	height = 0;
	pixels = TArray<FColor>();
	}
};

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

	UFUNCTION(BlueprintPure, Category = "NoxMagic")
	static FString UID();
	
	UFUNCTION(BlueprintCallable, Category = "NoxMagic")
	static TArray<uint8> SerializeObject(UObject* target);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic", Meta = (DeterminesOutputType = "resultClass"))
	static UObject* DeserializeObject(TSubclassOf<UObject> resultClass, TArray<uint8> data);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic")
	static class ANMCharacter* GetMainPlayer();

	UFUNCTION(BlueprintCallable, Category = "NoxMagic")
	static class ANMCharacter* GetPlayerByName(FString Name);

	template<typename T>
	static T* DeserializeObject(TArray<uint8> data)
	{
		return Cast<T>(DeserializeObject(T::StaticClass(), data));
	}

	UFUNCTION(BlueprintPure, Category = "NoxMagic")
	static float RoundWithZeroes(float input, int zeroCount);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic")
	static void MakeDirty(UObject* target);

	UFUNCTION(BlueprintPure, Category = "NoxMagic")
	static FString FormatDatetime(FDateTime datetime);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic")
	static void TextureFromRawData(FTextureRaw rawData, UTexture2D*& texture);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic")
	static void RawDataFromTexture(UTexture2D* texture, FTextureRaw& rawData);

	UFUNCTION(BlueprintCallable, Category = "NoxMagic")
	static void RawDataFromRenderTarget(UTextureRenderTarget2D* texture, FTextureRaw& rawData);

	template<typename T>
	static FString EnumToString(const FString& EnumName, T value, const FString& DefaultValue = "undefined")
	{
		if (UEnum* Enum = FindObject<UEnum>(ANY_PACKAGE, *EnumName, true))
		{
			FString Str = Enum->GetNameByIndex(static_cast<uint8>(value)).ToString();
			int Pos;
			Str.FindLastChar(':', Pos);
			return Str.Mid(Pos + 1);
		}

		return DefaultValue;
	}
};
