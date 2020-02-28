#include "NoxMagicFunctions.h"
#include <regex>
#include <string>
#include "MemoryWriter.h"
#include "MemoryReader.h"
#include "ObjectAndNameAsStringProxyArchive.h"

#if PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN        
#include <windows.h>      
#include <intrin.h>       
#include <iphlpapi.h>   
#include <string>

typedef unsigned short u16;
typedef unsigned int u32;

using namespace std;

// we just need this for purposes of unique machine id. So any one or two mac's is       
// fine. 
u16 hashMacAddress(PIP_ADAPTER_INFO info)
{
	u16 hash = 0;
	for (u32 i = 0; i < info->AddressLength; i++)
	{
		hash += (info->Address[i] << ((i & 1) * 8));
	}
	return hash;
}

void getMacHash(u16& mac1, u16& mac2)
{
	IP_ADAPTER_INFO AdapterInfo[32];
	DWORD dwBufLen = sizeof(AdapterInfo);

	DWORD dwStatus = GetAdaptersInfo(AdapterInfo, &dwBufLen);
	if (dwStatus != ERROR_SUCCESS)
		return; // no adapters.      

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
	mac1 = hashMacAddress(pAdapterInfo);
	if (pAdapterInfo->Next)
		mac2 = hashMacAddress(pAdapterInfo->Next);

	// sort the mac addresses. We don't want to invalidate     
	// both macs if they just change order.           
	if (mac1 > mac2)
	{
		u16 tmp = mac2;
		mac2 = mac1;
		mac1 = tmp;
	}
}

u16 getVolumeHash()
{
	DWORD serialNum = 0;

	// Determine if this volume uses an NTFS file system.      
	GetVolumeInformation(L"c:\\", NULL, 0, &serialNum, NULL, NULL, NULL, 0);
	u16 hash = (u16)((serialNum + (serialNum >> 16)) & 0xFFFF);

	return hash;
}

u16 getCpuHash()
{
	int cpuinfo[4] = { 0, 0, 0, 0 };
	__cpuid(cpuinfo, 0);
	u16 hash = 0;
	u16* ptr = (u16*)(&cpuinfo[0]);
	for (u32 i = 0; i < 8; i++)
		hash += ptr[i];

	return hash;
}

const string getMachineName()
{
	char computerName[1024];
	DWORD size = 1024;
	GetComputerName((LPWSTR)&computerName, &size);
	return string(computerName);
}

#endif

#if PLATFORM_LINUX || PLATFORM_MAC

#include <stdio.h>
#include <string.h>
#include <unistd.h>          
#include <errno.h>           
#include <sys/types.h>       
#include <sys/socket.h>      
#include <sys/ioctl.h>  
#include <sys/resource.h>    
#include <sys/utsname.h>       
#include <netdb.h>           
#include <netinet/in.h>      
#include <netinet/in_systm.h>                 
#include <netinet/ip.h>      
#include <netinet/ip_icmp.h> 
#include <assert.h>

#ifdef DARWIN                    
#include <net/if_dl.h>       
#include <ifaddrs.h>         
#include <net/if_types.h>    
#else //!DARWIN              
// #include <linux/if.h>        
// #include <linux/sockios.h>   
#endif //!DARWIN               

const char* getMachineName()
{
	static struct utsname u;

	if (uname(&u) < 0)
	{
		assert(0);
		return "unknown";
	}

	return u.nodename;
}


//---------------------------------get MAC addresses ------------------------------------unsigned short-unsigned short----------        
// we just need this for purposes of unique machine id. So any one or two mac's is fine.            
unsigned short hashMacAddress(unsigned char* mac)
{
	unsigned short hash = 0;

	for (unsigned int i = 0; i < 6; i++)
	{
		hash += (mac[i] << ((i & 1) * 8));
	}
	return hash;
}

