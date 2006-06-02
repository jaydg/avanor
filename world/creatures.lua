function InitAllCreatures()

------------------------------ RATS --------------------------------------
	CRVW(CN_RAT, "rat", 'r', xBROWN, CPT_IT, CRL_VERY_LOW, CR_RAT);
	CRBA("1d30+90", "0d0+1000", "1d200+600", CS_VERY_SMALL, "5d4");
	CRBO("");
	CRA(AIF_COWARD + AIF_RANDOM_MOVE + AIF_ALLOW_PACK);
	CRS("St 1d2 Dx 2d4 To 1d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 2d4 Ch 1d1");
	CRR("fire:5d5-50 cold:2d10");
	CRC("0d0", "1d2");
	CRM("1d3", "0d0", "1d3", "0d0");
	CRD("Scuttling about in the shadows, rats can be found most places in the valley.  Those in the caves have grown larger and bolder and present a threat to any explorers.");
	CRCOE(CET_VOMIT, 0);

	CRVW(CN_LARGE_RAT, "large rat", 'r', xBROWN, CPT_IT, CRL_VERY_LOW, CR_RAT, CN_RAT);
	CRBA("1d30+90", "0d0+1000", "1d200+600", CS_VERY_SMALL, "10d4");
	CRS("St 1d3 Dx 2d5 To 1d3");
	CRR("fire:5d5-25 cold:3d10");
	CRC("1d2", "1d3");
	CRM("2d2", "1d1", "2d3", "0d0");
	CRD("These large rodents have grown almost to the size of a housecat.  Their sharp incisors glitter brightly in the light from your torch.  It is said that their teeth never stop growing and they must constantly chew in order to keep them trimmed.  This particular rat seems to think your arm would make a great chew stick.");


	CRVW(CN_BLACK_RAT, "black rat", 'r', xDARKGRAY, CPT_IT, CRL_LOW, CR_RAT, CN_RAT);
	CRBA("1d30+100", "0d0+1000", "1d200+500", CS_VERY_SMALL, "10d4");
	CRS("St 1d5 Dx 2d8 To 2d2 Pe 2d5");
	CRR("fire:5d5 cold:5d10 acid:5d10+40");
	CRC("1d4", "1d6");
	CRM("2d3", "1d2", "3d3", "0d0");
	CRD("A foot long and covered in jet black fur, this rat appears rather harmless (as far as rats go).  Then you notice the wildness in its eyes and slight froth dripping from its jaw.  Surely this is the type of rat blamed for the spread of many diseases.  Suddenly you become much more wary about letting it nip at you, even a scratch could cause the dreaded black plague.");
	CRAT(BR_DISEASE, 30);
	CRCOE(CET_VOMIT, 0);

	CRVW(CN_HUGE_RAT, "huge rat", 'r', xLIGHTGRAY, CPT_IT, CRL_LOW, CR_RAT, CN_RAT);
	CRBA("1d30+110", "0d0+1000", "1d200+400", CS_SMALL, "50d4");
	CRS("St 2d5 Dx 2d7 To 3d2 Pe 2d6");
	CRR("cold:5d10");
	CRC("2d4", "2d3");
	CRM("2d3", "2d2", "5d3", "0d0");
	CRD("Come closer my child and you will hear of the giant rats that come to take bad children away...  Deep in the caves of Avanor they nest, big as a wolf and meaner too.  They come at night to take bad little children and feed them to their young.  Oh!  I see it is bedtime, you better hurry and get in bad or they might think you are one of the bad ones...");
	CRAT(BR_DISEASE, 30);
	CRAT(BR_POISON, 20);
	CRCOE(CET_VOMIT, 0);
	
------------------------------ BATS --------------------------------------	
	CRVW(CN_BAT, "bat", 'b', xBROWN, CPT_IT, CRL_VERY_LOW, CR_RAT);
	CRBA("1d30+120", "0d0+1000", "1d200+800", CS_VERY_SMALL, "3d4");
	CRBO("");
	CRA(AIF_COWARD + AIF_RANDOM_MOVE);
	CRS("St 1d1 Dx 1d4 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 3d10 Ch 1d1");
	CRR("fire:5d5-50 see_invisible:1d10+10");
	CRC("0d0", "1d2");
	CRM("1d4", "0d0", "1d2", "0d0");
	CRD("Flapping wings and squeaks in the darkness are a common sound to all who enter the caves of Avanor.  Usually bats leave travlers alone, but sickness and magic sometimes cause them to attack.");

	CRVW(CN_HUGE_BAT, "huge bat", 'b', xLIGHTGRAY, CPT_IT, CRL_VERY_LOW, CR_RAT, CN_BAT);
	CRBA("1d30+150", "0d0+1000", "1d200+700", CS_VERY_SMALL, "10d4");
	CRS("St 1d4 Dx 1d6 To 1d2 Pe 4d10");
	CRR("see_invisible:1d10+20");
	CRC("1d2", "1d4");
	CRM("1d4", "1d1", "1d6", "0d0");
	CRD("With a wing span up to 10 feet, these bats can carry away much larger prey than their smaller cousins.  They have been seen carrying creatures as large as a wolf away to feed their young in the dark corners of their cave.  If they can they will take down any prey available and them dismember it with razor sharp teeth to make it easier to carry.");
	
------------------------------ REPTILE --------------------------------------	

	CRVW(CN_SMALL_SNAKE, "small snake", 'R', xLIGHTGRAY, CPT_IT, CRL_VERY_LOW, CR_REPTILE);
	CRBA("1d10+80", "0d0+1000", "1d200+900", CS_VERY_SMALL, "3d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d1 Dx 1d2 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d2 Ch 1d1");
	CRR("cold:1d10-90");
	CRC("2d5", "1d3");
	CRM("1d3", "0d0", "1d2", "0d0");
	CRD("This tiny creature lies in the dust looking like a piece of rope.  Suddenly a pink tongue protrudes from one end and eyes open sensing your presence.  Though unimposing at best, this snake harbors one of the deadliest poisons in the valley.");

	CRVW(CN_GRAY_SNAKE, "gray snake", 'R', xLIGHTGRAY, CPT_IT, CRL_VERY_LOW, CR_REPTILE, CN_SMALL_SNAKE);
	CRBA("1d10+80", "0d0+1000", "1d200+900", CS_VERY_SMALL, "5d4");
	CRS("St 1d2 Dx 1d4 To 1d2 Pe 1d2");
	CRR("cold:1d10-50");
	CRC("2d5", "1d3");
	CRM("1d3", "0d0", "1d3", "0d0");
	CRD("A long serpent slithers towards you, its grey-black scales making it hard to see against the stone of the cavern floor.  The pale green eyes glimmer with hunger.  Its pink tounge tastes the air as it contemplates this, its lastest meal... YOU!");

	CRVW(CN_BROWN_SNAKE, "brown snake", 'R', xBROWN, CPT_IT, CRL_VERY_LOW, CR_REPTILE, CN_GRAY_SNAKE);
	CRR("cold:1d10-30");
	CRC("2d5", "1d4");
	CRM("1d3", "0d0", "1d5", "0d0");
	CRD("This snake will normally keep to the forests and caves of Avanor.  The naturally mottled brown coat allows it to blend in with the area around it.  Though not a great danger, travelers are urged to leave them alone since others may be nearby.");

	CRVW(CN_SALAMANDER, "salamander", 'R', xRED, CPT_IT, CRL_LOW, CR_REPTILE, CN_GRAY_SNAKE);
	CRR("cold:1d10-50 fire:1d90");
	CRC("2d4", "1d4");
	CRM("1d9", "1d3", "1d3", "0d0");
	CRD("Salamanders prefer living in dark, damp caves and eating the bugs that fly around.  The release of Power mutated the salamanders in the valley of Avanor until they were 4 feet long and had a taste for warm blooded flesh.  They can't see well but they have a powerful bite.");
	CRAT(BR_FIRE, 100);
	CRCOE(CET_MODIFY_R_FIRE, 1);

	CRVW(CN_LARGE_SNAKE, "large snake", 'R', xLIGHTGRAY, CPT_IT, CRL_LOW, CR_REPTILE, CN_GRAY_SNAKE);
	CRBA("1d10+80", "0d0+1000", "1d200+800", CS_VERY_SMALL, "10d4");
	CRS("St 1d3 Dx 1d5 To 1d3 Pe 1d2");
	CRR("cold:1d10-40");
	CRC("6d5", "1d5");
	CRM("1d4", "1d1", "2d3", "0d0");
	CRD("This serpent seems to have been lost from some sort of travelling show.  Its emerald green scales show up clearly against the stone and dirt.  Idly you wonder how it has survived for so long seeing as travelling shows just don't reach the valley anymore.  Then you realize that the snake is already closing the distance between its position and yours and suddenly you know exactly how it survived.");
	CRAT(BR_POISON, 10);
	CRCOE(CET_MODIFY_R_POISON, 1);

	CRVW(CN_COBRA, "cobra", 'R', xGREEN, CPT_IT, CRL_LOW, CR_REPTILE, CN_GRAY_SNAKE);
	CRBA("1d10+90", "0d0+1000", "1d200+700", CS_VERY_SMALL, "15d4");
	CRS("St 1d3 Dx 1d7 To 2d2 Pe 1d4");
	CRR("cold:1d10-30");
	CRC("7d5", "1d7");
	CRM("1d8", "1d1", "3d3", "0d0");
	CRD("The cobra looks much like a normal snake until it lifts its head and spreads its hood.  Then the markings clearly indicate the danger you face.  A cobra's poison is more deadly than other snakes and the cobra is much more aggressive.");
	CRAT(BR_POISON, 40);
	CRCOE(CET_MODIFY_R_POISON, 2);

	CRVW(CN_KING_COBRA, "king cobra", 'R', xLIGHTRED, CPT_IT, CRL_AVG, CR_REPTILE, CN_GRAY_SNAKE);
	CRBA("1d10+90", "0d0+1000", "1d200+500", CS_VERY_SMALL, "20d4");
	CRS("St 1d7 Dx 1d8 To 3d2 Pe 2d3");
	CRR("cold:1d10-20");
	CRC("8d5", "1d12");
	CRM("2d5", "2d2", "4d3", "0d0");
	CRD("The distinct crown shaped mark on the back of this snake's head gave the title it now carries.  However, if you are close enough to see the crown than you should have been able to distinguish it by its size alone.  Bigger, faster and stronger than its smaller cousin, the king cobra is best left alone by anyone without several anti-venom potions.");
	CRAT(BR_POISON, 60);
	CRCOE(CET_MODIFY_R_POISON, 5);

	CRVW(CN_RATTLESNAKE, "rattlesnake", 'R', xRED, CPT_IT, CRL_AVG, CR_REPTILE, CN_GRAY_SNAKE);
	CRBA("1d10+90", "0d0+1000", "1d200+500", CS_VERY_SMALL, "10d4");
	CRS("St 2d4 Dx 2d5 To 3d2 Pe 2d3");
	CRR("cold:1d10-30");
	CRC("10d5", "1d12");
	CRM("1d5", "1d2", "5d3", "0d0");
	CRD("An ominous rattle sounds from ahead of you as a coiled rattlesnake prepares to defend its territory.  Poison drips from its fangs and the cold glint in its eyes seem to foreshadow your certain death if you don't retreat now.  It may even be too late...");
	CRAT(BR_POISON, 80);
	CRCOE(CET_MODIFY_R_POISON, 10);


