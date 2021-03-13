window_frame_update();
// if the extension just finished readying up:
if (window_frame_is_ready && !was_ready) {
    was_ready = true;
    // override the close button behaviour, as an example:
    window_command_hook(window_command_close);
    //window_command_set_active(window_command_resize, 0); // can disable window resizing
    //window_command_set_active(window_command_maximize, 0); // or ability to maximize window
    //window_command_set_active(window_command_minimize, 0); // or ability to minimize window
    window_frame_set_min_size(224, 192); // how small the window can be allowed to get
    //window_frame_set_background(background_color); // if you don't fill the entire window
}

// if the close button was clicked (being overriden), close shortly:
if (window_command_check(window_command_close)) {
    leaving = 30;
}

// resize the game to fit the container window if it's visible
// (if you don't do window resizing, you don't need this)
if (window_frame_get_visible()) {
    var w = window_frame_get_width();
    var h = window_frame_get_height();
    if (w > 0 && h > 0 && surface_exists(application_surface)
    && (window_get_width() != w || window_get_height() != h)
    ) {
        // resize room (since we don't use views):
        room_width = w; room_height = h;
        // resize the game inside the frame-window to fit it's size:
        window_frame_set_region(0, 0, w, h);
        // also resize application_surface:
        surface_resize(application_surface, w, h);
    }
}

/*if (keyboard_check_pressed(vk_space)) {
    var l = window_frame_get_x();
    var t = window_frame_get_y();
    var w = window_frame_get_width();
    var h = window_frame_get_height();
    show_debug_message(string(w)+" "+string(h));
    window_frame_set_rect(l+10,t+10,w,h);
}*/

/* */
/*  */
