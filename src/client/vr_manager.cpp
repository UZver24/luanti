#include "vr_manager.h"
#include "log.h"
#include "debug.h"

#ifdef BUILD_WITH_VR

VRManager::VRManager() 
    : m_pHMD(nullptr)
    , m_pCompositor(nullptr)
    , m_bVRInitialized(false)
{
}

VRManager::~VRManager() {
    ShutdownVR();
}

bool VRManager::InitializeVR() {
    // Инициализация OpenVR
    vr::EVRInitError eError = vr::VRInitError_None;
    m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);
    
    if (eError != vr::VRInitError_None) {
        m_pHMD = nullptr;
        errorstream << "Unable to init VR runtime: " 
                   << vr::VR_GetVRInitErrorAsEnglishDescription(eError) << std::endl;
        return false;
    }
    
    // Получение интерфейса композитора
    m_pCompositor = vr::VRCompositor();
    if (!m_pCompositor) {
        errorstream << "Unable to initialize VR compositor!" << std::endl;
        vr::VR_Shutdown();
        m_pHMD = nullptr;
        return false;
    }
    
    m_bVRInitialized = true;
    
    // Вывод информации о VR устройстве
    std::string deviceName = GetVRDeviceName();
    std::string driverName = GetVRDriverName();
    
    infostream << "VR initialized successfully!" << std::endl;
    infostream << "Device: " << deviceName << std::endl;
    infostream << "Driver: " << driverName << std::endl;
    
    return true;
}

std::string VRManager::GetVRDeviceName() const {
    if (!m_pHMD) return "No HMD";
    
    char buf[1024];
    uint32_t size = sizeof(buf);
    m_pHMD->GetStringTrackedDeviceProperty(
        vr::k_unTrackedDeviceIndex_Hmd,
        vr::Prop_TrackingSystemName_String,
        buf, size);
    
    return std::string(buf);
}

std::string VRManager::GetVRDriverName() const {
    if (!m_pHMD) return "No HMD";
    
    char buf[1024];
    uint32_t size = sizeof(buf);
    m_pHMD->GetStringTrackedDeviceProperty(
        vr::k_unTrackedDeviceIndex_Hmd,
        vr::Prop_SerialNumber_String,
        buf, size);
    
    return std::string(buf);
}

void VRManager::ShutdownVR() {
    if (m_bVRInitialized) {
        infostream << "Shutting down VR..." << std::endl;
        vr::VR_Shutdown();
        m_pHMD = nullptr;
        m_pCompositor = nullptr;
        m_bVRInitialized = false;
    }
}

#else

// Реализация заглушки
VRManager::VRManager() {}
VRManager::~VRManager() {}
bool VRManager::InitializeVR() { return false; }
std::string VRManager::GetVRDeviceName() const { return "VR not available"; }
std::string VRManager::GetVRDriverName() const { return "VR not available"; }
void VRManager::ShutdownVR() {}

#endif // BUILD_WITH_VR 