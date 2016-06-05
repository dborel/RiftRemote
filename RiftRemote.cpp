#define WINVER 0X0500
#include <Windows.h>
#include <cstdlib>
#include <OVR_CAPI.h>
#include <set>

ovrSession g_session = nullptr;
ovrControllerType g_controllerType = ovrControllerType_Remote;

void Initialize()
{
    ovr_Initialize();
    ovr_Create(&g_session, nullptr);
}

void Shutdown()
{
    ovr_Destroy(g_session);
    ovr_Shutdown();
} 

void PressWindowsButton(int vk, bool value)
{
    static auto s_pressedKeys = new std::set<int>();

    if (value == (s_pressedKeys.count(vk) > 0))
        return;

    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;
    input.ki.wVk = vk;

    if (value)
    {
        input.ki.dwFlags = 0;
        s_pressedKeys.insert(vk);
    }
    else
    {
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        s_pressedKeys.erase(vk);
    }

    SendInput(1, &input, sizeof(INPUT));
}

void SendKeysForInputState(const ovrInputState& state)
{
    PressWindowsButton(VK_BROWSER_FORWARD, state.Buttons & ovrButton_Up);
    PressWindowsButton(VK_BROWSER_BACK, state.Buttons & ovrButton_Down);
    PressWindowsButton(VK_MEDIA_PREV_TRACK, state.Buttons & ovrButton_Left);
    PressWindowsButton(VK_MEDIA_NEXT_TRACK, state.Buttons & ovrButton_Right);
    PressWindowsButton(VK_MEDIA_PLAY_PAUSE, state.Buttons & ovrButton_Enter);
    PressWindowsButton(VK_MEDIA_STOP, state.Buttons & ovrButton_Back);
}

int main()
{
    Initialize();

    atexit(Shutdown);

    while (true)
    {
        Sleep(10);

        ovrInputState inputState;
        memset(&inputState, 0, sizeof(ovrInputState));

        ovr_GetInputState(g_session, g_controllerType, &state);

        SendKeysForInputState(state);
    }
}
