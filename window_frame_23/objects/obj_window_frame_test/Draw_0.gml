draw_set_font(fnt_window_frame_test);
draw_set_color(c_white);
var s;
if (leaving > 0) {
    s = "See you later!";
    if (--leaving <= 0) game_end();
} else s = string_format(current_time / 1000, 0, 1) + "s"
    + "\nTry things:"
    + "\n1: Enable border"
    + "\n2: Disable border"
    + "\n3: Toggle fulscreen"
    + "\n4: Minimize"
    + "\nT: Toggle stay-on-top"
    + "\nSize: " + string(window_get_width()) + "x" + string(window_get_height());
draw_text(5, 5, s);
if (keyboard_check_pressed(ord("1"))) window_frame_set_visible(true);
if (keyboard_check_pressed(ord("2"))) window_frame_set_visible(false);
if (keyboard_check_pressed(ord("3"))) window_frame_set_fullscreen(!window_get_fullscreen());
if (keyboard_check_pressed(ord("4"))) window_command_run(window_command_minimize);
if (keyboard_check_pressed(ord("T"))) window_set_topmost(!window_get_topmost());
draw_circle(mouse_x, mouse_y, 10, true);
if (gamepad_is_connected(0)) draw_line(mouse_x, mouse_y,
    mouse_x + gamepad_axis_value(0, gp_axislh) * 100,
    mouse_y + gamepad_axis_value(0, gp_axislv) * 100
);

