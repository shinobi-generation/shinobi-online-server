local temp = {
exhausted = 3,
}
local confg = {
level = 10,
chakra = 15
}
function onUse(cid, item, fromPosition, itemEx, toPosition)
-----[Restri��es]-----
	if	impossibleUse(cid) then
		return true
	end

	if checkDoing(cid) then
		return true
	end		
-----[Restri��es]-----
-----------------------
	if isPlayer(cid) then
		removeChakraLife(cid , - confg.chakra)
		doPlayerCastSpell(cid, "#YZmaito2")
		setPlayerStorageValue(cid, sto_jutsu[1], os.time() + temp.exhausted)
	end
end