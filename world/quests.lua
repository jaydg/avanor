QUEST_ELDER			= 1
QUEST_TORIN			= 2
QUEST_OZORIK		= 3
QUEST_YOHJI_BAT		= 4
QUEST_YOHJI_RAT		= 5
QUEST_ANCIENT_PART	= 6
QUEST_GIANA			= 7
QUEST_BANDITS		= 8
QUEST_USURPER		= 9
QUEST_GEFEON		= 10
QUEST_RODERICK_CRYPT	= 11
QUEST_RODERICK_EYE	= 12


function CreateAllQuests()

	-- Quest(id, status, know, complete, closed, score)
	--
	--   know      the line the quest log shows while the quest is KNOWN
	--   complete  the achievements line once the deed is done but not yet
	--             reported to whoever asked for it
	--   closed    the achievements line once it is done and reported
	--   score     what finishing it is worth, paid once on either COMPLETE
	--             or CLOSED (see XHero::EndGame)
	--
	-- A quest that has nothing to say in one of those states leaves the
	-- string empty and simply says nothing. Most quests never pass through
	-- COMPLETE at all - they go straight from KNOWN to CLOSED - and those
	-- only need a closed line.

	Quest(QUEST_ELDER, XQuest.UNKNOWN,
		"The Village Elder asked you to kill the demon who attacks villagers and has occupied the caves to the west of the village.",
		"You killed the demon that preyed on the village to the west.",
		"You killed the demon that preyed on the village, and the Elder saw his people safe again.",
		0)

	Quest(QUEST_TORIN, XQuest.UNKNOWN,
		"Torin, the Dwarven King asked you to switch on the gas pump at the bottom of gold mine.",
		"You helped to pump out gas from the dwarven gold mine.",
		"You helped to pump out gas from the dwarven gold mine.",
		5000)

	Quest(QUEST_OZORIK, XQuest.UNKNOWN,
		"Ozorick, the royal guard captain, has problems with an orcish war-party.",
		"You brought a useful thing to Ozorik.",
		"You armed the royal guard against the orcs, and Ozorick paid you for it.",
		0)

	Quest(QUEST_YOHJI_BAT, XQuest.UNKNOWN,
		"Yohjishiro, the elven wizard asked you bring a bat wing.",
		"",
		"",
		0)

	Quest(QUEST_YOHJI_RAT, XQuest.UNKNOWN,
		"Yohjishiro, the elven wizard asked you bring a rat tail.",
		"",
		"",
		0)

	Quest(QUEST_ANCIENT_PART, XQuest.UNKNOWN,
		"Ahk-Ulan asked you to bring 3 parts of ancient machine.",
		"",
		"You gathered the three parts of the ancient machine and put them in Ahk-Ulan's hands.",
		0)

	Quest(QUEST_USURPER, XQuest.UNKNOWN,
		"Ahk-Ulan asked you to kill Roderick, the King of Avanor - the only man left who could stand against him.",
		"You killed Roderick, the King of Avanor.",
		"You killed the King of Avanor for Ahk-Ulan, and left him the throne.",
		0)

	Quest(QUEST_GEFEON, XQuest.UNKNOWN,
		"Gefeon asked you to kill Ahk-Ulan, who works his evil in the dungeon beneath the ruins of his tower, south-east of the town.",
		"You killed Ahk-Ulan in the dungeon beneath his ruined tower.",
		"You killed Ahk-Ulan, and Gefeon named you the best of them.",
		0)

	Quest(QUEST_RODERICK_CRYPT, XQuest.UNKNOWN,
		"Roderick, the King of Avanor has asked you to cleanse his family crypt, which lies to the south-west of the city.",
		"",
		"You cleansed the tomb of Roderick's ancestors.",
		5000)

	Quest(QUEST_RODERICK_EYE, XQuest.UNKNOWN,
		"Roderick, the King of Avanor has asked you to find an artifact called the 'Eye of Raa'.",
		"",
		"You returned 'Eye of Raa' to Roderick.",
		10000)

	Quest(QUEST_BANDITS, XQuest.UNKNOWN,
		"Elder Gridor asked you to clear the forest brothers off the old road east of the village. Five of them hold the bridge, and the village has not been able to use the road in years.",
		"",
		"You cleared the forest brothers off the old road east, and the village walks it again.",
		0)

	Quest(QUEST_GIANA, XQuest.UNKNOWN,
		"Brida's daughter Giana has been kidnapped by a bandit named Rotmoth, who is holding her in a cave east of the village, past where the bandits lurk near the bridge. He demands 150 gold coins for her release.",
		"",
		"You brought Brida's daughter Giana home.",
		0)

end
