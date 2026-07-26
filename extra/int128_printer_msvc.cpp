#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


#include <boost/int128.hpp>


#define my_min(a, b) ((b)<(a)?(b):(a))


#define arch_k_arm64ec 1001
#define arch_k_arm64   1002
#define arch_k_arm32   1003
#define arch_k_mips    1004
#define arch_k_ppc     1005
#define arch_k_alpha   1006
#define arch_k_ia64    1007
#define arch_k_x8664   1008
#define arch_k_x8632   1009

#if 0
#elif defined _MSC_VER && defined _M_X64 && defined _M_AMD64 && defined _M_ARM64EC
#define arch_v arch_k_arm64ec
#elif defined _MSC_VER && defined _M_ARM64
#define arch_v arch_k_arm64
#elif defined _MSC_VER && defined _M_ARM
#define arch_v arch_k_arm32
#elif defined _MSC_VER && defined _M_MRX000
#define arch_v arch_k_mips
#elif defined _MSC_VER && (defined _M_PPC || defined _M_MPPC)
#define arch_v arch_k_ppc
#elif defined _MSC_VER && defined _M_ALPHA
#define arch_v arch_k_alpha
#elif defined _MSC_VER && defined _M_IA64
#define arch_v arch_k_ia64
#elif defined _MSC_VER && defined _M_X64 && defined _M_AMD64
#define arch_v arch_k_x8664
#elif defined _MSC_VER && defined _M_IX86 && !defined _M_I86 && !defined M_I86
#define arch_v arch_k_x8632
#elif defined _MSC_VER && (defined _M_I86 || defined M_I86)
#define arch_v arch_k_x8616
#else
#error unknown arch
#endif

#define arch_is_arm64ec (arch_v == arch_k_arm64ec)
#define arch_is_arm64   (arch_v == arch_k_arm64  )
#define arch_is_arm32   (arch_v == arch_k_arm32  )
#define arch_is_mips    (arch_v == arch_k_mips   )
#define arch_is_ppc     (arch_v == arch_k_ppc    )
#define arch_is_alpha   (arch_v == arch_k_alpha  )
#define arch_is_ia64    (arch_v == arch_k_ia64   )
#define arch_is_x8664   (arch_v == arch_k_x8664  )
#define arch_is_x8632   (arch_v == arch_k_x8632  )


struct DEBUGHELPER_s;
typedef struct DEBUGHELPER_s DEBUGHELPER_t;

struct DEBUGHELPER_s
{
	DWORD m_version;
	HRESULT(WINAPI*m_pfn_ReadDebuggeeMemory)(DEBUGHELPER_t* const self, DWORD const addr, DWORD const want, void* const where, DWORD* const got);
	DWORDLONG(WINAPI*m_pfn_GetRealAddress)(DEBUGHELPER_t* const self);
	HRESULT(WINAPI*m_pfn_ReadDebuggeeMemoryEx)(DEBUGHELPER_t* const self, DWORDLONG const addr, DWORD const want, void* const where, DWORD* const got);
	int(WINAPI*m_pfn_GetProcessorType)(DEBUGHELPER_t* const self);
};
typedef struct DEBUGHELPER_s DEBUGHELPER_t;

typedef HRESULT(WINAPI*customviewer_t)(DWORD const address, DEBUGHELPER_t* const helper, int const base, BOOL const unicode, char* const result, size_t const maximum, DWORD const reserved);


template<typename t>
static inline HRESULT formatter_128_dec(DWORD const address, DEBUGHELPER_t* const helper, int const base, BOOL const unicode, char* const result, size_t const maximum, DWORD const reserved)
{
	DWORDLONG addr2;
	HRESULT hr;
	t integer;
	DWORD got;
	char* chr;
	char txt_n[boost::int128::detail::mini_to_chars_buffer_size];
	size_t len;
	size_t n;
	size_t i;

	assert(helper);
	assert(result || maximum == 0);

	#if arch_is_x8664
	assert(helper->m_version >= 0x00020000);
	assert(helper->m_pfn_GetRealAddress);
	assert(helper->m_pfn_ReadDebuggeeMemoryEx);
	addr2 = helper->m_pfn_GetRealAddress(helper);
	hr = helper->m_pfn_ReadDebuggeeMemoryEx(helper, addr2, sizeof(integer), &integer, &got);
	#elif arch_is_x8632
	assert(helper->m_pfn_ReadDebuggeeMemory);
	hr = helper->m_pfn_ReadDebuggeeMemory(helper, address, sizeof(integer), &integer, &got);
	#else
	#error unsupported arch
	#endif
	if(hr != S_OK)
	{
		return hr;
	}
	if(got != sizeof(integer))
	{
		return E_FAIL;
	}
	chr = boost::int128::detail::mini_to_chars(txt_n, integer, 10, false);
	if(!(chr >= &txt_n[0] && chr < &txt_n[0] + boost::int128::detail::mini_to_chars_buffer_size))
	{
		return E_FAIL;
	}
	if(!(txt_n[boost::int128::detail::mini_to_chars_buffer_size - 1] == '\0'))
	{
		return E_FAIL;
	}
	len = ((size_t)((&txt_n[0] + boost::int128::detail::mini_to_chars_buffer_size) - chr));
	if(unicode)
	{
		n = my_min(len, maximum);
		for(i = 0; i != n; ++i)
		{
			((wchar_t*)(result))[i] = ((wchar_t)(chr[i]));
		}
	}
	else
	{
		n = my_min(len, maximum);
		for(i = 0; i != n; ++i)
		{
			result[i] = chr[i];
		}
	}
	return S_OK;
}


extern "C" __declspec(dllexport) HRESULT __stdcall formatter_u128_dec(DWORD const address, DEBUGHELPER_t* const helper, int const base, BOOL const unicode, char* const result, size_t const maximum, DWORD const reserved);
__declspec(dllexport) HRESULT __stdcall formatter_u128_dec(DWORD const address, DEBUGHELPER_t* const helper, int const base, BOOL const unicode, char* const result, size_t const maximum, DWORD const reserved)
{
	return formatter_128_dec<boost::int128::uint128_t>(address, helper, base, unicode, result, maximum, reserved);
}

extern "C" __declspec(dllexport) HRESULT __stdcall formatter_s128_dec(DWORD const address, DEBUGHELPER_t* const helper, int const base, BOOL const unicode, char* const result, size_t const maximum, DWORD const reserved);
__declspec(dllexport) HRESULT __stdcall formatter_s128_dec(DWORD const address, DEBUGHELPER_t* const helper, int const base, BOOL const unicode, char* const result, size_t const maximum, DWORD const reserved)
{
	return formatter_128_dec<boost::int128::int128_t>(address, helper, base, unicode, result, maximum, reserved);
}

BOOL APIENTRY DllMain(HMODULE const hmodule, DWORD  const reason, LPVOID const reserved)
{
	switch(reason)
	{
		case DLL_PROCESS_ATTACH: break;
		case DLL_THREAD_ATTACH : break;
		case DLL_THREAD_DETACH : break;
		case DLL_PROCESS_DETACH: break;
		default                : break;
	}
	return TRUE;
}
