function startGame()

    local stage = sys_getStage("main_plot")

    if stage == 0 then
        --sys_setView(false)
        --sys_talk(0,"Hey")
        --sys_talk(0,"anyone here?")
        --sys_talk(0,"HEY!!!", 1000, 80)
        --sys_talk(0,"...")
        --sys_talk(0,"wake up!")
        --sys_setView(true)
        sys_showText("I HATE MYSELF","i_hate_myself.wav")
        sys_showText("I FEEL USELESS","i_feel_useless.wav")
        sys_showTextWithImg("text","building.png");
        sys_showTextWithImg("tex2t","new.jpg");
        sys_showTextWithImg("text3","new2.jpg");
        sys_showText("I CANT STOP THINKIN ABOUT THIS")
        sys_setStage("main_plot", 1 )

    end

    -- 后续可判断其他值，例如玩家之前离开了现在重新回来了继续玩，不过要添加本地保存文件记录退出前的stage
end

function checkQuests()

    local stage = sys_getStage("main_plot")

    -- 进度1通关条件
    if stage == 1 then

        if sys_getFlag("read_0_2_3") == true and sys_getFlag("read_0_5_5") == true then

            sys_setStage("main_plot", 2 )

            sys_talk(0,"where am i?")
            sys_talk(0,"where am i?")
        
        end
            
    end


end