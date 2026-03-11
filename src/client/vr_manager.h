#pragma once

#ifdef BUILD_WITH_VR

#include <openvr.h>
#include <string>
#include <memory>

class VRManager {
private:
    vr::IVRSystem* m_pHMD;
    vr::IVRCompositor* m_pCompositor;
    bool m_bVRInitialized;
    
public:
    VRManager();
    ~VRManager();
    
    // Инициализация VR системы
    bool InitializeVR();
    
    // Проверка доступности VR
    bool IsVRReady() const { return m_bVRInitialized && m_pHMD != nullptr; }
    bool IsVRInitialized() const { return m_bVRInitialized; }
    
    // Получение информации о VR устройстве
    std::string GetVRDeviceName() const;
    std::string GetVRDriverName() const;
    
    // Обновление VR поз
    void UpdateVRPoses();
    
    // Отправка кадра в VR
    void SubmitVRFrame();
    
    // Очистка ресурсов
    void ShutdownVR();
    
    // Получение экземпляров интерфейсов
    vr::IVRSystem* GetHMD() const { return m_pHMD; }
    vr::IVRCompositor* GetCompositor() const { return m_pCompositor; }
};

#else

// Заглушка для случая, когда VR не включен
class VRManager {
public:
    VRManager() {}
    ~VRManager() {}
    
    bool InitializeVR() { return false; }
    bool IsVRReady() const { return false; }
    bool IsVRInitialized() const { return false; }
    std::string GetVRDeviceName() const { return "VR not available"; }
    std::string GetVRDriverName() const { return "VR not available"; }
    void UpdateVRPoses() {}
    void SubmitVRFrame() {}
    void ShutdownVR() {}
};

#endif // BUILD_WITH_VR 