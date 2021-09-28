keep_aspect = false;

// debug menu stuff:
function tm_start(_x = 5, _y = 5) {
	draw_set_font(fnt_test);
	draw_set_color(c_white);
	tm_x = _x;
	tm_y = _y;
	tm_width = 240;
}
tm_start();

function tm_button(_text) {
	var _w = tm_width, _h = string_height_ext(_text, -1, _w - 4);
	var _x = tm_x, _y = tm_y;
	var _mx = mouse_x, _my = mouse_y;
	var _over = _mx >= _x && _my >= _y && _mx < _x + _w && _my < _y + _h;
	draw_set_alpha(_over ? 0.2 : 0.1);
	draw_rectangle(_x, _y, _x + _w, _y + _h, false);
	draw_set_alpha(1);
	draw_text_ext(_x + 2, _y, _text, -1, _w - 4);
	tm_y = _y + _h + 5;
	return _over && mouse_check_button_released(mb_left);
}

function tm_toggle(_text, _value) {
	return tm_button(_text + ": " + (_value ? "ON" : "OFF"));
}

// REMINDER: Windows won't allow you to run window commands
// on mouse press, only on mouse release! Them's the rules.
function tm_command_toggle(_cmd, _text) {
	var _on = window_command_get_active(_cmd);
	var _rw = 20;
	var _w = tm_width;
	tm_width -= _rw + 5;
	var _x0 = tm_x, _y0 = tm_y;
	if (tm_toggle(_text, _on)) window_command_set_active(_cmd, !_on);
	var _y1 = tm_y;
	tm_x += tm_width + 5;
	tm_y = _y0;
	tm_width = _rw;
	if (tm_button(">")) window_command_run(_cmd);
	tm_width = _w;
	tm_x = _x0;
	tm_y = _y1;
}

trigger_count = {}; /// @is {struct}
function tm_command_hook(_cmd, _text) {
	var _on = window_command_get_hooked(_cmd);
	
	var i = trigger_count[$ _cmd];
	if (i == undefined) i = 0;
	if (window_command_check(_cmd)) {
		i += 1;
		trigger_count[$ _cmd] = i;
	}
	
	if (tm_toggle(_text + " [" + string(i) + "]", _on)) {
		window_command_set_hooked(_cmd, !_on);
	}
}