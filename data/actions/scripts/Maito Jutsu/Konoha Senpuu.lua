local temp = {
exhausted = 2,
}
local confg = {
level = 5,
chakra = 5
}

function onUse(cid, item, fromPosition, itemEx, toPosition)
-----[Restrições]-----
	if	impossibleUse(cid) then
		return true
	end

	if checkDoing(cid) then
		return true
	end	
-----[Restrições]-----
	if isPlayer(cid) then
		removeChakraLife(cid , - confg.chakra)
		doPlayerCastSpell(cid, "#YZmaito1")
		setPlayerStorageValue(cid, sto_jutsu[1], os.time() + temp.exhausted)
	end
end