function onLogin(cid)
---[EXP VIP]--
local rate = 1.1 -- 10%
local config = {
	welvip = "Parabens voce e premium, como recompensa voce tera "..((rate - 1)*100).."% de exp a mais agora!",
}

if isPremium(cid) then
	doPlayerSetExperienceRate(cid, rate)
	doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, config.welvip)
	--regen(cid, 2, 6)
else
	--regen(cid, 1, 6)
end

---[Config Hospital]--
if getPlayerStorageValue(cid, initStorages.death) == 1 then
	location = {
	{x=2580, y=1815, z=7},
	{x=2580, y=1811, z=7},
	{x=2570, y=1818, z=7},
	{x=2580, y=1807, z=7},
	{x=2571, y=1821, z=7},
	{x=2571, y=1825, z=7},
	{x=2571, y=1829, z=7},
	{x=2571, y=1833, z=7},
	{x=2579, y=1830, z=7},
	{x=2580, y=1833, z=7}, 
	{x=2567, y=1818, z=7},
	{x=2564, y=1818, z=7},
	{x=2564, y=1807, z=7},

}
	setPlayerStorageValue(cid, initStorages.death, 0)
	doTeleportThing(cid, location[math.random(#location)])
end
----------------------

---[Config Kikkais]---
if getPlayerStorageValue(cid, sto_kikkais[1]) < 0 then
	setPlayerStorageValue(cid, sto_kikkais[1], 10)
	setPlayerStorageValue(cid, sto_kikkais[2], getPlayerLevel(cid)+1)
 end
 --------------------


---INIT CMD---
setPlayerStorageValue(cid, 2137, 1) -- mold
setPlayerStorageValue(cid, 2135, 1) -- rest
setPlayerStorageValue(cid, 2136, 1) -- run
--------------

setPlayerStorageValue(cid, STORAGE_DEFENSE, 0)

if getGlobalStorageValue(cid, 910562) == 1 then
	doSendPlayerExtendedOpcode(cid, 124,  "Sepia")
end

if getGlobalStorageValue(cid, 910562) == 2 then
	doSendPlayerExtendedOpcode(cid, 124,  "Default")
end

if getGlobalStorageValue(cid, 910562) == 3 then
	doSendPlayerExtendedOpcode(cid, 124,  "Default")
end

-----INIT PONTOS----
	if getPlayerStorageValue(cid, initStorages.init) <= 0 then
		doPlayerAddMapMark(cid, {x = 2442, y = 1684, z = 7,}, 21, 'GYM KONOHA')
		doPlayerSendTextMessage(cid, MESSAGE_STATUS_CONSOLE_BLUE, "Bem-vindo ao otNaruto, sua primeira missao e ir a academia para comecar o seu teste gennin procure por Tekuno e entao comece seu treinamento, logo comecara a sua aventura ninja! (Academia marcada no minimap)")
		doPlayerSetSkillLevel(cid, 0, 0)
		doPlayerSetSkillLevel(cid, 1, 0)
		doPlayerSetSkillLevel(cid, 2, 0)
		doPlayerSetSkillLevel(cid, 3, 0)
		doPlayerSetSkillLevel(cid, 4, 0)
		doPlayerSetSkillLevel(cid, 5, 0)
		doPlayerSetSkillLevel(cid, 6, 0)
		doPlayerAddSoul(cid, -100)
		setPlayerStorageValue(cid, initStorages.init, 1)
	end
--------------------

-----INIT GRADUAÇÃO---
 if getPlayerStorageValue(cid, 600) <= 0 then
	setPlayerStorageValue(cid, 600, 1) -- Academy	
end

-----[Config Missão]---
	
	for i = 1, #sto_missao do
	  if getCreatureStorage(cid, sto_missao[i]) < 0 then
		 doCreatureSetStorage(cid, sto_missao[i], 0)
	  end
	end
   
-----------------------
	
	
---[Mizu Kinobori]---
	setPlayerStorageValue(cid, sto_jutsu[3], 1)

-------------------

if getPlayerStorageValue(cid, sto_resetPoints) < 0 then
	setPlayerStorageValue(cid, sto_resetPoints, 0)
end


---[Byakugan Config]-----  
	for i = 1, #sto_byakugan do
	  if getCreatureStorage(cid, sto_byakugan[i]) < 0 then
		 doCreatureSetStorage(cid, sto_byakugan[i], 1)
	  end
   end
 
	setPlayerStorageValue(cid, 9997, 0)
----[Sharingan Login]-----
	for i = 1, #sto_sharingan do
	  if getCreatureStorage(cid, sto_sharingan[i]) < 0 then
		 doCreatureSetStorage(cid, sto_sharingan[i], 1)
	  end
   end

	for i = 1, #sto_sharingan_reflect do
	  if getCreatureStorage(cid, sto_sharingan_reflect[i]) < 0 or getCreatureStorage(cid, sto_sharingan_reflect[i]) == 1 then
		 doCreatureSetStorage(cid, sto_sharingan_reflect[i], 0)
	  end
   end
-------------------------

---[HACHIMON TONKOU CONFIG]----
	if getPlayerStorageValue(cid, sto_hachimon[1]) == 0 then
		doPlayerSetSkillLevel(cid, 0, getPlayerSkill(cid, 0)-2)
		doPlayerSetSkillLevel(cid, 6, getPlayerSkill(cid, 6)-2)
		setPlayerStorageValue(cid, sto_hachimon[1], 1)
	end

	if getPlayerStorageValue(cid, sto_hachimon[2]) == 0 then
		doPlayerSetSkillLevel(cid, 0, getPlayerSkill(cid, 0)-4)
		doPlayerSetSkillLevel(cid, 6, getPlayerSkill(cid, 6)-4)
		setPlayerStorageValue(cid, sto_hachimon[2], 1)
	end
	
	if getPlayerStorageValue(cid, sto_hachimon[3]) == 0 then
		doPlayerSetSkillLevel(cid, 0, getPlayerSkill(cid, 0)-6)
		doPlayerSetSkillLevel(cid, 6, getPlayerSkill(cid, 6)-6)
		setPlayerStorageValue(cid, sto_hachimon[3], 1)
	end	
	
	if getPlayerStorageValue(cid, sto_hachimon[4]) == 0 then
		doPlayerSetSkillLevel(cid, 0, getPlayerSkill(cid, 0)-8)
		doPlayerSetSkillLevel(cid, 6, getPlayerSkill(cid, 6)-8)
		setPlayerStorageValue(cid, sto_hachimon[4], 1)
	end	
	
	if getPlayerStorageValue(cid, sto_hachimon[5]) == 0 then
		doPlayerSetSkillLevel(cid, 0, getPlayerSkill(cid, 0)-10)
		doPlayerSetSkillLevel(cid, 6, getPlayerSkill(cid, 6)-10)
		setPlayerStorageValue(cid, sto_hachimon[5], 1)
	end	
	
	if getPlayerStorageValue(cid, sto_hachimon[6]) == 0 then
		doPlayerSetSkillLevel(cid, 0, getPlayerSkill(cid, 0)-12)
		doPlayerSetSkillLevel(cid, 6, getPlayerSkill(cid, 6)-12)
		setPlayerStorageValue(cid, sto_hachimon[6], 1)
	end		
	
	if getPlayerStorageValue(cid, sto_hachimon[7]) == 0 then
		doPlayerSetSkillLevel(cid, 0, getPlayerSkill(cid, 0)-14)
		doPlayerSetSkillLevel(cid, 6, getPlayerSkill(cid, 6)-14)
		setPlayerStorageValue(cid, sto_hachimon[7], 1)
	end		
	
	if getPlayerStorageValue(cid, sto_hachimon[8]) == 0 then
		doPlayerSetSkillLevel(cid, 0, getPlayerSkill(cid, 0)-14)
		doPlayerSetSkillLevel(cid, 6, getPlayerSkill(cid, 6)-14)
		setPlayerStorageValue(cid, sto_hachimon[8], 1)
	end			

---[Hachimon tonkou login]---	
	for i = 1, #sto_hachimon do
	  if getCreatureStorage(cid, sto_hachimon[i]) < 0 then
		 doCreatureSetStorage(cid, sto_hachimon[i], 1)
	  end
   end
-----------------------------
   
----------------------------------------------------------------
	setPlayerStorageValue(cid, STORAGE_KAGEMANE_CHECK, 0)
	setPlayerStorageValue(cid, STORAGE_KAGEKUBI_CHECK, 0)
--------------------------------------------------------------
------[Pills Config]----
if getPlayerStorageValue(cid, sto_sensha[2]) == 0 then
	doPlayerSetSkillLevel(cid, 0, getPlayerSkill(cid, 0)-5)
	setPlayerStorageValue(cid, sto_sensha[2], 1)
end

if getPlayerStorageValue(cid, sto_sensha[3]) == 0 then
	doPlayerSetSkillLevel(cid, 0, getPlayerSkill(cid, 0)-8)
	setPlayerStorageValue(cid, sto_sensha[3], 1)
end

------------------------
---[Akimichi Storage]---
	setPlayerStorageValue(cid, sto_sensha[1], 0)
	setPlayerStorageValue(cid, sto_sensha[5], 0)
	setPlayerStorageValue(cid, sto_sensha[7], 0)
	setPlayerStorageValue(cid, sto_sensha[6], 1)
	setPlayerStorageValue(cid, sto_sensha[3], 1)
	setPlayerStorageValue(cid, sto_sensha[2], 1)
------------------------

	setPlayerStorageValue(cid, STORAGE_PET_REST, 1)
	
	if getPlayerStorageValue(cid, STORAGE_NICKPET) <= 0 then 
		setPlayerStorageValue(cid, petStorages.nick, "Dog")
	end
	
---[Config Inuzuka]--
	if getPlayerStorageValue(cid, sto_inuzuka[1]) == 0 then
		doPlayerSetSkillLevel(cid, 0, getPlayerSkill(cid, 0)-3)
		doPlayerSetSkillLevel(cid, 6, getPlayerSkill(cid, 6)-3)
		setPlayerStorageValue(cid, sto_inuzuka[1], 1)
	end

	if getPlayerStorageValue(cid, sto_inuzuka[2]) == 0 then
		doPlayerSetSkillLevel(cid, 0, getPlayerSkill(cid, 0)-4)
		doPlayerSetSkillLevel(cid, 6, getPlayerSkill(cid, 6)-4)
		setPlayerStorageValue(cid, sto_inuzuka[2], 1)
	end
	if getPlayerStorageValue(cid, sto_inuzuka[3]) == 0 then
		doPlayerSetSkillLevel(cid, 0, getPlayerSkill(cid, 0)-20)
		doPlayerSetSkillLevel(cid, 6, getPlayerSkill(cid, 6)-20)
		setPlayerStorageValue(cid, sto_inuzuka[3], 1)
	end

---[Config Inuzuka login]---
	for i = 1, #sto_inuzuka do
	  if getCreatureStorage(cid, sto_inuzuka[i]) < 0 then
		 doCreatureSetStorage(cid, sto_inuzuka[i], 1)
	  end
   end
   
---[Config Nara]--
	for i = 1, #sto_nara do
	  if getCreatureStorage(cid, sto_nara[i]) <= 0 then
		 doCreatureSetStorage(cid, sto_nara[i], 1)
	  end
   end
 
--[Config Genjutsu]--
	for i = 1, #sto_gen do
	  if getCreatureStorage(cid, sto_gen[i]) <= 0 then
		 doCreatureSetStorage(cid, sto_gen[i], 1)
	  end
	end 
---------------------------------------------------------------
if isPlayer(cid) and getPlayerVocation(cid) > 0 then

	local outfit = getCreatureOutfit(cid)
		
	if getPlayerStorageValue(cid, initStorages.graduation) == "Academy Student" then
		outfit.lookType =  infoClanS[getPlayerVocation(cid)].outfit
	elseif getPlayerStorageValue(cid, initStorages.graduation) == "Gennin" then
		outfit.lookType =  infoClan[getPlayerVocation(cid)].outfit
	end
	
	doCreatureChangeOutfit(cid, outfit)
end


 return true
end
