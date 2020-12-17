# window_frame
**Quick links:** [documentation](https://yal.cc/r/17/window_frame/) · [itch.io page](https://yellowafterlife.itch.io/window-freeze-fix)  
**Versions:** GameMaker: Studio, GameMaker Studio 2  
**Platforms:** Windows, Windows (YYC)

This fancy extension addresses a quirk where GameMaker games cease to update while the window is being dragged around or resized.

The issue is Windows-specific behaviour - unless you run game logic and rendering on separate threads, system's dragging/resizing logic overtakes anything else and no other code executes until that's done.

The approach this extension takes is best called "a delightful hack" - the DLL starts up an simple helper program, into the window of which the game's window is then inserted as a child window.

This has the game window continue to run while the window is being manipulated, as well as allowing to enable/disable the window frame, allowing to also use it for borderless fullscreen.

The extension also includes its own versions of [Window Commands](https://yellowafterlife.itch.io/gamemaker-window-commands) functions.

**Note:** Don't forget to disable the default border in Global Game Settings - Windows - Graphics (GMS1) / Game Options - Windows - Graphics (GMS1) / Game Options - Windows - Graphics (GMS2)!
