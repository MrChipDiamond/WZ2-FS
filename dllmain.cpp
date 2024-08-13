#include "stdafx.h"
#include <cstdint>
//#include <MinHook.h>
//#include <imgui.h>
//#include <imgui_impl_win32.h>
//#include <imgui_impl_dx12.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>
#include "lazyimporter.h"
#include "tahoma.ttf.h"
#include "imgui/Kiero/kiero.h"
#include "imgui_draw.h"
#include "esp.h"
#include "sdk.h"
#include "Menu.h"
#include "offsets.h"
#include "aim.h"
#include "settings.h"
#include "spoofcall.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "classes.h"
#include"unlock.h"
#include "game_hook.h"
#include "xor.hpp"
#include "gamefunctions.h"
#include"timer.h"
#include"logo.h"
#define BigAssSig "48 8d 0d ? ? ? ? e8 ? ? ? ? 48 83 c4 ? c3 cc 48 83 ec ? 41 b1 ? 48 8d 05 ? ? ? ? 45 0f b6 c1 48 89 44 24 ? 48 8d 15 ? ? ? ? 48"


uintptr_t discordpresent;
typedef long(__fastcall* Present)(IDXGISwapChain*, UINT, UINT);
static Present ori_present = NULL;
void WndProc_hk();

std::once_flag g_flag;
typedef LRESULT(CALLBACK* tWndProc)(HWND hWnd, UINT Msg, WPARAM wp, LPARAM lp);
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef LRESULT(CALLBACK* tWndProc)(HWND hWnd, UINT Msg, WPARAM wp, LPARAM lp);
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef enum class _MEMORY_INFORMATION_CLASS { MemoryBasicInformation } MEMORY_INFORMATION_CLASS, * PMEMORY_INFORMATION_CLASS;
typedef NTSTATUS(WINAPI* NtQueryVirtualMemoryFunction) (HANDLE ProcessHandle, PVOID BaseAddress, MEMORY_INFORMATION_CLASS MemoryInformationClass, PVOID MemoryInformation, SIZE_T MemoryInformationLength, PSIZE_T ReturnLength);
typedef SIZE_T(*VirtualQueryFunction) (LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
NtQueryVirtualMemoryFunction oNtQueryVirtualMemory;
NtQueryVirtualMemoryFunction NtQueryVirtualMemory_target = (NtQueryVirtualMemoryFunction)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryVirtualMemory");;
VirtualQueryFunction oVirtualQuery;
VirtualQueryFunction VirtualQuery_target = (VirtualQueryFunction)GetProcAddress(GetModuleHandleA("kernel32.dll"), "VirtualQuery");
typedef void (STDMETHODCALLTYPE* CopyTextureRegion_t) (ID3D12GraphicsCommandList* thisp, const D3D12_TEXTURE_COPY_LOCATION* pDst, UINT  DstX, UINT  DstY, UINT DstZ, const D3D12_TEXTURE_COPY_LOCATION* pSrc, const D3D12_BOX* pSrcBox);
CopyTextureRegion_t oCopyTextureRegion = nullptr;
using tbitblt = bool(WINAPI*)(HDC hdcdst, int x, int y, int cx, int cy, HDC hdcsrc, int x1, int y1, DWORD rop);
tbitblt obitblt = nullptr;
tbitblt bitblttramp = nullptr;
typedef bool(APIENTRY* NtGdiStretchBltHook_t)(HDC hdcDest, int xDest, int yDest, int wDest, int hDest, HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc, DWORD rop, DWORD dwBackColor);
NtGdiStretchBltHook_t NtGdiStretchBlt_original;

PVOID GetMappedLibFunction(const std::string& apiDllName, const std::string& funcName) // "ntdll.dll"
{
	auto dllBase = iat(GetModuleHandleA).get()(apiDllName.c_str());

	if (!dllBase) {
		return nullptr;
	}

	const auto textSectionBase = (HMODULE*)(reinterpret_cast<DWORD64>(dllBase) + 4096);

	MEMORY_BASIC_INFORMATION mbi = {};

	if (!VirtualQuery(textSectionBase, &mbi, sizeof mbi)) {
		return nullptr;
	}

	SIZE_T sizeOfTextSection = mbi.RegionSize;

	//if ("win32u.dll" == apiDllName)
		sizeOfTextSection -= 0x1000;

	if (sizeOfTextSection == 0) {
		return nullptr;
	}


	unsigned char* pAddress = nullptr,
		* pEndRegion = nullptr,
		* pFound;

	while (sizeof mbi == VirtualQuery(pEndRegion, &mbi, sizeof mbi)) {

		pAddress = pEndRegion;
		pEndRegion += mbi.RegionSize;
		if (mbi.Protect & PAGE_EXECUTE_READ &&
			mbi.RegionSize == sizeOfTextSection) {

			pFound = pAddress;
			break;
		}
	}


	const auto hModule = iat(GetModuleHandleA).get()(apiDllName.c_str());
	auto fProcAddress = hModule ? iat(GetProcAddress).get()(hModule, funcName.c_str()) : nullptr;


	if (!fProcAddress) {
		return nullptr;
	}

	return fProcAddress;
	//return (PVOID)((DWORD64)pAddress + ((DWORD64)fProcAddress - (DWORD64)dllBase - 0x1000));
}

void CreateConsole()
{
	if (!AllocConsole()) {
		// Add some error handling here.
		// You can call GetLastError() to get more info about the error.
		return;
	}

	// std::cout, std::clog, std::cerr, std::cin
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	std::cout.clear();
	std::clog.clear();
	std::cerr.clear();
	std::cin.clear();

	// std::wcout, std::wclog, std::wcerr, std::wcin
	HANDLE hConOut = CreateFile(("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hConIn = CreateFile(("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
	SetStdHandle(STD_ERROR_HANDLE, hConOut);
	SetStdHandle(STD_INPUT_HANDLE, hConIn);
	std::wcout.clear();
	std::wclog.clear();
	std::wcerr.clear();
	std::wcin.clear();
}
static uintptr_t __cdecl I_beginthreadex(
	void* _Security,
	unsigned                 _StackSize,
	_beginthreadex_proc_type _StartAddress,
	void* _ArgList,
	unsigned                 _InitFlag,
	unsigned* _ThrdAddr) {

	return iat(_beginthreadex).get()(_Security, _StackSize, _StartAddress, _ArgList, _InitFlag, _ThrdAddr);
}

bool LoadTextureFromFile(const char* filename, ID3D12Device* d3d_device, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load_from_memory((stbi_uc*)&Logo, sizeof(Logo), &image_width, &image_height, 0, 4);// stbi_load(filename, &image_width, &image_height, NULL, 4);

	if (image_data == NULL)
		return false;

	// Create texture resource
	D3D12_HEAP_PROPERTIES props;
	memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
	props.Type = D3D12_HEAP_TYPE_DEFAULT;
	props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = image_width;
	desc.Height = image_height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Resource* pTexture = NULL;
	d3d_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&pTexture));

	// Create a temporary upload resource to move the data in
	UINT uploadPitch = (image_width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
	UINT uploadSize = image_height * uploadPitch;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = uploadSize;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	props.Type = D3D12_HEAP_TYPE_UPLOAD;
	props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	ID3D12Resource* uploadBuffer = NULL;
	HRESULT hr = d3d_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploadBuffer));
	IM_ASSERT(SUCCEEDED(hr));

	// Write pixels into the upload resource
	void* mapped = NULL;
	D3D12_RANGE range = { 0, uploadSize };
	hr = uploadBuffer->Map(0, &range, &mapped);
	IM_ASSERT(SUCCEEDED(hr));
	for (int y = 0; y < image_height; y++)
		memcpy((void*)((uintptr_t)mapped + y * uploadPitch), image_data + y * image_width * 4, image_width * 4);
	uploadBuffer->Unmap(0, &range);

	// Copy the upload resource content into the real resource
	D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
	srcLocation.pResource = uploadBuffer;
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	srcLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srcLocation.PlacedFootprint.Footprint.Width = image_width;
	srcLocation.PlacedFootprint.Footprint.Height = image_height;
	srcLocation.PlacedFootprint.Footprint.Depth = 1;
	srcLocation.PlacedFootprint.Footprint.RowPitch = uploadPitch;

	D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
	dstLocation.pResource = pTexture;
	dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dstLocation.SubresourceIndex = 0;

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = pTexture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	// Create a temporary command queue to do the copy with
	ID3D12Fence* fence = NULL;
	hr = d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	IM_ASSERT(SUCCEEDED(hr));

	HANDLE event = CreateEvent(0, 0, 0, 0);
	IM_ASSERT(event != NULL);

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 1;

	ID3D12CommandQueue* cmdQueue = NULL;
	hr = d3d_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&cmdQueue));
	IM_ASSERT(SUCCEEDED(hr));

	ID3D12CommandAllocator* cmdAlloc = NULL;
	hr = d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc));
	IM_ASSERT(SUCCEEDED(hr));

	ID3D12GraphicsCommandList* cmdList = NULL;
	hr = d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc, NULL, IID_PPV_ARGS(&cmdList));
	IM_ASSERT(SUCCEEDED(hr));

	cmdList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, NULL);
	cmdList->ResourceBarrier(1, &barrier);

	hr = cmdList->Close();
	IM_ASSERT(SUCCEEDED(hr));

	// Execute the copy
	cmdQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&cmdList);
	hr = cmdQueue->Signal(fence, 1);
	IM_ASSERT(SUCCEEDED(hr));

	// Wait for everything to complete
	fence->SetEventOnCompletion(1, event);
	WaitForSingleObject(event, INFINITE);

	// Tear down our temporary command queue and release the upload resource
	cmdList->Release();
	cmdAlloc->Release();
	cmdQueue->Release();
	CloseHandle(event);
	fence->Release();
	uploadBuffer->Release();

	// Create a shader resource view for the texture
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	d3d_device->CreateShaderResourceView(pTexture, &srvDesc, srv_cpu_handle);

	// Return results
	*out_tex_resource = pTexture;
	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);

	return true;
}


