#pragma once
#include "../Custom/Custom.h"
#include <float.h>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
#include <xlocbuf>
#include <codecvt>
#include "../Custom/VTableHook.h"
#include "global.h"
# define M_PI           3.14159265358979323846  /* pi */
#undef min
#undef max
#define IsKeyHeld(key) (GetAsyncKeyState(key) & 0x8000)

std::ofstream logFile("PUT LOCATION OF FOLDER YOU WANT LOGS TO SAVE TO", std::ios::app);

bool debug = false;
Hook::VTableHook PRHook;    

void Log(const char* format, ...) {
    if (!debug) return;
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    logFile << buffer << std::endl;
    logFile.flush();

    printf("%s\n", buffer);
}

bool IsValid(const UObject* Test) {
    return Test != nullptr;
}



FVector GetBoneLocation(ACharacter* Player, SDK::FString BoneName) {
    if (!Player || !IsValid(Player) || !Player->GetMesh() || !IsValid(Player->GetMesh())) {
        Log("GetBoneLocation: Player or GetMesh() is invalid\n");
        return FVector();
    }

    USkeletalMeshComponent* mesh = Player->GetMesh();
    FName BoneFName = UKismetStringLibrary::Conv_StringToName(BoneName);
    FVector Location = mesh->GetSocketLocation(BoneFName);
    return Location;
}

FName GetAimBoneName() {
    if (mods::aimHitbox == "Head") return UKismetStringLibrary::Conv_StringToName(SDK::FString(L"head"));
    else if (mods::aimHitbox == "Pelvis") return UKismetStringLibrary::Conv_StringToName(SDK::FString(L"pelvis"));
    else if (mods::aimHitbox == "Neck") return UKismetStringLibrary::Conv_StringToName(SDK::FString(L"neck_01"));
    else if (mods::aimHitbox == "Chest") return UKismetStringLibrary::Conv_StringToName(SDK::FString(L"spine_01"));
    else if (mods::aimHitbox == "Left Hand") return UKismetStringLibrary::Conv_StringToName(SDK::FString(L"hand_l"));
    else if (mods::aimHitbox == "Right Hand") return UKismetStringLibrary::Conv_StringToName(SDK::FString(L"hand_r"));
    else return UKismetStringLibrary::Conv_StringToName(SDK::FString(L"head"));
}

AMarvelPlayerController* GetLocalPlayerController(UWorld* World)
{
    if (!World) return nullptr;

    if (AMarvelPlayerController* PlayerController = reinterpret_cast<AMarvelPlayerController*>(UGameplayStatics::GetPlayerController(World, 0)))
    {
        return PlayerController;
    }

    if (AMarvelPlayerController* PlayerController = reinterpret_cast<AMarvelPlayerController*>(UGameplayStatics::GetPlayerControllerFromID(World, 0)))
    {
        return PlayerController;
    }

    return nullptr;
}

FVector PredictTargetPosition(AMarvelBaseCharacter* Target, float ProjectileSpeed) {
    if (!Target || !IsValid(Target) || !Target->GetMesh() || !IsValid(Target->GetMesh())) {
        Log("PredictTargetPosition: Target or Mesh is invalid\n");
        return FVector();
    }

    FVector TargetVelocity = Target->GetVelocity();
    FName BoneFName = GetAimBoneName();
    if (!Target->GetMesh()->DoesSocketExist(BoneFName)) {
        Log("PredictTargetPosition: Selected bone does not exist\n");
        return FVector();
    }

    FVector TargetPosition = Target->GetMesh()->GetSocketLocation(BoneFName);
    FVector ShooterPosition = Variables::CameraLocation;

    float distance = UKismetMathLibrary::Vector_Distance(ShooterPosition, TargetPosition);
    float timeToHit = distance / ProjectileSpeed;

    FVector PredictedPosition = TargetPosition + (TargetVelocity * timeToHit);
    return PredictedPosition;
}

bool CheckIfPlayer(AActor* Player, UClass* PlayerClass) {
    if (Player && PlayerClass) {
        if (Player->IsA(PlayerClass)) return true;
    }
    return false;
}

bool IsPointInAimbotCircle(float radius, SDK::FVector2D point) {
    ImVec2 screenCenter = ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2);
    float dx = point.X - screenCenter.x;
    float dy = point.Y - screenCenter.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

ImVec2 NormalizeImVec2(const ImVec2& vec) {
    float mag = sqrt(vec.x * vec.x + vec.y * vec.y);
    if (mag > 0.0f) {
        return ImVec2(vec.x / mag, vec.y / mag);
    }
    return ImVec2(0.0f, 0.0f);
}
inline UObject* (*StaticLoadObject)(UClass* ObjectClass, UObject* Outer, const wchar_t* Name, const wchar_t* FileName, uint32_t LoadFlags, uintptr_t Sandbox, bool bAllowObjectReconciliation, const uintptr_t* InstancingContex);

static const std::vector<std::pair<std::wstring, std::wstring>> boneConnections = {
    { L"spine_01", L"pelvis" },
    { L"neck_01", L"spine_01" },
    { L"head", L"neck_01" },
    { L"upperarm_l", L"spine_01" },
    { L"lowerarm_l", L"upperarm_l" },
    { L"hand_l", L"lowerarm_l" },
    { L"upperarm_r", L"spine_01" },
    { L"lowerarm_r", L"upperarm_r" },
    { L"hand_r", L"lowerarm_r" },
    { L"thigh_l", L"pelvis" },
    { L"calf_l", L"thigh_l" },
    { L"foot_l", L"calf_l" },
    { L"thigh_r", L"pelvis" },
    { L"calf_r", L"thigh_r" },
    { L"foot_r", L"calf_r" }
};


USkeletalMeshComponent* VALIDATETHATFUCKINGSHIT(AMarvelBaseCharacter* Target) {
    if (Target) {
        if (Target->RootComponent) {
            if (Target->GetMesh()) {
                return Target->GetMesh();
            }
        }
    }
    return nullptr;
}

