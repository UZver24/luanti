#include "vr_manager.h"
#include "log.h"
#include "debug.h"

#include <cstdint>

#include "../../irr/src/OpenGL/Common.h"
#include "../../irr/src/COpenGLCoreTexture.h"

#ifdef BUILD_WITH_VR

static VRManager *g_active_vr_manager = nullptr;

static GLuint get_gl_texture_name(video::ITexture *texture)
{
	if (!texture)
		return 0;

	if (auto *gl3_tex = dynamic_cast<video::COpenGL3Texture *>(texture))
		return gl3_tex->getOpenGLTextureName();

	static bool warned = false;
	if (!warned) {
		warned = true;
		errorstream << "VR: texture is not OpenGL3. Set video_driver = opengl3 in minetest.conf." << std::endl;
	}

	return 0;
}

VRManager::VRManager() 
    : m_pHMD(nullptr)
    , m_pCompositor(nullptr)
    , m_bVRInitialized(false)
{
	if (!g_active_vr_manager)
		g_active_vr_manager = this;
}

VRManager::~VRManager() {
    if (m_bVRInitialized) {
        ShutdownVR();
    }

	SetEyeTextures(nullptr, nullptr);

	if (g_active_vr_manager == this)
		g_active_vr_manager = nullptr;
}

VRManager *VRManager::getActive()
{
	return g_active_vr_manager;
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

	if (!m_left_eye || !m_right_eye)
		return;

	GLuint left_id = get_gl_texture_name(m_left_eye);
	GLuint right_id = get_gl_texture_name(m_right_eye);
	if (!left_id || !right_id) {
		static bool warned = false;
		if (!warned) {
			warned = true;
			errorstream << "VR: OpenGL texture name is 0. Left=" << left_id
				<< " Right=" << right_id << std::endl;
		}
		return;
	}

	vr::Texture_t left = {(void*)(uintptr_t)left_id, vr::TextureType_OpenGL, vr::ColorSpace_Gamma};
	vr::Texture_t right = {(void*)(uintptr_t)right_id, vr::TextureType_OpenGL, vr::ColorSpace_Gamma};

	vr::EVRCompositorError left_err = m_pCompositor->Submit(vr::Eye_Left, &left);
	vr::EVRCompositorError right_err = m_pCompositor->Submit(vr::Eye_Right, &right);

	vr::EVRCompositorError err = (left_err != vr::VRCompositorError_None) ? left_err : right_err;
	if (err != vr::VRCompositorError_None && err != m_last_submit_error) {
		m_last_submit_error = err;
		errorstream << "VR Submit error: " << static_cast<int>(err) << std::endl;
	} else if (err == vr::VRCompositorError_None) {
		m_last_submit_error = err;
	}
}

void VRManager::SetEyeTextures(video::ITexture *left, video::ITexture *right)
{
	if (left == m_left_eye && right == m_right_eye)
		return;

	if (left) {
		auto size = left->getSize();
		infostream << "VR: left eye texture size " << size.Width << "x" << size.Height
			<< " fmt " << static_cast<int>(left->getColorFormat())
			<< " driver " << static_cast<int>(left->getDriverType()) << std::endl;
	}
	if (right) {
		auto size = right->getSize();
		infostream << "VR: right eye texture size " << size.Width << "x" << size.Height
			<< " fmt " << static_cast<int>(right->getColorFormat())
			<< " driver " << static_cast<int>(right->getDriverType()) << std::endl;
	}

	if (m_left_eye)
		m_left_eye->drop();
	if (m_right_eye)
		m_right_eye->drop();

	m_left_eye = left;
	m_right_eye = right;

	if (m_left_eye)
		m_left_eye->grab();
	if (m_right_eye)
		m_right_eye->grab();
}

void VRManager::ShutdownVR() {
    if (m_bVRInitialized) {
        infostream << "Shutting down VR..." << std::endl;
        vr::VR_Shutdown();
        m_pHMD = nullptr;
        m_pCompositor = nullptr;
        m_bVRInitialized = false;
    }

	SetEyeTextures(nullptr, nullptr);
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