STBIDEF stbi_uc* stbi_load_memory(stbi_uc const* buffer, int* x, int* y, int* comp, int req_comp)
{

	auto result = stbi_load_from_memory(buffer,sizeof(buffer), x, y, comp, req_comp);

	if (result != nullptr)
		return result;
	else
		exit( 0);
}


bool LoadTextureFromMemory(stbi_uc const* buffer, ID3D12Device* d3d_device, D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle, ID3D12Resource** out_tex_resource, int* out_width, int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	int n = 0;
	unsigned char* image_data = stbi_load_memory(buffer, &image_width, &image_height, &n, 4);
	if (image_data == NULL)
		return false;

	// Create texture resource
	D3D12_HEAP_PROPERTIES props;
	memset(&props, 0, sizeof(D3D12_HEAP_PROPERTIES));
	props.Type = D3D12_HEAP_TYPE_DEFAULT;
	props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Alignment = 0;
	desc.Width = image_width;
	desc.Height = image_height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	ID3D12Resource* pTexture = NULL;
	d3d_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATE_COPY_DEST, NULL, IID_PPV_ARGS(&pTexture));

	// Create a temporary upload resource to move the data in
	UINT uploadPitch = (image_width * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u) & ~(D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1u);
	UINT uploadSize = image_height * uploadPitch;
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Alignment = 0;
	desc.Width = uploadSize;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	props.Type = D3D12_HEAP_TYPE_UPLOAD;
	props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	ID3D12Resource* uploadBuffer = NULL;
	HRESULT hr = d3d_device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL, IID_PPV_ARGS(&uploadBuffer));
	IM_ASSERT(SUCCEEDED(hr));

	// Write pixels into the upload resource
	void* mapped = NULL;
	D3D12_RANGE range = { 0, uploadSize };
	hr = uploadBuffer->Map(0, &range, &mapped);
	IM_ASSERT(SUCCEEDED(hr));
	for (int y = 0; y < image_height; y++)
		memcpy((void*)((uintptr_t)mapped + y * uploadPitch), image_data + y * image_width * 4, image_width * 4);
	uploadBuffer->Unmap(0, &range);

	// Copy the upload resource content into the real resource
	D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
	srcLocation.pResource = uploadBuffer;
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
	srcLocation.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srcLocation.PlacedFootprint.Footprint.Width = image_width;
	srcLocation.PlacedFootprint.Footprint.Height = image_height;
	srcLocation.PlacedFootprint.Footprint.Depth = 1;
	srcLocation.PlacedFootprint.Footprint.RowPitch = uploadPitch;

	D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
	dstLocation.pResource = pTexture;
	dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	dstLocation.SubresourceIndex = 0;

	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = pTexture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	// Create a temporary command queue to do the copy with
	ID3D12Fence* fence = NULL;
	hr = d3d_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	IM_ASSERT(SUCCEEDED(hr));

	HANDLE event = CreateEvent(0, 0, 0, 0);
	IM_ASSERT(event != NULL);

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.NodeMask = 1;

	ID3D12CommandQueue* cmdQueue = NULL;
	hr = d3d_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&cmdQueue));
	IM_ASSERT(SUCCEEDED(hr));

	ID3D12CommandAllocator* cmdAlloc = NULL;
	hr = d3d_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc));
	IM_ASSERT(SUCCEEDED(hr));

	ID3D12GraphicsCommandList* cmdList = NULL;
	hr = d3d_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc, NULL, IID_PPV_ARGS(&cmdList));
	IM_ASSERT(SUCCEEDED(hr));

	cmdList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, NULL);
	cmdList->ResourceBarrier(1, &barrier);

	hr = cmdList->Close();
	IM_ASSERT(SUCCEEDED(hr));

	// Execute the copy
	cmdQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&cmdList);
	hr = cmdQueue->Signal(fence, 1);
	IM_ASSERT(SUCCEEDED(hr));

	// Wait for everything to complete
	fence->SetEventOnCompletion(1, event);
	WaitForSingleObject(event, INFINITE);

	// Tear down our temporary command queue and release the upload resource
	cmdList->Release();
	cmdAlloc->Release();
	cmdQueue->Release();
	CloseHandle(event);
	fence->Release();
	uploadBuffer->Release();

	// Create a shader resource view for the texture
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	d3d_device->CreateShaderResourceView(pTexture, &srvDesc, srv_cpu_handle);

	// Return results
	*out_tex_resource = pTexture;
	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);

	return true;
}

