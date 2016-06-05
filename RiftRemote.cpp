#define WINVER 0X0500
#include <Windows.h>
#include <cstdlib>
#include <OVR_CAPI.h>
#include <set>

ovrSession g_session = nullptr;
ovrControllerType g_controllerType = ovrControllerType_Remote; //TODO: Support Touch/Xbox controller?

/// Tear-down logic.
void Shutdown()
{
	// Do nothing if already shutdown.
	if (g_session == nullptr)
		return;

    ovr_Destroy(g_session);
	g_session = nullptr;

    ovr_Shutdown();
}

/// Start-up logic.
void Initialize()
{
	// Do nothing if already initialized.
	if (g_session != nullptr)
		return;

	// Fail now and retry later if initialization fails.
	if (!OVR_SUCCESS(ovr_Initialize(nullptr)))
		return;

	// Dummy because you can't pass nullptr.
	ovrGraphicsLuid luid;
	memset(&luid, 0, sizeof(ovrGraphicsLuid));

	// Set up the session we will need to query input each frame.
	if (!OVR_SUCCESS(ovr_Create(&g_session, &luid)))
		Shutdown();
}

/// Simulates a Windows keypress.
void PressWindowsButton(int vk, bool value)
{
	// If a key is in this set, it is currently pressed.
    static std::set<int> s_pressedKeys;

	// Do nothing if we are re-setting an already-set state.
    if (value == (s_pressedKeys.count(vk) > 0))
        return;

	// Set up a keyboard event notification.
    INPUT input;
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0;
    input.ki.time = 0;
    input.ki.dwExtraInfo = 0;
    input.ki.wVk = vk;

	// Set the key as down or up, depending on value.
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

	// Send the event to Windows.
    SendInput(1, &input, sizeof(INPUT));
}

/// Converts each Oculus input button state to a keyboard press/release in Windows.
void SendKeysForInputState(const ovrInputState& state)
{
    PressWindowsButton(VK_BROWSER_FORWARD, (state.Buttons & ovrButton_Up) != 0);
    PressWindowsButton(VK_BROWSER_BACK, (state.Buttons & ovrButton_Down) != 0);
    PressWindowsButton(VK_MEDIA_PREV_TRACK, (state.Buttons & ovrButton_Left) != 0);
    PressWindowsButton(VK_MEDIA_NEXT_TRACK, (state.Buttons & ovrButton_Right) != 0);
    PressWindowsButton(VK_MEDIA_PLAY_PAUSE, (state.Buttons & ovrButton_Enter) != 0);
    PressWindowsButton(VK_MEDIA_STOP, (state.Buttons & ovrButton_Back) != 0);
}

int main()
{
	// Register a callback to clean up when the process exits.
    atexit(Shutdown);

    while (true)
    {
		// Retry initialization each frame until it succeeds. It will early-out thereafter.
		Initialize();

		// Each frame is 10ms to save CPU.
        Sleep(10);

		// Handle button input from LibOVR.

        ovrInputState inputState;
        memset(&inputState, 0, sizeof(ovrInputState));

        if (OVR_SUCCESS(ovr_GetInputState(g_session, g_controllerType, &inputState)))
			SendKeysForInputState(inputState);
    }
}
