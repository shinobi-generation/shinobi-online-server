local temp = {
exhausted = 4,
}


local confg = {
level = 45,
chakra = 80,
}
function onUse(cid, item, fromPosition, itemEx, toPosition)
    if not isCreature(cid) then
        return true
    end
-----[Restri��es]-----
	if	impossibleUse(cid) then
		return true
	end

	if checkDoing(cid) then
		return true
	end	
-----[Restri��es]-----
    if isPlayer(cid) then
		removeChakraLife(cid, - confg.chakra)
		doPlayerCastSpell(cid, "#YZhyuuga6")
        setPlayerStorageValue(cid, sto_jutsu[1], os.time() + temp.exhausted)
    end
end