------------------------------ FELINE --------------------------------------

	CRVW(CN_CAT, "cat", 'f', xBROWN, CPT_IT, CRL_VERY_LOW, CR_FELINE);
	CRBA("1d10+110", "0d0+1000", "1d500+500", CS_SMALL, "1d40+50");
	CRBO("");
	CRA(AIF_COWARD + AIF_RANDOM_MOVE);
	CRS("St 2d2 Dx 2d4 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 3d3 Ch 1d1");
	CRR("fire:5d5-80 cold:3d10");
	CRC("1d2", "1d3");
	CRM("2d3", "1d1", "1d4", "0d0");
	CRD("This mangy little furball looks so cute and cuddly that you just want hug it.  That is you would want to if it weren't hissing, spitting and taking swipes at your face with its claws.");

	CRVW(CN_WILD_CAT, "wildcat", 'f', xLIGHTGRAY, CPT_IT, CRL_VERY_LOW, CR_FELINE);
	CRBA("1d20+110", "0d0+1000", "1d300+300", CS_SMALL, "1d40+70");
	CRBO("");
	CRA(AIF_COWARD + AIF_RANDOM_MOVE);
	CRS("St 2d3 Dx 2d6 To 1d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 3d3 Ch 1d1");
	CRR("fire:5d5-25 cold:5d10");
	CRC("1d4", "1d4");
	CRM("2d4", "1d2", "2d4", "0d0");
	CRD("Tawny and muscled, this cat silently pads forward.  Almost as large as a wolf, the mottled markings make it dificult to see in the shadows.  As its slitted eyes glance from side to side you can tell it is hunting for something.  Only then do you realize it is looking for you!");

------------------------------ CANINE --------------------------------------

	CRVW(CN_DOG, "dog", 'C', xBROWN, CPT_IT, CRL_VERY_LOW, CR_CANINE);
	CRBA("1d10+100", "0d0+1000", "1d100+900", CS_SMALL, "1d100+200");
	CRBO("");
	CRA(AIF_HI_ANIMAL + AIF_ALLOW_PACK + AIF_ALLOW_PACK);
	CRS("St 2d3 Dx 2d3 To 1d3 Le 1d1 Wi 1d1 Ma 1d1 Pe 3d3 Ch 1d1");
	CRR("fire:5d5-40 cold:5d10");
	CRC("1d2", "2d3");
	CRM("1d4", "1d2", "2d3", "0d0");
	CRD("Dogs were brought to the valley of Avanor to serve the residents of the kingdom, but some of them escaped and became feral and breeding in the caves below the surface.  Mongrels in every sense, these animals now prey upon other creatures for food, including silly adventurers.");

	CRVW(CN_LARGE_DOG, "large dog", 'C', xBROWN, CPT_IT, CRL_VERY_LOW, CR_CANINE);
	CRBA("1d10+100", "0d0+1000", "1d100+900", CS_SMALL, "1d200+300");
	CRBO("");
	CRA(AIF_HI_ANIMAL + AIF_ALLOW_PACK + AIF_ALLOW_PACK);
	CRS("St 2d4 Dx 2d4 To 2d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 3d3 Ch 1d1");
	CRR("fire:5d5-30 cold:6d10");
	CRC("1d4", "2d4");
	CRM("1d4", "1d3", "2d4", "0d0");
	CRD("Obviously the leader of his pack, this dog is larger than the others and may be the offspring of a dog and a wolf.  Its muzzle and body are scarred from numerous fights and it limps slightly.  The deference the other dogs show it indicates that it is still a dangerous foe.");

	CRVW(CN_RABID_DOG, "rabid dog", 'C', xBROWN, CPT_IT, CRL_LOW, CR_CANINE);
	CRBA("1d10+120", "0d0+1000", "1d100+700", CS_SMALL, "1d100+200");
	CRBO("");
	CRA(AIF_HI_ANIMAL + AIF_ALLOW_PACK);
	CRS("St 2d4 Dx 2d2 To 1d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d3 Ch 1d1");
	CRR("fire:5d5-30 cold:4d10");
	CRC("2d4", "2d3");
	CRM("1d2", "1d1", "2d3", "0d0");
	CRD("White foam drips from the jaws of this dog.  The wild look in its eyes indicate that something is not quite right, and it snaps at everything and everyone around it with the strength born of its diseased madness.");
	CRAT(BR_DISEASE, 50);
	CRCOE(CET_DISEASE, 50);

	CRVW(CN_WOLF, "wolf", 'C', xLIGHTGRAY, CPT_IT, CRL_LOW, CR_CANINE);
	CRBA("1d10+130", "0d0+1000", "1d100+700", CS_SMALL, "1d300+500");
	CRBO("");
	CRA(AIF_HI_ANIMAL + AIF_ALLOW_PACK);
	CRS("St 4d3+2 Dx 2d4+1 To 2d3 Le 1d1 Wi 1d1 Ma 1d1 Pe 3d4 Ch 1d1");
	CRR("fire:5d5-20 cold:7d10");
	CRC("3d4", "2d6");
	CRM("2d4", "2d3", "3d4", "0d0");
	CRD("One glance at this wolf tells you why it has come down from the mountains... to look for food.  The shaggy pelt and thin body are proof of hard times since the eruption of Power.");

	CRVW(CN_LARGE_WOLF, "large wolf", 'C', xLIGHTGRAY, CPT_IT, CRL_AVG, CR_CANINE);
	CRBA("1d10+130", "0d0+1000", "1d100+700", CS_NORMAL, "1d300+700");
	CRBO("");
	CRA(AIF_HI_ANIMAL + AIF_ALLOW_PACK);
	CRS("St 4d4+2 Dx 3d4+1 To 3d4 Le 1d1 Wi 1d1 Ma 1d1 Pe 3d4 Ch 1d1");
	CRR("cold:9d10");
	CRC("5d5", "3d6");
	CRM("2d4", "3d3", "5d4", "0d0");
	CRD("This wolf is almost the size of a small pony.  It is fast, strong and eager to eat anything smaller than itself.  Its hungry gaze makes you uncomfortable as it faces you with fangs bared.");

	CRVW(CN_WEREWOLF, "werewolf", 'C', xDARKGRAY, CPT_IT, CRL_HI, CR_CANINE);
	CRBA("1d10+150", "0d0+1000", "1d100+500", CS_NORMAL, "1d300+1000");
	CRBO("");
	CRA(AIF_HI_ANIMAL);
	CRS("St 7d4 Dx 5d4 To 5d4 Le 1d1 Wi 1d1 Ma 1d1 Pe 5d4 Ch 1d1");
	CRR("fire:4d10 cold:9d10");
	CRC("6d5", "4d6");
	CRM("2d8", "3d4", "7d4", "7d4");
	CRD("As you gaze on this creature its features shift from wolf to man and back again.  Product of a dreadful experiment in magic, the werewolf hunts in the caves of Avanor for food.  It is said that silver weapons are the only true way to defeat a werewolf.");
	CRAT(BR_DISEASE, 100);
	CRAT(BR_PARALYSE, 20);
	CRCOE(CET_PARALYSE, 50);
	CRCOE(CET_MODIFY_R_PARALYSE, 5);
	CRCOE(CET_DISEASE, 50);