namespace d3d12
{
	
	IDXGISwapChain3* pSwapChain;
	ID3D12Device* pDevice;
	ID3D12CommandQueue* pCommandQueue;
	ID3D12Fence* pFence;
	ID3D12DescriptorHeap* d3d12DescriptorHeapBackBuffers = nullptr;
	ID3D12DescriptorHeap* d3d12DescriptorHeapImGuiRender = nullptr;
	ID3D12DescriptorHeap* pSrvDescHeap = nullptr;;
	ID3D12DescriptorHeap* pRtvDescHeap = nullptr;;
	ID3D12GraphicsCommandList* pCommandList;

	
	FrameContext* FrameContextArray;
	ID3D12Resource** pID3D12ResourceArray;
	D3D12_CPU_DESCRIPTOR_HANDLE* RenderTargetDescriptorArray;

	
	HANDLE hSwapChainWaitableObject;
	HANDLE hFenceEvent;

	
	UINT NUM_FRAMES_IN_FLIGHT;
	UINT NUM_BACK_BUFFERS;

	
	UINT   frame_index = 0;
	UINT64 fenceLastSignaledValue = 0;
}
namespace ogr_function
{
	tWndProc WndProc;
}

namespace imgui
{
	bool is_ready;
	bool is_need_reset_imgui;

	bool IsReady()
	{
		return is_ready;
	}

	void reset_imgui_request()
	{
		is_need_reset_imgui = true;
	}

	__forceinline bool get_is_need_reset_imgui()
	{
		return is_need_reset_imgui;
	}

	void init_d3d12(IDXGISwapChain3* pSwapChain, ID3D12CommandQueue* pCommandQueue)
	{

		d3d12::pSwapChain = pSwapChain;
		d3d12::pCommandQueue = pCommandQueue;

		if (!SUCCEEDED(d3d12::pSwapChain->GetDevice(__uuidof(ID3D12Device), (void**)&d3d12::pDevice)))
			Exit();

		{
			
			DXGI_SWAP_CHAIN_DESC1 desc;

			

			if (!SUCCEEDED(d3d12::pSwapChain->GetDesc1(&desc)))
				Exit();
	
			d3d12::NUM_BACK_BUFFERS = desc.BufferCount;
			d3d12::NUM_FRAMES_IN_FLIGHT = desc.BufferCount;
		
			
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.NumDescriptors = d3d12::NUM_BACK_BUFFERS;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 1;

			if (!SUCCEEDED(d3d12::pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3d12::pRtvDescHeap))))
				Exit();
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.NumDescriptors = 2;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			desc.NodeMask = 0;

