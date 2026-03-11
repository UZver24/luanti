```mermaid
graph TD
    A[hellovr_opengl_main.cpp] --> B[CMainApplication Class]
    A --> C[CGLRenderModel Class]
    A --> D[Main Function]
    
    B --> B1[BInit()]
    B --> B2[BInitGL()]
    B --> B3[BInitCompositor()]
    B --> B4[HandleInput()]
    B --> B5[RunMainLoop()]
    B --> B6[ProcessVREvent()]
    B --> B7[RenderFrame()]
    B --> B8[CreateAllShaders()]
    B --> B9[SetupTexturemaps()]
    B --> B10[SetupScene()]
    
    C --> C1[CGLRenderModel Constructor]
    C --> C2[~CGLRenderModel()]
    C --> C3[LoadMesh()]
    C --> C4[Cleanup()]
    C --> C5[Draw()]
    
    D --> D1[Create CMainApplication]
    D --> D2[BInit()]
    D --> D3[RunMainLoop()]
    D --> D4[Shutdown()]
    
    subgraph "OpenVR Integration"
        B1 --> B11[OpenVR API Calls]
        B2 --> B21[OpenGL Initialization]
        B3 --> B31[Compositor Setup]
        B4 --> B41[Input Handling]
        B6 --> B61[Event Processing]
        B7 --> B71[Frame Rendering]
    end
    
    subgraph "OpenGL Rendering"
        B7 --> B72[Shader Usage]
        B7 --> B73[Texture Mapping]
        B7 --> B74[Model Drawing]
        B9 --> B91[Texture Loading]
        B8 --> B81[Shader Compilation]
    end
    
    subgraph "VR Hardware"
        B41 --> B42[Controller Input]
        B41 --> B43[Headset Tracking]
        B61 --> B62[VR Events]
        B11 --> B12[VR Device Management]
    end
```