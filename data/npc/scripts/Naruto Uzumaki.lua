---------------------------------
------- NPC Missão 1.0 ---------
--- by Fox - xtibia.com/forum ---
---------------------------------

--------------
--- Config ---
--------------
-- item necessário para fazer a busca
local i_required = 11525

-- contagem de item necessário para fazer a busca
local i_required_count = 1

-- recompensa padrão id item (crystal coins)
local i_reward = 2148

-- contagem de recompensa id padrão (10)
local i_reward_count = 90

-- --
local storage = 60375

-- --
local questname = "Ichiraku Ramen"

-- NPC Messages --
local npc_message ={

"Eu preciso que voce me traga 1 Ramen, voce ja tem o Ramen?",
"Voce nao tem os items necessarios.", 
"YOOOOO OBRIGADO!!!, agora estou feliz por voce ser uma boa pessoa te darei uns trocados, {pegue isso.}",
"Voce ja me ajudou.",
"YOOOOO, EU AINDA SEREI UM GRANDE HOKAGE.\nEi voce ai pode me ajudar?!, estou com muita fome agora porque treinei o dia todo, e estou com vontade de comer um RAMEN do {ICHIRAKU RAMEN}, voce poderia me ajudar ?"
}

-------------------
--- End Confing --- 
-------------------

local keywordHandler = KeywordHandler:new()
local npcHandler = NpcHandler:new(keywordHandler)
NpcSystem.parseParameters(npcHandler)
local talkState = {}
function onCreatureAppear(cid) npcHandler:onCreatureAppear(cid) end
function onCreatureDisappear(cid) npcHandler:onCreatureDisappear(cid) end
function onCreatureSay(cid, type, msg) npcHandler:onCreatureSay(cid, type, msg) end
function onThink() npcHandler:onThink() end
function creatureSayCallback(cid, type, msg)
if(not npcHandler:isFocused(cid)) then
return false
end
local talkUser = NPCHANDLER_CONVBEHAVIOR == CONVERSATION_DEFAULT and 0 or cid

if(msgcontains(msg, 'missao')) then
selfSay(npc_message[5], cid)
end

if(msgcontains(msg, questname)) then
selfSay(npc_message[1], cid)
talkState[talkUser] = 1
elseif(msgcontains(msg, 'yes') and talkState[talkUser] == 1) then

if (getPlayerStorageValue(cid,storage) > 0) then
selfSay(npc_message[4], cid)
else

if(doPlayerRemoveItem(cid,i_required,i_required_count)) then
setPlayerStorageValue(cid,storage,1)
doPlayerAddItem(cid,i_reward,i_reward_count)
selfSay(npc_message[3], cid)
doSendMagicEffect(getCreaturePosition(cid), 10)
doCreatureSay(cid, "Missao Completada!", TALKTYPE_ORANGE_1)
doPlayerAddExp(cid, 3000)
else
selfSay(npc_message[2], cid)
end
end
return true
end
end
npcHandler:setCallback(CALLBACK_MESSAGE_DEFAULT, creatureSayCallback)
npcHandler:addModule(FocusModule:new())