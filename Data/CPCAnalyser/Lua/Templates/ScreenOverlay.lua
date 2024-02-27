
-- this gets called after the emulator screen has been drawn
function OnDrawScreenOverlay(screenOverlay)
    
    -- draw screen centre
    screenOverlay:drawCoord(320/2,200/2)

    -- draw UI under screen
    imgui.Text("This came from the Lua function OnDrawScreenOverlay")
end

-- Register Function
SetScreenDrawOverlayFunc(OnDrawScreenOverlay)