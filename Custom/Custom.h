#pragma once
#include <xlocbuf>
#include <codecvt>
bool InScreen(SDK::FVector2D ScreenLocation)
{
	if (ScreenLocation.X < 5.0 || ScreenLocation.X > 1920 - (5.0 * 2) && ScreenLocation.Y < 5.0 || ScreenLocation.Y > 1080 - (5.0 * 2))
		return false;
	return true;
}

bool InRect(double Radius, SDK::FVector2D ScreenLocation)
{
	return 1920/2 >= (1920 / 2 - Radius) && 1920/2 <= (1920/2 + Radius) &&
		ScreenLocation.Y >= (ScreenLocation.Y - Radius) && ScreenLocation.Y <= (ScreenLocation.Y + Radius);
}

bool InCircle(double Radius, SDK::FVector2D ScreenLocation)
{
	if (InRect(Radius, ScreenLocation))
	{
		double dx = 1920/2 - ScreenLocation.X; dx *= dx;
		double dy = 1080/2 - ScreenLocation.Y; dy *= dy;
		return dx + dy <= Radius * Radius;
	} return false;
}


void* PatternScan(const char* szModule, const char* szSignature) {
    static auto PatternToByte = [](const char* pattern) {
        auto arrBytes = std::vector<int>{};
        auto pStart = const_cast<char*>(pattern);
        auto pEnd = const_cast<char*>(pattern) + strlen(pattern);

        for (char* pCurrent = pStart; pCurrent < pEnd; ++pCurrent) {
            if (*pCurrent == '?') {
                ++pCurrent;
                if (*pCurrent == '?')
                    ++pCurrent;
                arrBytes.push_back(-1);
            }
            else {
                arrBytes.push_back(strtoul(pCurrent, &pCurrent, 16));
            }
        }
        return arrBytes;
        };

    void* pModule = (void*)GetModuleHandleA(szModule);
    PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)pModule;
    PIMAGE_NT_HEADERS NTHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)pModule + DosHeader->e_lfanew);

    DWORD uSizeOfImage = NTHeaders->OptionalHeader.SizeOfImage;
    std::vector<int> arrPatternBytes = PatternToByte(szSignature);
    std::uint8_t* pScanBytes = reinterpret_cast<std::uint8_t*>(pModule);

    size_t uPatternBytesSize = arrPatternBytes.size();
    int* pPatternBytesData = arrPatternBytes.data();

    for (auto i = 0ul; i < uSizeOfImage - uPatternBytesSize; ++i) {
        bool bFound = true;
        for (auto j = 0ul; j < uPatternBytesSize; ++j) {
            if (pScanBytes[i + j] != pPatternBytesData[j] && pPatternBytesData[j] != -1) {
                bFound = false;
                break;
            }
        }
        if (bFound) {
            return &pScanBytes[i];
        }
    }
    return nullptr;
}