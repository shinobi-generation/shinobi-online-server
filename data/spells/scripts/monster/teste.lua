function onCastSpell(cid, var)
	if not isCreature(cid) then
return true
end
local target = getCreatureTarget(cid)
doCreatureSetNoMove(target, true)
addEvent(doCreatureSetNoMove, 2000, target, false)
doPlayerSendCancel(cid, "teste")
end