			if (!SUCCEEDED(d3d12::pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&d3d12::pSrvDescHeap))))
				Exit();
		}

		if (!SUCCEEDED(d3d12::pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d12::pFence))))
			Exit();

		d3d12::FrameContextArray = new FrameContext[d3d12::NUM_FRAMES_IN_FLIGHT];
		d3d12::pID3D12ResourceArray = new ID3D12Resource * [d3d12::NUM_BACK_BUFFERS];
		d3d12::RenderTargetDescriptorArray = new D3D12_CPU_DESCRIPTOR_HANDLE[d3d12::NUM_BACK_BUFFERS];

		for (UINT i = 0; i < d3d12::NUM_FRAMES_IN_FLIGHT; ++i)
		{
			if (!SUCCEEDED(d3d12::pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&d3d12::FrameContextArray[i].CommandAllocator))))
				Exit();
		}

		SIZE_T nDescriptorSize = d3d12::pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12::pRtvDescHeap->GetCPUDescriptorHandleForHeapStart();

		for (UINT i = 0; i < d3d12::NUM_BACK_BUFFERS; ++i)
		{
			d3d12::RenderTargetDescriptorArray[i] = rtvHandle;
			rtvHandle.ptr += nDescriptorSize;
		}


		if (!SUCCEEDED(d3d12::pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3d12::FrameContextArray[0].CommandAllocator, NULL, IID_PPV_ARGS(&d3d12::pCommandList))) ||
			!SUCCEEDED(d3d12::pCommandList->Close()))
		{
			Exit();
		}


		d3d12::hSwapChainWaitableObject = d3d12::pSwapChain->GetFrameLatencyWaitableObject();

		d3d12::hFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		if (d3d12::hFenceEvent == NULL)
			Exit();


		ID3D12Resource* pBackBuffer;
		for (UINT i = 0; i < d3d12::NUM_BACK_BUFFERS; ++i)
		{
			if (!SUCCEEDED(d3d12::pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer))))
				Exit();

			d3d12::pDevice->CreateRenderTargetView(pBackBuffer, NULL, d3d12::RenderTargetDescriptorArray[i]);
			d3d12::pID3D12ResourceArray[i] = pBackBuffer;
		}
	}

	void _clear()
	{
		d3d12::pSwapChain = nullptr;
		d3d12::pDevice = nullptr;
		d3d12::pCommandQueue = nullptr;

		if (d3d12::pFence)
		{
			d3d12::pFence->Release();
			d3d12::pFence = nullptr;
		}

		if (d3d12::pSrvDescHeap)
		{
			d3d12::pSrvDescHeap->Release();
			d3d12::pSrvDescHeap = nullptr;
		}

		if (d3d12::pRtvDescHeap)
		{
			d3d12::pRtvDescHeap->Release();
			d3d12::pRtvDescHeap = nullptr;
		}

		if (d3d12::pCommandList)
		{
			d3d12::pCommandList->Release();
			d3d12::pCommandList = nullptr;
		}

		if (d3d12::FrameContextArray)
		{
			for (UINT i = 0; i < d3d12::NUM_FRAMES_IN_FLIGHT; ++i)
			{
				if (d3d12::FrameContextArray[i].CommandAllocator)
				{
					d3d12::FrameContextArray[i].CommandAllocator->Release();
					d3d12::FrameContextArray[i].CommandAllocator = nullptr;
				}
			}

			delete[] d3d12::FrameContextArray;
			d3d12::FrameContextArray = NULL;
		}

		if (d3d12::pID3D12ResourceArray)
		{
			for (UINT i = 0; i < d3d12::NUM_BACK_BUFFERS; ++i)
			{
				if (d3d12::pID3D12ResourceArray[i])
				{
					d3d12::pID3D12ResourceArray[i]->Release();
					d3d12::pID3D12ResourceArray[i] = nullptr;
				}
			}

			delete[] d3d12::pID3D12ResourceArray;
			d3d12::pID3D12ResourceArray = NULL;
		}

		if (d3d12::RenderTargetDescriptorArray)
		{
			delete[] d3d12::RenderTargetDescriptorArray;
			d3d12::RenderTargetDescriptorArray = NULL;
		}


		if (d3d12::hSwapChainWaitableObject)
		{
			d3d12::hSwapChainWaitableObject = nullptr;
		}

		if (d3d12::hFenceEvent)
		{
			CloseHandle(d3d12::hFenceEvent);
			d3d12::hFenceEvent = nullptr;
		}


		d3d12::NUM_FRAMES_IN_FLIGHT = 0;
		d3d12::NUM_BACK_BUFFERS = 0;


		d3d12::frame_index = 0;
	}

	void cleard3d12()
	{
		if (d3d12::FrameContextArray)
		{
			FrameContext* frameCtxt = &d3d12::FrameContextArray[d3d12::frame_index % d3d12::NUM_FRAMES_IN_FLIGHT];

			UINT64 fenceValue = frameCtxt->FenceValue;

			if (fenceValue == 0)
				return; // No fence was signaled

			frameCtxt->FenceValue = 0;

			bool bNotWait = d3d12::pFence->GetCompletedValue() >= fenceValue;

			if (!bNotWait)
			{
				d3d12::pFence->SetEventOnCompletion(fenceValue, d3d12::hFenceEvent);

				WaitForSingleObject(d3d12::hFenceEvent, INFINITE);
			}

			_clear();
		}
	}

	FrameContext* WaitForNextFrameResources()
	{
		UINT nextFrameIndex = d3d12::frame_index + 1;
		d3d12::frame_index = nextFrameIndex;

		HANDLE waitableObjects[] = { d3d12::hSwapChainWaitableObject, NULL };
		constexpr DWORD numWaitableObjects = 1;

		FrameContext* frameCtxt = &d3d12::FrameContextArray[nextFrameIndex % d3d12::NUM_FRAMES_IN_FLIGHT];

		WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

		return frameCtxt;
	}

	void reinit(IDXGISwapChain3* pSwapChain, ID3D12CommandQueue* pCommandQueue)
	{
		init_d3d12(pSwapChain, pCommandQueue);
		ImGui_ImplDX12_CreateDeviceObjects();
	}



	ImFont* start(IDXGISwapChain3* pSwapChain, ID3D12CommandQueue* pCommandQueue, type::tImguiStyle SetStyleFunction)
	{
		static ImFont* s_main_font;

		if (is_ready && get_is_need_reset_imgui())
		{

			reinit(pSwapChain, pCommandQueue);


			is_need_reset_imgui = false;
		}

		if (is_ready)
			return s_main_font;

		init_d3d12(pSwapChain, pCommandQueue);

		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		if (SetStyleFunction == nullptr)
			ImGui::StyleColorsDark();
		else
			SetStyleFunction();


		ImGui_ImplWin32_Init(g_data::hWind);
		ImGui_ImplDX12_Init(
			d3d12::pDevice,
			d3d12::NUM_FRAMES_IN_FLIGHT,
			DXGI_FORMAT_R8G8B8A8_UNORM, d3d12::pSrvDescHeap,
			d3d12::pSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
			d3d12::pSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
			
	
		
		init_Font_List();
		ImFont* main_font = io.Fonts->AddFontFromMemoryTTF(tahoma_ttf, sizeof(tahoma_ttf), 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
		
		
		if (main_font == nullptr)
			Exit();

		s_main_font = main_font;

		WndProc_hk();
		
		is_ready = true;

		return s_main_font;
	}

	ImFont* add_font(const char* font_path, float font_size)
	{
		if (!is_ready)
			return nullptr;

		ImGuiIO& io = ImGui::GetIO();
		ImFont* font = io.Fonts->AddFontFromMemoryTTF(tahoma_ttf, sizeof(tahoma_ttf), 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());

		if (font == nullptr)
			return 0;

		return font;
	}

	void imgui_frame_header()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void imgui_no_border(type::tESP esp_function, ImFont* font)
	{


	


			auto& style = ImGui::GetStyle();

			style.WindowMinSize = ImVec2(915, 450);

			style.FrameBorderSize = 0;
			style.WindowRounding = 0;
			style.WindowPadding = ImVec2(0, 0);
			style.TabRounding = 0;
			style.ScrollbarRounding = 0;
			style.FramePadding = ImVec2(8, 6);
			style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
			esp_function(font);
			style.Colors[ImGuiCol_TitleBg] = ImColor(227, 0, 40, 255);
			style.Colors[ImGuiCol_TitleBgActive] = ImColor(227, 0, 40, 255);
			style.Colors[ImGuiCol_TitleBgCollapsed] = ImColor(15, 15, 15, 50);

			style.Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 0);
			style.Colors[ImGuiCol_WindowBg] = ImColor(12, 12, 12, 255);

			style.Colors[ImGuiCol_Button] = ImColor(24, 25, 24, 255);
			style.Colors[ImGuiCol_ButtonActive] = ImColor(44, 44, 44, 255);
			style.Colors[ImGuiCol_ButtonHovered] = ImColor(44, 44, 44, 255);

			style.Colors[ImGuiCol_CheckMark] = ImColor(255, 255, 255, 255);

			style.Colors[ImGuiCol_FrameBg] = ImColor(36, 37, 36, 255);
			style.Colors[ImGuiCol_FrameBgActive] = ImColor(36, 37, 36, 255);
			style.Colors[ImGuiCol_FrameBgHovered] = ImColor(36, 37, 36, 255);

			style.Colors[ImGuiCol_Header] = ImColor(54, 56, 54, 255);			// for collapsing headers , etc
			style.Colors[ImGuiCol_HeaderActive] = ImColor(54, 56, 54, 255);
			style.Colors[ImGuiCol_HeaderHovered] = ImColor(54, 56, 54, 255);

			style.Colors[ImGuiCol_ResizeGrip] = ImColor(51, 49, 50, 255);		    // the resize grip thing bottom right
			style.Colors[ImGuiCol_ResizeGripActive] = ImColor(54, 53, 55);			// when you hold it / active
			style.Colors[ImGuiCol_ResizeGripHovered] = ImColor(51, 49, 50, 255);			// when you hover over it

			style.Colors[ImGuiCol_SliderGrab] = ImColor(55, 55, 55, 255);
			style.Colors[ImGuiCol_SliderGrabActive] = ImColor(55, 55, 55, 255);
			// 54 , 56, 54 , 255
			style.Colors[ImGuiCol_Border] = ImColor(54, 54, 54);
			style.Colors[ImGuiCol_Separator] = ImColor(54, 54, 54);
			style.Colors[ImGuiCol_SeparatorActive] = ImColor(54, 54, 54);
			style.Colors[ImGuiCol_SeparatorHovered] = ImColor(54, 54, 54);

		




	}

	void imgui_frame_end()
	{
		FrameContext* frameCtxt = WaitForNextFrameResources();
		UINT backBufferIdx = d3d12::pSwapChain->GetCurrentBackBufferIndex();

		{
			frameCtxt->CommandAllocator->Reset();
			static D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			barrier.Transition.pResource = d3d12::pID3D12ResourceArray[backBufferIdx];
			d3d12::pCommandList->Reset(frameCtxt->CommandAllocator, NULL);
			d3d12::pCommandList->ResourceBarrier(1, &barrier);
			d3d12::pCommandList->OMSetRenderTargets(1, &d3d12::RenderTargetDescriptorArray[backBufferIdx], FALSE, NULL);
			d3d12::pCommandList->SetDescriptorHeaps(1, &d3d12::pSrvDescHeap);
		}

		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12::pCommandList);

		static D3D12_RESOURCE_BARRIER barrier = { };
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.pResource = d3d12::pID3D12ResourceArray[backBufferIdx];

		d3d12::pCommandList->ResourceBarrier(1, &barrier);
		d3d12::pCommandList->Close();

		d3d12::pCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&d3d12::pCommandList);

		//HRESULT results = ori_present(pSwapChain, SyncInterval, Flags);

		UINT64 fenceValue = d3d12::fenceLastSignaledValue + 1;
		d3d12::pCommandQueue->Signal(d3d12::pFence, fenceValue);
		d3d12::fenceLastSignaledValue = fenceValue;
		frameCtxt->FenceValue = fenceValue;

	}
}

