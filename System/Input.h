class Input
{
public:
    static void Startup();
    static void Shutdown();

    static void Tick();
    static void OnInput(LPARAM lParam);

    static char  key_down[255];
    static bool  mousebutton_left_down;
    static bool  mousebutton_middle_down;
    static bool  mousebutton_right_down;
    static float mouse_dx;
    static float mouse_dy;

	static float gamepad_left_x, gamepad_left_y;
	static float gamepad_right_x, gamepad_right_y;
	static float gamepad_left_trigger;
	static float gamepad_right_trigger;
	static bool  gamepad_button_a;
	static bool  gamepad_button_b;
	static bool  gamepad_button_x;
	static bool  gamepad_button_y;
    static bool  gamepad_left_shoulder;
    static bool  gamepad_right_shoulder;
	static bool  gamepad_left_pop;
	static bool  gamepad_right_pop;
};