FVector GetBoneLocation(ACharacter* Player, FName BoneName)
{
    if (!Player || !Player->RootComponent) return FVector();

    if (USceneComponent* RootComponent = Player->RootComponent)
    {
        return RootComponent->GetSocketLocation(BoneName);
    }

    return FVector();
}
void DrawSkeleton(ImDrawList* drawList, SDK::AMarvelBaseCharacter* marvelChar, APlayerController* controller) {
    if (!marvelChar || !controller) return;

    SDK::USkeletalMeshComponent* mesh = marvelChar->GetMesh();
    if (!mesh) return;

    for (const auto& [boneNameStr, parentBoneNameStr] : boneConnections) {
        SDK::FName boneName = UKismetStringLibrary::Conv_StringToName(SDK::FString(boneNameStr.c_str()));
        if (!mesh->DoesSocketExist(boneName)) continue;

        SDK::FVector boneWorldPos = mesh->GetSocketLocation(boneName);
        SDK::FVector2D boneScreenPos;
        if (!controller->ProjectWorldLocationToScreen(boneWorldPos, &boneScreenPos, true)) continue;

        if (!parentBoneNameStr.empty()) {
            SDK::FName parentBoneName = UKismetStringLibrary::Conv_StringToName(SDK::FString(parentBoneNameStr.c_str()));
            if (mesh->DoesSocketExist(parentBoneName)) {
                SDK::FVector parentBoneWorldPos = mesh->GetSocketLocation(parentBoneName);
                SDK::FVector2D parentBoneScreenPos;

                if (controller->ProjectWorldLocationToScreen(parentBoneWorldPos, &parentBoneScreenPos, true)) {
                    drawList->AddLine(
                        ImVec2(boneScreenPos.X, boneScreenPos.Y),
                        ImVec2(parentBoneScreenPos.X, parentBoneScreenPos.Y),
                        mods::skeletonESPColor);
                }
            }
        }
    }
}

FVector GetVectorForward(const FRotator& angles)
{
    float sp, sy, cp, cy;
    float angle;

    angle = angles.Yaw * (M_PI / 180.0f);
    sy = sinf(angle);
    cy = cosf(angle);
    angle = -angles.Pitch * (M_PI / 180.0f);
    sp = sinf(angle);
    cp = cosf(angle);

    return { cp * cy, cp * sy, -sp };
}

void PressLeftMouseDown()
{
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &input, sizeof(INPUT));
}

// Release left mouse buPressLeftMouseDowntton
void ReleaseLeftMouse()
{
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &input, sizeof(INPUT));
}

void DrawCrosshair(ImDrawList* drawList, ImVec2 center) {
    if (mods::crosshairType == mods::NONE) return;

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    center = ImVec2(displaySize.x / 2, displaySize.y / 2);

    switch (mods::crosshairType) {
    case mods::DOT:
        drawList->AddCircleFilled(center, mods::crosshairSize, mods::crosshairColor);
        break;
    case mods::CROSS: {
        float halfSize = mods::crosshairSize / 2;
        drawList->AddLine(ImVec2(center.x - halfSize, center.y), ImVec2(center.x + halfSize, center.y), mods::crosshairColor, mods::crosshairThickness);
        drawList->AddLine(ImVec2(center.x, center.y - halfSize), ImVec2(center.x, center.y + halfSize), mods::crosshairColor, mods::crosshairThickness);
        break;
    }
    case mods::CIRCLE:
        drawList->AddCircle(center, mods::crosshairSize, mods::crosshairColor, 0, mods::crosshairThickness);
        break;
    }
}

void AddTextWithOutline(ImDrawList* drawList, ImFont* font, float fontSize, ImVec2 pos, ImU32 textColor, ImU32 outlineColor, const char* text) {
    drawList->AddText(font, fontSize, ImVec2(pos.x - 1, pos.y), outlineColor, text);
    drawList->AddText(font, fontSize, ImVec2(pos.x + 1, pos.y), outlineColor, text);
    drawList->AddText(font, fontSize, ImVec2(pos.x, pos.y - 1), outlineColor, text);
    drawList->AddText(font, fontSize, ImVec2(pos.x, pos.y + 1), outlineColor, text);
    drawList->AddText(font, fontSize, pos, textColor, text);
}


