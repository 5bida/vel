#pragma once
#include <map>
#include <unordered_map>
#include <string>
#include <vector>

namespace mods {
    enum ESPBoxType {
        ESP_BOX_2D,
        ESP_BOX_3D,
        ESP_BOX_CORNERED
    };
    enum TracerStartPosition {
        TRACER_TOP,
        TRACER_CENTER,
        TRACER_BOTTOM
    };
    enum AimbotMode {
        REGULAR,
        SILENT
    };
    enum CrosshairType {
        NONE,
        DOT,
        CROSS,
        CIRCLE
    };
    enum ThemeType {
        DARK,
        LIGHT,
        CUSTOM
    };
    enum TargetingPriority {
        LEAST_HP,
        LEAST_FOV,
        LEAST_DISTANCE
    };
    enum ESPFeaturePosition {
        LEFT,
        RIGHT,
        TOP,
        BOTTOM
    };
    enum RadarDesign {
        CIRCULAR,
        SQUARE
    };

    namespace FUCKED_UP_SHIT {
        bool shit1 = false;
        bool shit2 = false;
        bool shit3 = false;
        bool shit4 = false;


        float shit1float = 0;
        float shit2float = 0;
        float shit3float = 0;
        float shit4float = 0;
    };


    bool SelfCustomTimeDilationBool = false;
    float SelfCustomTimeDilationFloat = 0.150;

    bool CustomTimeDilationBool = false;
    float CustomTimeDilationFloat = 0.150;

    bool TriggerBot = false;
    float TriggerBotDistance = 999999999;


    bool NoSpread = false;


    bool aimbot = true;
    float smoothing = 8;
    float speed = 2;
    int fov = 8;
    int actualfovcircle = 0;
    bool aimbotFovCircle = true;
    bool VisCheck = true;
    bool esp = true;
    bool fov_changer = true;
    int fov_changer_amount = 115;
    bool LocalCheck = true;
    bool bAimbotTeamCheck = true;
    bool bESPTeamCheck = true;
    bool bHealthBar = true;
    bool bUltimatePercentage = true;
    bool bGlow = true;
    bool bGlowThroughWalls = true;
    bool bGlowIgnoreTeammates = true;
    float aimSmoothing = 8.0f;
    bool isSettingAimbotKey = false;
    int aimbotKey = VK_LBUTTON;
    std::string aimbotKeyName = "LMB";
    bool bShowHeroNames = true;

    bool bESPBox = true;
    ImColor visibleColor = ImColor(0, 255, 0);
    ImColor nonVisibleColor = ImColor(255, 0, 0);
    std::string aimHitbox = "Head";
    float aimOffset = 0.0f;
    bool bulletTP = false;
    bool bAimPrediction = true;
    bool bFilterByLowHealth = true;
    bool bSkeletonESP = true;
    bool bOutOfFOVArrows = false;
    bool bRadar = false;

    ESPBoxType espBoxType = ESP_BOX_2D;
    bool bTracerLines = false;
    TracerStartPosition tracerStartPos = TRACER_CENTER;
    bool bShowDistance = false;
    bool bShowHealthText = false;
    bool bShowUltPercentageText = false;
    bool bAimHumanizer = false;
    float humanizerLevel = 5.0f;
    float projectileSpeed = 10000.0f;
    bool isSettingMenuKey = false;
    int menuToggleKey = VK_INSERT;
    std::string menuToggleKeyName = "Insert";
    AimbotMode aimbotMode = REGULAR;

    CrosshairType crosshairType = NONE;
    ImColor crosshairColor = ImColor(255, 255, 255);
    float crosshairSize = 5.0f;
    float crosshairThickness = 1.0f;
    bool bSpinbot = false;
    bool bSpinbotX = false;
    bool bSpinbotY = false;
    bool bSpinbotZ = false;
    float SpiningSpeedX = 10;
    float SpiningSpeedY = 10;
    float SpiningSpeedZ = 10;
    ThemeType currentTheme = DARK;
    ImColor customWindowBg = ImColor(0.1f, 0.1f, 0.1f, 1.0f);
    ImColor customText = ImColor(1.0f, 1.0f, 1.0f, 1.0f);
    std::string settingHotkeyFor = "";
    int espHotkey = 0;
    std::string espHotkeyName = "None";
    int glowHotkey = 0;
    std::string glowHotkeyName = "None";
    int bulletTPHotkey = 0;
    std::string bulletTPHotkeyName = "None";
    int spinbotHotkey = 0;
    std::string spinbotHotkeyName = "None";
    int SelfTimeHotkey = 0;
    std::string SelfTimekeyName = "None";
    bool bRapidFire = false;
    float rapidFireRate = 0.1f;
    int rapidFireHotkey = 0;
    std::string rapidFireHotkeyName = "None";

