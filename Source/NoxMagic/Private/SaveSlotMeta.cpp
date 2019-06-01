#include "SaveSlotMeta.h"
#include "FileHelper.h"
#include <filesystem>

USaveSlotMeta* USaveSlotMeta::Load(FString path)
{
	USaveSlotMeta* result = NewObject<USaveSlotMeta>();

	TArray<uint8> bytes;
	FFileHelper::LoadFileToArray(bytes, *path);

	UNoxMagicFunctions::DeserializeObject(result, bytes);

	return result;
}

void USaveSlotMeta::Save(FString path, FDateTime inTime, UTexture2D* inThumb)
{
	USaveSlotMeta* object = NewObject<USaveSlotMeta>();
	object->name = FString(std::experimental::filesystem::path(*path).stem().c_str());
	object->time = inTime;
	object->thumb = inThumb;

	TArray<uint8> bytes = UNoxMagicFunctions::SerializeObject(object);
	FFileHelper::SaveArrayToFile(bytes, *path);
}

FSaveMeta USaveSlotMeta::GetStruct()
{
	return FSaveMeta(name, time, thumb);
}
