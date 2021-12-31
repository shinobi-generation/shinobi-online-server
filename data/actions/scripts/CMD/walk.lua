	function onUse(cid, item, itemEx)

	local info = {
		speed = 200,
		storage = 2135,
	}

	if not isCreature(cid) then
		return true
	end
	if checkCmd(cid, "mold") then
		return true
	end

	if checkCmd(cid, "rest") then
		return true
	end

	if(getPlayerStorageValue(cid, sto_jutsu[1]) > os.time() and getPlayerStorageValue(cid, sto_jutsu[1]) < 100+os.time()) then
		doPlayerSendTextMessage(cid, 24, "Voce ja esta fazendo um jutsu")
	return true
	end

	
	if checkJutsu(cid, "Kagemane") then
		return doPlayerSendCancel(cid, "you cannot use jutsu")
	end
	if checkJutsu(cid, "Nikudan") then
		return doPlayerSendCancel(cid, "you cannot use jutsu")
	end	
         if getPlayerStorageValue(cid, sto_nara[2]) == 0 then
               return doPlayerSendCancel(cid, "Sorry this is not possible.")
         end
		
	if checkJutsu(cid, "Hari") then
		return doPlayerSendCancel(cid, "you cannot use jutsu")
	end
	if getPlayerStorageValue(cid, sto_gen[1]) == 0 then
		return doPlayerSendCancel(cid, "Sorry this is not possible.")		
	end	

	if getPlayerStorageValue(cid, temp.storage) >= os.time() then 
		return doPlayerSendCancel(cid, "You are exhausted.")
	end

	if not infoClan[getPlayerVocation(cid)] then
		return doPlayerSendCancel(cid, "You cant use this item.")
	end

	local outfitN = getCreatureOutfit(cid)
		outfitN.lookType = infoClan[getPlayerVocation(cid)].outfit
		
			
	local Aoutfit = getCreatureOutfit(cid)
	Aoutfit.lookType = infoClanS[getPlayerVocation(cid)].outfit

	if (getCreatureStorage(cid, info.storage) == 0) then
		if getPlayerStorageValue(cid, 87564) >= 1 then
		setPlayerStorageValue(cid,87564, 0)
		doSendMagicEffect(getThingPos(cid), 110)
		end
		if getPlayerStorageValue(cid, initStorages.graduation) == "Academy Student" then
		cmdMove(cid, Aoutfit)
		elseif getPlayerStorageValue(cid, initStorages.graduation) ~= "Academy Student" then
		cmdMove(cid, outfitN)
		end
		doPlayerSendCancel(cid, "You are now walking.")
		doChangeSpeed(cid, -getCreatureSpeed(cid))
		doChangeSpeed(cid, getCreatureBaseSpeed(cid))
		setPlayerStorageValue(cid, info.storage, 1)
		setPlayerStorageValue(cid, temp.storage, os.time() + temp.exhausted)
		else
		doPlayerSendCancel(cid, "You are already walking.") 
	end
	return true
end