void getMacHash(unsigned short& mac1, unsigned short& mac2)
{
	mac1 = 0;
	mac2 = 0;

#ifdef DARWIN                

	struct ifaddrs* ifaphead;
	if (getifaddrs(&ifaphead) != 0)
		return;

	// iterate over the net interfaces         
	bool foundMac1 = false;
	struct ifaddrs* ifap;
	for (ifap = ifaphead; ifap; ifap = ifap->ifa_next)
	{
		struct sockaddr_dl* sdl = (struct sockaddr_dl*)ifap->ifa_addr;
		if (sdl && (sdl->sdl_family == AF_LINK) && (sdl->sdl_type == IFT_ETHER))
		{
			if (!foundMac1)
			{
				foundMac1 = true;
				mac1 = hashMacAddress((unsigned char*)(LLADDR(sdl))); //sdl->sdl_data) + sdl->sdl_nlen) );       
			}
			else {
				mac2 = hashMacAddress((unsigned char*)(LLADDR(sdl))); //sdl->sdl_data) + sdl->sdl_nlen) );       
				break;
			}
		}
	}

	freeifaddrs(ifaphead);

#else // !DARWIN             

	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (sock < 0) return;

	// enumerate all IP addresses of the system         
	struct ifconf conf;
	char ifconfbuf[128 * sizeof(struct ifreq)];
	memset(ifconfbuf, 0, sizeof(ifconfbuf));
	conf.ifc_buf = ifconfbuf;
	conf.ifc_len = sizeof(ifconfbuf);
	if (ioctl(sock, SIOCGIFCONF, &conf))
	{
		assert(0);
		return;
	}

	// get MAC address        
	bool foundMac1 = false;
	struct ifreq* ifr;
	for (ifr = conf.ifc_req; (char*)ifr < (char*)conf.ifc_req + conf.ifc_len; ifr++)
	{
		if (ifr->ifr_addr.sa_data == (ifr + 1)->ifr_addr.sa_data)
			continue;  // duplicate, skip it     

		if (ioctl(sock, SIOCGIFFLAGS, ifr))
			continue;  // failed to get flags, skip it    
		if (ioctl(sock, SIOCGIFHWADDR, ifr) == 0)
		{
			if (!foundMac1)
			{
				foundMac1 = true;
				mac1 = hashMacAddress((unsigned char*)&(ifr->ifr_addr.sa_data));
			}
			else {
				mac2 = hashMacAddress((unsigned char*)&(ifr->ifr_addr.sa_data));
				break;
			}
		}
	}

	close(sock);

#endif // !DARWIN            

	// sort the mac addresses. We don't want to invalidate                
	// both macs if they just change order.    
	if (mac1 > mac2)
	{
		unsigned short tmp = mac2;
		mac2 = mac1;
		mac1 = tmp;
	}
}

unsigned short getVolumeHash()
{
	// we don't have a 'volume serial number' like on windows. Lets hash the system name instead.    
	unsigned char* sysname = (unsigned char*)getMachineName();
	unsigned short hash = 0;

	for (unsigned int i = 0; sysname[i]; i++)
		hash += (sysname[i] << ((i & 1) * 8));

	return hash;
}

#ifdef DARWIN                
#include <mach-o/arch.h>    
unsigned short getCpuHash()
{
	const NXArchInfo* info = NXGetLocalArchInfo();
	unsigned short val = 0;
	val += (unsigned short)info->cputype;
	val += (unsigned short)info->cpusubtype;
	return val;
}

#else // !DARWIN             

static void getCpuid(unsigned int* p, unsigned int ax)
{
	__asm __volatile
	("movl %%ebx, %%esi\n\t"
		"cpuid\n\t"
		"xchgl %%ebx, %%esi"
		: "=a" (p[0]), "=S" (p[1]),
		"=c" (p[2]), "=d" (p[3])
		: "0" (ax)
	);
}

unsigned short getCpuHash()
{
	unsigned int cpuinfo[4] = { 0, 0, 0, 0 };
	getCpuid(cpuinfo, 0);
	unsigned short hash = 0;
	unsigned int* ptr = (&cpuinfo[0]);
	for (unsigned int i = 0; i < 4; i++)
		hash += (ptr[i] & 0xFFFF) + (ptr[i] >> 16);

	return hash;
}
#endif // !DARWIN            

int main()
{

	printf("Machine: %s\n", getMachineName());
	printf("CPU: %d\n", getCpuHash());
	printf("Volume: %d\n", getVolumeHash());
	return 0;
}

#endif

using namespace std;

#define normalstring(str)					(string(TCHAR_TO_UTF8(*str)))
#define truncnum(num, count)				(FString::FromInt(num).LeftPad(count).Replace(TEXT(" "), TEXT("0")))

#undef UpdateResource

TArray<FString> UNoxMagicFunctions::StringToArray(FString input)
{
	TArray<FString> result = TArray<FString>();
	FString str = FString();

	for (int i = 0; i < input.Len(); i++)
	{
		if (input[i] == ' ' || input[i] == '\r' || input[i] == '\n')
		{
			if (str != "") result.Add(FString(str));
			str = "";
		}
		else str += input[i];
		if (i == input.Len() - 1) result.Add(FString(str));
	}

	return result;
}

void UNoxMagicFunctions::ReplaceInString(UPARAM(ref) FString& target, FString from, FString to)
{
	TArray<int> points;
	FString result;
	FString debug;

	int ptr = 0;
	int a = 0;
	for (int i = 0; i < target.Len(); i++)
	{
		if (target[i] == from[ptr])
		{
			ptr++;
			if (ptr == 1) a = i;
			if (ptr == from.Len())
			{
				points.Add(a);
				ptr = 0;
			}
		}
		else
		{
			ptr = 0;
		}
	}

	ptr = 0;
	for (int i = 0; i < target.Len(); i++)
	{
		if (ptr >= points.Num() || i < points[ptr]) result += target[i];
		else
		{
			ptr++;
			result += to;
			i += from.Len();
		}
	}

	target = result;
}

void UNoxMagicFunctions::RemoveFromString(UPARAM(ref) FString& target, FString remove)
{
	ReplaceInString(target, remove, "");
}

void UNoxMagicFunctions::TryParseInt(FString input, bool& success, int& result)
{
	try
	{
		result = stoi(normalstring(input));
		success = true;
	}
	catch (...)
	{
		success = false;
	}
}

