// Copyright 2017 Sebastien Ronsse

#include "ImGuiHUD.h"
#include "MyImGuiHUD.h"

const struct FKey *KeyMap[] =
{
	&EKeys::Tab,
	&EKeys::Left,
	&EKeys::Right,
	&EKeys::Up,
	&EKeys::Down,
	&EKeys::PageUp,
	&EKeys::PageDown,
	&EKeys::Home,
	&EKeys::End,
	&EKeys::Delete,
	&EKeys::BackSpace,
	&EKeys::Enter,
	&EKeys::Escape,
	&EKeys::A,
	&EKeys::C,
	&EKeys::V,
	&EKeys::X,
	&EKeys::Y,
	&EKeys::Z,
};

AMyImGuiHUD::AMyImGuiHUD() :
	Super(),
	FontTexture(NULL)
{
	// Get master material reference
	static ConstructorHelpers::FObjectFinder<UMaterial> mat(TEXT("Material'/Game/Material.Material'"));
	MasterMaterial = mat.Object;
}

void AMyImGuiHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Create dynamic material instance
	if (MasterMaterial)
		MaterialInstance = UMaterialInstanceDynamic::Create(MasterMaterial, NULL);

	// Setup ImGui binding
	ImGui_ImplUE_Init();
}

void AMyImGuiHUD::BeginDestroy()
{
	Super::BeginDestroy();

	// Cleanup
	ImGui_ImplUE_Shutdown();

	MasterMaterial = NULL;
	MaterialInstance = NULL;
}

void AMyImGuiHUD::DrawHUD()
{
	// Process events
	ImGui_ImplUE_ProcessEvent();

	// Prepare new frame
	ImGui_ImplUE_NewFrame();

	// Rendering
	ImGui::Render();
}

bool AMyImGuiHUD::ImGui_ImplUE_Init()
{
	ImGuiIO &io = ImGui::GetIO();

	// Keyboard mapping
	for (int i = 0; i < ImGuiKey_COUNT; i++)
		io.KeyMap[i] = i;

	// Fill callbacks
	io.RenderDrawListsFn = ImGui_ImplUE_RenderDrawLists;
	io.SetClipboardTextFn = ImGui_ImplUE_SetClipboardText;
	io.GetClipboardTextFn = ImGui_ImplUE_GetClipboardText;
	io.UserData = this;

	return true;
}

void AMyImGuiHUD::ImGui_ImplUE_Shutdown()
{
	ImGui_ImplUE_InvalidateDeviceObjects();
	ImGui::Shutdown();
}

bool AMyImGuiHUD::ImGui_ImplUE_CreateDeviceObjects()
{
	// Build texture atlas
	ImGuiIO &io = ImGui::GetIO();
	unsigned char *pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	// Upload texture to graphics system
	FontTexture = UTexture2D::CreateTransient(width, height, PF_R8G8B8A8);
	FTexture2DMipMap &mip = FontTexture->PlatformData->Mips[0];
	void *data = mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(data, pixels, width * height * 4);
	mip.BulkData.Unlock();
	FontTexture->UpdateResource();

	// Store our identifier
	io.Fonts->TexID = (void *)FontTexture;

	return true;
}

void AMyImGuiHUD::ImGui_ImplUE_InvalidateDeviceObjects()
{
	FontTexture = NULL;
}

void AMyImGuiHUD::ImGui_ImplUE_ProcessEvent()
{
}

void AMyImGuiHUD::ImGui_ImplUE_NewFrame()
{
	if (!FontTexture)
		ImGui_ImplUE_CreateDeviceObjects();

	ImGuiIO &io = ImGui::GetIO();

	// Setup display size
	io.DisplaySize = ImVec2((float)Canvas->SizeX, (float)Canvas->SizeY);
	io.DisplayFramebufferScale = ImVec2(1, 1);

	// Setup inputs
	APlayerController *pc = GetOwningPlayerController();
	pc->GetMousePosition(io.MousePos.x, io.MousePos.y);
	io.MouseDown[0] = pc->IsInputKeyDown(EKeys::LeftMouseButton);
	io.MouseDown[1] = pc->IsInputKeyDown(EKeys::RightMouseButton);
	io.MouseDown[2] = pc->IsInputKeyDown(EKeys::MiddleMouseButton);

	ImGui::NewFrame();
}

void AMyImGuiHUD::ImGui_ImplUE_RenderDrawLists(ImDrawData *draw_data)
{
}

const char *AMyImGuiHUD::ImGui_ImplUE_GetClipboardText()
{
	return NULL;
}

void AMyImGuiHUD::ImGui_ImplUE_SetClipboardText(const char *text)
{
}
