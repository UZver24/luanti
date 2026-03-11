#pragma once

#ifdef BUILD_WITH_VR

#include <openvr.h>
#include <string>
#include <memory>

namespace video {
	class ITexture;
}

class VRManager {
private:
	vr::IVRSystem* m_pHMD;
	vr::IVRCompositor* m_pCompositor;
	bool m_bVRInitialized;
	video::ITexture *m_left_eye {nullptr};
	video::ITexture *m_right_eye {nullptr};
	vr::EVRCompositorError m_last_submit_error = vr::VRCompositorError_None;

public:
    VRManager();
    ~VRManager();

	static VRManager *getActive();
    
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
	void SetEyeTextures(video::ITexture *left, video::ITexture *right);
    
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

	static VRManager *getActive() { return nullptr; }
	
	bool InitializeVR() { return false; }
	bool IsVRReady() const { return false; }
	bool IsVRInitialized() const { return false; }
    std::string GetVRDeviceName() const { return "VR not available"; }
    std::string GetVRDriverName() const { return "VR not available"; }
    void UpdateVRPoses() {}
    void SubmitVRFrame() {}
	void SetEyeTextures(video::ITexture *, video::ITexture *) {}
    void ShutdownVR() {}
};

#endif // BUILD_WITH_VR 