// i cant get this hook to work..
 void WINAPI HK_CopyTextureRegion(ID3D12GraphicsCommandList3* thisp, const D3D12_TEXTURE_COPY_LOCATION* pDst, UINT  DstX, UINT  DstY, UINT DstZ, const D3D12_TEXTURE_COPY_LOCATION* pSrc, const D3D12_BOX* pSrcBox)
{

	if (!thisp)
		return oCopyTextureRegion(thisp, pDst, DstX, DstY, DstZ, pSrc, pSrcBox);

	screenshot::visuals = false;
	screenshot::screenshot_counter++;

	Sleep(500);
	oCopyTextureRegion(thisp, pDst, DstX, DstY, DstZ, pSrc, pSrcBox);
	screenshot::visuals = true;
}



bool WINAPI NtGdiStretchBltHook1(HDC hdcDest, int xDest, int yDest, int wDest, int hDest, HDC hdcSrc, int xSrc, int ySrc, int wSrc, int hSrc, DWORD rop, DWORD dwBackColor) {

	screenshot::screenshot_counter++;
	screenshot::visuals = false;
	Sleep(3000);
	bool ok = NtGdiStretchBlt_original(hdcDest, xDest, yDest, wDest, hDest, hdcSrc, xSrc, ySrc, wSrc, hSrc, rop, dwBackColor);
	screenshot::visuals = true;
	return ok;
}