    // Existing color customization
    ImColor aimbotFovCircleColor = ImColor(255, 255, 255);
    ImColor heroNameTextColor = ImColor(255, 255, 255);
    ImColor heroNameBgColor = ImColor(0, 0, 0, 150);
    ImColor skeletonESPColor = ImColor(255, 255, 255);
    ImColor outOfFOVArrowsColor = ImColor(255, 0, 0);
    ImColor radarBgColor = ImColor(0, 0, 0, 100);
    ImColor radarLocalColor = ImColor(0, 255, 0);
    ImColor radarEnemyColor = ImColor(255, 0, 0);
    ImColor tracerColor = ImColor(255, 255, 0);

    // New color customization
    ImColor distanceTextColor = ImColor(255, 255, 255);
    ImColor distanceTextOutlineColor = ImColor(0, 0, 0);
    ImColor heroNameTextOutlineColor = ImColor(0, 0, 0);
    ImColor healthTextColor = ImColor(255, 255, 255);
    ImColor healthTextOutlineColor = ImColor(0, 0, 0);
    ImColor ultTextColor = ImColor(255, 255, 255);
    ImColor ultTextOutlineColor = ImColor(0, 0, 0);
    ImColor healthHighColor = ImColor(0, 255, 0, 250);
    ImColor healthMidColor = ImColor(255, 255, 0, 250);
    ImColor healthLowColor = ImColor(255, 0, 0, 250);
    ImColor ultBarColor = ImColor(255, 255, 0, 250);
    ImColor healthBarOutlineColor = ImColor(0, 0, 0);
    ImColor ultBarOutlineColor = ImColor(0, 0, 0);
    ImColor barBackgroundColor = ImColor(0, 0, 0, 200);

    namespace Experimental {
        bool SmallPerson = false;
        float SmallPersonScale = 0.5f;
        bool HideLocalPlayer = false;
        bool StreamProof = true;
    }

    float aimbotMaxDistance = 500.0f;
    float espMaxDistance = 500.0f;


    static const std::unordered_map<int, std::string> heroIDToName = {
        {1011, "Hulk"}, {1014, "Punisher"}, {1015, "Storm"}, {1016, "Loki"}, {1017, "Human Torch"},
        {1018, "Doctor Strange"}, {1020, "Mantis"}, {1021, "Hawkeye"}, {1022, "Captain America"},
        {1023, "Rocket Raccoon"}, {1024, "Hela"}, {1025, "Dagger"}, {1026, "Black Panther"},
        {1027, "Groot"}, {1029, "Magik"}, {1030, "Moon Knight"}, {1031, "Luna Snow"},
        {1032, "Squirrel Girl"}, {1033, "Black Widow"}, {1034, "Iron Man"}, {1035, "Venom"},
        {1036, "Spider Man"}, {1037, "Magneto"}, {1038, "Scarlet Witch"}, {1039, "Thor"},
        {1040, "Mister Fantastic"}, {1041, "Winter Soldier"}, {1042, "Peni Parker"},
        {1043, "Star Lord"}, {1045, "Namor"}, {1046, "Adam Warlock"}, {1047, "Jeff"},
        {1048, "Psylocke"}, {1049, "Wolverine"}, {1050, "Invisible Woman"}, {1051, "The Thing"},
        {1052, "Iron Fist"}, {4016, "Galacta Bot"}, {4018, "Galacta Bot Plus"}
    };

    TargetingPriority aimbotPriority = LEAST_HP;
    bool bAimbotSnapLine = true;
    ImColor snapLineColor = ImColor(255, 0, 0);
    float snapLineThickness = 1.0f;

    // Updated font rendering customizations
    std::vector<std::string> fontNames;
    std::vector<ImFont*> availableFonts;
    int selectedESPFontIndex = 0;  // For ESP renders
    int selectedMenuFontIndex = 0; // For menu UI
    float textScale = 1.0f;
    float baseFontSize = 12.0f;

    bool bESPBoxOutline = true;
    ImColor espBoxOutlineColor = ImColor(0, 0, 0);
    float espBoxThickness = 1.0f;

    ESPFeaturePosition healthBarPosition = LEFT;
    ESPFeaturePosition ultBarPosition = RIGHT;
    ESPFeaturePosition distancePosition = BOTTOM;
    ESPFeaturePosition heroNamePosition = TOP;

    bool bDynamicFOV = false;
    float minFovMultiplier = 0.5f;
    float maxFovMultiplier = 1.5f;

    bool bShowEnemyOverlay = true;
    struct EnemyInfoOverlay {
        std::string name;
        float ultPercentage;
        float CurrentHP;
    };

    std::vector<EnemyInfoOverlay> enemyOverlayData;

    std::map<std::wstring, ImVec2> capturedBoneRelativePositions;
    std::vector<ImVec2> capturedBoxCornerRelativePositions;
    bool skeletonCaptured = false;
    bool bShowESPPreview = false;

    // New radar design variable
    RadarDesign radarDesign = CIRCULAR;
    float closestDistance = FLT_MAX;  // Initialized to maximum float value
}

namespace Keys {
    SDK::FKey Insert;
    SDK::FKey LeftMouseButton;
}