void UNoxMagicFunctions::TryParseFloat(FString input, bool& success, float& result)
{
	try
	{
		result = stof(normalstring(input));
		success = true;
	}
	catch (...)
	{
		success = false;
	}
}

void UNoxMagicFunctions::SystemCall(FString args)
{
	
}

FString UNoxMagicFunctions::UID()
{
	u16 mac1;
	u16 mac2;

	getMacHash(mac1, mac2);

	return FString(string(to_string(mac1) + ":" + to_string(mac2) + ":" + to_string(getVolumeHash()) + ":" + to_string(getCpuHash()) + ":" + getMachineName()).c_str());
}

TArray<uint8> UNoxMagicFunctions::SerializeObject(UObject* target)
{
	TArray<uint8> bytes;
	FMemoryWriter memoryWriter(bytes, true);
	FObjectAndNameAsStringProxyArchive ar(memoryWriter, false);
	target->Serialize(ar);

	return bytes;
}

UObject* UNoxMagicFunctions::DeserializeObject(TSubclassOf<UObject> resultClass, TArray<uint8> bytes)
{
	UObject* result = NewObject<UObject>(GetTransientPackage(), resultClass);
	FMemoryReader memoryReader(bytes, true);
	FObjectAndNameAsStringProxyArchive ar(memoryReader, false);
	result->Serialize(ar);
	return result;
}

float UNoxMagicFunctions::RoundWithZeroes(float input, int zeroCount)
{
	float decimator = pow(10, zeroCount);
	return roundf(input * decimator) / decimator;
}

void UNoxMagicFunctions::MakeDirty(UObject* target)
{
	target->MarkPackageDirty();
}

FString UNoxMagicFunctions::FormatDatetime(FDateTime datetime)
{
	FDateTime now = FDateTime::Now();

	if (now.GetYear() != datetime.GetYear())
	{
		return truncnum(datetime.GetDay(), 2) + "." + truncnum(datetime.GetMonth(), 2) + "." + truncnum(datetime.GetYear(), 4) + " " + truncnum(datetime.GetHour(), 2) + ":" + truncnum(datetime.GetMinute(), 2) + ":" + truncnum(datetime.GetSecond(), 2);
	}
	else if (now.GetDayOfYear() != datetime.GetDayOfYear())
	{
		return truncnum(datetime.GetDay(), 2) + "." + truncnum(datetime.GetMonth(), 2) + " " + truncnum(datetime.GetHour(), 2) + ":" + truncnum(datetime.GetMinute(), 2) + ":" + truncnum(datetime.GetSecond(), 2);
	}
	else
	{
		return truncnum(datetime.GetHour(), 2) + ":" + truncnum(datetime.GetMinute(), 2) + ":" + truncnum(datetime.GetSecond(), 2);
	}
}

void UNoxMagicFunctions::TextureFromRawData(FTextureRaw rawData, UTexture2D*& texture)
{
	texture = UTexture2D::CreateTransient(rawData.width, rawData.height);

	// setup parameters
	texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
#if WITH_EDITOR
	texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
	texture->SRGB = false;
	texture->UpdateResource();

	// write data
	FColor* data = (FColor*)texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(data, rawData.pixels.GetData(), sizeof(FColor) * rawData.width * rawData.height);
	
	texture->PlatformData->Mips[0].BulkData.Unlock();
	texture->UpdateResource();
}

void UNoxMagicFunctions::RawDataFromTexture(UTexture2D* texture, FTextureRaw& rawData)
{
	// setup required parameters
	TextureCompressionSettings OldCompressionSettings = texture->CompressionSettings;
#if WITH_EDITOR
	TextureMipGenSettings OldMipGenSettings = texture->MipGenSettings;
#endif
	bool OldSRGB = texture->SRGB;

	texture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
#if WITH_EDITOR
	texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
	texture->SRGB = false;
	texture->UpdateResource();

	// read data
	rawData.width = texture->Resource->GetSizeX();
	rawData.height = texture->Resource->GetSizeY();
	rawData.pixels.Init(FColor::White, rawData.width * rawData.height);

	FColor* data = (FColor*)texture->PlatformData->Mips[0].BulkData.LockReadOnly();
	FMemory::Memcpy(rawData.pixels.GetData(), data, sizeof(FColor) * rawData.width * rawData.height);

	texture->PlatformData->Mips[0].BulkData.Unlock();

	// return old parameters
	texture->CompressionSettings = OldCompressionSettings;
#if WITH_EDITOR
	texture->MipGenSettings = OldMipGenSettings;
#endif
	texture->SRGB = OldSRGB;
	texture->UpdateResource();
}

void UNoxMagicFunctions::RawDataFromRenderTarget(UTextureRenderTarget2D* texture, FTextureRaw& rawData)
{
	rawData.width = texture->Resource->GetSizeX();
	rawData.height = texture->Resource->GetSizeY();
	texture->GameThread_GetRenderTargetResource()->ReadPixels(rawData.pixels);

	for (int i = 0; i < rawData.pixels.Num(); i++)
	{
		rawData.pixels[i].A = 255;
	}
}