typedef BOOL(WINAPI* NtGdiBitBlt_t)(HDC hDCDest,
	INT 	XDest,
	INT 	YDest,
	INT 	Width,
	INT 	Height,
	HDC 	hDCSrc,
	INT 	XSrc,
	INT 	YSrc,
	DWORD 	dwRop,
	DWORD 	crBackColor,
	FLONG 	fl
	);

NtGdiBitBlt_t NtGdiBitBlt_original = nullptr;

BOOL WINAPI NtGdiBitBltHook(HDC 	hDCDest,
	INT 	XDest,
	INT 	YDest,
	INT 	Width,
	INT 	Height,
	HDC 	hDCSrc,
	INT 	XSrc,
	INT 	YSrc,
	DWORD 	dwRop,
	IN DWORD 	crBackColor,
	IN FLONG 	fl
) {

	screenshot::screenshot_counter++;
	/* *screenshot::visuals = false;*/
	screenshot::visuals = false;
	Sleep(3000);
	bool ok = NtGdiBitBlt_original(hDCDest, XDest, YDest, Width, Height, hDCSrc, XSrc, YSrc, dwRop, crBackColor, fl);
	/* *screenshot::visuals = true;*/
	screenshot::visuals = true;
	return ok;
}


void(*oExecuteCommandListsD3D12)(ID3D12CommandQueue*, UINT, ID3D12CommandList*);

void hookExecuteCommandListsD3D12(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* ppCommandLists)
{
	if (!d3d12test::d3d12CommandQueue2)
		d3d12test::d3d12CommandQueue2 = queue;

	oExecuteCommandListsD3D12(queue, NumCommandLists, ppCommandLists);
}



















bool is_hooked = false;
bool is_hooked2 = false;

bool init_textures = true; // true = enable texture
bool b_panic = false;
Timer timer = Timer();
bool start_timer = false;

Timer timer2 = Timer();
bool start_timer2 = false;
HRESULT present_hk(IDXGISwapChain3* pSwapChain, UINT SyncInterval, UINT Flags)
{

	d3d12::pSwapChain = pSwapChain;
	ID3D12CommandQueue* g_pD3DCommandQueue = *reinterpret_cast<decltype(g_pD3DCommandQueue)*>((std::uintptr_t)pSwapChain + 0x118);

	if (!pSwapChain || !screenshot::visuals )
		return ori_present(pSwapChain, SyncInterval, Flags);




	ImFont* main_font = imgui::start(static_cast<IDXGISwapChain3*>(pSwapChain), g_pD3DCommandQueue, nullptr);


	
	imgui::imgui_frame_header();
	

	
		g_menu::menu();

		

	imgui::imgui_frame_end();
	

	return ori_present(pSwapChain, SyncInterval, Flags);;
}


