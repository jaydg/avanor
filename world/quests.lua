-- The quests, and what the log and the achievements screen say about each.
--
-- A quest is named by its id: "torin" is the same quest in the script that
-- gives it, the one that finishes it, and the saved game that remembers
-- how far the hero got. There is no numbering to keep in step.
--
-- "orc_war_party" is not created here: it is added to the world at the
-- moment it is earned, by the death tally in world/tally.lua, and only if
-- it is earned. See Quest() there.

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

	Quest("elder", XQuest.UNKNOWN,
		"The Village Elder asked you to kill the demon who attacks villagers and has occupied the caves to the west of the village.",
		"You killed the demon that preyed on the village to the west.",
		"You killed the demon that preyed on the village, and the Elder saw his people safe again.",
		0)

	Quest("torin", XQuest.UNKNOWN,
		"Torin, the Dwarven King asked you to switch on the gas pump at the bottom of gold mine.",
		"You helped to pump out gas from the dwarven gold mine.",
		"You helped to pump out gas from the dwarven gold mine.",
		5000)

	Quest("ozorik", XQuest.UNKNOWN,
		"Ozorick, the royal guard captain, has problems with an orcish war-party.",
		"You brought a useful thing to Ozorik.",
		"You armed the royal guard against the orcs, and Ozorick paid you for it.",
		0)

	Quest("yohji_bat", XQuest.UNKNOWN,
		"Yohjishiro, the elven wizard asked you bring a bat wing.",
		"",
		"",
		0)

	Quest("yohji_rat", XQuest.UNKNOWN,
		"Yohjishiro, the elven wizard asked you bring a rat tail.",
		"",
		"",
		0)

	Quest("ancient_part", XQuest.UNKNOWN,
		"Ahk-Ulan asked you to bring 3 parts of ancient machine.",
		"",
		"You gathered the three parts of the ancient machine and put them in Ahk-Ulan's hands.",
		0)

	-- The two commissions that end the game. Their scores are as follows:
	-- ridding Avanor of Ahk-Ulan is worth 10000, taking the throne from
	-- Roderick 20000, and a hero who is asked for both and delivers both
	-- collects 30000 without anything having to add the case up.
	Quest("usurper", XQuest.UNKNOWN,
		"Ahk-Ulan asked you to kill Roderick, the King of Avanor - the only man left who could stand against him.",
		"You killed Roderick, the King of Avanor.",
		"You killed the King of Avanor for Ahk-Ulan, and left him the throne.",
		20000)

	Quest("gefeon", XQuest.UNKNOWN,
		"Gefeon asked you to kill Ahk-Ulan, who works his evil in the dungeon beneath the ruins of his tower, south-east of the town.",
		"You killed Ahk-Ulan in the dungeon beneath his ruined tower.",
		"You killed Ahk-Ulan, and Gefeon named you the best of them.",
		10000)

	Quest("roderick_crypt", XQuest.UNKNOWN,
		"Roderick, the King of Avanor has asked you to cleanse his family crypt, which lies to the south-west of the city.",
		"",
		"You cleansed the tomb of Roderick's ancestors.",
		5000)

	Quest("roderick_eye", XQuest.UNKNOWN,
		"Roderick, the King of Avanor has asked you to find an artifact called the 'Eye of Raa'.",
		"",
		"You returned 'Eye of Raa' to Roderick.",
		10000)

	Quest("bandits", XQuest.UNKNOWN,
		"Elder Gridor asked you to clear the forest brothers off the old road east of the village. Five of them hold the bridge, and the village has not been able to use the road in years.",
		"",
		"You cleared the forest brothers off the old road east, and the village walks it again.",
		0)

	Quest("giana", XQuest.UNKNOWN,
		"Brida's daughter Giana has been kidnapped by a bandit named Rotmoth, who is holding her in a cave east of the village, past where the bandits lurk near the bridge. He demands 150 gold coins for her release.",
		"",
		"You brought Brida's daughter Giana home.",
		0)

end
