CAVE = 0
DUNGEON = 1
L_SMALL_CAVE1 = 55
L_SMALL_CAVE2 = 56
UP = 1
DOWN = 2

CreateLocation(L_SMALL_CAVE1, "SmCv:1", "Small Cave Level 1", CAVE)
	Way(UP, L_MAIN)
	Way(DOWN, L_SMALL_CAVE2)
	Settle(CR_RAT + CR_FELINE + CR_INSECT, CRL_VERY_LOW)


CreateLocation(L_SMALL_CAVE2, "SmCv:2", "Small Cave Level 2", CAVE)
	Way(UP, L_SMALL_CAVE1)
	Creature(CN_ROTMOTH)