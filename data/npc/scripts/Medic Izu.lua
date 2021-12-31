local focuses = {}
local function isFocused(cid)
	for i, v in pairs(focuses) do
		if(v == cid) then
			return true
		end
	end
	return false
end

local function addFocus(cid)
	if(not isFocused(cid)) then
		table.insert(focuses, cid)
	end
end
local function removeFocus(cid)
	for i, v in pairs(focuses) do
		if(v == cid) then
			table.remove(focuses, i)
			break
		end
	end
end
local function lookAtFocus()
	for i, v in pairs(focuses) do
		if(isPlayer(v)) then
			doNpcSetCreatureFocus(v)
			return
		end
	end
	doNpcSetCreatureFocus(0)
end


function onCreatureAppear(cid)
end

function onCreatureDisappear(cid)
	if(isFocused(cid)) then
		selfSay("Hmph!")
		removeFocus(cid)
		if(isPlayer(cid)) then --Be sure he's online
			closeShopWindow(cid)
		end
	end
end

function onCreatureSay(cid, type, msg)
	if not isPremium(cid) then
		doPlayerSendCancel(cid, "Voc� n�o tem premmium account. (VIP)")
		return selfSay("Desculpa mas n�o posso falar com voc�.", cid)
	end
	if((msg == "hi") and not (isFocused(cid))) then
		selfSay("Eu posso te ensinar um jutsu medicinal({Chiyute}), quer aprender ?", cid)
		addFocus(cid)
	elseif((isFocused(cid)) and (msg == "yes" or msg == "sim")) then
		selfSay("Muito bem ai esta", cid)
		if(getPlayerItemCount(cid, 13400) >= 1) then
		local item = getPlayerItemById(cid, true, 13400)
				doTransformItem(item.uid, 11467)
				doPlayerSendTextMessage(cid, MESSAGE_EVENT_ADVANCE, "Voce aprendeu um novo jutsu! (Chiyute) Menu->Jutsus->Ninjutsu->Medicinal")
				selfSay('Voce aprendeu um novo jutsu! (Kai) Menu->Jutsus->Ninjutsu->Medicinal', cid)
			else
							selfSay('desculpe, eu n�o tenho nada pra voc�.', cid)
	end
	elseif((isFocused(cid)) and (msg == "bye" or msg == "goodbye" or msg == "cya")) then
		selfSay("Goodbye!", cid, true)
		removeFocus(cid)
	end
end


function onPlayerCloseChannel(cid)
	if(isFocused(cid)) then
		selfSay("Hmph!")
		closeShopWindow(cid)
		removeFocus(cid)
	end
end


function onThink()

       local direction = getCreatureLookDirection(getNpcId())
	   doCreatureSetLookDirection(getNpcId(), 1)
	for i, focus in pairs(focuses) do
		if(not isCreature(focus)) then
			removeFocus(focus)
		else
			local distance = getDistanceTo(focus) or -1
			if((distance > 4) or (distance == -1)) then
				selfSay("Hmph!")
				closeShopWindow(focus)
				removeFocus(focus)
			end
		end
	end
	lookAtFocus()
end