------------------------------ OOZE, JELLY, etc. --------------------------------------
	
	CRVW(CN_GRAY_OOZE, "gray ooze", 'j', xLIGHTGRAY, CPT_IT, CRL_LOW, CR_BLOB);
	CRBA("1d10+40", "0d0+1000", "1d100+600", CS_VERY_SMALL, "1d10+10");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("acid:0d0+100 poison:0d0+100");
	CRC("3d5", "1d3");
	CRM("1d2", "0d0", "1d3", "0d0");
	CRD("This quivering gelatinous mass slowly oozes across the floor towards you.  It appears almost that two separate intelligences control the creature, straining to separate from each other.  It is said that the gray ooze steals life energy from those it attacks in order to reproduce.  It also is rumored to have a deadly poison.");
	CRAT(BR_POISON, 15);
	CRCOE(CET_MODIFY_R_POISON, 1);
	CRCOE(CET_POISON, 10);

	CRVW(CN_GELATINOUS_CUBE, "gelatinous cube", 'j', xWHITE, CPT_IT, CRL_AVG, CR_BLOB);
	CRBA("1d10+50", "0d0+1000", "1d100+200", CS_LARGE, "20d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("");
	CRC("2d4", "2d15");
	CRM("1d1", "1d10", "1d8", "0d0");
	CRD("As you watch this clear mass you realize that you can still see the remains of its last meal suspended inside.  The skull seems to laugh at you as ready your weapon to face this three foot oozing mass.  The skelatal hand still grips the remains of a sword, slowly being digested.");
	CRAT(BR_PARALYSE, 100);
	CRAT(BR_ACID, 100);


------------------------------ INSECTS --------------------------------------

	CRVW(CN_SPIDER, "spider", 'S', xBROWN, CPT_IT, CRL_VERY_LOW, CR_INSECT);
	CRBA("1d10+70", "0d0+1000", "1d100+900", CS_VERY_SMALL, "3d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("fire:1d30-50 cold:1d20-40");
	CRC("2d5", "1d2");
	CRM("1d1", "0d0", "1d2", "0d0");
	CRD("A foot wide and covered with grayish brown hair, this spider contemplates you with its eight eyes.  Spiders like this one have filled the corners of the caves of Avanor with webs.  Usually just a nuisance, some of them are equipped with deadly venom.");

	CRVW(CN_GIANT_SPIDER, "giant spider", 'S', xLIGHTGRAY, CPT_IT, CRL_VERY_LOW, CR_INSECT);
	CRBA("1d10+80", "0d0+1000", "1d100+600", CS_VERY_SMALL, "1d10+10");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d2 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("fire:1d30-50 cold:1d20-40");
	CRC("3d5", "1d3");
	CRM("1d2", "0d0", "1d3", "0d0");
	CRD("These eight legged monstrosities escaped from a long dead wizard's laboratory and made homes in the deep caves of the mountains surrounding the valley.  Their webs are capable of ensnaring an unwary adventurer and their venom can be quite deadly.");
	CRAT(BR_POISON, 15);
	CRCOE(CET_MODIFY_R_POISON, 1);
	CRCOE(CET_POISON, 10);

	CRVW(CN_TARANTULA, "tarantula", 'S', xLIGHTGRAY, CPT_IT, CRL_LOW, CR_INSECT);
	CRBA("1d10+70", "0d0+1000", "1d100+600", CS_VERY_SMALL, "3d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("fire:1d30-50 cold:1d20-60");
	CRC("3d5", "1d6");
	CRM("1d1", "0d0", "1d3", "0d0");
	CRD("These spiders live in nooks and crannies in the rock and can pop into view when least expected.  Unable to spin webs, they rely on brute strength and venom to overcome a victim.  Creatures may be paralyzed by their venom only to be hauled back to a hole and eaten alive by the tarantula's young.");
	CRAT(BR_POISON, 40);
	CRAT(BR_PARALYSE, 10);
	CRCOE(CET_MODIFY_R_POISON, 2);
	CRCOE(CET_POISON, 15);

	CRVW(CN_SCORPION, "scorpion", 'S', xYELLOW, CPT_IT, CRL_LOW, CR_INSECT);
	CRBA("1d10+70", "0d0+1000", "1d100+600", CS_VERY_SMALL, "3d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("fire:1d30-50 cold:1d20-60");
	CRC("5d5", "1d6");
	CRM("1d1", "0d0", "1d5", "0d0");
	CRD("This creature is a foot long and colored a dusty brown with a tail arching high over it's back.  Scorpions use the poison in their stinging tail as well as formidable pincers as a powerful attack.  They also have a strong exoskeleton which protects them from many attacks.");
	CRAT(BR_POISON, 60);
	CRAT(BR_PARALYSE, 20);
	CRCOE(CET_MODIFY_R_POISON, 3);
	CRCOE(CET_POISON, 20);
	CRCOE(CET_PARALYSE, 20);

	CRVW(CN_BLACK_SCORPION, "black scorpion", 'S', xDARKGRAY, CPT_IT, CRL_LOW, CR_INSECT);
	CRBA("1d10+70", "0d0+1000", "1d100+600", CS_VERY_SMALL, "3d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("fire:1d30-50 cold:1d20-60");
	CRC("10d5", "1d10");
	CRM("1d1", "0d0", "1d7", "0d0");
	CRD("Black scorpions hide in the shadows of caves and will often flee from a light source.  When cornered, they fight with both pincers and their tail with great agility.  Though not as strong as their dusty cousins, they do move quite a bit faster.");
	CRAT(BR_POISON, 80);
	CRAT(BR_PARALYSE, 30);
	CRCOE(CET_MODIFY_R_POISON, 5);
	CRCOE(CET_POISON, 30);
	CRCOE(CET_PARALYSE, 30);

	CRVW(CN_PINK_SCORPION, "pink scorpion", 'S', xLIGHTMAGENTA, CPT_IT, CRL_AVG, CR_INSECT);
	CRBA("1d10+70", "0d0+1000", "1d100+600", CS_VERY_SMALL, "3d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("fire:1d30-50 cold:1d20-60");
	CRC("20d5", "1d20");
	CRM("1d1", "0d0", "1d9", "0d0");
	CRD("This scorpion originally evolved with this coloring in order to hide among plant flowers and surprise prey.  These scorpions are extremely aggressive and dangerous.  Sometimes they still carry exotic pollen from the flowers on the tip of their tails.");
	CRAT(BR_POISON, 100);
	CRAT(BR_PARALYSE, 40);
	CRAT(BR_STUN, 40);
	CRAT(BR_CONFUSE, 40);
	CRCOE(CET_MODIFY_R_POISON, 10);
	CRCOE(CET_POISON, 50);
	CRCOE(CET_PARALYSE, 50);


	CRVW(CN_FIRE_BEETLE, "fire beetle", 'i', xRED, CPT_IT, CRL_VERY_LOW, CR_INSECT);
	CRBA("1d10+100", "0d0+1000", "1d300+900", CS_VERY_SMALL, "5d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d3 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("fire:1d90 cold:1d20-80");
	CRC("1d3", "1d3");
	CRM("1d1", "1d8", "1d3", "0d0");
	CRD("Usually living only in depths of the earth, fire beetles were drawn to the surface by the intense energies used to raise the mountains surrounding Avanor.  Their normal habitat has made them immune to great heat and their thick exo-skeleton protects them from many physical attacks.");
	CRAT(BR_FIRE, 100);
	CRCOE(CET_MODIFY_R_FIRE, 2);
	CRCOE(CET_MODIFY_R_COLD, -1);


	CRVW(CN_FROST_BEETLE, "frost beetle", 'i', xWHITE, CPT_IT, CRL_LOW, CR_INSECT);
	CRBA("1d10+110", "0d0+1000", "1d300+900", CS_VERY_SMALL, "7d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d3 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("fire:1d20-80 cold:1d80");
	CRC("1d5", "1d10");
	CRM("1d1", "4d4", "1d5", "0d0");
	CRD("Frost beetles are characterized by a white shell and faint blue markings.  They usually live high in the mountains upon snow capped peaks.  With the release of power in the mountain chain, many of them fled to the caverns below to seek shelter.  They are immune to cold attacks and their thick exo-skeleton protects them from many physical attacks.");
	CRAT(BR_COLD, 100);
	CRCOE(CET_MODIFY_R_COLD, 2);
	CRCOE(CET_MODIFY_R_FIRE, -1);


	CRVW(CN_GREEN_BEETLE, "green beetle", 'i', xGREEN, CPT_IT, CRL_LOW, CR_INSECT);
	CRBA("1d10+120", "0d0+1000", "1d300+900", CS_VERY_SMALL, "7d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d5 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("acid:1d80");
	CRC("1d5", "1d12");
	CRM("1d1", "5d4", "1d8", "0d0");
	CRD("Green beetles are often seen in packs.  Their exo-skeleton is not as tough as the frost or fire beetle, but they are able to shoot streams of acid at their target with great accuracy.");
	CRAT(BR_ACID, 100);
	CRCOE(CET_MODIFY_R_ACID, 2);
	CRCOE(CET_MODIFY_STOMACH, 1);


	CRVW(CN_KILLER_BEETLE, "killer beetle", 'i', xBLUE, CPT_IT, CRL_AVG, CR_INSECT);
	CRBA("1d10+150", "0d0+1000", "1d300+900", CS_VERY_SMALL, "10d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d10 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("acid:1d80 cold:1d80 fire:1d80");
	CRC("1d10", "2d15");
	CRM("1d1", "6d5", "1d8", "0d0");
	CRD("This beetle scuttles around with great energy, moving half and again as fast as any other beetle you have seen.  Its shell seems thin but the formidable jaws leave no doubt of the damage it can inflict.  Its large antennae make it able to detect creatures around it, visible or not.");
	CRAT(BR_POISON, 30);
	CRAT(BR_PARALYSE, 70);
	CRCOE(CET_MODIFY_R_PARALYSE, 10);
	CRCOE(CET_MODIFY_STOMACH, 5);


	CRVW(CN_DEATH_BEETLE, "death beetle", 'i', xDARKGRAY, CPT_IT, CRL_HI, CR_INSECT);
	CRBA("1d30+170", "0d0+1000", "1d300+900", CS_VERY_SMALL, "15d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d15 Dx 1d5 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("acid:1d100 cold:1d100 fire:1d100");
	CRC("2d10", "3d15");
	CRM("1d1", "6d7", "1d10", "0d0");
	CRD("Extremely heavily armored, the death beetle is a dark blue color bordering on black.  The white markings on it's shell resemble bleached skulls.  Although slow moving, the death beetles venom is extremely potent and unless treated immeadiately can kill an individual within minutes.");
	CRAT(BR_POISON, 50);
	CRAT(BR_PARALYSE, 100);
	CRCOE(CET_MODIFY_R_PARALYSE, 15);
	CRCOE(CET_MODIFY_STOMACH, 10);


	CRVW(CN_GIANT_BEE, "giant bee", 'b', xYELLOW, CPT_IT, CRL_AVG, CR_INSECT);
	CRBA("1d100+200", "0d0+1000", "1d300+1300", CS_VERY_SMALL, "5d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("");
	CRC("1d5", "1d30");
	CRM("5d5", "1d1", "1d2", "0d0");
	CRD("The buzz in the air reminds you more of a hailstorm on a tin roof than an insect but as the giant bee rounds the corner ahead you can see why.  Three feet long and armed with a six inch stinger, this is a far cry from a normal bee.  It doesn't look like it is happy to see you in its territory either.");
	CRAT(BR_POISON, 100);

	CRVW(CN_GIANT_WASP, "giant wasp", 'b', xBROWN, CPT_IT, CRL_AVG, CR_INSECT);
	CRBA("1d100+200", "0d0+1000", "1d300+600", CS_VERY_SMALL, "5d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("");
	CRC("1d8", "1d10");
	CRM("7d5", "1d1", "1d2", "0d0");
	CRD("Wasps tend to build nests in areas inaccessible to larger creatures and many wasps can sometimes be found clustered around the opening.  The wasp is stronger than the bee and generally more irritable.  The thin waist of the wasp is a weak point and can be severed with a carefully aimed blow.");
	CRAT(BR_POISON, 100);

	CRVW(CN_CENTIPEDE, "centipede", 'S', xLIGHTGRAY, CPT_IT, CRL_VERY_LOW, CR_INSECT);
	CRBA("1d30+80", "0d0+1000", "1d100+950", CS_VERY_SMALL, "3d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("");
	CRC("1d3", "1d3");
	CRM("1d4", "1d1", "1d2", "0d0");
	CRD("Marching forward the centipede comes into view.  Low to the ground and a dark red color, it clearly advertises the fact that it carries poison.  The multiple legs make it easy for the centipede to capture its prey.");
	CRAT(BR_PARALYSE, 3);

	CRVW(CN_STEGOCENTIPEDE, "stegocentipede", 'S', xDARKGRAY, CPT_IT, CRL_LOW, CR_INSECT);
	CRBA("1d30+80", "0d0+1000", "1d100+950", CS_VERY_SMALL, "3d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("");
	CRC("2d3", "2d3");
	CRM("1d8", "1d2", "1d4", "0d0");
	CRD("Slower than their smaller cousins, the stegocentipede is also that much more heavily armored.  It uses its spiked tail to its advantage to knock prey down before attempting to poison it.");
	CRAT(BR_PARALYSE, 7);

	CRVW(CN_DUNGEON_CRAWLER, "dungeon crawler", 'c', xWHITE, CPT_IT, CRL_VERY_LOW, CR_INSECT);
	CRBA("1d10+50", "0d0+1000", "1d200+300", CS_VERY_SMALL, "10d2");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("");
	CRC("2d4", "1d3");
	CRM("1d1", "1d4", "1d2", "0d0");
	CRD("This filthy little insect wanders the floor.  Evolving in the caves, it almost completely forsook the use of eyes, relying on other senses to help it through the darkness.  Usually more of a nuisance than anything else, many adventurers simply crush them with a boot as they walk past.");
	CRAT(BR_POISON, 1);

	CRVW(CN_GIANT_CRAWLER, "giant crawler", 'c', xLIGHTGRAY, CPT_IT, CRL_AVG, CR_INSECT);
	CRBA("1d10+50", "0d0+1000", "1d200+300", CS_SMALL, "10d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("");
	CRC("2d4", "1d3");
	CRM("1d1", "1d8", "1d4", "0d0");
	CRD("This appears to be a mound of flesh approximately a foot high.  Only as it moves towards you do you realize that it still lives and sees you as viable prey.  The greyish green surface undulates hypnotically as you watch it ooze its way across the floor.");
	CRAT(BR_PARALYSE, 40);
	CRAT(BR_POISON, 60);

	CRVW(CN_CARRION_CRAWLER, "carrion crawler", 'c', xDARKGRAY, CPT_IT, CRL_AVG, CR_INSECT);
	CRBA("1d10+50", "0d0+1000", "1d100+200", CS_LARGE, "20d4");
	CRBO("");
	CRA(AIF_INSECT);
	CRS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	CRR("");
	CRC("2d4", "2d15");
	CRM("1d1", "1d10", "1d8", "0d0");
	CRD("This giant bug has numerous paralyzing tentacles just waiting for you to wander into range.  They live mostly in subterranean caverns and feed on dead or dying matter, but they will attack if hungry...");
	CRAT(BR_PARALYSE, 100);
	CRAT(BR_POISON, 100);

------------------------------ KOBOLDS --------------------------------------

	CRVW(CN_KOBOLD, "kobold", 'k', xLIGHTGREEN, CPT_HE, CRL_VERY_LOW, CR_KOBOLD);
	CRBA("1d10+90", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+600");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 1);
	CRA(AIF_CREATURE + AIF_ALLOW_PACK);
	CRS("St 2d3 Dx 2d3 To 1d3 Le 2d3 Wi 1d3 Ma 1d2 Pe 1d6 Ch 1d3");
	CRR("");
	CRC("1d2", "1d2");
	CRM("1d1", "0d0", "2d3", "2d2");
	CRD("Kobolds resemble nothing better than a small yipping dog that has somehow learned to walk on hind legs and wield weapons.  They are more nuisance than anything else but more than one adventurer has been swarmed under by a pack of them.");
	CRL(SKT_HEALING, 4);
	CRL(SKT_FINDWEAKNESS, 4);
	CRL(SKT_ARCHERY, 4);
	CREQ2(IM_SCROLL + IM_POTION + IM_BOOK + IM_FOOD, 2, 10);
	CREQ(IM_WEAPON, IT_DAGGER, 100);
	CREQ(IM_MISSILEW, IT_SHORTBOW, 10);
	CRCOE(CET_DISEASE, 20);

	CRVW(CN_LARGE_KOBOLD, "large kobold", 'k', xGREEN, CPT_HE, CRL_LOW, CR_KOBOLD);
	CRBA("1d10+90", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+800");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 2);
	CRA(AIF_CREATURE + AIF_ALLOW_PACK);
	CRS("St 2d5 Dx 2d4 To 2d3 Le 2d4 Wi 1d5 Ma 1d3 Pe 1d8 Ch 1d4");
	CRR("");
	CRC("1d3", "1d2");
	CRM("1d3", "1d1", "2d5", "2d3");
	CRD("Somehow this kobold managed to survive its early development. It is not much smarter than the smaller, younger kobolds but definately has more strength.");
	CRL(SKT_HEALING, 5);
	CRL(SKT_FINDWEAKNESS, 5);
	CRL(SKT_ARCHERY, 5);
	CREQ2(IM_SCROLL + IM_POTION + IM_BOOK + IM_FOOD, 3, 12);
	CREQ(IM_WEAPON, IT_DAGGER, 100);
	CREQ(IM_WEAPON, IT_DAGGER, 10);
	CREQ(IM_MISSILEW, IT_SHORTBOW, 15);
	CRCOE(CET_DISEASE, 30);

	CRVW(CN_CHIEFTAIN_KOBOLD, "kobold chieftain", 'k', xGREEN, CPT_HE, CRL_LOW, CR_KOBOLD);
	CRBA("1d10+90", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+800");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 5);
	CRA(AIF_CREATURE + AIF_ALLOW_PACK);
	CRS("St 3d5 Dx 2d5 To 3d3 Le 2d4 Wi 1d5 Ma 1d3 Pe 1d8 Ch 1d4");
	CRR("");
	CRC("1d4", "1d3");
	CRM("1d4", "1d2", "2d6", "2d3");
	CRD("The largest kobold you have yet seen. There is a glint of wicked intelligence behind its eyes as it approaches.  The kobold chieftain only acheived his position by being stronger, smarter and meaner then everyone else.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CRL(SKT_ARCHERY, 6);
	CREQ2(IM_SCROLL + IM_POTION + IM_BOOK + IM_FOOD, 4, 15);
	CREQ(IM_WEAPON, IT_DAGGER, 100);
	CREQ(IM_WEAPON, IT_DAGGER, 20);
	CREQ(IM_MISSILEW, IT_SHORTBOW, 10);
	CRCOE(CET_DISEASE, 30);
	

	CRVW(CN_SHAMAN_KOBOLD, "kobold shaman", 'k', xLIGHTCYAN, CPT_HE, CRL_LOW, CR_KOBOLD);
	CRBA("1d10+90", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+600");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 5);
	CRA(AIF_CREATURE + AIF_ALLOW_PACK);
	CRS("St 2d4 Dx 2d4 To 2d3 Le 4d4 Wi 4d5 Ma 4d6 Pe 1d6+6 Ch 2d4");
	CRR("");
	CRC("1d2", "1d2");
	CRM("1d1", "0d0", "1d8", "5d5");
	CRD("Smeared with arcane symbols in white chalk and wearing a necklace of bones, the kobold shaman is only a little larger than the average kobold.  The intensity of its gaze shows the resentment it feels for having a superior mind locked into an inferior body.  As you watch, it begins a chant to call forth its powers of magic against you...");
	CRL(SPELL_MAGIC_ARROW);
	CRL(SPELL_CURE_LIGHT_WOUNDS);
	CRL(SKT_HEALING, 4);
	CRL(SKT_CONCENTRATION, 4);
	CREQ2(IM_SCROLL + IM_POTION + IM_BOOK + IM_FOOD, 4, 15);
	CREQ(IM_WEAPON, IT_DAGGER, 100);
	CRCOE(CET_DISEASE, 30);
	CRCOE(CET_MODIFY_MA, 1);


	CRVW(CN_GNOLL, "gnoll", 'g', xBROWN, CPT_HE, CRL_ABOVE_LOW, CR_KOBOLD);
	CRBA("1d10+100", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+800");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 5);
	CRA(AIF_CREATURE + AIF_ALLOW_PACK);
	CRS("St 3d4 Dx 3d3 To 3d5 Le 1d4 Wi 1d4 Ma 1d4 Pe 2d5 Ch 2d3");
	CRR("");
	CRC("1d3", "1d4");
	CRM("1d4", "1d2", "2d5+15", "2d5");
	CRD("A gnoll is to a kobold what a giant is to a man.  This towering cross between man and dog is known for its cunning and strength.  Carrying an axe it will plow right through most opponents leaving them dazed... if they are lucky.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CREQ2(IM_SCROLL + IM_POTION + IM_BOOK + IM_FOOD, 2, 10);
	CREQ(IM_WEAPON, IT_WARAXE, 100);
	CRCOE(CET_MODIFY_TO, 1);

	CRVW(CN_GNOLL_WARMASTER, "gnoll warmaster", 'g', xYELLOW, CPT_HE, CRL_AVG, CR_KOBOLD);
	CRBA("1d10+100", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+800");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 7);
	CRA(AIF_CREATURE + AIF_ALLOW_PACK);
	CRS("St 6d4 Dx 7d3 To 6d5 Le 3d4 Wi 3d4 Ma 3d4 Pe 8d5 Ch 2d3");
	CRR("");
	CRC("2d5", "2d4");
	CRM("2d5", "3d2", "3d5+25", "2d5");
	CRD("The gnoll warmaster is an eight foot fighting machine.  The double axes it carries weave an intricate dance of death around it as it takes on multiple opponents.  Only a fool would get in this berserker's way.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CREQ2(IM_SCROLL + IM_POTION + IM_BOOK + IM_FOOD, 3, 10);
	CREQ(IM_WEAPON, IT_WARAXE, 100);
	CRCOE(CET_MODIFY_TO, 1);
	

------------------------------ GOBLINS --------------------------------------	

	CRVW(CN_GOBLIN, "goblin", 'g', xLIGHTGREEN, CPT_HE, CRL_VERY_LOW, CR_GOBLIN);
	CRBA("1d10+90", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+800");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 3);
	CRA(AIF_CREATURE);
	CRS("St 3d3 Dx 2d3 To 2d3 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	CRR("");
	CRC("1d3", "1d3");
	CRM("1d3", "1d1", "1d5+6", "1d5");
	CRD("This scruffy looking humanoid glances at you with fear in its face.  The stench it carries indicates that it has probably never been clean in its life.  Goblins normally perfer secrecy and stealing to open confrontation but have been known to attack travellers.");
	CRL(SKT_HEALING, 4);
	CRL(SKT_FINDWEAKNESS, 4);
	CREQ2(IM_SCROLL + IM_POTION, 1, 10);
	CREQ(IM_WEAPON, IT_SHORTSWORD, 100);

	CRVW(CN_GOBLIN_WARRIOR, "goblin warrior", 'g', xGREEN, CPT_HE, CRL_LOW, CR_GOBLIN);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+800");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 5);
	CRA(AIF_CREATURE);
	CRS("St 3d4 Dx 3d3 To 3d3 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 2d3");
	CRR("");
	CRC("1d4", "1d3");
	CRM("1d4", "1d1", "1d5+10", "1d5");
	CRD("This goblin carries a nasty looking knife, almost a sword for one as short as he is.  His posture and multiple scars indicate that he is more than a novice at handling them as well.");
	CRL(SKT_HEALING, 5);
	CRL(SKT_FINDWEAKNESS, 5);
	CREQ2(IM_SCROLL + IM_POTION, 2, 10);
	CREQ(IM_WEAPON, IT_SHORTSWORD, 100);

	CRVW(CN_GOBLIN_WARMASTER, "goblin warmaster", 'g', xBLUE, CPT_HE, CRL_LOW, CR_GOBLIN);
	CRBA("1d10+100", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+800");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 7);
	CRA(AIF_CREATURE);
	CRS("St 5d4 Dx 6d3 To 5d5 Le 2d4 Wi 2d4 Ma 2d4 Pe 5d5 Ch 3d3");
	CRR("");
	CRC("1d8", "2d3");
	CRM("1d8", "1d3", "2d5+20", "2d5");
	CRD("Arms criss-crossed with scars and bulging with muscle, this goblin has a hint of white hair.  Not many goblins can live so long but the worn weapons at his side indicate that he is capable of keeping his place in the earth.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CREQ2(IM_SCROLL + IM_POTION, 3, 10);
	CREQ(IM_WEAPON, IT_SHORTSWORD, 100);

	CRVW(CN_GOBLIN_CHIEFTAIN, "goblin chieftain", 'g', xRED, CPT_HE, CRL_AVG, CR_GOBLIN);
	CRBA("1d10+100", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+800");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 10);
	CRA(AIF_CREATURE);
	CRS("St 5d4 Dx 6d3 To 5d5 Le 2d4 Wi 2d4 Ma 2d4 Pe 5d5 Ch 3d3");
	CRR("");
	CRC("1d8", "2d4");
	CRM("1d10", "2d2", "2d5+25", "2d5");
	CRD("Stronger and smarter than the other goblins.  This goblin has become the chief and now commands all the troops.  His shock of white hair is the only part of him that shows his age and his muscular body is toned from battles past.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CREQ2(IM_SCROLL + IM_POTION, 4, 10);
	CREQ(IM_WEAPON, IT_SHORTSWORD, 100);


------------------------------ ORCS --------------------------------------	

	CRVW(CN_ORC, "orc", 'o', xLIGHTGREEN, CPT_HE, CRL_AVG, CR_ORC);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d400+1400");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 30);
	CRA(AIF_CREATURE);
	CRS("St 2d5+15 Dx 1d5+5 To 2d5+15 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	CRR("");
	CRC("1d6", "2d4");
	CRM("2d3", "1d2", "2d5+25", "2d5");
	CRD("Orcs are roughly dwarf sized with a light green pallor to their skin.  Not extremely intelligent, they avoid sunlight whenever possible.  It has been speculated that they were once humans elves or dwarves but something drove them deep underground and they became the hardy race they are today.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CREQ2(IM_SCROLL + IM_POTION, 3, 50);
	CREQ(IM_WEAPON, IT_BATTLEAXE, 100);

	CRVW(CN_LARGE_ORC, "large orc", 'o', xGREEN, CPT_HE, CRL_AVG, CR_ORC);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d400+1400");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 30);
	CRA(AIF_CREATURE);
	CRS("St 2d5+20 Dx 1d5+5 To 2d5+20 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	CRR("");
	CRC("1d6", "2d4");
	CRM("2d3", "1d2", "2d5+25", "2d5");
	CRD("Bigger, meaner and nastier than its smaller cousins, this orc stands near as tall as a man.  It bears a cruel looking axe and is well armored.  The wicked grin on its face tells you that it will enjoy picking your flesh from your bones.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CREQ2(IM_SCROLL + IM_POTION, 3, 50);
	CREQ(IM_WEAPON, IT_BATTLEAXE, 100);
	CRCOE(CET_MODIFY_ST, 1);

	CRVW(CN_HILL_ORC, "hill orc", 'o', xYELLOW, CPT_HE, CRL_AVG, CR_ORC);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d400+1400");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 30);
	CRA(AIF_CREATURE);
	CRS("St 2d5+25 Dx 1d5+5 To 2d5+25 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	CRR("");
	CRC("1d6", "2d4");
	CRM("2d3", "1d2", "2d5+25", "2d5");
	CRD("Hill orcs are descendants of orcs that left their caves to live back under the sky.  They still prefer to sleep during the day but move around enough during the day to acquire a healthy tan.  Perhaps their return to sunlight is the reason they are stronger than their cousins.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CREQ2(IM_SCROLL + IM_POTION, 3, 50);
	CREQ(IM_WEAPON, IT_BATTLEAXE, 100);
	CRCOE(CET_MODIFY_ST, 1);
 
	CRVW(CN_DARK_ORC, "dark orc", 'o', xDARKGRAY, CPT_HE, CRL_AVG, CR_ORC);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d400+1400");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 30);
	CRA(AIF_CREATURE);
	CRS("St 2d5+30 Dx 1d5+5 To 2d5+30 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	CRR("");
	CRC("1d6", "2d4");
	CRM("2d3", "1d2", "2d5+25", "2d5");
	CRD("Dark orcs retreated to the farthest corners of the caverns and were enslaved by the other orcs.  They were used to delve great cities beneath the earth like the dwarves.  Because of all this manual labor they grew extremely strong.  Before any battle, they cover themselves in coal dust to appear jet black and frighten their opponents.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CREQ2(IM_SCROLL + IM_POTION, 3, 50);
	CREQ(IM_WEAPON, IT_BATTLEAXE, 100);
	CRCOE(CET_MODIFY_ST, 1);

	CRVW(CN_LIEUTENANT_ORC, "orc lieutenant", 'o', xBROWN, CPT_HE, CRL_AVG, CR_ORC);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d400+1400");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 30);
	CRA(AIF_CREATURE);
	CRS("St 2d5+30 Dx 1d5+5 To 2d5+30 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	CRR("");
	CRC("1d6", "2d4");
	CRM("2d3", "1d2", "2d5+25", "2d5");
	CRD("\"Git yur sorry corpses movin'!\"  Orc sergeants act as the first tier of leadership for any orcish raid.  Known to be exceptionally mean spirited, they have been seen to kill their own troops in order to \"inspire\" the rest to better results.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CREQ2(IM_SCROLL + IM_POTION, 3, 50);
	CREQ(IM_WEAPON, IT_BATTLEAXE, 100);
	CRCOE(CET_MODIFY_ST, 2);

	CRVW(CN_CAPTAIN_ORC, "orc captain", 'o', xBROWN, CPT_HE, CRL_AVG, CR_ORC);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d400+1400");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 30);
	CRA(AIF_CREATURE);
	CRS("St 2d5+40 Dx 1d5+5 To 2d5+40 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	CRR("");
	CRC("1d6", "2d4");
	CRM("2d3", "1d2", "2d5+25", "2d5");
	CRD("Captain is a precarious position for any orc.  Displeased chiefs might have their captains killed for any failure, success or even on a whim.  The orcs under him are ready to mutiny and kill him for his position.  Thus the orc captain is always on edge and ready to strike down anything that might pose a danger.  His large frame and hefty axe are usually more than adequate to take care of any and all threats.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CREQ2(IM_SCROLL + IM_POTION, 3, 50);
	CREQ(IM_WEAPON, IT_BATTLEAXE, 100);
	CRCOE(CET_MODIFY_ST, 2);

	CRVW(CN_CHIEFTAIN_ORC, "orc chieftain", 'o', xBLUE, CPT_HE, CRL_AVG, CR_ORC);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d400+1400");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 30);
	CRA(AIF_CREATURE);
	CRS("St 2d5+50 Dx 1d5+5 To 2d5+50 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	CRR("");
	CRC("1d6", "2d4");
	CRM("2d3", "1d2", "2d5+25", "2d5");
	CRD("The orc chieftain is the biggest, meanest orc you have ever seen.  His skin alone looks tougher than studded leather.  The axe he carries is well worn and scarred from many battles.  Only someone this mean and nasty could get the various orc tribes to work together for a raid.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CREQ2(IM_SCROLL + IM_POTION, 3, 50);
	CREQ(IM_WEAPON, IT_BATTLEAXE, 100);
	CRCOE(CET_MODIFY_ST, 3);


------------------------------ HUMANS & HUMANOIDS --------------------------------------	

	CRVW(CN_FARMER, "farmer", 'p', xBROWN, CPT_HE, CRL_VERY_LOW, CR_HUMAN);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 0, GFS_SUPRESS_INVIS);
	CRA(AIF_HUMAN + AIF_PEACEFUL);
	CRS("St 2d3 Dx 2d3 To 1d3 Le 2d3 Wi 1d3 Ma 1d2 Pe 1d6+6 Ch 1d3");
	CRR("");
	CRC("1d2", "1d2");
	CRM("1d1", "0d0", "2d3+1", "2d2");
	CRD("The sunburnt face and arms of this fellow, as well as the pitchfork he leans upon give no doubt as to his profession.  He and his fellows produce and gather the food for the entire valley.");
	CRL(SKT_HEALING, 2);
	CREQ(IM_WEAPON, IT_PITCHFORK, 100);
	CREQ(IM_BODY, IT_ROBE, 100);

	CRVW(CN_GOODWIFE, "goodwife", 'p', xYELLOW, CPT_SHE, CRL_VERY_LOW, CR_HUMAN);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 0, GFS_SUPRESS_INVIS);
	CRA(AIF_HUMAN + AIF_PEACEFUL);
	CRS("St 2d3 Dx 2d3 To 1d3 Le 2d3 Wi 1d3 Ma 1d2 Pe 1d6+6 Ch 1d3");
	CRR("");
	CRC("1d2", "1d2");
	CRM("1d1", "0d0", "2d3+1", "2d2");
	CRD("This woman's clothing tells a story of a simple but hard life.  She prepares the meals for her husband every day and takes care of their cottage.");
	CRL(SKT_HEALING, 2);
	CREQ(IM_BODY, IT_DRESS, 100);


	CRVW(CN_BANDIT, "bandit", 'p', xLIGHTGRAY, CPT_HE, CRL_LOW, CR_HUMAN);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 50);
	CRA(AIF_HUMAN);
	CRS("St 1d4+10 Dx 1d4+10 To 1d4+10 Le 1d4+10 Wi 1d4+10 Ma 1d4+10 Pe 1d4+15 Ch 1d4+10");
	CRR("");
	CRC("1d4", "1d3");
	CRM("1d4", "0d0", "1d4+10", "2d5");
	CRD("A scruffy looking fellow, he looks much like any citizen of the valley except for the green cape, the twin daggers and the malicious glint in his eye.  You get the distinct impression that he would rob his own mother if she had anything worth taking.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CREQ2(IM_SCROLL + IM_POTION, 3, 30);
	CREQ(IM_WEAPON, IT_LONGDAGGER, 100);
	CREQ(IM_WEAPON, IT_LONGDAGGER, 100);

	CRVW(CN_CITIZEN, "citizen", 'p', xLIGHTGRAY, CPT_HE, CRL_VERY_LOW, CR_HUMAN);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 0, GFS_SUPRESS_INVIS);
	CRA(AIF_HUMAN + AIF_PEACEFUL);
	CRS("St 2d3 Dx 2d3 To 1d3 Le 2d3 Wi 1d3 Ma 1d2 Pe 1d6+6 Ch 1d3");
	CRR("");
	CRC("1d2", "1d2");
	CRM("1d1", "0d0", "2d3+1", "2d2");
	CRD("This upstanding citizen of the castle of Avanor hurries about the duties of the day.  The calm relaxed confidence surrounding him demonstrates his faith in the forces of the guard and the king to protect him and his family.");
	CREQ(IM_BODY, IT_CLOTHES, 100);

	CRVW(CN_FCITIZEN, "citizen", 'p', xLIGHTGRAY, CPT_SHE, CRL_VERY_LOW, CR_HUMAN);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 0, GFS_SUPRESS_INVIS);
	CRA(AIF_HUMAN + AIF_PEACEFUL);
	CRS("St 2d3 Dx 2d3 To 1d3 Le 2d3 Wi 1d3 Ma 1d2 Pe 1d6+6 Ch 1d3");
	CRR("");
	CRC("1d2", "1d2");
	CRM("1d1", "0d0", "2d3+1", "2d2");
	CRD("This upstanding citizen of the castle of Avanor hurries about the duties of the day.  The calm relaxed confidence surrounding her demonstrates her faith in the forces of the guard and the king to protect her and her family.");
	CREQ(IM_BODY, IT_DRESS, 100);

	CRVW(CN_ROYAL_GUARD, "royal guardian", 'p', xBLUE, CPT_HE, CRL_LOW, CR_HUMAN);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 50, GFS_SUPRESS_INVIS + GFS_SEE_INVIS);
	CRA(AIF_HUMAN + AIF_PEACEFUL);
	CRS("St 1d8+15 Dx 1d8+20 To 1d8+10 Le 1d5+10 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 5d5");
	CRR("");
	CRC("1d6", "2d2");
	CRM("3d2", "1d2", "1d5+10", "1d3+5");
	CRD("The elite guard of Avanor was organized by King Rodrick to protect citizens from the depradations of bandits and the monsters that live in the valley.  Every child dreams of someday serving in the King's guard.  Royal Guardians are well equipped to protect the people and themselves.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CREQ(IM_WEAPON, IT_LONGSWORD, 100);
	CREQ(IM_SHIELD, IT_TOWERSHIELD, 100);

	CRVW(CN_DEATH_KNIGHT, "death knight", 'p', xDARKGRAY, CPT_HE, CRL_LOW, CR_HUMAN);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 50);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD);
	CRS("St 1d8+20 Dx 1d8+20 To 1d8+10 Le 1d5+10 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 5d5");
	CRR("");
	CRC("1d6", "2d2");
	CRM("3d2", "1d2", "1d5+10", "1d3+5");
	CRD("The elite guard of Ahk-Ulan the sorcerer.  Deprived of all human emotion, they have become finely tuned killing machines who worship only death.  The stench of blood surrounds them and their soulless gaze makes your flesh crawl.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CREQ(IM_WEAPON, IT_LONGSWORD, 100);
	CREQ(IM_SHIELD, IT_TOWERSHIELD, 100);

	CRVW(CN_SHOPKEEPER, "shopkeeper", 'p', xLIGHTGRAY, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMAN);
	CRBA("1d10+120", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS + GFS_SEE_INVIS);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD + AIF_PEACEFUL);
	CRS("St 1d8+20 Dx 1d8+20 To 1d8+20 Le 1d5+20 Wi 1d4+20 Ma 1d4+20 Pe 3d6 Ch 5d5");
	CRR("");
	CRC("1d6", "2d2");
	CRM("3d2", "1d2", "1d5+10", "1d3+5");
	CRD("It takes a special breed to set up shop in this remote valley.  Shopkeepers must be able to defend their wares as well as chat amiably with customers.  Behind the cheery exterior is a sharp intelligence and well toned muscles.  Only the desperate or the foolish would try to steal from one of them.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);

	CRVW(CN_DWARF, "dwarf", 'h', xBROWN, CPT_HE, CRL_VERY_LOW, CR_HUMANOID);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 50);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD + AIF_PEACEFUL);
	CRS("St 1d3+10 Dx 1d3+7 To 1d3+12 Le 2d3 Wi 1d3 Ma 1d2 Pe 1d6 Ch 1d3");
	CRR("");
	CRC("1d2", "1d2");
	CRM("1d1", "0d0", "2d3+1", "2d2");
	CRD("Standing only about 4 feet high, dwarves are known to be master diggers and metal workers.  With a beard starting right below the merrily twinkling eyes, the dwarf whistles a lively tune as he goes about his business.  The axe stuck in his belt and his easy balance indicate that he can be a formidable warrior in times of danger.");

	CRVW(CN_DWARF_GUARD, "dwarven guardian", 'h', xLIGHTBLUE, CPT_HE, CRL_VERY_LOW, CR_HUMANOID);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 50, GFS_SEE_INVIS);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD + AIF_PEACEFUL);
	CRS("St 1d3+20 Dx 1d3+7 To 1d3+22 Le 2d3 Wi 1d3 Ma 1d2 Pe 1d6 Ch 1d3");
	CRR("");
	CRC("1d2", "1d2");
	CRM("1d1", "0d0", "2d3+1", "2d2");
	CRD("Guardians of the underground dwarven city, these dwarves are well armed and prepared for any breach of security both physical and magical.  Only a fool would attempt to accost one as he goes about his duties.  The cold glare and cold steel they carry is usually more than enough to keep trouble makers in line.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);
	CREQ(IM_WEAPON, IT_BATTLEAXE, 100);
	CREQ(IM_SHIELD, IT_LARGESHIELD, 100);


------------------------------ UNDEAD --------------------------------------	

	CRVW(CN_SKELETON, "skeleton", 's', xWHITE, CPT_IT, CRL_VERY_LOW, CR_UNDEAD);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body hand hand", 2);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM);
	CRS("St 4d3 Dx 2d2 To 2d3 Le 1d3 Wi 1d3 Ma 1d2 Pe 1d2 Ch 1d1");
	CRR("");
	CRC("1d3", "2d3");
	CRM("1d3", "1d2", "1d8", "1d3");
	CRD("Returned to life by evil magics, this creature shuffles towards you, fueled by hate for those still living.  It seeks only to return to the sweet oblivion of death, but first it wants to take you with it.");

	CRVW(CN_ZOMBIE, "zombie", 'z', xBROWN, CPT_IT, CRL_LOW, CR_UNDEAD);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body hand hand boots", 5);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM);
	CRS("St 8d3 Dx 2d2 To 5d5 Le 1d3 Wi 1d3 Ma 1d2 Pe 1d2 Ch 1d1");
	CRR("");
	CRC("1d3", "2d3");
	CRM("1d3", "1d2", "2d9", "1d3");
	CRD("The zombie stumbles across the floor, leaving a trail of decaying flesh behind it.  Though muscles makes the zombie stronger, the magics that animate it can do nothing to preserve the diseased hulk.  The stench is almost unbearable.");
	CRAT(BR_PARALYSE, 2);

	CRVW(CN_GHOUL, "ghoul", 'z', xLIGHTGRAY, CPT_IT, CRL_LOW, CR_UNDEAD);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body hand hand boots", 10);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM);
	CRS("St 10d3 Dx 2d3 To 7d5 Le 1d3 Wi 1d3 Ma 1d2 Pe 1d2 Ch 1d1");
	CRR("");
	CRC("1d3", "2d3");
	CRM("1d2", "1d4", "3d9", "1d3");
	CRD("The pasty white flesh of the ghoul is the result of numerous alchemical and magical experiments.  The preserved flesh makes it strong and tough but not much else.  Ghouls are not smart and have been observed attacking walls that stand in their way.");
	CRAT(BR_PARALYSE, 10);

	CRVW(CN_GHOST, "ghost", 'G', xLIGHTGRAY, CPT_IT, CRL_ABOVE_LOW, CR_UNDEAD);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("", 0);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM);
	CRS("St 6d5 Dx 4d5 To 2d5 Le 5d5 Wi 5d5 Ma 5d5 Pe 1d2 Ch 1d1");
	CRR("invisible:2d5 see_invisible:2d5+10");
	CRC("4d4", "2d4");
	CRM("6d4", "0d0", "1d9", "1d3");
	CRD("A chill breeze passing by is all most ever felt of this spirit set to wander upon the earth.  Those with good eyesight may spot ghosts in the dark corridors of the earth, but they do not take kindly to people who invade their domain.");

	CRVW(CN_SPECTRE, "spectre", 'G', xMAGENTA, CPT_IT, CRL_AVG, CR_UNDEAD);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("", 0);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM);
	CRS("St 6d5 Dx 4d5 To 2d5 Le 5d5 Wi 5d5 Ma 5d5 Pe 1d2 Ch 1d1");
	CRR("invisible:2d5+5 see_invisible:2d5+15");
	CRC("4d4", "4d4");
	CRM("6d4", "0d0", "2d9", "1d3");
	CRD("It is said the spectres were once the evil people of this earth.  Forbidden entry into the next world, their spirits seek to destroy those of this world.  They are almost never seen before their chill hands take hold upon the heart of their victim.");
	CRL(SPELL_DRAIN_LIFE);
	CRL(SKT_CONCENTRATION, 10);

	CRVW(CN_DREAD, "dread", 'G', xBROWN, CPT_IT, CRL_HI, CR_UNDEAD);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("", 0);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM);
	CRS("St 6d10 Dx 5d5 To 3d5 Le 8d5 Wi 8d5 Ma 8d5 Pe 1d2 Ch 1d1");
	CRR("invisible:2d5+10 see_invisible:2d5+20");
	CRC("6d6", "5d5");
	CRM("8d5", "0d0", "8d5", "10d8");
	CRD("Panic and fear threaten to overwhelm you as you face thise evil spirit.  Aptly named for the feelings it causes, The dread seeks only to destroy all living things.  So stealthy and invisible are they that most people will never know exactly what did them in.");
	CRL(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL);
	CRL(SKT_CONCENTRATION, 10);
	CRL(SPELL_DRAIN_LIFE);

	CRVW(CN_VAMPIRE, "vampire", 'V', xBLUE, CPT_HE, CRL_HI, CR_UNDEAD);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots", 100);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD);
	CRS("St 4d6 Dx 5d5 To 3d5 Le 8d5 Wi 8d5 Ma 8d8 Pe 1d2 Ch 1d1");
	CRR("invisible:2d5+30 see_invisible:2d5+30");
	CRC("4d6", "3d5");
	CRM("8d5", "0d0", "3d5", "10d10");
	CRD("This foul creature of the night drinks of its victims for sustenance, slowly draining their victim of life until they also become a vampire.  They possess numerous abilities, and have much power in the undead realms.  They fear light and holy powers, but are immune to many non-magical weapons.");
	CRAT(BR_PARALYSE, 20);
	CRL(SKT_BACKSTABBING, SKILL_MAX_LEVEL);
	CRL(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL);
	CRL(SKT_CONCENTRATION, SKILL_MAX_LEVEL);
	CRL(SPELL_DRAIN_LIFE);
	
	CRVW(CN_LICH, "lich", 'L', xLIGHTGRAY, CPT_HE, CRL_HI, CR_UNDEAD);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots", 100);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD);
	CRS("St 3d8 Dx 5d5 To 3d5 Le 8d5 Wi 10d5 Ma 10d8 Pe 1d2 Ch 1d1");
	CRR("invisible:2d5+30 see_invisible:2d5+40");
	CRC("2d5", "1d4");
	CRM("8d5", "0d0", "3d5", "10d10");
	CRD("\"Once a sorcerer, always a sorcerer.\" Liches were powerful magic wielders in life and they continue to wield that power after their death.  They are able to summon great magics down upon the living and absorb living energies to sustain their own existence.");
	CRL(SKT_HEALING, SKILL_MAX_LEVEL);
	CRL(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL);
	CRL(SKT_CONCENTRATION, SKILL_MAX_LEVEL);
	CRL(SPELL_DRAIN_LIFE);
	CRL(SPELL_MAGIC_ARROW); 
	CRL(SPELL_HEAL);


------------------------------ UNIQUE --------------------------------------	

	CRVW(CN_ELDER_GRIDOR, "Elder Gridor", 'p', xWHITE, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMAN);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 0, GFS_SUPRESS_INVIS);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD + AIF_PEACEFUL);
	CRS("St 1d3+5 Dx 1d3+5 To 1d3+4 Le 9d5 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 4d4");
	CRR("");
	CRC("1d1", "1d1");
	CRM("1d1", "0d0", "1d5+5", "1d5+5");
	CRD("This kind looking old man leans on a cane as he gazes towards you.  As leader of this small farming community, he is responsible for their well being.  He gazes at you and asks for help to a serious problem that has recently arisen and is out of his power to control.");
	CRL(SKT_HEALING, 6);
	CREQ2(IM_SCROLL + IM_POTION, 3, 30);
	CREQ(IM_BODY, IT_CLOTHES, 100);
	CREQ(IM_WEAPON, IT_STAFF, 100);

	CRVW(CN_JORGUS, "Jorgus, the master thief", 'p', xWHITE, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMAN);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 50, GFS_SUPRESS_INVIS + GFS_SEE_INVIS);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD + AIF_PEACEFUL);
	CRS("St 1d8+30 Dx 1d8+30 To 1d8+15 Le 1d5+15 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 5d5");
	CRR("");
	CRC("1d8", "2d2");
	CRM("4d2", "1d3", "1d5+15", "1d5+5");
	CRD("As you enter his dwelling you keep a tight hold on your purse.  The shifty eyes and too quick smile of the owner makes you nervous.  Jorgush is the leader of his outlaw band.  They are known for stealing from the rich and keeping it for themselves.  He appears to evaluate you and the weapons you bear and then gestures to the cahir at the table.  Perhaps he has a proposition for you...");
	CRL(SKT_STEALING, 15);
	CREQ2(IM_SCROLL + IM_POTION, 3, 30);
	CREQ(IM_BODY, IT_CLOTHES, 100);

	CRVW(CN_GEKTA, "Gekta, the sheep dog", 'C', xDARKGRAY, CPT_NAMED_IT, CRL_UNIQUE, CR_CANINE);
	CRBA("1d10+200", "0d0+1000", "1d100+300", CS_SMALL, "1d200+400");
	CRBO("neck", 100);
	CRA(AIF_HI_ANIMAL + AIF_PEACEFUL);
	CRS("St 1d3+10 Dx 1d5+20 To 1d3+8 Le 1d3+3 Wi 1d3+3 Ma 1d3+3 Pe 5d6 Ch 4d5");
	CRR("see_invisible:0d0+30");
	CRC("5d5", "2d6");
	CRM("4d5", "2d1", "1d5+10", "1d1+1");
	CRD("The sheep dog is the mascot for the royal guardians.  It must represent strength, intelligence and unswerving loyalty.  Gekta is the current favorite.  She bounds towards you and knocks you flat, covering you with slobbery dog kisses.");
	CRL(SKT_HEALING, 15);

	CRVW(CN_HIGHPRIEST, "Aphilius, the high priest of Avanor", 'p', xWHITE, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMANOID);
	CRBA("1d10+100", "0d0+800", "0d0+800", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS + GFS_SEE_INVIS);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD + AIF_PEACEFUL);
	CRS("St 1d8+10 Dx 1d8+15 To 1d8+10 Le 1d5+25 Wi 1d5+25 Ma 1d5+10 Pe 5d6 Ch 6d5");
	CRR("see_invisible:0d0+100");
	CRC("1d3", "1d2");
	CRM("1d4", "1d2", "1d5+30", "1d5+10");
	CRD("This compassionate soul gives his time and devotion to maintaining the temple.  He is dressed in the vestments of his position and bears the mitre of the priesthood...");
	CRL(SKT_HEALING, SKILL_MAX_LEVEL);
	CRL(SKT_RELIGION, SKILL_MAX_LEVEL);
	CRL(SPELL_HEAL);
	CREQ(IM_BODY, IT_ROBE, 100);

	CRVW(CN_OZORIK, "Ozorick, the royal guard captain", 'p', xLIGHTBLUE, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMAN);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS + GFS_SEE_INVIS);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD + AIF_PEACEFUL);
	CRS("St 1d8+30 Dx 1d8+30 To 1d8+15 Le 1d5+15 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 5d5");
	CRR("see_invisible:0d0+50");
	CRC("1d8", "2d2");
	CRM("4d2", "1d3", "1d5+15", "1d3+5");
	CRD("Standing just over five feet tall, Ozorick doesn't seem like such an imposing figure but the air of command about him is complete.  His thickly corded neck and arms lightly balance a huge broadsword covered with runes.  He swings it down as you enter and you can't help but gasp as the point cuts right through a stone in the floor and barely even slows.  No one who has seen him wield his mighty sword in battle would dream of opposing him.");
	CRL(SKT_HEALING, 10);
	CRL(SKT_FINDWEAKNESS, 10);

	CRVW(CN_YOHJISHIRO, "Yohjishiro, the elven wizard", 'h', xWHITE, CPT_NAMED_SHE, CRL_UNIQUE, CR_HUMANOID);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS + GFS_SEE_INVIS);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD + AIF_PEACEFUL);
	CRS("St 1d8+15 Dx 1d8+40 To 1d8+10 Le 1d5+45 Wi 1d5+45 Ma 1d5+45 Pe 5d6 Ch 7d5");
	CRR("see_invisible:0d0+100");
	CRC("1d3", "1d2");
	CRM("1d4", "1d1", "1d5+10", "5d5+50");
	CRD("Last of the elder wizards to live in the vale of Avanor, Yohjishiro quietly tends her herbs and gardens. Her white hair hangs down her back as she kneels down to look at one of her bushes.  Her pointed ears stick out from under a floppy hat that keeps the sun from her eyes.  As she walks through the garden, you notice plants sprouting wherever her feet touch the soil.  Truly she is a power of life.  Frail but intelligent, she remains apart from most of the happenings of Avanor.  Occasionally she will accept a pupil or give aid to those in need.");
	CRL(SKT_HEALING, SKILL_MAX_LEVEL);
	CRL(SPELL_LIGHTNING_BOLT);
	CRL(SPELL_HEAL);
	CREQ(IM_HAT, IT_HAT, 100);
	CREQ(IM_WEAPON, IT_STAFF, 100);

	CRVW(CN_GEFEON, "Gefeon, great master of Fire", 'p', xRED, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMAN);
	CRBA("1d10+110", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS + GFS_SEE_INVIS);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD + AIF_PEACEFUL);
	CRS("St 1d8+15 Dx 1d8+40 To 1d8+10 Le 1d5+45 Wi 1d5+45 Ma 1d5+45 Pe 5d6 Ch 7d5");
	CRR("see_invisible:0d0+100");
	CRC("1d3", "1d2");
	CRM("1d4", "1d1", "1d5+10", "5d5+50");
	CRD("As master of the eternal flame of Avanor, Gefeon advises the king in all matters arcane.  Clad only in robes and wearing rings of power, he sits and meditates on the state of the universe.  As you watch a furrow creases on his brow as if he is greatly worried by something.");
	CRL(SKT_HEALING, SKILL_MAX_LEVEL);
	CRL(SPELL_FIRE_BOLT);
	CRL(SPELL_HEAL);

	CRVW(CN_RODERIK, "Roderick, king of Avanor", 'p', xYELLOW, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMAN);
	CRBA("1d30+150", "0d0+700", "0d0+700", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS + GFS_SEE_INVIS);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD + AIF_PEACEFUL);
	CRS("St 1d8+55 Dx 1d8+60 To 1d8+40 Le 1d5+35 Wi 1d5+20 Ma 1d5+20 Pe 8d6 Ch 12d5");
	CRR("see_invisible:0d0+100");
	CRC("1d6", "1d5");
	CRM("1d8", "1d3", "1d5+70", "1d5+30");
	CRD("The mystical crown of Avanor rests upon the head of this noble looking man.  Curls of red hair stick out from under it but don't detract from his noble bearing.  The sceptre of his rule lies in his hand looking like an ornament but it has been said it is a formidable weapon wrought with great magic in days of yore.  Rodrick's face is happy but you can see great concern in his eyes.  The responsibilty for this nation must truly weigh on him.");
	CRL(SKT_HEALING, SKILL_MAX_LEVEL);
	CRL(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL);

	CRVW(CN_AHKULAN, "Ahk-Ulan, great master of Darkness", 'p', xDARKGRAY, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMAN);
	CRBA("1d30+120", "0d0+900", "0d0+900", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS + GFS_SEE_INVIS);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD + AIF_PEACEFUL);
	CRS("St 1d8+15 Dx 1d8+40 To 1d8+10 Le 1d5+45 Wi 1d5+45 Ma 1d5+45 Pe 5d6 Ch 7d5");
	CRR("see_invisible:0d0+100");
	CRC("1d3", "1d2");
	CRM("1d4", "1d1", "1d5+30", "5d5+50");
	CRD("Once a bright and promising sorcerer, Ahk-Ulan delved into dark magics and soon became the dark and twisted being he is today.  The aura of power and decay surrounds him as he seeks to control the destructive magics he has consumed.");
	CRL(SKT_HEALING, SKILL_MAX_LEVEL);
	CRL(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL);
	CRL(SKT_CONCENTRATION, SKILL_MAX_LEVEL);
	CRL(SPELL_ACID_BOLT);
	CRL(SPELL_HEAL);

	CRVW(CN_TODIN, "Todin, dwarven weaponsmith", 'h', xBROWN, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMANOID);
	CRBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS + GFS_SEE_INVIS);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD + AIF_PEACEFUL);
	CRS("St 1d8+30 Dx 1d8+30 To 1d8+15 Le 1d5+15 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 5d5");
	CRR("see_invisible:0d0+20");
	CRC("1d8", "2d2");
	CRM("4d2", "1d3", "1d5+15", "1d5+5");
	CRD("Squat, sturdy and built like a boulder here stands the King's twin brother and master smith.  Todin stands at his forge and works the bellows with one hand while nonchalantly shaping a sword with the hammer in his other hand.  The ruddy glow of the forge glimmers on his sweat drenched skin.  Truly he is a master smith as the weapons hanging about the room display his craft.");
	CRL(SKT_HEALING, 6);
	CRL(SKT_FINDWEAKNESS, 6);

	CRVW(CN_TORIN, "Torin the Great, dwarven king", 'h', xLIGHTGREEN, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMANOID);
	CRBA("1d10+100", "0d0+800", "0d0+800", CS_NORMAL, "1d200+1200");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS + GFS_SEE_INVIS);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD + AIF_PEACEFUL);
	CRS("St 1d8+35 Dx 1d8+20 To 1d8+30 Le 1d5+25 Wi 1d5+25 Ma 1d5+25 Pe 5d6 Ch 4d5");
	CRR("see_invisible:0d0+100");
	CRC("1d3", "1d2");
	CRM("1d4", "1d2", "1d5+30", "1d5+10");
	CRD("Twin brother to the master smith, Torin's obvious strength and coloring are all that own the relationship.  The crown on his head glitters and his great golden beard flows over his expansive chest.  The hand he rests on the hammer at his belt belies his prowess in battle.  As your eyes drift to the door at the back of his throne room he clears his throat as if to address you, and draw your attention.");
	CRL(SKT_HEALING, SKILL_MAX_LEVEL);
	CRL(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL);

	CRVW(CN_BEELZEVILE, "Beelzevile, the horned demon", '&', xLIGHTCYAN, CPT_NAMED_HE, CRL_UNIQUE, CR_DEMON);
	CRBA("1d30+50", "0d0+1500", "0d0+900", CS_LARGE, "1d400+4000");
	CRBO("head body ring ring", 100, GFS_SUPRESS_INVIS + GFS_SEE_INVIS);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD);
	CRS("St 5d5+50 Dx 3d3+10 To 2d15+20 Le 9d5 Wi 3d4 Ma 5d4 Pe 1d3 Ch 1d3");
	CRR("see_invisible:0d0+70");
	CRC("0d0+10", "3d15");
	CRM("0d0-10", "0d0+15", "5d5+100", "5d5+100");
	CRD("How he got to the mushroom caves is anyones guess but it is commonly believed that some kobold shaman got a little carried away with his magic and summoned Beelzevile from the netherworld.  Standing 10 feet high and carrying a reek of sulphur about him, he strides around the cavern, furious at being trapped in this cold place.  His massive claws can shred flesh and steel alike.  You catch a glimpse of a magnificent ring on one of his hands as he noisely chews on the haunch of a kobold who wandered a little too close.");
	CRAT(BR_POISON, 80);
	CRAT(BR_FIRE, 100);
	CRAT(BR_COLD, 100);

	CRVW(CN_XSHEE_VOO, "Xshee-Voo, the Cyclope", 'H', xLIGHTMAGENTA, CPT_NAMED_HE, CRL_UNIQUE, CR_GIANT);
	CRBA("1d30+120", "0d0+900", "0d0+900", CS_LARGE, "1d400+3000");
	CRBO("head neck body cloak hand hand boots", 50);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM + AIF_COWARD);
	CRS("St 5d5+150 Dx 1d10+10 To 1d10+80 Le 1d5+5 Wi 1d5+5 Ma 1d5+5 Pe 1d6 Ch 1d5");
	CRR("");
	CRC("1d5", "2d5");
	CRM("0d0-10", "0d0+15", "1d5+70", "1d5+5");
	CRD("Xshee-Voo has lived in his mountain cave for as long as anyone can remember.  He never shows himself outside, and the few that have been in his cave and returned speak of piles of bones and armor slowly decaying.  They also speak of his enormous club which looks to have been carved from the rock of the mountain and is written over with Runes of great power.");
	CRL(SKT_HEALING, SKILL_MAX_LEVEL);
	CRL(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL);


	CRVW(CN_MAGNUSH, "Magnush, the Kobold Lord", 'k', xRED, CPT_NAMED_HE, CRL_UNIQUE, CR_KOBOLD);
	CRBA("1d10+120", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+900");
	CRBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 5);
	CRA(AIF_RANDOM_MOVE + AIF_ALLOW_PICK_UP + AIF_ALLOW_WEAR_ITEM);
	CRS("St 4d5 Dx 3d5 To 5d3 Le 2d4 Wi 3d5 Ma 3d3 Pe 1d8 Ch 1d4");
	CRR("");
	CRC("1d4", "1d3");
	CRM("1d4", "1d2", "2d6", "2d3");
	CRD("Magnush the kobold lord stands before you with all the majesty he can muster.  Bigger than most kobolds (some say his father was a gnoll), he has managed to bully his way to the prime position in his pack.  His mangy fur and crooked legs give him an almost laughable appearance though and you have difficulty hiding your mirth from this overgrown puppy.");
	CRL(SKT_HEALING, 10);
	CRL(SKT_FINDWEAKNESS, 10);
	CREQ2(IM_SCROLL + IM_POTION + IM_BOOK + IM_FOOD, 6, 20);
	CREQ(IM_WEAPON, IT_DAGGER, 100);
	CRCOE(CET_DISEASE, 30);
	CRL(SPELL_CURE_SERIOUS_WOUNDS);

	CRVW(CN_ROTMOTH, "Rotmoth", 'p', xDARKGRAY, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMAN, CN_BANDIT);
	CRD("NO DESCRIPTION, PLEASE HELP TO CREATE THEM...");

	CRVW(CN_GIANA, "Giana", 'p', xLIGHTRED, CPT_NAMED_SHE, CRL_UNIQUE, CR_HUMAN, CN_GOODWIFE);
	CRD("NO DESCRIPTION, PLEASE HELP TO CREATE THEM...");
end