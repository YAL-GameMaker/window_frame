/// @description aspect ratio fit
var _frame_width = window_frame_get_width();
var _frame_height = window_frame_get_height();
var _game_width = 760;
var _game_height = 320;
var _scale = min(_frame_width / _game_width, _frame_height / _game_height);
var _reg_width = floor(_game_width * _scale);
var _reg_height = floor(_game_height * _scale);
var _reg_x = (_frame_width - _reg_width) div 2;
var _reg_y = (_frame_height - _reg_height) div 2;
if (_reg_width > 0 && _reg_height > 0
	&& (window_get_width() != _reg_width || window_get_height() != _reg_height)
) {
	room_width = _game_width;
	room_height = _game_height;
	window_frame_set_region(_reg_x, _reg_y, _reg_width, _reg_height);
	//trace("changing region to", _reg_x, _reg_y, room_width, room_height);
}