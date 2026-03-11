```mermaid
graph TD
    A[game.cpp] --> B[Game Class]
    A --> C[GameGlobalShaderUniformSetter]
    A --> D[GameGlobalShaderUniformSetterFactory]
    A --> E[NodeShaderConstantSetter]
    A --> F[SoundMaker]
    A --> G[NodeDugEvent]
    
    B --> B1[Game Constructor]
    B --> B2[Game Destructor]
    B --> B3[startup()]
    B --> B4[run()]
    B --> B5[shutdown()]
    B --> B6[init()]
    B --> B7[initSound()]
    B --> B8[createServer()]
    B --> B9[createClient()]
    B --> B10[connectToServer()]
    B --> B11[getServerContent()]
    B --> B12[processQueues()]
    B --> B13[updateInteractTimers()]
    
    subgraph "Core Game Loop"
        B4 --> B41[Frame Processing]
        B4 --> B42[Input Handling]
        B4 --> B43[Network Updates]
        B4 --> B44[Rendering]
        B4 --> B45[Audio Processing]
    end
    
    subgraph "Game Initialization"
        B3 --> B31[Setup Signals]
        B3 --> B32[Initialize Components]
        B6 --> B61[Initialize Graphics]
        B6 --> B62[Initialize Audio]
        B6 --> B63[Initialize Network]
        B9 --> B91[Create Client Instance]
        B10 --> B101[Connect to Server]
    end
    
    subgraph "Game Components"
        B --> B14[ActiveObjectMgr]
        B --> B15[ClientEnvironment]
        B --> B16[RenderingEngine]
        B --> B17[InputHandler]
        B --> B18[LocalPlayer]
        B --> B19[ChatBackend]
        B --> B20[Camera]
        B --> B21[HUD]
        B --> B22[Particles]
        B --> B23[Clouds]
        B --> B24[Minimap]
        B --> B25[SoundManager]
    end
    
    subgraph "Networking"
        B10 --> B102[Server Connection]
        B10 --> B103[Authentication]
        B10 --> B104[Data Sync]
        B11 --> B111[Content Download]
        B11 --> B112[Map Loading]
    end
    
    subgraph "Rendering"
        B --> B26[Shader Management]
        B --> B27[Mesh Generation]
        B --> B28[Texture Management]
        B --> B29[Sky Rendering]
        B --> B30[Map Block Rendering]
        B --> B31[Particle Rendering]
        B --> B32[HUD Rendering]
    end
    
    subgraph "Audio"
        D --> D1[Sound Manager]
        F --> F1[Sound Playback]
        B25 --> B251[Audio Initialization]
        B25 --> B252[Sound Effects]
        B25 --> B253[Musical Themes]
    end
    
    subgraph "Input Handling"
        B18 --> B181[Player Input]
        B17 --> B171[Keyboard Input]
        B17 --> B172[Mouse Input]
        B17 --> B173[Joystick Input]
        B17 --> B174[Touch Controls]
    end
    
    subgraph "Game State"
        B --> B33[Game States]
        B --> B34[Loading Screens]
        B --> B35[Pause Menu]
        B --> B36[Main Menu]
        B --> B37[In-Game UI]
    end
```