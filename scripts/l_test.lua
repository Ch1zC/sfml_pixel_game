function startGame()

    local stage = sys_getStage("main_plot")

    if stage == 0 then
        sys_setView(false)
        sys_talk("player","nigga")
        sys_talk("player","where the fuck is my chinko")
        sys_talk("player","someone has solen my chinko while i was sleeping!?")
        sys_talk("player","i gotta find that shit nigga,i cant wait anymore")
        sys_setView(true)
        sys_setStage("main_plot", 1 )
        
    elseif stage == 1 then
        sys_talk("player","stage = 1")
        
    elseif stage == 2 then

    end
end