int EATshit = 0;
static bool LilXanxPerkies = false;
bool DONTSCAN = false;
void DrawTransition(ImDrawList* BackgroundList, ImDrawList* ForegroundList) {
    try {


        UEngine* engine = UEngine::GetEngine();
        if (!engine) {
            Log("Invalid Engine");
            return;
        }

        UWorld* aWorld = engine->GameViewport->World;
        if (!aWorld) {
            Log("Invalid UWorld");
            return;
        }

        if (!aWorld || !IsValid(aWorld) || !BackgroundList || !ForegroundList) {
            Log("DrawTransition: Invalid input parameters\n");
            return;
        }

        auto gameplayStatics = (SDK::UGameplayStatics*)SDK::UGameplayStatics::StaticClass();
        if (gameplayStatics && aWorld) {
            if (gameplayStatics->GetTimeSeconds(aWorld) <= 10) {
                return;
            }
        }

        Variables::World = aWorld;

        ULevel* PersistentLevel = aWorld->PersistentLevel;
        if (!PersistentLevel || !IsValid(PersistentLevel)) {
            Log("DrawTransition: PersistentLevel is invalid\n");
            return;
        }

        APlayerController* PlayerController = GetLocalPlayerController(aWorld);
        if (!PlayerController || !IsValid(PlayerController)) {
            Log("DrawTransition: PlayerController is invalid\n");
            return;
        }
        Variables::PlayerController = PlayerController;

        APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(aWorld, 0);
        if (!PlayerCameraManager || !IsValid(PlayerCameraManager)) {
            Log("DrawTransition: PlayerCameraManager is invalid\n");
            return;
        }

        APawn* AcknowledgedPawn = PlayerController->AcknowledgedPawn;
        if (!AcknowledgedPawn || !IsValid(AcknowledgedPawn)) {
            return;
        }
        Variables::AcknowledgedPawn = AcknowledgedPawn;
        Variables::CameraLocation = PlayerCameraManager->GetCameraLocation();
        Variables::CameraRotation = PlayerCameraManager->GetCameraRotation();

        ////AMarvelAbilityTargetActor_Projectile
        //AMarvelAbilityTargetActor_Trace* TargetActor = reinterpret_cast<AMarvelAbilityTargetActor_Trace*>(PlayerController);
        //if (!TargetActor) return;

        //if (TargetActor) {
        //    TargetActor->AimingSpreadMod = 0.F;
        //    TargetActor->BaseSpread = 0.0f;
        //    TargetActor->TargetingSpreadIncrement = 0.f;
        //    TargetActor->TargetingSpreadMax = 0.0f;
        //    
        //}

        if (PlayerController->Character || IsValid(PlayerController->Character)) {
            if (GetAsyncKeyState(VK_LCONTROL) && mods::SelfCustomTimeDilationBool) {
                PlayerController->Character->CustomTimeDilation = mods::SelfCustomTimeDilationFloat;
            }
            else {
                PlayerController->Character->CustomTimeDilation = 1;
            }
        }

        if (mods::Experimental::HideLocalPlayer) {
            if (PlayerController->Character) {
                SDK::USkeletalMeshComponent* MeshComponent = PlayerController->Character->GetMesh();
                if (MeshComponent) {
                    if (mods::Experimental::HideLocalPlayer) {
                        MeshComponent->SetHiddenInGame(true, true);
                    }
                    else if (!mods::Experimental::HideLocalPlayer && MeshComponent->bHiddenInGame) {
                        MeshComponent->SetHiddenInGame(false, false);
                    }
                }
            }
        }
        if (mods::Experimental::SmallPerson) {
            if (PlayerController->Character) {
                SDK::USkeletalMeshComponent* MeshComponent = PlayerController->Character->GetMesh();
                if (MeshComponent) {
                    if (mods::Experimental::SmallPerson) {
                        FVector desiredScale(mods::Experimental::SmallPersonScale,
                            mods::Experimental::SmallPersonScale,
                            mods::Experimental::SmallPersonScale);
                        if (MeshComponent->RelativeScale3D != desiredScale) {
                            MeshComponent->SetWorldScale3D(desiredScale);
                        }
                    }
                }
            }
        }

        if (mods::bSpinbot && PlayerController->Character) {
            if (PlayerController->Character) {
                TArray<AActor*> components;
                PlayerController->Character->GetAllChildActors(&components, true);
                static float Yaw, pitch, roll = 1.0f;
                Yaw += mods::SpiningSpeedX;
                pitch += mods::SpiningSpeedY;
                roll += mods::SpiningSpeedZ;
                if (Yaw >= 360.0f) Yaw = 1.0f;
                if (pitch >= 360.0f) pitch = 1.0f;
                if (roll >= 360.0f) roll = 1.0f;
                SDK::FRotator NewRotation(mods::bSpinbotX ? Yaw : 0.0f, mods::bSpinbotY ? pitch : 0.0f, mods::bSpinbotZ ? roll : 0.0f);
                SDK::FHitResult fhit;
                for (AActor* comp : components) {
                    comp->K2_SetActorRelativeRotation(NewRotation, false, &fhit, false);
                }

            }
        }

        if (mods::fov_changer && PlayerController) {
            PlayerController->FOV(mods::fov_changer_amount);
        }

        ImVec2 displaySize = ImGui::GetIO().DisplaySize;
        ImVec2 screenCenter = ImVec2(displaySize.x / 2, displaySize.y / 2);

        DrawCrosshair(ForegroundList, screenCenter);

        struct EnemyInfo {
            AMarvelBaseCharacter* player;
            float health;
            float distanceToCenter;
            float distance;
        };
        std::vector<EnemyInfo> enemies;
        std::vector<FVector> enemyPositionsForRadar;
        mods::closestDistance = FLT_MAX;

        TArray<AActor*> ActorList = PersistentLevel->Actors;
        if (!ActorList.IsValid()) {
            Log("DrawTransition: ActorList is invalid\n");
            return;
        }

        UClass* BulletClass = AGameplayAbilityTargetActor::StaticClass();
        if (!BulletClass) {
            Log("FUCK YOU BULLET CLASS");
            return;
        }

        UClass* ClassToFind = AMarvelBaseCharacter::StaticClass();
        if (!ClassToFind) {
            Log("DrawTransition: ClassToFind is null\n");
            return;
        }



        for (int i = 0; i < ActorList.Num(); i++) {
            if (!ActorList.IsValidIndex(i)) continue;
            AActor* actor = ActorList[i];
            if (!actor || !IsValid(actor)) continue;

         

            if (CheckIfPlayer(actor, ClassToFind)) {

                AMarvelBaseCharacter* Player = reinterpret_cast<AMarvelBaseCharacter*>(actor);
                if (!Player || !IsValid(Player)) continue;

                USkeletalMeshComponent* Mesh = Player->GetMesh();
                if (!Mesh || !IsValid(Mesh)) continue;

                float currentHealth = Player->GetCurrentHealth();
                if (currentHealth <= 0.f) continue;

                FName HeadBoneName = UKismetStringLibrary::Conv_StringToName(SDK::FString(L"Head"));
                if (!Mesh->DoesSocketExist(HeadBoneName)) continue;

                FVector TargetHead3D = Mesh->GetSocketLocation(HeadBoneName);
                SDK::FVector2D TargetHead2D;
                if (!PlayerController->ProjectWorldLocationToScreen(TargetHead3D, &TargetHead2D, true)) continue;

                if (mods::LocalCheck && Player->IsLocallyControlled()) continue;

                float distance = UKismetMathLibrary::Vector_Distance(Variables::CameraLocation, Player->K2_GetActorLocation()) / 100.0f;

                if (mods::bAimbotTeamCheck) {
                    AMarvelPlayerState* PlayerState = static_cast<AMarvelPlayerState*>(Player->PlayerState);
                    AMarvelPlayerState* LocalState = static_cast<AMarvelPlayerState*>(AcknowledgedPawn->PlayerState);
                    if (!PlayerState || !IsValid(PlayerState) || !LocalState || !IsValid(LocalState)) continue;
                    if (PlayerState->TeamID == LocalState->TeamID) continue;
                }

                if (distance <= mods::aimbotMaxDistance &&
                    TargetHead2D.X >= 0 && TargetHead2D.X <= displaySize.x &&
                    TargetHead2D.Y >= 0 && TargetHead2D.Y <= displaySize.y) {
                    if (distance < mods::closestDistance) {
                        mods::closestDistance = distance;
                    }
                }
            }
        }

        if (mods::bDynamicFOV && mods::closestDistance < mods::aimbotMaxDistance) {
            float baseRadius = static_cast<float>(mods::fov) * Variables::ScreenSize.X / static_cast<float>(mods::fov_changer_amount) / 2.0f;
            float minRadius = baseRadius * mods::minFovMultiplier;
            float maxRadius = baseRadius * mods::maxFovMultiplier;
            float t = mods::closestDistance / mods::aimbotMaxDistance;
            mods::actualfovcircle = minRadius + (1 - t) * (maxRadius - minRadius);
        }
        else {
            mods::actualfovcircle = static_cast<float>(mods::fov) * Variables::ScreenSize.X / static_cast<float>(mods::fov_changer_amount) / 2.0f;
        }

        if (mods::aimbotFovCircle) {
            BackgroundList->AddCircle(screenCenter, mods::actualfovcircle, mods::aimbotFovCircleColor);
        }

        ImFont* font = mods::availableFonts[mods::selectedESPFontIndex];
        float fontSize = mods::baseFontSize * mods::textScale;

        mods::enemyOverlayData.clear();
        for (int i = 0; i < ActorList.Num(); i++) {
            if (!ActorList.IsValidIndex(i)) continue;

            AActor* actor = ActorList[i];
            if (!actor || !IsValid(actor)) continue;

            if (CheckIfPlayer(actor, ClassToFind)) {

                AMarvelBaseCharacter* Player = reinterpret_cast<AMarvelBaseCharacter*>(actor);
                if (!Player || !IsValid(Player)) continue;

                USkeletalMeshComponent* Mesh = Player->GetMesh();
                if (!Mesh || !IsValid(Mesh)) continue;

                float currentHealth = Player->GetCurrentHealth();
                if (currentHealth <= 0.f) continue;

                FName HeadBoneName = UKismetStringLibrary::Conv_StringToName(SDK::FString(L"Head"));
                if (!Mesh->DoesSocketExist(HeadBoneName)) continue;

                FVector TargetHead3D = Mesh->GetSocketLocation(HeadBoneName);
                SDK::FVector2D TargetHead2D;
                if (!PlayerController->ProjectWorldLocationToScreen(TargetHead3D, &TargetHead2D, true)) continue;

                if (mods::LocalCheck && Player->IsLocallyControlled()) continue;

                float distance = UKismetMathLibrary::Vector_Distance(Variables::CameraLocation, Player->K2_GetActorLocation()) / 100.0f;

                bool bIsEnemy = false;
                if (mods::bESPTeamCheck || mods::bAimbotTeamCheck) {
                    AMarvelPlayerState* PlayerState = static_cast<AMarvelPlayerState*>(Player->PlayerState);
                    AMarvelPlayerState* LocalState = static_cast<AMarvelPlayerState*>(AcknowledgedPawn->PlayerState);

                    if (!PlayerState || !IsValid(PlayerState) || !LocalState || !IsValid(LocalState)) continue;
                    if (PlayerState->TeamID == LocalState->TeamID) continue;
                    bIsEnemy = true;
                }

                if (GetAsyncKeyState(mods::aimbotKey) && mods::CustomTimeDilationBool) {
                    Player->CustomTimeDilation = mods::CustomTimeDilationFloat;
                }
                else {
                    Player->CustomTimeDilation = 1;
                }

                bool bIsVisible = PlayerController->LineOfSightTo(Player, Variables::CameraLocation, false);

                if (distance <= mods::espMaxDistance) {
                    if (mods::bESPBox) {
                        FVector Origin = Mesh->GetSocketLocation(UKismetStringLibrary::Conv_StringToName(SDK::FString(L"pelvis")));
                        FVector BoxExtent = FVector(50.0f, 50.0f, 100.0f);

                        FVector Corners[8] = {
                            Origin + FVector(-BoxExtent.X, -BoxExtent.Y, -BoxExtent.Z),
                            Origin + FVector(BoxExtent.X, -BoxExtent.Y, -BoxExtent.Z),
                            Origin + FVector(BoxExtent.X, BoxExtent.Y, -BoxExtent.Z),
                            Origin + FVector(-BoxExtent.X, BoxExtent.Y, -BoxExtent.Z),
                            Origin + FVector(-BoxExtent.X, -BoxExtent.Y, BoxExtent.Z),
                            Origin + FVector(BoxExtent.X, -BoxExtent.Y, BoxExtent.Z),
                            Origin + FVector(BoxExtent.X, BoxExtent.Y, BoxExtent.Z),
                            Origin + FVector(-BoxExtent.X, BoxExtent.Y, BoxExtent.Z)
                        };
                        bool bOnScreen = false;
                        SDK::FVector2D ScreenMin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
                        SDK::FVector2D ScreenMax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
                        std::vector<SDK::FVector2D> screenCorners(8);

                        for (int j = 0; j < 8; j++) {
                            if (PlayerController->ProjectWorldLocationToScreen(Corners[j], &screenCorners[j], true)) {
                                if (!bOnScreen) {
                                    ScreenMin = screenCorners[j];
                                    ScreenMax = screenCorners[j];
                                    bOnScreen = true;
                                }
                                else {
                                    ScreenMin.X = std::min(ScreenMin.X, screenCorners[j].X);
                                    ScreenMin.Y = std::min(ScreenMin.Y, screenCorners[j].Y);
                                    ScreenMax.X = std::max(ScreenMax.X, screenCorners[j].X);
                                    ScreenMax.Y = std::max(ScreenMax.Y, screenCorners[j].Y);
                                }
                            }
                        }

                        if (bOnScreen) {
                            ImU32 color = bIsVisible ? mods::visibleColor : mods::nonVisibleColor;

                            if (mods::espBoxType == mods::ESP_BOX_2D) {
                                if (mods::bESPBoxOutline) {
                                    BackgroundList->AddRect(ImVec2(ScreenMin.X, ScreenMin.Y), ImVec2(ScreenMax.X, ScreenMax.Y), mods::espBoxOutlineColor, 0.0f, ImDrawFlags_None, mods::espBoxThickness + 2.0f);
                                }
                                BackgroundList->AddRect(ImVec2(ScreenMin.X, ScreenMin.Y), ImVec2(ScreenMax.X, ScreenMax.Y), color, 0.0f, ImDrawFlags_None, mods::espBoxThickness);
                            }
                            else if (mods::espBoxType == mods::ESP_BOX_3D) {
                                std::vector<std::pair<int, int>> edges = {
                                    {0,1}, {1,2}, {2,3}, {3,0},
                                    {4,5}, {5,6}, {6,7}, {7,4},
                                    {0,4}, {1,5}, {2,6}, {3,7}
                                };
                                for (const auto& edge : edges) {
                                    ImVec2 p1 = ImVec2(screenCorners[edge.first].X, screenCorners[edge.first].Y);
                                    ImVec2 p2 = ImVec2(screenCorners[edge.second].X, screenCorners[edge.second].Y);
                                    if (mods::bESPBoxOutline) {
                                        BackgroundList->AddLine(p1, p2, mods::espBoxOutlineColor, mods::espBoxThickness + 2.0f);
                                    }
                                    BackgroundList->AddLine(p1, p2, color, mods::espBoxThickness);
                                }
                                edges.clear();
                            }

                            if (mods::bHealthBar) {
                                float maxHealth = Player->GetMaxHealth();
                                float healthPercent = currentHealth / maxHealth;
                                ImVec2 barStart, barEnd;
                                if (mods::healthBarPosition == mods::LEFT) {
                                    barStart = ImVec2(ScreenMin.X - 10.0f, ScreenMin.Y);
                                    barEnd = ImVec2(ScreenMin.X - 5.0f, ScreenMax.Y);
                                }
                                else if (mods::healthBarPosition == mods::RIGHT) {
                                    barStart = ImVec2(ScreenMax.X + 5.0f, ScreenMin.Y);
                                    barEnd = ImVec2(ScreenMax.X + 10.0f, ScreenMax.Y);
                                }
                                else if (mods::healthBarPosition == mods::TOP) {
                                    barStart = ImVec2(ScreenMin.X, ScreenMin.Y - 10.0f);
                                    barEnd = ImVec2(ScreenMax.X, ScreenMin.Y - 5.0f);
                                }
                                else if (mods::healthBarPosition == mods::BOTTOM) {
                                    barStart = ImVec2(ScreenMin.X, ScreenMax.Y + 5.0f);
                                    barEnd = ImVec2(ScreenMax.X, ScreenMax.Y + 10.0f);
                                }

                                ImU32 healthColor = (healthPercent >= 0.7f) ? mods::healthHighColor :
                                    (healthPercent >= 0.3f) ? mods::healthMidColor :
                                    mods::healthLowColor;

                                if (mods::healthBarPosition == mods::LEFT || mods::healthBarPosition == mods::RIGHT) {
                                    float barHeight = barEnd.y - barStart.y;
                                    ImVec2 fillTopLeft(barStart.x, barEnd.y - (healthPercent * barHeight));
                                    ImVec2 fillBottomRight(barEnd.x, barEnd.y);
                                    BackgroundList->AddRectFilled(barStart, barEnd, mods::barBackgroundColor);
                                    BackgroundList->AddRectFilled(fillTopLeft, fillBottomRight, healthColor);
                                    BackgroundList->AddRect(barStart, barEnd, mods::healthBarOutlineColor);
                                }
                                else {
                                    float barWidth = barEnd.x - barStart.x;
                                    ImVec2 fillTopLeft(barStart.x, barStart.y);
                                    ImVec2 fillBottomRight(barStart.x + (healthPercent * barWidth), barEnd.y);
                                    BackgroundList->AddRectFilled(barStart, barEnd, mods::barBackgroundColor);
                                    BackgroundList->AddRectFilled(fillTopLeft, fillBottomRight, healthColor);
                                    BackgroundList->AddRect(barStart, barEnd, mods::healthBarOutlineColor);
                                }
                            }

                            if (mods::bUltimatePercentage) {
                                AThreatValueAdmin* ThreatValueAdmin = SDK::UMarvelAudioLibrary::GetThreatValueAdmin(aWorld);
                                if (ThreatValueAdmin && IsValid(ThreatValueAdmin)) {
                                    auto ThreatInfoArray = ThreatValueAdmin->GetPlayerThreatInfo();
                                    for (const auto& ThreatInfo : ThreatInfoArray) {
                                        if (ThreatInfo.Character == Player) {
                                            float ultimatePercentage = ThreatInfo.UltimatePercentage;
                                            float ultPercent = ultimatePercentage / 100.0f;

                                            ImVec2 barStart, barEnd;
                                            if (mods::ultBarPosition == mods::LEFT) {
                                                barStart = ImVec2(ScreenMin.X - 10.0f, ScreenMin.Y);
                                                barEnd = ImVec2(ScreenMin.X - 5.0f, ScreenMax.Y);
                                            }
                                            else if (mods::ultBarPosition == mods::RIGHT) {
                                                barStart = ImVec2(ScreenMax.X + 5.0f, ScreenMin.Y);
                                                barEnd = ImVec2(ScreenMax.X + 10.0f, ScreenMax.Y);
                                            }
                                            else if (mods::ultBarPosition == mods::TOP) {
                                                barStart = ImVec2(ScreenMin.X, ScreenMin.Y - 10.0f);
                                                barEnd = ImVec2(ScreenMax.X, ScreenMin.Y - 5.0f);
                                            }
                                            else if (mods::ultBarPosition == mods::BOTTOM) {
                                                barStart = ImVec2(ScreenMin.X, ScreenMax.Y + 5.0f);
                                                barEnd = ImVec2(ScreenMax.X, ScreenMax.Y + 10.0f);
                                            }

                                            if (mods::ultBarPosition == mods::LEFT || mods::ultBarPosition == mods::RIGHT) {
                                                float barHeight = barEnd.y - barStart.y;
                                                ImVec2 fillTopLeft(barStart.x, barEnd.y - (ultPercent * barHeight));
                                                ImVec2 fillBottomRight(barEnd.x, barEnd.y);
                                                BackgroundList->AddRectFilled(barStart, barEnd, mods::barBackgroundColor);
                                                BackgroundList->AddRectFilled(fillTopLeft, fillBottomRight, mods::ultBarColor);
                                                BackgroundList->AddRect(barStart, barEnd, mods::ultBarOutlineColor);
                                            }
                                            else {
                                                float barWidth = barEnd.x - barStart.x;
                                                ImVec2 fillTopLeft(barStart.x, barStart.y);
                                                ImVec2 fillBottomRight(barStart.x + (ultPercent * barWidth), barEnd.y);
                                                BackgroundList->AddRectFilled(barStart, barEnd, mods::barBackgroundColor);
                                                BackgroundList->AddRectFilled(fillTopLeft, fillBottomRight, mods::ultBarColor);
                                                BackgroundList->AddRect(barStart, barEnd, mods::ultBarOutlineColor);
                                            }
                                            break;
                                        }
                                    }
                                }
                            }

                            if (mods::bShowDistance) {
                                std::string distanceStr = std::to_string(static_cast<int>(distance)) + "m";
                                ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, distanceStr.c_str());
                                ImVec2 textPos;

                                switch (mods::distancePosition)
                                {
                                case mods::TOP:
								textPos = ImVec2((ScreenMin.X + ScreenMax.X) / 2.0f - textSize.x / 2, ScreenMin.Y - textSize.y - 5);
								break;
								case mods::BOTTOM:
									textPos = ImVec2((ScreenMin.X + ScreenMax.X) / 2.0f - textSize.x / 2, ScreenMax.Y + 5);
									break;
								case mods::LEFT:
									textPos = ImVec2(ScreenMin.X - textSize.x - 5, (ScreenMin.Y + ScreenMax.Y) / 2.0f - textSize.y / 2);
									break;
								case mods::RIGHT:
									textPos = ImVec2(ScreenMax.X + 5, (ScreenMin.Y + ScreenMax.Y) / 2.0f - textSize.y / 2);
									break;

                                }

                                AddTextWithOutline(BackgroundList, font, fontSize, textPos, mods::distanceTextColor, mods::distanceTextOutlineColor, distanceStr.c_str());
                            }

                            if (mods::bShowHeroNames) {
                                // For hero actual names not player names
                                /*int heroID = Player->HeroID;
                                auto heroNameIt = mods::heroIDToName.find(heroID);
                                if (heroNameIt != mods::heroIDToName.end()) {
                                    const std::string& heroNameDisplay = heroNameIt->second;
                                    ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, heroNameDisplay.c_str());
                                    ImVec2 textPos;
                                    if (mods::heroNamePosition == mods::TOP) {
                                        textPos = ImVec2((ScreenMin.X + ScreenMax.X) / 2.0f - textSize.x / 2, ScreenMin.Y - textSize.y - 5);
                                    }
                                    else if (mods::heroNamePosition == mods::BOTTOM) {
                                        textPos = ImVec2((ScreenMin.X + ScreenMax.X) / 2.0f - textSize.x / 2, ScreenMax.Y + 5);
                                    }
                                    else if (mods::heroNamePosition == mods::LEFT) {
                                        textPos = ImVec2(ScreenMin.X - textSize.x - 5, (ScreenMin.Y + ScreenMax.Y) / 2.0f - textSize.y / 2);
                                    }
                                    else if (mods::heroNamePosition == mods::RIGHT) {
                                        textPos = ImVec2(ScreenMax.X + 5, (ScreenMin.Y + ScreenMax.Y) / 2.0f - textSize.y / 2);
                                    }
                                    ImVec2 bgStart = { textPos.x - 5.0f, textPos.y - 2.0f };
                                    ImVec2 bgEnd = { textPos.x + textSize.x + 5.0f, textPos.y + textSize.y + 2.0f };
                                    BackgroundList->AddRectFilled(bgStart, bgEnd, mods::heroNameBgColor);
                                    AddTextWithOutline(BackgroundList, font, fontSize, textPos, mods::heroNameTextColor, mods::heroNameTextOutlineColor, heroNameDisplay.c_str());
                                }*/
                                char PlayerName[32];
                                snprintf(PlayerName, sizeof(PlayerName), "%s", Player->GetPlayerName(true).ToString().c_str());
                                
                                ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, PlayerName);
                                ImVec2 textPos;

                                switch (mods::heroNamePosition) {
								case mods::TOP:
									textPos = ImVec2((ScreenMin.X + ScreenMax.X) / 2.0f - textSize.x / 2, ScreenMin.Y - textSize.y - 5);
									break;
								case mods::BOTTOM:
									textPos = ImVec2((ScreenMin.X + ScreenMax.X) / 2.0f - textSize.x / 2, ScreenMax.Y + 5);
									break;
								case mods::LEFT:
									textPos = ImVec2(ScreenMin.X - textSize.x - 5, (ScreenMin.Y + ScreenMax.Y) / 2.0f - textSize.y / 2);
									break;
								case mods::RIGHT:
									textPos = ImVec2(ScreenMax.X + 5, (ScreenMin.Y + ScreenMax.Y) / 2.0f - textSize.y / 2);
									break;
                                }
                                ImVec2 bgStart = { textPos.x - 5.0f, textPos.y - 2.0f };
                                ImVec2 bgEnd = { textPos.x + textSize.x + 5.0f, textPos.y + textSize.y + 2.0f };
                                BackgroundList->AddRectFilled(bgStart, bgEnd, mods::heroNameBgColor);
                                AddTextWithOutline(BackgroundList, font, fontSize, textPos, mods::heroNameTextColor, mods::heroNameTextOutlineColor, PlayerName);
                                
                            }

                            if (mods::bShowHealthText) {
                                std::string healthStr = std::to_string(static_cast<int>(currentHealth));
                                ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, healthStr.c_str());
                                ImVec2 textPos;

                                switch (mods::healthBarPosition) {
                                case mods::TOP:
                                    textPos = ImVec2((ScreenMin.X + ScreenMax.X) / 2.0f - textSize.x / 2, ScreenMin.Y - textSize.y - 15);
                                    break;
                                case mods::BOTTOM:
                                    textPos = ImVec2((ScreenMin.X + ScreenMax.X) / 2.0f - textSize.x / 2, ScreenMax.Y + 15);
                                    break;
                                case mods::LEFT:
                                    textPos = ImVec2(ScreenMin.X - textSize.x - 15, (ScreenMin.Y + ScreenMax.Y) / 2.0f - textSize.y / 2);
                                    break;
                                case mods::RIGHT:
                                    textPos = ImVec2(ScreenMax.X + 15, (ScreenMin.Y + ScreenMax.Y) / 2.0f - textSize.y / 2);
                                    break;
                                }
                                AddTextWithOutline(BackgroundList, font, fontSize, textPos, mods::healthTextColor, mods::healthTextOutlineColor, healthStr.c_str());
                            }

                            if (mods::bShowUltPercentageText) {
                                AThreatValueAdmin* ThreatValueAdmin = SDK::UMarvelAudioLibrary::GetThreatValueAdmin(aWorld);
                                if (ThreatValueAdmin && IsValid(ThreatValueAdmin)) {
                                    auto ThreatInfoArray = ThreatValueAdmin->GetPlayerThreatInfo();
                                    for (const auto& ThreatInfo : ThreatInfoArray) {
                                        if (ThreatInfo.Character == Player) {
                                            float ultPercentage = ThreatInfo.UltimatePercentage;
                                            std::string ultStr = std::to_string(static_cast<int>(ultPercentage)) + "%";
                                            ImVec2 textSize = font->CalcTextSizeA(fontSize, FLT_MAX, 0.0f, ultStr.c_str());
                                            ImVec2 textPos;
                                            if (mods::ultBarPosition == mods::TOP) {
                                                textPos = ImVec2((ScreenMin.X + ScreenMax.X) / 2.0f - textSize.x / 2, ScreenMin.Y - textSize.y - 15);
                                            }
                                            else if (mods::ultBarPosition == mods::BOTTOM) {
                                                textPos = ImVec2((ScreenMin.X + ScreenMax.X) / 2.0f - textSize.x / 2, ScreenMax.Y + 15);
                                            }
                                            else if (mods::ultBarPosition == mods::LEFT) {
                                                textPos = ImVec2(ScreenMin.X - textSize.x - 15, (ScreenMin.Y + ScreenMax.Y) / 2.0f - textSize.y / 2);
                                            }
                                            else if (mods::ultBarPosition == mods::RIGHT) {
                                                textPos = ImVec2(ScreenMax.X + 15, (ScreenMin.Y + ScreenMax.Y) / 2.0f - textSize.y / 2);
                                            }
                                            AddTextWithOutline(BackgroundList, font, fontSize, textPos, mods::ultTextColor, mods::ultTextOutlineColor, ultStr.c_str());
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }

                    if (mods::bSkeletonESP) {
                        DrawSkeleton(BackgroundList, Player, PlayerController);
                    }

                    if (mods::bTracerLines) {
                        ImVec2 startPos;
                        if (mods::tracerStartPos == mods::TRACER_TOP) startPos = ImVec2(displaySize.x / 2, 0);
                        else if (mods::tracerStartPos == mods::TRACER_CENTER) startPos = ImVec2(displaySize.x / 2, displaySize.y / 2);
                        else if (mods::tracerStartPos == mods::TRACER_BOTTOM) startPos = ImVec2(displaySize.x / 2, displaySize.y);
                        BackgroundList->AddLine(startPos, ImVec2(TargetHead2D.X, TargetHead2D.Y), mods::tracerColor);
                    }
                    

                    if (mods::bGlow) {
                        if (mods::bGlowIgnoreTeammates && !bIsEnemy) continue;

                        auto Mesh = Player->GetMesh();
                        if (Mesh && IsValid(Mesh)) {
                            auto status = mods::bGlowThroughWalls ?
                                SDK::ETeamOutlineShowStatus::ETOSS_Always :
                                SDK::ETeamOutlineShowStatus::ETOSS_Unoccluded;
                            Mesh->SetTeamOutlineShowStatus(status);
                        }
                    }
                }

                if (distance <= mods::aimbotMaxDistance && IsPointInAimbotCircle(mods::actualfovcircle, TargetHead2D) && (!mods::VisCheck || bIsVisible)) {
                    if (mods::bAimbotTeamCheck && !bIsEnemy) continue;
                    float distToCenter = sqrt(pow(TargetHead2D.X - screenCenter.x, 2) + pow(TargetHead2D.Y - screenCenter.y, 2));
                    enemies.push_back({ Player, currentHealth, distToCenter, distance });
                }

                if (bIsEnemy) {
                    int heroID = Player->HeroID;
                    auto heroNameIt = mods::heroIDToName.find(heroID);
                    if (heroNameIt != mods::heroIDToName.end()) {
                        std::string heroName = heroNameIt->second;
                        float ultPercentage = 0.0f;
                        float CurrentHPercent = 0.0f;
                        AThreatValueAdmin* ThreatValueAdmin = SDK::UMarvelAudioLibrary::GetThreatValueAdmin(aWorld);
                        if (ThreatValueAdmin && IsValid(ThreatValueAdmin)) {
                            auto ThreatInfoArray = ThreatValueAdmin->GetPlayerThreatInfo();
                            for (const auto& ThreatInfo : ThreatInfoArray) {
                                if (ThreatInfo.Character == Player) {
                                    float CurrentHP = ThreatInfo.Character.Get()->GetCurrentHealth();
                                    float MaxHP = ThreatInfo.Character.Get()->GetMaxHealth();
                                    CurrentHPercent = (CurrentHP / MaxHP) * 100.0f;
                                    ultPercentage = ThreatInfo.UltimatePercentage;
                                    break;
                                }
                            }
                        }
                        mods::enemyOverlayData.push_back({ heroName, ultPercentage, CurrentHPercent });
                    }
                }
            }
        }

        AMarvelBaseCharacter* TargetPlayer = nullptr;
        
        if (!enemies.empty()) {
            if (mods::aimbotPriority == mods::LEAST_HP) {
                std::sort(enemies.begin(), enemies.end(), [](const EnemyInfo& a, const EnemyInfo& b) {
                    return a.health < b.health;
                    });
            }
            else if (mods::aimbotPriority == mods::LEAST_FOV) {
                std::sort(enemies.begin(), enemies.end(), [](const EnemyInfo& a, const EnemyInfo& b) {
                    return a.distanceToCenter < b.distanceToCenter;
                    });
            }
            else if (mods::aimbotPriority == mods::LEAST_DISTANCE) {
                std::sort(enemies.begin(), enemies.end(), [](const EnemyInfo& a, const EnemyInfo& b) {
                    return a.distance < b.distance;
                    });
            }
            TargetPlayer = enemies.front().player;
            Variables::TargetPlayerPTR = TargetPlayer;

            if (mods::bAimbotSnapLine) {
                FName BoneFName = GetAimBoneName();
                if (TargetPlayer && TargetPlayer->GetMesh() && TargetPlayer->GetMesh()->DoesSocketExist(BoneFName)) {
                    FVector TargetBone3D = TargetPlayer->GetMesh()->GetSocketLocation(BoneFName);// TargetPlayer->GetMesh()->GetSocketLocation(BoneFName);
                    SDK::FVector2D TargetBone2D;
                    if (PlayerController->ProjectWorldLocationToScreen(TargetBone3D, &TargetBone2D, true)) {
                        ImVec2 targetPos = ImVec2(TargetBone2D.X, TargetBone2D.Y);
                        BackgroundList->AddLine(screenCenter, targetPos, mods::snapLineColor, mods::snapLineThickness);
                    }
                }
            }
        }


        if (mods::bRapidFire && PlayerController && AcknowledgedPawn) {
            if (AMarvelBaseCharacter* Character = reinterpret_cast<AMarvelBaseCharacter*>(AcknowledgedPawn)) {
                if (UEquipComponent* EquipComponent = Character->EquipComponent) {
                    if (EquipComponent && UKismetSystemLibrary::IsValid(EquipComponent)) {
                        if (AShootingWeapon* Weapon = EquipComponent->GetCurrentWeapon()) {
                            if (TArray<UShootingLogic_Base*> LogiBase = Weapon->ShootingLogics) {
                                for (UShootingLogic_Base* logic : LogiBase) {
                                    logic->ThisFireTime = mods::rapidFireRate;
                                    logic->NextFireTime = mods::rapidFireRate;
                                    logic->TimeBetweenShots = mods::rapidFireRate;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (mods::TriggerBot && TargetPlayer && IsValid(TargetPlayer) && TargetPlayer->GetMesh() && IsValid(TargetPlayer->GetMesh())) {
			FVector Start = PlayerController->PlayerCameraManager->GetCameraLocation();
			FVector End = Start + PlayerController->PlayerCameraManager->GetActorForwardVector() * mods::TriggerBotDistance;

            FHitResult HitResult;
            PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, &HitResult);
            const std::string& FullName = HitResult.Component.Get()->GetFullName().c_str();
            const std::string& FullObjectName = HitResult.Component.Get()->GetDefaultObj()->GetFullName().c_str();
			
            if (FullName.find("CharacterBP") != std::string::npos || FullObjectName.find("CharacterBP") != std::string::npos)
            {
                PressLeftMouseDown();
                ReleaseLeftMouse();
            }
          /*  Log(HitResult.Component.Get()->GetFullName().c_str());
            Log(HitResult.Component.Get()->GetDefaultObj()->GetFullName().c_str());*/
        }


        if (GetAsyncKeyState(mods::aimbotKey) && TargetPlayer && IsValid(TargetPlayer) && TargetPlayer->GetMesh() && IsValid(TargetPlayer->GetMesh())) {
            if (mods::aimbot) {
                FName BoneFName = GetAimBoneName();
                if (!TargetPlayer->GetMesh()->DoesSocketExist(BoneFName)) {
                    Log("DrawTransition: Aimbot - Target bone does not exist\n");
                    return;
                }

                FVector TargetBone3D = mods::bAimPrediction ?
                    PredictTargetPosition(TargetPlayer, mods::projectileSpeed) :
                    TargetPlayer->GetMesh()->GetSocketLocation(BoneFName);
                TargetBone3D.Z += mods::aimOffset;

                if (mods::bAimHumanizer) {
                    float offset = mods::humanizerLevel;
                    FVector randomOffset = FVector(
                        (float)rand() / RAND_MAX * offset - offset / 2,
                        (float)rand() / RAND_MAX * offset - offset / 2,
                        (float)rand() / RAND_MAX * offset - offset / 2
                    );
                    TargetBone3D += randomOffset;
                }

                float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(aWorld);

                FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(PlayerCameraManager->GetCameraLocation(), TargetBone3D);

                float SpeedFactor = UKismetMathLibrary::Clamp(mods::smoothing, 1.0f, 200.0f) * 0.5f;

                FRotator CurrentRotation = PlayerController->GetControlRotation();

                FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(TargetRotation, CurrentRotation);

                float NewPitch = UKismetMathLibrary::FInterpTo(CurrentRotation.Pitch, CurrentRotation.Pitch + DeltaRot.Pitch, DeltaTime, SpeedFactor);
                float NewYaw = UKismetMathLibrary::FInterpTo(CurrentRotation.Yaw, CurrentRotation.Yaw + DeltaRot.Yaw, DeltaTime, SpeedFactor);

                PlayerController->SetControlRotation(FRotator(NewPitch, NewYaw, 0));

            }
        }
        
    }
    catch (const std::exception& e) {
        Log("DrawTransition: Exception caught - %s\n", e.what());
    }
}