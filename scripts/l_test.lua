function startGame()

    local stage = sys_getStage("main_plot")

    if stage == 0 then
        sys_setView(false)
        sys_talk(0,"nigga")
        sys_talk(1,"where the fuck is my chinko")
        sys_talk(0,"someone has solen my chinko while i was sleeping!?")
        sys_talk(1,"i gotta find that shit nigga,i cant wait anymore")
        sys_setView(true)
        sys_setStage("main_plot", 1 )

        -- ui:showQuest_ManagerText("我需要找到 Bob。")
        
    elseif stage == 1 then
        sys_talk("player","stage = 1")
        
    elseif stage == 2 then
    end
end

-- C++ 在玩家与 "npc_bob" 交互时会调用这个
function onInteract_npc_bob()
    -- "Quest_Manager" 对象来自 C++ 绑定
    local stage = Quest_Manager:getStage("main_plot")

    if stage == 1 then
        -- 阶段 1 (正在找 Bob)：执行 "2"
        Player_Controller:talk("你好，请问你是？")
        -- 'npc' 也需要被 C++ 绑定
        -- npc:get("npc_bob"):talk("我是Bob。你看起来需要帮助。")
        
        -- 检查玩家背包 (假设 C++ 绑定了 "Player_Controller:hasItem")
        if Player_Controller:hasItem("ancient_coin") == true then
            Player_Controller:talk("我这有一枚奇怪的硬币...")
            -- npc:get("npc_bob"):talk("天哪！这就是我需要的！")
            
            -- 推进剧情到阶段 3
            Quest_Manager:setStage("main_plot", 3)
            -- ui:showQuest_ManagerText("把硬币交给 Bob。")
        
        else
            -- 推进剧情到阶段 2
            Quest_Manager:setStage("main_plot", 2)
            -- ui:showQuest_ManagerText("Bob 让我去帮他找'古老硬币'。")
            Quest_Manager:setFlag("bob_item_name", "古老硬币")
        end
        
    elseif stage == 2 then
        -- 阶段 2 (正在帮 Bob 找东西)
        if Player_Controller:hasItem("ancient_coin") == true then
            -- 找到了，推进到阶段 3
            Player_Controller:talk("我找到了'古老硬币'！")
            -- npc:get("npc_bob"):talk("太好了！")
            Quest_Manager:setStage("main_plot", 3)
            -- ui:showQuest_ManagerText("把硬币交给 Bob。")
        else
            -- 还没找到
            -- npc:get("npc_bob"):talk("你找到'古老硬币'了吗？")
        end
        
    elseif stage == 3 then
        -- 阶段 3 (把硬币给 Bob)
        -- C++ 绑定: Player_Controller:removeItem("ancient_coin")
        Player_Controller:talk("这是你...咦？硬币不见了！")
        -- npc:get("npc_bob"):talk("...")
        
        -- 推进剧情到阶段 4
        Quest_Manager:setStage("main_plot", 4)
        
    else
        -- 剧情的
        -- npc:get("npc_bob"):talk("天气真好。")
    end
end