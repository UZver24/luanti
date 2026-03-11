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
    if (m_bVRInitialized) {
        ShutdownVR();
    }
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
    
    // Композитор готов к использованию
    
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

void VRManager::UpdateVRPoses() {
    if (!m_bVRInitialized || !m_pCompositor || !m_pHMD) return;
    
    // Получение поз от VR системы
    vr::TrackedDevicePose_t trackedDevicePoseArray[vr::k_unMaxTrackedDeviceCount];
    vr::EVRCompositorError error = m_pCompositor->WaitGetPoses(trackedDevicePoseArray, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
    
    if (error != vr::VRCompositorError_None) {
        // Тихо игнорируем ошибки композитора
        return;
    }
    
    // Здесь можно обработать позы для управления камерой
    // Пока просто получаем позы без обработки
}

void VRManager::SubmitVRFrame() {
    if (!m_bVRInitialized || !m_pCompositor || !m_pHMD) return;
    
    // Отправка кадра в VR композитор
    // Пока просто вызываем композитор без передачи текстур
    // В будущем здесь будет передача стерео текстур
    vr::EVRCompositorError leftError = m_pCompositor->Submit(vr::Eye_Left, nullptr);
    vr::EVRCompositorError rightError = m_pCompositor->Submit(vr::Eye_Right, nullptr);
    
    // Тихо игнорируем ошибки композитора
    (void)leftError;
    (void)rightError;
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
void VRManager::UpdateVRPoses() {}
void VRManager::SubmitVRFrame() {}
void VRManager::ShutdownVR() {}

#endif // BUILD_WITH_VR 