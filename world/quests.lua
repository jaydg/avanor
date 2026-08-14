QUEST_ELDER			= 1
QUEST_TORIN			= 2
QUEST_OZORIK		= 3
QUEST_YOHJI_BAT		= 4
QUEST_YOHJI_RAT		= 5
QUEST_ANCIENT_PART	= 6
QUEST_GIANA			= 7


function CreateAllQuests()

	Quest(QUEST_ELDER, XQuest.UNKNOWN, 
		"The Village Elder asked you to kill the demon who attacks villagers and has occupied the caves to the west of the village.", 
		"", 
		"")

	Quest(QUEST_TORIN, XQuest.UNKNOWN, 
		"Torin, the Dwarven King asked you to switch on the gas pump at the bottom of gold mine.",
		"", 
		"")

	Quest(QUEST_OZORIK, XQuest.UNKNOWN, 
		"Ozorick, the royal guard captain, has problems with an orcish war-party.",
		"", 
		"")

	Quest(QUEST_YOHJI_BAT, XQuest.UNKNOWN, 
		"Yohjishiro, the elven wizard asked you bring a bat wing.", 
		"", 
		"")

	Quest(QUEST_YOHJI_RAT, XQuest.UNKNOWN, 
		"Yohjishiro, the elven wizard asked you bring a rat tail.", 
		"", 
		"")

	Quest(QUEST_ANCIENT_PART, XQuest.UNKNOWN,
		"Ahk-Ulan asked you to bring 3 parts of ancient machine.",
		"",
		"")

	Quest(QUEST_GIANA, XQuest.UNKNOWN,
		"Brida's daughter Giana has been kidnapped by a bandit named Rotmoth, who is holding her in a cave east of the village, past where the bandits lurk near the bridge. He demands 150 gold coins for her release.",
		"",
		"")

end