HRESULT(WINAPI* ResizeBuffersOriginal)(IDXGISwapChain* SwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) = NULL;
HRESULT hkResizeBuffers(IDXGISwapChain* SwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
{

	

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();

	imgui::cleard3d12();

	ImGui_ImplDX12_InvalidateDeviceObjects();


	imgui::reset_imgui_request();




	return ResizeBuffersOriginal(SwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}





void lobby_check()
{
	int test = 0;
	while (true)
	{
		Sleep(4000);
		test = *(int*)(sig_offsets::gamemode);
		if (test >= 1)
		{
			break;
		}
		else
		{
			continue;
		}
	}
}

void WriteLog(const char* filePath, const char* text)
{
	FILE* fp;
	fopen_s(&fp, filePath, "a");
	if (fp != NULL) {
		SYSTEMTIME st;
		GetLocalTime(&st);
		char buf[128] = { 0 };
		sprintf_s(buf, 128, ("%04d%02d%02d %02d:%02d:%02d:%03d "), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		fwrite(buf, sizeof(char), strlen(buf), fp);
		fwrite(text, sizeof(char), strlen(text), fp);
		fwrite("\r\n", sizeof(char), 2, fp);
		fclose(fp);
	}
}

HMODULE m_hModule = NULL;






uintptr_t game_ret{};
typedef void(WINAPI* LeaveCriticalSection_t)(LPCRITICAL_SECTION lpCriticalSection);
LeaveCriticalSection_t LeaveCriticalSection_original;
inline VOID WINAPI hookLeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection)
{


	
	void* RetAddress = _ReturnAddress();

	if (GetCurrentThread() == g_data::hMainThread && reinterpret_cast<uintptr_t>(RetAddress) == game_ret) //"45 85 ? 0F 84 ? ? ? ? 33 C9 E8 ? ? ? ? 84 C0"
	{
		main_game::cache_bones();
		//main_game::aimbot_thread();
		return LeaveCriticalSection_original(lpCriticalSection);
	}

	LeaveCriticalSection_original(lpCriticalSection);
}

////
//
//typedef void(__fastcall* R_RenderScene_t)(__int64 refdef, __int64 lodoverride, int drawtype);
//R_RenderScene_t RenderScene;
//void __fastcall R_RenderScenehook(__int64 refdef, __int64 lodoverride, int drawtype)
//{
//	
//
//	 spoof_call((void*)(sig_offsets::jmp_rbx), RenderScene, refdef, lodoverride, drawtype);
//}


void log( )
{

}

bool b_log = false;
uintptr_t g_lua_get_Crosshaircolor = 0;

void scan_engineFunction(bool blog)
{

	
	sig_offsets::s_blueprintsDirty = Scanner::Scanner((xorstr("48 8d 05 ? ? ? ? c6 04 01 ? 33 c0")), xorstr("s_blueprintsDirty")).resolve_lea();


	sig_offsets::BlueprintDataBuffer = Scanner::Scanner((xorstr("48 8D 05 ? ? ? ? 48 89 5C 24 ? 49 69 CE ? ? ? ? 48 89 5C 24 ? 4C 8D 4C 24 ? 48 03 C8 48 89 5C 24 ? 4D 8B C5 BA ? ? ? ? E8")), xorstr("blueprintdata")).resolve_lea();
	sig_offsets::sizeofBuffer = Scanner::Scanner((xorstr("BA ? ? ? ? 48 89 44 24 ? E8 ? ? ? ? 84 C0 0F 84")), xorstr("sizeofBuffer"), 1).get_offset<int>();
	sig_offsets::updateByte = Scanner::Scanner((xorstr("48 8D 05 ? ? ? ? C6 04 01 01 33 C0")), xorstr("updateByte")).resolve_lea();

	sig_offsets::StringTable_GetAsset = Scanner::Scanner(xorstr("E8 ? ? ? ? 48 8B 4D B0 E8 ? ? ? ? 45 33 ED "), xorstr("StringTable_GetAsset")).resolve_call();
	sig_offsets::OnlineLoot_GetInstance = Scanner::Scanner(xorstr("E8 ? ? ? ? 8B D3 48 8B C8 E8 ? ? ? ? 84 C0 74 14 E8 ? ? ? ? "), xorstr("OnlineLoot_GetInstance")).resolve_call();
	sig_offsets::StringTable_GetColumnValueForRow = Scanner::Scanner(xorstr("E8 ? ? ? ? 48 8B D0 0F B7 CB E8 ? ? ? ? 48 8B 4C 24 ? 41 B8 ? ? ? ? 8B D7 E8"), xorstr("StringTable_GetColumnValueForRow")).resolve_call();
	sig_offsets::OnlineLoot_UpdateItemInInventory = Scanner::Scanner(xorstr("e8 ? ? ? ? 48 8d 4c 24 ? e8 ? ? ? ? 48 8d 4c 24 ? e8 ? ? ? ? 48 8d 4d ? ff c6"), xorstr("OnlineLoot_UpdateItemInInventory")).resolve_call();
	sig_offsets::OnlineLoot_GetItemQuantity = Scanner::Scanner(xorstr("E8 ? ? ? ? 80 BC 24 ? ? ? ? ? 74 15 85 C0 75 11 85 FF "), xorstr("OnlineLoot_GetItemQuantity")).resolve_call();
	sig_offsets::jmp_rbx = memory::find_ida_sig("cod.exe", "FF 23");
}


void init_rendering()
{
	Sleep(100);
	
	//materialWhite = GameFunctions::Material_RegisterShader(xorstr("white"));
	//globals::weaponname = materialWhite->name;
	//regularFont =GameFunctions::R_RegisterFont(xorstr("fonts/main_bold.ttf"), 1);


	//globals::custom::gfx_name = regularFont->fontName;
	//globals::custom::pixel_height = regularFont->pixelHeight;

}

//E8 ? ? ? ? F3 0F 10 4D ? 41 0F 28 D8 4C cg_draw void __fastcall CG_DrawRotatedPicPhysical(__int64 sourceplace, float x, float y, float width, float height, float angle, float *color, __int64 material)
//48 8D 3D ? ? ? ? 4C 8B C5 src
VOID initialize()
{
	scan_engineFunction(0);
	//SetVEH();
	
	//init_rendering();




	
	if (kiero::init(kiero::RenderType::D3D12) == kiero::Status::Success)
	{



		kiero::bind(54, (void**)&oExecuteCommandListsD3D12, hookExecuteCommandListsD3D12);
		kiero::bind(140, (void**)&ori_present, present_hk);
	

	

		//*(uintptr_t*)getprocessor = (uintptr_t)hookLeaveCriticalSection;

	}

}


BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) 
{
	if (reason == DLL_PROCESS_ATTACH) 
	{
		m_hModule = module;
		Settings::Auto_Load();
		g_data::init();

		I_beginthreadex(0, 0, (_beginthreadex_proc_type)initialize, 0, 0, 0);
	
	}

	return TRUE;
}




LRESULT hkWndProc(HWND hWnd, UINT Msg, WPARAM wp, LPARAM lp)
{
	switch (Msg)
	{
		case 0x403:
		case WM_SIZE:
		{
			if (Msg == WM_SIZE && wp == SIZE_MINIMIZED)
				break;

			if (imgui::IsReady())
			{
			
			}
			break;
		}
	};
	ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wp, lp);



	return ogr_function::WndProc(hWnd, Msg, wp, lp);
}

