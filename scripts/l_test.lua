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
        sys_setMap("test_level.txt")
        
        sys_showText("you shouldn't be here","you_shouldnt_be_here.wav")
        sys_showText("you know what have you done","you_know_what_have_you_done.wav")
        sys_showText("you know what have you said","you_know_what_have_you_said.wav")
        sys_showText("they took my skin","they_took_my_skin.wav")
        sys_showText("they took my throat","they_took_my_throat.wav")
        sys_showText("they took my eyes","they_took_my_eyes.wav")
        sys_showText("they took everything from me","they_took_everything_from_me.wav")
        sys_showText("why didnt you save me?","why_didnt_you_save_me.wav")
        sys_showText("why didnt you help me?","why_didnt_you_help_me.wav")
        sys_showText("i hate you","i_hate_you.wav")
        sys_showTextWithImg("","x01.png")

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