void WndProc_hk()
{
	ogr_function::WndProc = (WNDPROC)SetWindowLongPtrW(g_data::hWind, GWLP_WNDPROC, (LONG_PTR)hkWndProc);
}



//[0]   QueryInterface
//[1]   AddRef
//[2]   Release
//[3]   GetPrivateData
//[4]   SetPrivateData
//[5]   SetPrivateDataInterface
//[6]   SetName
//[7]   GetNodeCount
//[8]   CreateCommandQueue
//[9]   CreateCommandAllocator
//[10]  CreateGraphicsPipelineState
//[11]  CreateComputePipelineState
//[12]  CreateCommandList
//[13]  CheckFeatureSupport
//[14]  CreateDescriptorHeap
//[15]  GetDescriptorHandleIncrementSize
//[16]  CreateRootSignature
//[17]  CreateConstantBufferView
//[18]  CreateShaderResourceView
//[19]  CreateUnorderedAccessView
//[20]  CreateRenderTargetView
//[21]  CreateDepthStencilView
//[22]  CreateSampler
//[23]  CopyDescriptors
//[24]  CopyDescriptorsSimple
//[25]  GetResourceAllocationInfo
//[26]  GetCustomHeapProperties
//[27]  CreateCommittedResource
//[28]  CreateHeap
//[29]  CreatePlacedResource
//[30]  CreateReservedResource
//[31]  CreateSharedHandle
//[32]  OpenSharedHandle
//[33]  OpenSharedHandleByName
//[34]  MakeResident
//[35]  Evict
//[36]  CreateFence
//[37]  GetDeviceRemovedReason
//[38]  GetCopyableFootprints
//[39]  CreateQueryHeap
//[40]  SetStablePowerState
//[41]  CreateCommandSignature
//[42]  GetResourceTiling
//[43]  GetAdapterLuid
//[44]  QueryInterface
//[45]  AddRef
//[46]  Release
//[47]  GetPrivateData
//[48]  SetPrivateData
//[49]  SetPrivateDataInterface
//[50]  SetName
//[51]  GetDevice
//[52]  UpdateTileMappings
//[53]  CopyTileMappings
//[54]  ExecuteCommandLists
//[55]  SetMarker
//[56]  BeginEvent
//[57]  EndEvent
//[58]  Signal
//[59]  Wait
//[60]  GetTimestampFrequency
//[61]  GetClockCalibration
//[62]  GetDesc
//[63]  QueryInterface
//[64]  AddRef
//[65]  Release
//[66]  GetPrivateData
//[67]  SetPrivateData
//[68]  SetPrivateDataInterface
//[69]  SetName
//[70]  GetDevice
//[71]  Reset
//[72]  QueryInterface
//[73]  AddRef
//[74]  Release
//[75]  GetPrivateData
//[76]  SetPrivateData
//[77]  SetPrivateDataInterface
//[78]  SetName
//[79]  GetDevice
//[80]  GetType
//[81]  Close
//[82]  Reset
//[83]  ClearState
//[84]  DrawInstanced
//[85]  DrawIndexedInstanced
//[86]  Dispatch
//[87]  CopyBufferRegion
//[88]  CopyTextureRegion
//[89]  CopyResource
//[90]  CopyTiles
//[91]  ResolveSubresource
//[92]  IASetPrimitiveTopology
//[93]  RSSetViewports
//[94]  RSSetScissorRects
//[95]  OMSetBlendFactor
//[96]  OMSetStencilRef
//[97]  SetPipelineState
//[98]  ResourceBarrier
//[99]  ExecuteBundle
//[100] SetDescriptorHeaps
//[101] SetComputeRootSignature
//[102] SetGraphicsRootSignature
//[103] SetComputeRootDescriptorTable
//[104] SetGraphicsRootDescriptorTable
//[105] SetComputeRoot32BitConstant
//[106] SetGraphicsRoot32BitConstant
//[107] SetComputeRoot32BitConstants
//[108] SetGraphicsRoot32BitConstants
//[109] SetComputeRootConstantBufferView
//[110] SetGraphicsRootConstantBufferView
//[111] SetComputeRootShaderResourceView
//[112] SetGraphicsRootShaderResourceView
//[113] SetComputeRootUnorderedAccessView
//[114] SetGraphicsRootUnorderedAccessView
//[115] IASetIndexBuffer
//[116] IASetVertexBuffers
//[117] SOSetTargets
//[118] OMSetRenderTargets
//[119] ClearDepthStencilView
//[120] ClearRenderTargetView
//[121] ClearUnorderedAccessViewUint
//[122] ClearUnorderedAccessViewFloat
//[123] DiscardResource
//[124] BeginQuery
//[125] EndQuery
//[126] ResolveQueryData
//[127] SetPredication
//[128] SetMarker
//[129] BeginEvent
//[130] EndEvent
//[131] ExecuteIndirect
//[132] QueryInterface
//[133] AddRef
//[134] Release
//[135] SetPrivateData
//[136] SetPrivateDataInterface
//[137] GetPrivateData
//[138] GetParent
//[139] GetDevice
//[140] Present
//[141] GetBuffer
//[142] SetFullscreenState
//[143] GetFullscreenState
//[144] GetDesc
//[145] ResizeBuffers
//[146] ResizeTarget
//[147] GetContainingOutput
//[148] GetFrameStatistics
//[149] GetLastPresentCount

