/*
This file is part of "Avanor, the Land of Mystery" roguelike game
Home page: http://www.avanor.com/
Copyright (C) 2000-2003 Vadim Gaidukevich

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "creatures.h"
#include "xapi.h"

_CREATURE XCreatureStorage::creature_storage[CN_EOF];
CREATURE_SET_REC XCreatureStorage::creature_set[32];

struct FILL_OUT_CR_DB
{
	FILL_OUT_CR_DB();
};

FILL_OUT_CR_DB fill_out_cr_db;


FILL_OUT_CR_DB::FILL_OUT_CR_DB()
{
	////////////////////////////////////////////////////////////////
	// RAT
	////////////////////////////////////////////////////////////////
	
	XVW(CN_RAT, "rat", 'r', xBROWN, CPT_IT, CRL_VERY_LOW, CR_RAT);
	XBA("1d30+90", "0d0+1000", "1d200+600", CS_VERY_SMALL, "5d4");
	XBO("");
	XA(AIF_COWARD | AIF_RANDOM_MOVE | AIF_ALLOW_PACK);
	XS("St 1d2 Dx 2d4 To 1d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 2d4 Ch 1d1");
	XR("fire:5d5-50 cold:2d10");
	XC("0d0", "1d2");
	XM("1d3", "0d0", "1d3", "0d0");
	XD("Scuttling about in the shadows, rats can be found most places in the valley.  Those in the caves have grown larger and bolder and present a threat to any explorers.");
	COE(CET_VOMIT, 0);

	XVW(CN_LARGE_RAT, "large rat", 'r', xBROWN, CPT_IT, CRL_VERY_LOW, CR_RAT);
	XBA("1d30+90", "0d0+1000", "1d200+600", CS_VERY_SMALL, "10d4");
	XBO("");
	XA(AIF_COWARD | AIF_RANDOM_MOVE | AIF_ALLOW_PACK);
	XS("St 1d4 Dx 2d5 To 1d3 Le 1d1 Wi 1d1 Ma 1d1 Pe 2d4 Ch 1d1");
	XR("fire:5d5-25 cold:3d10");
	XC("1d2", "1d3");
	XM("2d2", "1d1", "2d3", "0d0");
	XD("These large rodents have grown almost to the size of a housecat.  Their sharp incisors glitter brightly in the light from your torch.  It is said that their teeth never stop growing and they must constantly chew in order to keep them trimmed.  This particular rat seems to think your arm would make a great chew stick.");
	COE(CET_VOMIT, 0);

	XVW(CN_BLACK_RAT, "black rat", 'r', xDARKGRAY, CPT_IT, CRL_LOW, CR_RAT);
	XBA("1d30+100", "0d0+1000", "1d200+500", CS_VERY_SMALL, "10d4");
	XBO("");
	XA(AIF_COWARD | AIF_RANDOM_MOVE | AIF_ALLOW_PACK);
	XS("St 1d5 Dx 2d8 To 2d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 2d5 Ch 1d1");
	XR("fire:5d5 cold:5d10 acid:5d10+40");
	XC("1d4", "1d6");
	XM("2d3", "1d2", "3d3", "0d0");
	XD("A foot long and covered in jet black fur, this rat appears rather harmless (as far as rats go).  Then you notice the wildness in its eyes and slight froth dripping from its jaw.  Surely this is the type of rat blamed for the spread of many diseases.  Suddenly you become much more wary about letting it nip at you, even a scratch could cause the dreaded black plague.");
	XAT(BR_DISEASE, 30);
	COE(CET_VOMIT, 0);

	XVW(CN_HUGE_RAT, "huge rat", 'r', xLIGHTGRAY, CPT_IT, CRL_LOW, CR_RAT);
	XBA("1d30+110", "0d0+1000", "1d200+400", CS_SMALL, "50d4");
	XBO("");
	XA(AIF_COWARD | AIF_RANDOM_MOVE | AIF_ALLOW_PACK);
	XS("St 2d5 Dx 2d7 To 3d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 2d6 Ch 1d1");
	XR("cold:5d10");
	XC("2d4", "2d3");
	XM("2d3", "2d2", "5d3", "0d0");
	XD("Come closer my child and you will hear of the giant rats that come to take bad children away...  Deep in the caves of Avanor they nest, big as a wolf and meaner too.  They come at night to take bad little children and feed them to their young.  Oh!  I see it is bedtime, you better hurry and get in bad or they might think you are one of the bad ones...");
	XAT(BR_DISEASE, 30);
	XAT(BR_POISON, 20);
	COE(CET_VOMIT, 0);

	////////////////////////////////////////////////////////////////
	// BAT
	////////////////////////////////////////////////////////////////

	XVW(CN_BAT, "bat", 'b', xBROWN, CPT_IT, CRL_VERY_LOW, CR_RAT);
	XBA("1d30+120", "0d0+1000", "1d200+800", CS_VERY_SMALL, "3d4");
	XBO("");
	XA(AIF_COWARD | AIF_RANDOM_MOVE);
	XS("St 1d1 Dx 1d4 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 3d10 Ch 1d1");
	XR("fire:5d5-50 see_invisible:1d10+10");
	XC("0d0", "1d2");
	XM("1d4", "0d0", "1d2", "0d0");
	XD("Flapping wings and squeaks in the darkness are a common sound to all who enter the caves of Avanor.  Usually bats leave travlers alone, but sickness and magic sometimes cause them to attack.");

	XVW(CN_HUGE_BAT, "huge bat", 'b', xLIGHTGRAY, CPT_IT, CRL_VERY_LOW, CR_RAT);
	XBA("1d30+150", "0d0+1000", "1d200+700", CS_VERY_SMALL, "10d4");
	XBO("");
	XA(AIF_COWARD | AIF_RANDOM_MOVE);
	XS("St 1d4 Dx 1d6 To 1d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 4d10 Ch 1d1");
	XR("see_invisible:1d10+20");
	XC("1d2", "1d4");
	XM("1d4", "1d1", "1d6", "0d0");
	XD("With a wing span up to 10 feet, these bats can carry away much larger prey than their smaller cousins.  They have been seen carrying creatures as large as a wolf away to feed their young in the dark corners of their cave.  If they can they will take down any prey available and them dismember it with razor sharp teeth to make it easier to carry.");


	////////////////////////////////////////////////////////////////
	// REPTILE
	////////////////////////////////////////////////////////////////

	XVW(CN_SMALL_SNAKE, "small snake", 'R', xLIGHTGRAY, CPT_IT, CRL_VERY_LOW, CR_REPTILE);
	XBA("1d10+80", "0d0+1000", "1d200+900", CS_VERY_SMALL, "3d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d1 Dx 1d2 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d2 Ch 1d1");
	XR("cold:1d10-90");
	XC("2d5", "1d3");
	XM("1d3", "0d0", "1d2", "0d0");
	XD("This tiny creature lies in the dust looking like a piece of rope.  Suddenly a pink tongue protrudes from one end and eyes open sensing your presence.  Though unimposing at best, this snake harbors one of the deadliest poisons in the valley.");

	XVW(CN_GRAY_SNAKE, "gray snake", 'R', xLIGHTGRAY, CPT_IT, CRL_VERY_LOW, CR_REPTILE);
	XBA("1d10+80", "0d0+1000", "1d200+900", CS_VERY_SMALL, "5d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d2 Dx 1d4 To 1d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d2 Ch 1d1");
	XR("cold:1d10-50");
	XC("2d5", "1d3");
	XM("1d3", "0d0", "1d3", "0d0");
	XD("A long serpent slithers towards you, its grey-black scales making it hard to see against the stone of the cavern floor.  The pale green eyes glimmer with hunger.  Its pink tounge tastes the air as it contemplates this, its lastest meal... YOU!");

	XVW(CN_BROWN_SNAKE, "brown snake", 'R', xBROWN, CPT_IT, CRL_VERY_LOW, CR_REPTILE);
	XBA("1d10+80", "0d0+1000", "1d200+900", CS_VERY_SMALL, "5d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d2 Dx 1d4 To 1d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d2 Ch 1d1");
	XR("cold:1d10-30");
	XC("2d5", "1d4");
	XM("1d3", "0d0", "1d5", "0d0");
	XD("This snake will normally keep to the forests and caves of Avanor.  The naturally mottled brown coat allows it to blend in with the area around it.  Though not a great danger, travelers are urged to leave them alone since others may be nearby.");

	XVW(CN_SALAMANDER, "salamander", 'R', xRED, CPT_IT, CRL_LOW, CR_REPTILE);
	XBA("1d10+80", "0d0+1000", "1d200+900", CS_VERY_SMALL, "5d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d2 Dx 1d4 To 1d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d2 Ch 1d1");
	XR("cold:1d10-50 fire:1d90");
	XC("2d4", "1d4");
	XM("1d9", "1d3", "1d3", "0d0");
	XD("Salamanders prefer living in dark, damp caves and eating the bugs that fly around.  The release of Power mutated the salamanders in the valley of Avanor until they were 4 feet long and had a taste for warm blooded flesh.  They can't see well but they have a powerful bite.");
	XAT(BR_FIRE, 100);
	COE(CET_MODIFY_R_FIRE, 1);

	XVW(CN_LARGE_SNAKE, "large snake", 'R', xLIGHTGRAY, CPT_IT, CRL_LOW, CR_REPTILE);
	XBA("1d10+80", "0d0+1000", "1d200+800", CS_VERY_SMALL, "10d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d3 Dx 1d5 To 1d3 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d2 Ch 1d1");
	XR("cold:1d10-40");
	XC("6d5", "1d5");
	XM("1d4", "1d1", "2d3", "0d0");
	XD("This serpent seems to have been lost from some sort of travelling show.  Its emerald green scales show up clearly against the stone and dirt.  Idly you wonder how it has survived for so long seeing as travelling shows just don't reach the valley anymore.  Then you realize that the snake is already closing the distance between its position and yours and suddenly you know exactly how it survived.");
	XAT(BR_POISON, 10);
	COE(CET_MODIFY_R_POISON, 1);

	XVW(CN_COBRA, "cobra", 'R', xGREEN, CPT_IT, CRL_LOW, CR_REPTILE);
	XBA("1d10+90", "0d0+1000", "1d200+700", CS_VERY_SMALL, "15d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d3 Dx 1d7 To 2d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d4 Ch 1d1");
	XR("cold:1d10-30");
	XC("7d5", "1d7");
	XM("1d8", "1d1", "3d3", "0d0");
	XD("The cobra looks much like a normal snake until it lifts its head and spreads its hood.  Then the markings clearly indicate the danger you face.  A cobra's poison is more deadly than other snakes and the cobra is much more aggressive.");
	XAT(BR_POISON, 40);
	COE(CET_MODIFY_R_POISON, 2);

	XVW(CN_KING_COBRA, "king cobra", 'R', xLIGHTRED, CPT_IT, CRL_AVG, CR_REPTILE);
	XBA("1d10+90", "0d0+1000", "1d200+500", CS_VERY_SMALL, "20d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d7 Dx 1d8 To 3d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 2d3 Ch 1d1");
	XR("cold:1d10-20");
	XC("8d5", "1d12");
	XM("2d5", "2d2", "4d3", "0d0");
	XD("The distinct crown shaped mark on the back of this snake's head gave the title it now carries.  However, if you are close enough to see the crown than you should have been able to distinguish it by its size alone.  Bigger, faster and stronger than its smaller cousin, the king cobra is best left alone by anyone without several anti-venom potions.");
	XAT(BR_POISON, 60);
	COE(CET_MODIFY_R_POISON, 5);

	XVW(CN_RATTLESNAKE, "rattlesnake", 'R', xRED, CPT_IT, CRL_AVG, CR_REPTILE);
	XBA("1d10+90", "0d0+1000", "1d200+500", CS_VERY_SMALL, "10d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 2d4 Dx 2d5 To 3d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 2d3 Ch 1d1");
	XR("cold:1d10-30");
	XC("10d5", "1d12");
	XM("1d5", "1d2", "5d3", "0d0");
	XD("An ominous rattle sounds from ahead of you as a coiled rattlesnake prepares to defend its territory.  Poison drips from its fangs and the cold glint in its eyes seem to foreshadow your certain death if you don't retreat now.  It may even be too late...");
	XAT(BR_POISON, 80);
	COE(CET_MODIFY_R_POISON, 10);



	////////////////////////////////////////////////////////////////
	// FELINE
	////////////////////////////////////////////////////////////////

	XVW(CN_CAT, "cat", 'f', xBROWN, CPT_IT, CRL_VERY_LOW, CR_FELINE);
	XBA("1d10+110", "0d0+1000", "1d500+500", CS_SMALL, "1d40+50");
	XBO("");
	XA(AIF_COWARD | AIF_RANDOM_MOVE);
	XS("St 2d2 Dx 2d4 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 3d3 Ch 1d1");
	XR("fire:5d5-80 cold:3d10");
	XC("1d2", "1d3");
	XM("2d3", "1d1", "1d4", "0d0");
	XD("This mangy little furball looks so cute and cuddly that you just want hug it.  That is you would want to if it weren't hissing, spitting and taking swipes at your face with its claws.");

	XVW(CN_WILD_CAT, "wildcat", 'f', xLIGHTGRAY, CPT_IT, CRL_VERY_LOW, CR_FELINE);
	XBA("1d20+110", "0d0+1000", "1d300+300", CS_SMALL, "1d40+70");
	XBO("");
	XA(AIF_COWARD | AIF_RANDOM_MOVE);
	XS("St 2d3 Dx 2d6 To 1d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 3d3 Ch 1d1");
	XR("fire:5d5-25 cold:5d10");
	XC("1d4", "1d4");
	XM("2d4", "1d2", "2d4", "0d0");
	XD("Tawny and muscled, this cat silently pads forward.  Almost as large as a wolf, the mottled markings make it dificult to see in the shadows.  As its slitted eyes glance from side to side you can tell it is hunting for something.  Only then do you realize it is looking for you!");


	////////////////////////////////////////////////////////////////
	// CANINE
	////////////////////////////////////////////////////////////////

	XVW(CN_DOG, "dog", 'C', xBROWN, CPT_IT, CRL_VERY_LOW, CR_CANINE);
	XBA("1d10+100", "0d0+1000", "1d100+900", CS_SMALL, "1d100+200");
	XBO("");
	XA(AIF_HI_ANIMAL | AIF_ALLOW_PACK | AIF_ALLOW_PACK);
	XS("St 2d3 Dx 2d3 To 1d3 Le 1d1 Wi 1d1 Ma 1d1 Pe 3d3 Ch 1d1");
	XR("fire:5d5-40 cold:5d10");
	XC("1d2", "2d3");
	XM("1d4", "1d2", "2d3", "0d0");
	XD("Dogs were brought to the valley of Avanor to serve the residents of the kingdom, but some of them escaped and became feral and breeding in the caves below the surface.  Mongrels in every sense, these animals now prey upon other creatures for food, including silly adventurers.");

	XVW(CN_LARGE_DOG, "large dog", 'C', xBROWN, CPT_IT, CRL_VERY_LOW, CR_CANINE);
	XBA("1d10+100", "0d0+1000", "1d100+900", CS_SMALL, "1d200+300");
	XBO("");
	XA(AIF_HI_ANIMAL | AIF_ALLOW_PACK | AIF_ALLOW_PACK);
	XS("St 2d4 Dx 2d4 To 2d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 3d3 Ch 1d1");
	XR("fire:5d5-30 cold:6d10");
	XC("1d4", "2d4");
	XM("1d4", "1d3", "2d4", "0d0");
	XD("Obviously the leader of his pack, this dog is larger than the others and may be the offspring of a dog and a wolf.  Its muzzle and body are scarred from numerous fights and it limps slightly.  The deference the other dogs show it indicates that it is still a dangerous foe.");

	XVW(CN_RABID_DOG, "rabid dog", 'C', xBROWN, CPT_IT, CRL_LOW, CR_CANINE);
	XBA("1d10+120", "0d0+1000", "1d100+700", CS_SMALL, "1d100+200");
	XBO("");
	XA(AIF_HI_ANIMAL | AIF_ALLOW_PACK);
	XS("St 2d4 Dx 2d2 To 1d2 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d3 Ch 1d1");
	XR("fire:5d5-30 cold:4d10");
	XC("2d4", "2d3");
	XM("1d2", "1d1", "2d3", "0d0");
	XD("White foam drips from the jaws of this dog.  The wild look in its eyes indicate that something is not quite right, and it snaps at everything and everyone around it with the strength born of its diseased madness.");
	XAT(BR_DISEASE, 50);
	COE(CET_DISEASE, 50);

	XVW(CN_WOLF, "wolf", 'C', xLIGHTGRAY, CPT_IT, CRL_LOW, CR_CANINE);
	XBA("1d10+130", "0d0+1000", "1d100+700", CS_SMALL, "1d300+500");
	XBO("");
	XA(AIF_HI_ANIMAL | AIF_ALLOW_PACK);
	XS("St 4d3+2 Dx 2d4+1 To 2d3 Le 1d1 Wi 1d1 Ma 1d1 Pe 3d4 Ch 1d1");
	XR("fire:5d5-20 cold:7d10");
	XC("3d4", "2d6");
	XM("2d4", "2d3", "3d4", "0d0");
	XD("One glance at this wolf tells you why it has come down from the mountains... to look for food.  The shaggy pelt and thin body are proof of hard times since the eruption of Power.");

	XVW(CN_LARGE_WOLF, "large wolf", 'C', xLIGHTGRAY, CPT_IT, CRL_AVG, CR_CANINE);
	XBA("1d10+130", "0d0+1000", "1d100+700", CS_NORMAL, "1d300+700");
	XBO("");
	XA(AIF_HI_ANIMAL | AIF_ALLOW_PACK);
	XS("St 4d4+2 Dx 3d4+1 To 3d4 Le 1d1 Wi 1d1 Ma 1d1 Pe 3d4 Ch 1d1");
	XR("cold:9d10");
	XC("5d5", "3d6");
	XM("2d4", "3d3", "5d4", "0d0");
	XD("This wolf is almost the size of a small pony.  It is fast, strong and eager to eat anything smaller than itself.  Its hungry gaze makes you uncomfortable as it faces you with fangs bared.");

	XVW(CN_WEREWOLF, "werewolf", 'C', xDARKGRAY, CPT_IT, CRL_HI, CR_CANINE);
	XBA("1d10+150", "0d0+1000", "1d100+500", CS_NORMAL, "1d300+1000");
	XBO("");
	XA(AIF_HI_ANIMAL);
	XS("St 7d4 Dx 5d4 To 5d4 Le 1d1 Wi 1d1 Ma 1d1 Pe 5d4 Ch 1d1");
	XR("fire:4d10 cold:9d10");
	XC("6d5", "4d6");
	XM("2d8", "3d4", "7d4", "7d4");
	XD("As you gaze on this creature its features shift from wolf to man and back again.  Product of a dreadful experiment in magic, the werewolf hunts in the caves of Avanor for food.  It is said that silver weapons are the only true way to defeat a werewolf.");
	XAT(BR_DISEASE, 100);
	XAT(BR_PARALYSE, 20);
	COE(CET_PARALYSE, 50);
	COE(CET_MODIFY_R_PARALYSE, 5);
	COE(CET_DISEASE, 50);


	////////////////////////////////////////////////////////////////
	// OOZE, JELLY, etc.
	////////////////////////////////////////////////////////////////
	
	XVW(CN_GRAY_OOZE, "gray ooze", 'j', xLIGHTGRAY, CPT_IT, CRL_LOW, CR_BLOB);
	XBA("1d10+40", "0d0+1000", "1d100+600", CS_VERY_SMALL, "1d10+10");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("acid:0d0+100 poison:0d0+100");
	XC("3d5", "1d3");
	XM("1d2", "0d0", "1d3", "0d0");
	XD("This quivering gelatinous mass slowly oozes across the floor towards you.  It appears almost that two separate intelligences control the creature, straining to separate from each other.  It is said that the gray ooze steals life energy from those it attacks in order to reproduce.  It also is rumored to have a deadly poison.");
	XAT(BR_POISON, 15);
	COE(CET_MODIFY_R_POISON, 1);
	COE(CET_POISON, 10);

	XVW(CN_GELATINOUS_CUBE, "gelatinous cube", 'j', xWHITE, CPT_IT, CRL_AVG, CR_BLOB);
	XBA("1d10+50", "0d0+1000", "1d100+200", CS_LARGE, "20d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("");
	XC("2d4", "2d15");
	XM("1d1", "1d10", "1d8", "0d0");
	XD("As you watch this clear mass you realize that you can still see the remains of its last meal suspended inside.  The skull seems to laugh at you as ready your weapon to face this three foot oozing mass.  The skelatal hand still grips the remains of a sword, slowly being digested.");
	XAT(BR_PARALYSE, 100);
	XAT(BR_ACID, 100);

	////////////////////////////////////////////////////////////////
	// INSECTS
	////////////////////////////////////////////////////////////////

	XVW(CN_SPIDER, "spider", 'S', xBROWN, CPT_IT, CRL_VERY_LOW, CR_INSECT);
	XBA("1d10+70", "0d0+1000", "1d100+900", CS_VERY_SMALL, "3d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("fire:1d30-50 cold:1d20-40");
	XC("2d5", "1d2");
	XM("1d1", "0d0", "1d2", "0d0");
	XD("A foot wide and covered with grayish brown hair, this spider contemplates you with its eight eyes.  Spiders like this one have filled the corners of the caves of Avanor with webs.  Usually just a nuisance, some of them are equipped with deadly venom.");

	XVW(CN_GIANT_SPIDER, "giant spider", 'S', xLIGHTGRAY, CPT_IT, CRL_VERY_LOW, CR_INSECT);
	XBA("1d10+80", "0d0+1000", "1d100+600", CS_VERY_SMALL, "1d10+10");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d2 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("fire:1d30-50 cold:1d20-40");
	XC("3d5", "1d3");
	XM("1d2", "0d0", "1d3", "0d0");
	XD("These eight legged monstrosities escaped from a long dead wizard's laboratory and made homes in the deep caves of the mountains surrounding the valley.  Their webs are capable of ensnaring an unwary adventurer and their venom can be quite deadly.");
	XAT(BR_POISON, 15);
	COE(CET_MODIFY_R_POISON, 1);
	COE(CET_POISON, 10);

	XVW(CN_TARANTULA, "tarantula", 'S', xLIGHTGRAY, CPT_IT, CRL_LOW, CR_INSECT);
	XBA("1d10+70", "0d0+1000", "1d100+600", CS_VERY_SMALL, "3d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("fire:1d30-50 cold:1d20-60");
	XC("3d5", "1d6");
	XM("1d1", "0d0", "1d3", "0d0");
	XD("These spiders live in nooks and crannies in the rock and can pop into view when least expected.  Unable to spin webs, they rely on brute strength and venom to overcome a victim.  Creatures may be paralyzed by their venom only to be hauled back to a hole and eaten alive by the tarantula's young.");
	XAT(BR_POISON, 40);
	XAT(BR_PARALYSE, 10);
	COE(CET_MODIFY_R_POISON, 2);
	COE(CET_POISON, 15);

	XVW(CN_SCORPION, "scorpion", 'S', xYELLOW, CPT_IT, CRL_LOW, CR_INSECT);
	XBA("1d10+70", "0d0+1000", "1d100+600", CS_VERY_SMALL, "3d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("fire:1d30-50 cold:1d20-60");
	XC("5d5", "1d6");
	XM("1d1", "0d0", "1d5", "0d0");
	XD("This creature is a foot long and colored a dusty brown with a tail arching high over it's back.  Scorpions use the poison in their stinging tail as well as formidable pincers as a powerful attack.  They also have a strong exoskeleton which protects them from many attacks.");
	XAT(BR_POISON, 60);
	XAT(BR_PARALYSE, 20);
	COE(CET_MODIFY_R_POISON, 3);
	COE(CET_POISON, 20);
	COE(CET_PARALYSE, 20);

	XVW(CN_BLACK_SCORPION, "black scorpion", 'S', xDARKGRAY, CPT_IT, CRL_LOW, CR_INSECT);
	XBA("1d10+70", "0d0+1000", "1d100+600", CS_VERY_SMALL, "3d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("fire:1d30-50 cold:1d20-60");
	XC("10d5", "1d10");
	XM("1d1", "0d0", "1d7", "0d0");
	XD("Black scorpions hide in the shadows of caves and will often flee from a light source.  When cornered, they fight with both pincers and their tail with great agility.  Though not as strong as their dusty cousins, they do move quite a bit faster.");
	XAT(BR_POISON, 80);
	XAT(BR_PARALYSE, 30);
	COE(CET_MODIFY_R_POISON, 5);
	COE(CET_POISON, 30);
	COE(CET_PARALYSE, 30);

	XVW(CN_PINK_SCORPION, "pink scorpion", 'S', xLIGHTMAGENTA, CPT_IT, CRL_AVG, CR_INSECT);
	XBA("1d10+70", "0d0+1000", "1d100+600", CS_VERY_SMALL, "3d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("fire:1d30-50 cold:1d20-60");
	XC("20d5", "1d20");
	XM("1d1", "0d0", "1d9", "0d0");
	XD("This scorpion originally evolved with this coloring in order to hide among plant flowers and surprise prey.  These scorpions are extremely aggressive and dangerous.  Sometimes they still carry exotic pollen from the flowers on the tip of their tails.");
	XAT(BR_POISON, 100);
	XAT(BR_PARALYSE, 40);
	XAT(BR_STUN, 40);
	XAT(BR_CONFUSE, 40);
	COE(CET_MODIFY_R_POISON, 10);
	COE(CET_POISON, 50);
	COE(CET_PARALYSE, 50);


	XVW(CN_FIRE_BEETLE, "fire beetle", 'i', xRED, CPT_IT, CRL_VERY_LOW, CR_INSECT);
	XBA("1d10+100", "0d0+1000", "1d300+900", CS_VERY_SMALL, "5d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d3 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("fire:1d90 cold:1d20-80");
	XC("1d3", "1d3");
	XM("1d1", "1d8", "1d3", "0d0");
	XD("Usually living only in depths of the earth, fire beetles were drawn to the surface by the intense energies used to raise the mountains surrounding Avanor.  Their normal habitat has made them immune to great heat and their thick exo-skeleton protects them from many physical attacks.");
	XAT(BR_FIRE, 100);
	COE(CET_MODIFY_R_FIRE, 2);
	COE(CET_MODIFY_R_COLD, -1);


	XVW(CN_FROST_BEETLE, "frost beetle", 'i', xWHITE, CPT_IT, CRL_LOW, CR_INSECT);
	XBA("1d10+110", "0d0+1000", "1d300+900", CS_VERY_SMALL, "7d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d3 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("fire:1d20-80 cold:1d80");
	XC("1d5", "1d10");
	XM("1d1", "4d4", "1d5", "0d0");
	XD("Frost beetles are characterized by a white shell and faint blue markings.  They usually live high in the mountains upon snow capped peaks.  With the release of power in the mountain chain, many of them fled to the caverns below to seek shelter.  They are immune to cold attacks and their thick exo-skeleton protects them from many physical attacks.");
	XAT(BR_COLD, 100);
	COE(CET_MODIFY_R_COLD, 2);
	COE(CET_MODIFY_R_FIRE, -1);


	XVW(CN_GREEN_BEETLE, "green beetle", 'i', xGREEN, CPT_IT, CRL_LOW, CR_INSECT);
	XBA("1d10+120", "0d0+1000", "1d300+900", CS_VERY_SMALL, "7d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d5 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("acid:1d80");
	XC("1d5", "1d12");
	XM("1d1", "5d4", "1d8", "0d0");
	XD("Green beetles are often seen in packs.  Their exo-skeleton is not as tough as the frost or fire beetle, but they are able to shoot streams of acid at their target with great accuracy.");
	XAT(BR_ACID, 100);
	COE(CET_MODIFY_R_ACID, 2);
	COE(CET_MODIFY_STOMACH, 1);


	XVW(CN_KILLER_BEETLE, "killer beetle", 'i', xBLUE, CPT_IT, CRL_AVG, CR_INSECT);
	XBA("1d10+150", "0d0+1000", "1d300+900", CS_VERY_SMALL, "10d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d10 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("acid:1d80 cold:1d80 fire:1d80");
	XC("1d10", "2d15");
	XM("1d1", "6d5", "1d8", "0d0");
	XD("This beetle scuttles around with great energy, moving half and again as fast as any other beetle you have seen.  Its shell seems thin but the formidable jaws leave no doubt of the damage it can inflict.  Its large antennae make it able to detect creatures around it, visible or not.");
	XAT(BR_POISON, 30);
	XAT(BR_PARALYSE, 70);
	COE(CET_MODIFY_R_PARALYSE, 10);
	COE(CET_MODIFY_STOMACH, 5);


	XVW(CN_DEATH_BEETLE, "death beetle", 'i', xDARKGRAY, CPT_IT, CRL_HI, CR_INSECT);
	XBA("1d30+170", "0d0+1000", "1d300+900", CS_VERY_SMALL, "15d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d15 Dx 1d5 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("acid:1d100 cold:1d100 fire:1d100");
	XC("2d10", "3d15");
	XM("1d1", "6d7", "1d10", "0d0");
	XD("Extremely heavily armored, the death beetle is a dark blue color bordering on black.  The white markings on it's shell resemble bleached skulls.  Although slow moving, the death beetles venom is extremely potent and unless treated immeadiately can kill an individual within minutes.");
	XAT(BR_POISON, 50);
	XAT(BR_PARALYSE, 100);
	COE(CET_MODIFY_R_PARALYSE, 15);
	COE(CET_MODIFY_STOMACH, 10);


	XVW(CN_GIANT_BEE, "giant bee", 'b', xYELLOW, CPT_IT, CRL_AVG, CR_INSECT);
	XBA("1d100+200", "0d0+1000", "1d300+1300", CS_VERY_SMALL, "5d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("");
	XC("1d5", "1d30");
	XM("5d5", "1d1", "1d2", "0d0");
	XD("The buzz in the air reminds you more of a hailstorm on a tin roof than an insect but as the giant bee rounds the corner ahead you can see why.  Three feet long and armed with a six inch stinger, this is a far cry from a normal bee.  It doesn't look like it is happy to see you in its territory either.");
	XAT(BR_POISON, 100);

	XVW(CN_GIANT_WASP, "giant wasp", 'b', xBROWN, CPT_IT, CRL_AVG, CR_INSECT);
	XBA("1d100+200", "0d0+1000", "1d300+600", CS_VERY_SMALL, "5d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("");
	XC("1d8", "1d10");
	XM("7d5", "1d1", "1d2", "0d0");
	XD("Wasps tend to build nests in areas inaccessible to larger creatures and many wasps can sometimes be found clustered around the opening.  The wasp is stronger than the bee and generally more irritable.  The thin waist of the wasp is a weak point and can be severed with a carefully aimed blow.");
	XAT(BR_POISON, 100);

	XVW(CN_CENTIPEDE, "centipede", 'S', xLIGHTGRAY, CPT_IT, CRL_VERY_LOW, CR_INSECT);
	XBA("1d30+80", "0d0+1000", "1d100+950", CS_VERY_SMALL, "3d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("");
	XC("1d3", "1d3");
	XM("1d4", "1d1", "1d2", "0d0");
	XD("Marching forward the centipede comes into view.  Low to the ground and a dark red color, it clearly advertises the fact that it carries poison.  The multiple legs make it easy for the centipede to capture its prey.");
	XAT(BR_PARALYSE, 3);

	XVW(CN_STEGOCENTIPEDE, "stegocentipede", 'S', xDARKGRAY, CPT_IT, CRL_LOW, CR_INSECT);
	XBA("1d30+80", "0d0+1000", "1d100+950", CS_VERY_SMALL, "3d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("");
	XC("2d3", "2d3");
	XM("1d8", "1d2", "1d4", "0d0");
	XD("Slower than their smaller cousins, the stegocentipede is also that much more heavily armored.  It uses its spiked tail to its advantage to knock prey down before attempting to poison it.");
	XAT(BR_PARALYSE, 7);

	XVW(CN_DUNGEON_CRAWLER, "dungeon crawler", 'c', xWHITE, CPT_IT, CRL_VERY_LOW, CR_INSECT);
	XBA("1d10+50", "0d0+1000", "1d200+300", CS_VERY_SMALL, "10d2");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("");
	XC("2d4", "1d3");
	XM("1d1", "1d4", "1d2", "0d0");
	XD("This filthy little insect wanders the floor.  Evolving in the caves, it almost completely forsook the use of eyes, relying on other senses to help it through the darkness.  Usually more of a nuisance than anything else, many adventurers simply crush them with a boot as they walk past.");
	XAT(BR_POISON, 1);

	XVW(CN_GIANT_CRAWLER, "giant crawler", 'c', xLIGHTGRAY, CPT_IT, CRL_AVG, CR_INSECT);
	XBA("1d10+50", "0d0+1000", "1d200+300", CS_SMALL, "10d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("");
	XC("2d4", "1d3");
	XM("1d1", "1d8", "1d4", "0d0");
	XD("This appears to be a mound of flesh approximately a foot high.  Only as it moves towards you do you realize that it still lives and sees you as viable prey.  The greyish green surface undulates hypnotically as you watch it ooze its way across the floor.");
	XAT(BR_PARALYSE, 40);
	XAT(BR_POISON, 60);

	XVW(CN_CARRION_CRAWLER, "carrion crawler", 'c', xDARKGRAY, CPT_IT, CRL_AVG, CR_INSECT);
	XBA("1d10+50", "0d0+1000", "1d100+200", CS_LARGE, "20d4");
	XBO("");
	XA(AIF_INSECT);
	XS("St 1d1 Dx 1d1 To 1d1 Le 1d1 Wi 1d1 Ma 1d1 Pe 1d1 Ch 1d1");
	XR("");
	XC("2d4", "2d15");
	XM("1d1", "1d10", "1d8", "0d0");
	XD("This giant bug has numerous paralyzing tentacles just waiting for you to wander into range.  They live mostly in subterranean caverns and feed on dead or dying matter, but they will attack if hungry...");
	XAT(BR_PARALYSE, 100);
	XAT(BR_POISON, 100);

	////////////////////////////////////////////////////////////////
	// KOBOLDS
	////////////////////////////////////////////////////////////////

	XVW(CN_KOBOLD, "kobold", 'k', xLIGHTGREEN, CPT_HE, CRL_VERY_LOW, CR_KOBOLD);
	XBA("1d10+90", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+600");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 1);
	XA(AIF_CREATURE | AIF_ALLOW_PACK);
	XS("St 2d3 Dx 2d3 To 1d3 Le 2d3 Wi 1d3 Ma 1d2 Pe 1d6 Ch 1d3");
	XR("");
	XC("1d2", "1d2");
	XM("1d1", "0d0", "2d3", "2d2");
	XD("Kobolds resemble nothing better than a small yipping dog that has somehow learned to walk on hind legs and wield weapons.  They are more nuisance than anything else but more than one adventurer has been swarmed under by a pack of them.");
	XL(SKT_HEALING, 4);
	XL(SKT_FINDWEAKNESS, 4);
	XL(SKT_ARCHERY, 4);
	EQ(IM_SCROLL | IM_POTION | IM_BOOK | IM_FOOD, 2, 10);
	EQ(IM_WEAPON, IT_DAGGER, 100);
	EQ(IM_MISSILEW, IT_SHORTBOW, 10);
	COE(CET_DISEASE, 20);

	XVW(CN_LARGE_KOBOLD, "large kobold", 'k', xGREEN, CPT_HE, CRL_LOW, CR_KOBOLD);
	XBA("1d10+90", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+800");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 2);
	XA(AIF_CREATURE | AIF_ALLOW_PACK);
	XS("St 2d5 Dx 2d4 To 2d3 Le 2d4 Wi 1d5 Ma 1d3 Pe 1d8 Ch 1d4");
	XR("");
	XC("1d3", "1d2");
	XM("1d3", "1d1", "2d5", "2d3");
	XD("Somehow this kobold managed to survive its early development. It is not much smarter than the smaller, younger kobolds but definately has more strength.");
	XL(SKT_HEALING, 5);
	XL(SKT_FINDWEAKNESS, 5);
	XL(SKT_ARCHERY, 5);
	EQ(IM_SCROLL | IM_POTION | IM_BOOK | IM_FOOD, 3, 12);
	EQ(IM_WEAPON, IT_DAGGER, 100);
	EQ(IM_WEAPON, IT_DAGGER, 10);
	EQ(IM_MISSILEW, IT_SHORTBOW, 15);
	COE(CET_DISEASE, 30);

	XVW(CN_CHIEFTAIN_KOBOLD, "kobold chieftain", 'k', xGREEN, CPT_HE, CRL_LOW, CR_KOBOLD);
	XBA("1d10+90", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+800");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 5);
	XA(AIF_CREATURE | AIF_ALLOW_PACK);
	XS("St 3d5 Dx 2d5 To 3d3 Le 2d4 Wi 1d5 Ma 1d3 Pe 1d8 Ch 1d4");
	XR("");
	XC("1d4", "1d3");
	XM("1d4", "1d2", "2d6", "2d3");
	XD("The largest kobold you have yet seen. There is a glint of wicked intelligence behind its eyes as it approaches.  The kobold chieftain only acheived his position by being stronger, smarter and meaner then everyone else.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	XL(SKT_ARCHERY, 6);
	EQ(IM_SCROLL | IM_POTION | IM_BOOK | IM_FOOD, 4, 15);
	EQ(IM_WEAPON, IT_DAGGER, 100);
	EQ(IM_WEAPON, IT_DAGGER, 20);
	EQ(IM_MISSILEW, IT_SHORTBOW, 10);
	COE(CET_DISEASE, 30);
	

	XVW(CN_SHAMAN_KOBOLD, "kobold shaman", 'k', xLIGHTCYAN, CPT_HE, CRL_LOW, CR_KOBOLD);
	XBA("1d10+90", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+600");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 5);
	XA(AIF_CREATURE | AIF_ALLOW_PACK);
	XS("St 2d4 Dx 2d4 To 2d3 Le 4d4 Wi 4d5 Ma 4d6 Pe 1d6+6 Ch 2d4");
	XR("");
	XC("1d2", "1d2");
	XM("1d1", "0d0", "1d8", "5d5");
	XD("Smeared with arcane symbols in white chalk and wearing a necklace of bones, the kobold shaman is only a little larger than the average kobold.  The intensity of its gaze shows the resentment it feels for having a superior mind locked into an inferior body.  As you watch, it begins a chant to call forth its powers of magic against you...");
	XL(SPELL_MAGIC_ARROW);
	XL(SPELL_CURE_LIGHT_WOUNDS);
	XL(SKT_HEALING, 4);
	XL(SKT_CONCENTRATION, 4);
	EQ(IM_SCROLL | IM_POTION | IM_BOOK | IM_FOOD, 4, 15);
	EQ(IM_WEAPON, IT_DAGGER, 100);
	COE(CET_DISEASE, 30);
	COE(CET_MODIFY_MA, 1);


	XVW(CN_GNOLL, "gnoll", 'g', xBROWN, CPT_HE, CRL_ABOVE_LOW, CR_KOBOLD);
	XBA("1d10+100", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+800");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 5);
	XA(AIF_CREATURE | AIF_ALLOW_PACK);
	XS("St 3d4 Dx 3d3 To 3d5 Le 1d4 Wi 1d4 Ma 1d4 Pe 2d5 Ch 2d3");
	XR("");
	XC("1d3", "1d4");
	XM("1d4", "1d2", "2d5+15", "2d5");
	XD("A gnoll is to a kobold what a giant is to a man.  This towering cross between man and dog is known for its cunning and strength.  Carrying an axe it will plow right through most opponents leaving them dazed... if they are lucky.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	EQ(IM_SCROLL | IM_POTION | IM_BOOK | IM_FOOD, 2, 10);
	EQ(IM_WEAPON, IT_WARAXE, 100);
	COE(CET_MODIFY_TO, 1);

	XVW(CN_GNOLL_WARMASTER, "gnoll warmaster", 'g', xYELLOW, CPT_HE, CRL_AVG, CR_KOBOLD);
	XBA("1d10+100", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+800");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 7);
	XA(AIF_CREATURE | AIF_ALLOW_PACK);
	XS("St 6d4 Dx 7d3 To 6d5 Le 3d4 Wi 3d4 Ma 3d4 Pe 8d5 Ch 2d3");
	XR("");
	XC("2d5", "2d4");
	XM("2d5", "3d2", "3d5+25", "2d5");
	XD("The gnoll warmaster is an eight foot fighting machine.  The double axes it carries weave an intricate dance of death around it as it takes on multiple opponents.  Only a fool would get in this berserker's way.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	EQ(IM_SCROLL | IM_POTION | IM_BOOK | IM_FOOD, 3, 10);
	EQ(IM_WEAPON, IT_WARAXE, 100);
	COE(CET_MODIFY_TO, 1);
	
	
	////////////////////////////////////////////////////////////////
	// GOBLINS
	////////////////////////////////////////////////////////////////

	XVW(CN_GOBLIN, "goblin", 'g', xLIGHTGREEN, CPT_HE, CRL_VERY_LOW, CR_GOBLIN);
	XBA("1d10+90", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+800");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 3);
	XA(AIF_CREATURE);
	XS("St 3d3 Dx 2d3 To 2d3 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	XR("");
	XC("1d3", "1d3");
	XM("1d3", "1d1", "1d5+6", "1d5");
	XD("This scruffy looking humanoid glances at you with fear in its face.  The stench it carries indicates that it has probably never been clean in its life.  Goblins normally perfer secrecy and stealing to open confrontation but have been known to attack travellers.");
	XL(SKT_HEALING, 4);
	XL(SKT_FINDWEAKNESS, 4);
	EQ(IM_SCROLL | IM_POTION, 1, 10);
	EQ(IM_WEAPON, IT_SHORTSWORD, 100);

	XVW(CN_GOBLIN_WARRIOR, "goblin warrior", 'g', xGREEN, CPT_HE, CRL_LOW, CR_GOBLIN);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+800");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 5);
	XA(AIF_CREATURE);
	XS("St 3d4 Dx 3d3 To 3d3 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 2d3");
	XR("");
	XC("1d4", "1d3");
	XM("1d4", "1d1", "1d5+10", "1d5");
	XD("This goblin carries a nasty looking knife, almost a sword for one as short as he is.  His posture and multiple scars indicate that he is more than a novice at handling them as well.");
	XL(SKT_HEALING, 5);
	XL(SKT_FINDWEAKNESS, 5);
	EQ(IM_SCROLL | IM_POTION, 2, 10);
	EQ(IM_WEAPON, IT_SHORTSWORD, 100);

	XVW(CN_GOBLIN_WARMASTER, "goblin warmaster", 'g', xBLUE, CPT_HE, CRL_LOW, CR_GOBLIN);
	XBA("1d10+100", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+800");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 7);
	XA(AIF_CREATURE);
	XS("St 5d4 Dx 6d3 To 5d5 Le 2d4 Wi 2d4 Ma 2d4 Pe 5d5 Ch 3d3");
	XR("");
	XC("1d8", "2d3");
	XM("1d8", "1d3", "2d5+20", "2d5");
	XD("Arms criss-crossed with scars and bulging with muscle, this goblin has a hint of white hair.  Not many goblins can live so long but the worn weapons at his side indicate that he is capable of keeping his place in the earth.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	EQ(IM_SCROLL | IM_POTION, 3, 10);
	EQ(IM_WEAPON, IT_SHORTSWORD, 100);

	XVW(CN_GOBLIN_CHIEFTAIN, "goblin chieftain", 'g', xRED, CPT_HE, CRL_AVG, CR_GOBLIN);
	XBA("1d10+100", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+800");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 10);
	XA(AIF_CREATURE);
	XS("St 5d4 Dx 6d3 To 5d5 Le 2d4 Wi 2d4 Ma 2d4 Pe 5d5 Ch 3d3");
	XR("");
	XC("1d8", "2d4");
	XM("1d10", "2d2", "2d5+25", "2d5");
	XD("Stronger and smarter than the other goblins.  This goblin has become the chief and now commands all the troops.  His shock of white hair is the only part of him that shows his age and his muscular body is toned from battles past.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	EQ(IM_SCROLL | IM_POTION, 4, 10);
	EQ(IM_WEAPON, IT_SHORTSWORD, 100);


	////////////////////////////////////////////////////////////////
	// ORCS
	////////////////////////////////////////////////////////////////

	XVW(CN_ORC, "orc", 'o', xLIGHTGREEN, CPT_HE, CRL_AVG, CR_ORC);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d400+1400");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 30);
	XA(AIF_CREATURE);
	XS("St 2d5+15 Dx 1d5+5 To 2d5+15 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	XR("");
	XC("1d6", "2d4");
	XM("2d3", "1d2", "2d5+25", "2d5");
	XD("Orcs are roughly dwarf sized with a light green pallor to their skin.  Not extremely intelligent, they avoid sunlight whenever possible.  It has been speculated that they were once humans elves or dwarves but something drove them deep underground and they became the hardy race they are today.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	EQ(IM_SCROLL | IM_POTION, 3, 50);
	EQ(IM_WEAPON, IT_BATTLEAXE, 100);

	XVW(CN_LARGE_ORC, "large orc", 'o', xGREEN, CPT_HE, CRL_AVG, CR_ORC);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d400+1400");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 30);
	XA(AIF_CREATURE);
	XS("St 2d5+20 Dx 1d5+5 To 2d5+20 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	XR("");
	XC("1d6", "2d4");
	XM("2d3", "1d2", "2d5+25", "2d5");
	XD("Bigger, meaner and nastier than its smaller cousins, this orc stands near as tall as a man.  It bears a cruel looking axe and is well armored.  The wicked grin on its face tells you that it will enjoy picking your flesh from your bones.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	EQ(IM_SCROLL | IM_POTION, 3, 50);
	EQ(IM_WEAPON, IT_BATTLEAXE, 100);
	COE(CET_MODIFY_ST, 1);

	XVW(CN_HILL_ORC, "hill orc", 'o', xYELLOW, CPT_HE, CRL_AVG, CR_ORC);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d400+1400");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 30);
	XA(AIF_CREATURE);
	XS("St 2d5+25 Dx 1d5+5 To 2d5+25 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	XR("");
	XC("1d6", "2d4");
	XM("2d3", "1d2", "2d5+25", "2d5");
	XD("Hill orcs are descendants of orcs that left their caves to live back under the sky.  They still prefer to sleep during the day but move around enough during the day to acquire a healthy tan.  Perhaps their return to sunlight is the reason they are stronger than their cousins.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	EQ(IM_SCROLL | IM_POTION, 3, 50);
	EQ(IM_WEAPON, IT_BATTLEAXE, 100);
	COE(CET_MODIFY_ST, 1);
 
	XVW(CN_DARK_ORC, "dark orc", 'o', xDARKGRAY, CPT_HE, CRL_AVG, CR_ORC);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d400+1400");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 30);
	XA(AIF_CREATURE);
	XS("St 2d5+30 Dx 1d5+5 To 2d5+30 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	XR("");
	XC("1d6", "2d4");
	XM("2d3", "1d2", "2d5+25", "2d5");
	XD("Dark orcs retreated to the farthest corners of the caverns and were enslaved by the other orcs.  They were used to delve great cities beneath the earth like the dwarves.  Because of all this manual labor they grew extremely strong.  Before any battle, they cover themselves in coal dust to appear jet black and frighten their opponents.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	EQ(IM_SCROLL | IM_POTION, 3, 50);
	EQ(IM_WEAPON, IT_BATTLEAXE, 100);
	COE(CET_MODIFY_ST, 1);

	XVW(CN_LIEUTENANT_ORC, "orc lieutenant", 'o', xBROWN, CPT_HE, CRL_AVG, CR_ORC);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d400+1400");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 30);
	XA(AIF_CREATURE);
	XS("St 2d5+30 Dx 1d5+5 To 2d5+30 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	XR("");
	XC("1d6", "2d4");
	XM("2d3", "1d2", "2d5+25", "2d5");
	XD("\"Git yur sorry corpses movin'!\"  Orc sergeants act as the first tier of leadership for any orcish raid.  Known to be exceptionally mean spirited, they have been seen to kill their own troops in order to \"inspire\" the rest to better results.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	EQ(IM_SCROLL | IM_POTION, 3, 50);
	EQ(IM_WEAPON, IT_BATTLEAXE, 100);
	COE(CET_MODIFY_ST, 2);

	XVW(CN_CAPTAIN_ORC, "orc captain", 'o', xBROWN, CPT_HE, CRL_AVG, CR_ORC);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d400+1400");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 30);
	XA(AIF_CREATURE);
	XS("St 2d5+40 Dx 1d5+5 To 2d5+40 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	XR("");
	XC("1d6", "2d4");
	XM("2d3", "1d2", "2d5+25", "2d5");
	XD("Captain is a precarious position for any orc.  Displeased chiefs might have their captains killed for any failure, success or even on a whim.  The orcs under him are ready to mutiny and kill him for his position.  Thus the orc captain is always on edge and ready to strike down anything that might pose a danger.  His large frame and hefty axe are usually more than adequate to take care of any and all threats.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	EQ(IM_SCROLL | IM_POTION, 3, 50);
	EQ(IM_WEAPON, IT_BATTLEAXE, 100);
	COE(CET_MODIFY_ST, 2);

	XVW(CN_CHIEFTAIN_ORC, "orc chieftain", 'o', xBLUE, CPT_HE, CRL_AVG, CR_ORC);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d400+1400");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 30);
	XA(AIF_CREATURE);
	XS("St 2d5+50 Dx 1d5+5 To 2d5+50 Le 1d4 Wi 1d4 Ma 1d4 Pe 3d3 Ch 1d5");
	XR("");
	XC("1d6", "2d4");
	XM("2d3", "1d2", "2d5+25", "2d5");
	XD("The orc chieftain is the biggest, meanest orc you have ever seen.  His skin alone looks tougher than studded leather.  The axe he carries is well worn and scarred from many battles.  Only someone this mean and nasty could get the various orc tribes to work together for a raid.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	EQ(IM_SCROLL | IM_POTION, 3, 50);
	EQ(IM_WEAPON, IT_BATTLEAXE, 100);
	COE(CET_MODIFY_ST, 3);


	////////////////////////////////////////////////////////////////
	// HUMANS & HUMANOIDS
	////////////////////////////////////////////////////////////////

	XVW(CN_BANDIT, "bandit", 'p', xLIGHTGRAY, CPT_HE, CRL_LOW, CR_HUMAN);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 50);
	XA(AIF_HUMAN);
	XS("St 1d4+10 Dx 1d4+10 To 1d4+10 Le 1d4+10 Wi 1d4+10 Ma 1d4+10 Pe 1d4+15 Ch 1d4+10");
	XR("");
	XC("1d4", "1d3");
	XM("1d4", "0d0", "1d4+10", "2d5");
	XD("A scruffy looking fellow, he looks much like any citizen of the valley except for the green cape, the twin daggers and the malicious glint in his eye.  You get the distinct impression that he would rob his own mother if she had anything worth taking.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	EQ(IM_SCROLL | IM_POTION, 3, 30);
	EQ(IM_WEAPON, IT_LONGDAGGER, 100);
	EQ(IM_WEAPON, IT_LONGDAGGER, 100);

	XVW(CN_FARMER, "farmer", 'p', xBROWN, CPT_HE, CRL_VERY_LOW, CR_HUMAN);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 0, GFS_SUPRESS_INVIS);
	XA(AIF_HUMAN | AIF_PEACEFUL);
	XS("St 2d3 Dx 2d3 To 1d3 Le 2d3 Wi 1d3 Ma 1d2 Pe 1d6+6 Ch 1d3");
	XR("");
	XC("1d2", "1d2");
	XM("1d1", "0d0", "2d3+1", "2d2");
	XD("The sunburnt face and arms of this fellow, as well as the pitchfork he leans upon give no doubt as to his profession.  He and his fellows produce and gather the food for the entire valley.");
	XL(SKT_HEALING, 2);
	EQ(IM_WEAPON, IT_PITCHFORK, 100);
	EQ(IM_BODY, IT_ROBE, 100);

	XVW(CN_GOODWIFE, "goodwife", 'p', xYELLOW, CPT_SHE, CRL_VERY_LOW, CR_HUMAN);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 0, GFS_SUPRESS_INVIS);
	XA(AIF_HUMAN | AIF_PEACEFUL);
	XS("St 2d3 Dx 2d3 To 1d3 Le 2d3 Wi 1d3 Ma 1d2 Pe 1d6+6 Ch 1d3");
	XR("");
	XC("1d2", "1d2");
	XM("1d1", "0d0", "2d3+1", "2d2");
	XD("This woman's clothing tells a story of a simple but hard life.  She prepares the meals for her husband every day and takes care of their cottage.");
	XL(SKT_HEALING, 2);
	EQ(IM_BODY, IT_DRESS, 100);

	XVW(CN_CITIZEN, "citizen", 'p', xLIGHTGRAY, CPT_HE, CRL_VERY_LOW, CR_HUMAN);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 0, GFS_SUPRESS_INVIS);
	XA(AIF_HUMAN | AIF_PEACEFUL);
	XS("St 2d3 Dx 2d3 To 1d3 Le 2d3 Wi 1d3 Ma 1d2 Pe 1d6+6 Ch 1d3");
	XR("");
	XC("1d2", "1d2");
	XM("1d1", "0d0", "2d3+1", "2d2");
	XD("This upstanding citizen of the castle of Avanor hurries about the duties of the day.  The calm relaxed confidence surrounding him demonstrates his faith in the forces of the guard and the king to protect him and his family.");
	EQ(IM_BODY, IT_CLOTHES, 100);

	XVW(CN_FCITIZEN, "citizen", 'p', xLIGHTGRAY, CPT_SHE, CRL_VERY_LOW, CR_HUMAN);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 0, GFS_SUPRESS_INVIS);
	XA(AIF_HUMAN | AIF_PEACEFUL);
	XS("St 2d3 Dx 2d3 To 1d3 Le 2d3 Wi 1d3 Ma 1d2 Pe 1d6+6 Ch 1d3");
	XR("");
	XC("1d2", "1d2");
	XM("1d1", "0d0", "2d3+1", "2d2");
	XD("This upstanding citizen of the castle of Avanor hurries about the duties of the day.  The calm relaxed confidence surrounding her demonstrates her faith in the forces of the guard and the king to protect her and her family.");
	EQ(IM_BODY, IT_DRESS, 100);

	XVW(CN_ROYAL_GUARD, "royal guardian", 'p', xBLUE, CPT_HE, CRL_LOW, CR_HUMAN);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 50, GFS_SUPRESS_INVIS | GFS_SEE_INVIS);
	XA(AIF_HUMAN | AIF_PEACEFUL);
	XS("St 1d8+15 Dx 1d8+20 To 1d8+10 Le 1d5+10 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 5d5");
	XR("");
	XC("1d6", "2d2");
	XM("3d2", "1d2", "1d5+10", "1d3+5");
	XD("The elite guard of Avanor was organized by King Rodrick to protect citizens from the depradations of bandits and the monsters that live in the valley.  Every child dreams of someday serving in the King's guard.  Royal Guardians are well equipped to protect the people and themselves.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	EQ(IM_WEAPON, IT_LONGSWORD, 100);
	EQ(IM_SHIELD, IT_TOWERSHIELD, 100);

	XVW(CN_DEATH_KNIGHT, "death knight", 'p', xDARKGRAY, CPT_HE, CRL_LOW, CR_HUMAN);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 50);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD);
	XS("St 1d8+20 Dx 1d8+20 To 1d8+10 Le 1d5+10 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 5d5");
	XR("");
	XC("1d6", "2d2");
	XM("3d2", "1d2", "1d5+10", "1d3+5");
	XD("The elite guard of Ahk-Ulan the sorcerer.  Deprived of all human emotion, they have become finely tuned killing machines who worship only death.  The stench of blood surrounds them and their soulless gaze makes your flesh crawl.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	EQ(IM_WEAPON, IT_LONGSWORD, 100);
	EQ(IM_SHIELD, IT_TOWERSHIELD, 100);

	XVW(CN_SHOPKEEPER, "shopkeeper", 'p', xLIGHTGRAY, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMAN);
	XBA("1d10+120", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS | GFS_SEE_INVIS);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD | AIF_PEACEFUL);
	XS("St 1d8+20 Dx 1d8+20 To 1d8+20 Le 1d5+20 Wi 1d4+20 Ma 1d4+20 Pe 3d6 Ch 5d5");
	XR("");
	XC("1d6", "2d2");
	XM("3d2", "1d2", "1d5+10", "1d3+5");
	XD("It takes a special breed to set up shop in this remote valley.  Shopkeepers must be able to defend their wares as well as chat amiably with customers.  Behind the cheery exterior is a sharp intelligence and well toned muscles.  Only the desperate or the foolish would try to steal from one of them.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);

	XVW(CN_DWARF, "dwarf", 'h', xBROWN, CPT_HE, CRL_VERY_LOW, CR_HUMANOID);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 50);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD | AIF_PEACEFUL);
	XS("St 1d3+10 Dx 1d3+7 To 1d3+12 Le 2d3 Wi 1d3 Ma 1d2 Pe 1d6 Ch 1d3");
	XR("");
	XC("1d2", "1d2");
	XM("1d1", "0d0", "2d3+1", "2d2");
	XD("Standing only about 4 feet high, dwarves are known to be master diggers and metal workers.  With a beard starting right below the merrily twinkling eyes, the dwarf whistles a lively tune as he goes about his business.  The axe stuck in his belt and his easy balance indicate that he can be a formidable warrior in times of danger.");

	XVW(CN_DWARF_GUARD, "dwarven guardian", 'h', xLIGHTBLUE, CPT_HE, CRL_VERY_LOW, CR_HUMANOID);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 50, GFS_SEE_INVIS);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD | AIF_PEACEFUL);
	XS("St 1d3+20 Dx 1d3+7 To 1d3+22 Le 2d3 Wi 1d3 Ma 1d2 Pe 1d6 Ch 1d3");
	XR("");
	XC("1d2", "1d2");
	XM("1d1", "0d0", "2d3+1", "2d2");
	XD("Guardians of the underground dwarven city, these dwarves are well armed and prepared for any breach of security both physical and magical.  Only a fool would attempt to accost one as he goes about his duties.  The cold glare and cold steel they carry is usually more than enough to keep trouble makers in line.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);
	EQ(IM_WEAPON, IT_BATTLEAXE, 100);
	EQ(IM_SHIELD, IT_LARGESHIELD, 100);



	////////////////////////////////////////////////////////////////
	// UNDEAD
	////////////////////////////////////////////////////////////////

	XVW(CN_SKELETON, "skeleton", 's', xWHITE, CPT_IT, CRL_VERY_LOW, CR_UNDEAD);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body hand hand", 2);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM);
	XS("St 4d3 Dx 2d2 To 2d3 Le 1d3 Wi 1d3 Ma 1d2 Pe 1d2 Ch 1d1");
	XR("");
	XC("1d3", "2d3");
	XM("1d3", "1d2", "1d8", "1d3");
	XD("Returned to life by evil magics, this creature shuffles towards you, fueled by hate for those still living.  It seeks only to return to the sweet oblivion of death, but first it wants to take you with it.");

	XVW(CN_ZOMBIE, "zombie", 'z', xBROWN, CPT_IT, CRL_LOW, CR_UNDEAD);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body hand hand boots", 5);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM);
	XS("St 8d3 Dx 2d2 To 5d5 Le 1d3 Wi 1d3 Ma 1d2 Pe 1d2 Ch 1d1");
	XR("");
	XC("1d3", "2d3");
	XM("1d3", "1d2", "2d9", "1d3");
	XD("The zombie stumbles across the floor, leaving a trail of decaying flesh behind it.  Though muscles makes the zombie stronger, the magics that animate it can do nothing to preserve the diseased hulk.  The stench is almost unbearable.");
	XAT(BR_PARALYSE, 2);

	XVW(CN_GHOUL, "ghoul", 'z', xLIGHTGRAY, CPT_IT, CRL_LOW, CR_UNDEAD);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body hand hand boots", 10);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM);
	XS("St 10d3 Dx 2d3 To 7d5 Le 1d3 Wi 1d3 Ma 1d2 Pe 1d2 Ch 1d1");
	XR("");
	XC("1d3", "2d3");
	XM("1d2", "1d4", "3d9", "1d3");
	XD("The pasty white flesh of the ghoul is the result of numerous alchemical and magical experiments.  The preserved flesh makes it strong and tough but not much else.  Ghouls are not smart and have been observed attacking walls that stand in their way.");
	XAT(BR_PARALYSE, 10);

	XVW(CN_GHOST, "ghost", 'G', xLIGHTGRAY, CPT_IT, CRL_ABOVE_LOW, CR_UNDEAD);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("", 0);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM);
	XS("St 6d5 Dx 4d5 To 2d5 Le 5d5 Wi 5d5 Ma 5d5 Pe 1d2 Ch 1d1");
	XR("invisible:2d5 see_invisible:2d5+10");
	XC("4d4", "2d4");
	XM("6d4", "0d0", "1d9", "1d3");
	XD("A chill breeze passing by is all most ever felt of this spirit set to wander upon the earth.  Those with good eyesight may spot ghosts in the dark corridors of the earth, but they do not take kindly to people who invade their domain.");

	XVW(CN_SPECTRE, "spectre", 'G', xMAGENTA, CPT_IT, CRL_AVG, CR_UNDEAD);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("", 0);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM);
	XS("St 6d5 Dx 4d5 To 2d5 Le 5d5 Wi 5d5 Ma 5d5 Pe 1d2 Ch 1d1");
	XR("invisible:2d5+5 see_invisible:2d5+15");
	XC("4d4", "4d4");
	XM("6d4", "0d0", "2d9", "1d3");
	XD("It is said the spectres were once the evil people of this earth.  Forbidden entry into the next world, their spirits seek to destroy those of this world.  They are almost never seen before their chill hands take hold upon the heart of their victim.");
	XL(SPELL_DRAIN_LIFE);
	XL(SKT_CONCENTRATION, 10);

	XVW(CN_DREAD, "dread", 'G', xBROWN, CPT_IT, CRL_HI, CR_UNDEAD);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("", 0);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM);
	XS("St 6d10 Dx 5d5 To 3d5 Le 8d5 Wi 8d5 Ma 8d5 Pe 1d2 Ch 1d1");
	XR("invisible:2d5+10 see_invisible:2d5+20");
	XC("6d6", "5d5");
	XM("8d5", "0d0", "8d5", "10d8");
	XD("Panic and fear threaten to overwhelm you as you face thise evil spirit.  Aptly named for the feelings it causes, The dread seeks only to destroy all living things.  So stealthy and invisible are they that most people will never know exactly what did them in.");
	XL(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL);
	XL(SKT_CONCENTRATION, 10);
	XL(SPELL_DRAIN_LIFE);

	XVW(CN_VAMPIRE, "vampire", 'V', xBLUE, CPT_HE, CRL_HI, CR_UNDEAD);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots", 100);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD);
	XS("St 4d6 Dx 5d5 To 3d5 Le 8d5 Wi 8d5 Ma 8d8 Pe 1d2 Ch 1d1");
	XR("invisible:2d5+30 see_invisible:2d5+30");
	XC("4d6", "3d5");
	XM("8d5", "0d0", "3d5", "10d10");
	XD("This foul creature of the night drinks of its victims for sustenance, slowly draining their victim of life until they also become a vampire.  They possess numerous abilities, and have much power in the undead realms.  They fear light and holy powers, but are immune to many non-magical weapons.");
	XAT(BR_PARALYSE, 20);
	XL(SKT_BACKSTABBING, SKILL_MAX_LEVEL);
	XL(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL);
	XL(SKT_CONCENTRATION, SKILL_MAX_LEVEL);
	XL(SPELL_DRAIN_LIFE);
	
	XVW(CN_LICH, "lich", 'L', xLIGHTGRAY, CPT_HE, CRL_HI, CR_UNDEAD);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots", 100);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD);
	XS("St 3d8 Dx 5d5 To 3d5 Le 8d5 Wi 10d5 Ma 10d8 Pe 1d2 Ch 1d1");
	XR("invisible:2d5+30 see_invisible:2d5+40");
	XC("2d5", "1d4");
	XM("8d5", "0d0", "3d5", "10d10");
	XD("\"Once a sorcerer, always a sorcerer.\" Liches were powerful magic wielders in life and they continue to wield that power after their death.  They are able to summon great magics down upon the living and absorb living energies to sustain their own existence.");
	XL(SKT_HEALING, SKILL_MAX_LEVEL);
	XL(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL);
	XL(SKT_CONCENTRATION, SKILL_MAX_LEVEL);
	XL(SPELL_DRAIN_LIFE);
	XL(SPELL_MAGIC_ARROW); // They are a wizard after all...
	XL(SPELL_HEAL);




	////////////////////////////////////////////////////////////////
	// UNIQUE
	////////////////////////////////////////////////////////////////

	XVW(CN_ELDER_GRIDOR, "Elder Gridor", 'p', xWHITE, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMAN);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 0, GFS_SUPRESS_INVIS);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD | AIF_PEACEFUL);
	XS("St 1d3+5 Dx 1d3+5 To 1d3+4 Le 9d5 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 4d4");
	XR("");
	XC("1d1", "1d1");
	XM("1d1", "0d0", "1d5+5", "1d5+5");
	XD("This kind looking old man leans on a cane as he gazes towards you.  As leader of this small farming community, he is responsible for their well being.  He gazes at you and asks for help to a serious problem that has recently arisen and is out of his power to control.");
	XL(SKT_HEALING, 6);
	EQ(IM_SCROLL | IM_POTION, 3, 30);
	EQ(IM_BODY, IT_CLOTHES, 100);
	EQ(IM_WEAPON, IT_STAFF, 100);

	XVW(CN_JORGUS, "Jorgus, the master thief", 'p', xWHITE, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMAN);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 50, GFS_SUPRESS_INVIS | GFS_SEE_INVIS);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD | AIF_PEACEFUL);
	XS("St 1d8+30 Dx 1d8+30 To 1d8+15 Le 1d5+15 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 5d5");
	XR("");
	XC("1d8", "2d2");
	XM("4d2", "1d3", "1d5+15", "1d5+5");
	XD("As you enter his dwelling you keep a tight hold on your purse.  The shifty eyes and too quick smile of the owner makes you nervous.  Jorgush is the leader of his outlaw band.  They are known for stealing from the rich and keeping it for themselves.  He appears to evaluate you and the weapons you bear and then gestures to the cahir at the table.  Perhaps he has a proposition for you...");
	XL(SKT_STEALING, 15);
	EQ(IM_SCROLL | IM_POTION, 3, 30);
	EQ(IM_BODY, IT_CLOTHES, 100);

	XVW(CN_GEKTA, "Gekta, the sheep dog", 'C', xDARKGRAY, CPT_NAMED_IT, CRL_UNIQUE, CR_CANINE);
	XBA("1d10+125", "0d0+1000", "1d100+300", CS_SMALL, "1d200+400");
	XBO("neck", 100);
	XA(AIF_HI_ANIMAL | AIF_PEACEFUL);
	XS("St 1d3+10 Dx 1d5+20 To 1d3+8 Le 1d3+3 Wi 1d3+3 Ma 1d3+3 Pe 5d6 Ch 4d5");
	XR("see_invisible:0d0+30");
	XC("5d5", "2d6");
	XM("4d5", "2d1", "1d5+10", "1d1+1");
	XD("The sheep dog is the mascot for the royal guardians.  It must represent strength, intelligence and unswerving loyalty.  Gekta is the current favorite.  She bounds towards you and knocks you flat, covering you with slobbery dog kisses.");
	XL(SKT_HEALING, 15);

	XVW(CN_HIGHPRIEST, "Aphilius, the high priest of Avanor", 'p', xWHITE, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMANOID);
	XBA("1d10+100", "0d0+800", "0d0+800", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS | GFS_SEE_INVIS);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD | AIF_PEACEFUL);
	XS("St 1d8+10 Dx 1d8+15 To 1d8+10 Le 1d5+25 Wi 1d5+25 Ma 1d5+10 Pe 5d6 Ch 6d5");
	XR("see_invisible:0d0+100");
	XC("1d3", "1d2");
	XM("1d4", "1d2", "1d5+30", "1d5+10");
	XD("This compassionate soul gives his time and devotion to maintaining the temple.  He is dressed in the vestments of his position and bears the mitre of the priesthood...");
	XL(SKT_HEALING, SKILL_MAX_LEVEL);
	XL(SKT_RELIGION, SKILL_MAX_LEVEL);
	XL(SPELL_HEAL);
	EQ(IM_BODY, IT_ROBE, 100);

	XVW(CN_OZORIK, "Ozorick, the royal guard captain", 'p', xLIGHTBLUE, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMAN);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS | GFS_SEE_INVIS);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD | AIF_PEACEFUL);
	XS("St 1d8+30 Dx 1d8+30 To 1d8+15 Le 1d5+15 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 5d5");
	XR("see_invisible:0d0+50");
	XC("1d8", "2d2");
	XM("4d2", "1d3", "1d5+15", "1d3+5");
	XD("Standing just over five feet tall, Ozorick doesn't seem like such an imposing figure but the air of command about him is complete.  His thickly corded neck and arms lightly balance a huge broadsword covered with runes.  He swings it down as you enter and you can't help but gasp as the point cuts right through a stone in the floor and barely even slows.  No one who has seen him wield his mighty sword in battle would dream of opposing him.");
	XL(SKT_HEALING, 10);
	XL(SKT_FINDWEAKNESS, 10);

	XVW(CN_YOHJISHIRO, "Yohjishiro, the elven wizard", 'h', xWHITE, CPT_NAMED_SHE, CRL_UNIQUE, CR_HUMANOID);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS | GFS_SEE_INVIS);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD | AIF_PEACEFUL);
	XS("St 1d8+15 Dx 1d8+40 To 1d8+10 Le 1d5+45 Wi 1d5+45 Ma 1d5+45 Pe 5d6 Ch 7d5");
	XR("see_invisible:0d0+100");
	XC("1d3", "1d2");
	XM("1d4", "1d1", "1d5+10", "5d5+50");
	XD("Last of the elder wizards to live in the vale of Avanor, Yohjishiro quietly tends her herbs and gardens. Her white hair hangs down her back as she kneels down to look at one of her bushes.  Her pointed ears stick out from under a floppy hat that keeps the sun from her eyes.  As she walks through the garden, you notice plants sprouting wherever her feet touch the soil.  Truly she is a power of life.  Frail but intelligent, she remains apart from most of the happenings of Avanor.  Occasionally she will accept a pupil or give aid to those in need.");
	XL(SKT_HEALING, SKILL_MAX_LEVEL);
	XL(SPELL_LIGHTNING_BOLT);
	XL(SPELL_HEAL);
	EQ(IM_WEAPON, IT_HAT, 100);
	EQ(IM_WEAPON, IT_STAFF, 100);

	XVW(CN_GEFEON, "Gefeon, great master of Fire", 'p', xRED, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMAN);
	XBA("1d10+110", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS | GFS_SEE_INVIS);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD | AIF_PEACEFUL);
	XS("St 1d8+15 Dx 1d8+40 To 1d8+10 Le 1d5+45 Wi 1d5+45 Ma 1d5+45 Pe 5d6 Ch 7d5");
	XR("see_invisible:0d0+100");
	XC("1d3", "1d2");
	XM("1d4", "1d1", "1d5+10", "5d5+50");
	XD("As master of the eternal flame of Avanor, Gefeon advises the king in all matters arcane.  Clad only in robes and wearing rings of power, he sits and meditates on the state of the universe.  As you watch a furrow creases on his brow as if he is greatly worried by something.");
	XL(SKT_HEALING, SKILL_MAX_LEVEL);
	XL(SPELL_FIRE_BOLT);
	XL(SPELL_HEAL);

	XVW(CN_RODERIK, "Roderick, king of Avanor", 'p', xYELLOW, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMAN);
	XBA("1d30+150", "0d0+700", "0d0+700", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS | GFS_SEE_INVIS);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD | AIF_PEACEFUL);
	XS("St 1d8+55 Dx 1d8+60 To 1d8+40 Le 1d5+35 Wi 1d5+20 Ma 1d5+20 Pe 8d6 Ch 12d5");
	XR("see_invisible:0d0+100");
	XC("1d6", "1d5");
	XM("1d8", "1d3", "1d5+70", "1d5+30");
	XD("The mystical crown of Avanor rests upon the head of this noble looking man.  Curls of red hair stick out from under it but don't detract from his noble bearing.  The sceptre of his rule lies in his hand looking like an ornament but it has been said it is a formidable weapon wrought with great magic in days of yore.  Rodrick's face is happy but you can see great concern in his eyes.  The responsibilty for this nation must truly weigh on him.");
	XL(SKT_HEALING, SKILL_MAX_LEVEL);
	XL(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL);

	XVW(CN_AHKULAN, "Ahk-Ulan, great master of Darkness", 'p', xDARKGRAY, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMAN);
	XBA("1d30+120", "0d0+900", "0d0+900", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS | GFS_SEE_INVIS);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD | AIF_PEACEFUL);
	XS("St 1d8+15 Dx 1d8+40 To 1d8+10 Le 1d5+45 Wi 1d5+45 Ma 1d5+45 Pe 5d6 Ch 7d5");
	XR("see_invisible:0d0+100");
	XC("1d3", "1d2");
	XM("1d4", "1d1", "1d5+30", "5d5+50");
	XD("Once a bright and promising sorcerer, Ahk-Ulan delved into dark magics and soon became the dark and twisted being he is today.  The aura of power and decay surrounds him as he seeks to control the destructive magics he has consumed.");
	XL(SKT_HEALING, SKILL_MAX_LEVEL);
	XL(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL);
	XL(SKT_CONCENTRATION, SKILL_MAX_LEVEL);
	XL(SPELL_ACID_BOLT);
	XL(SPELL_HEAL);

	XVW(CN_TODIN, "Todin, dwarven weaponsmith", 'h', xBROWN, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMANOID);
	XBA("1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS | GFS_SEE_INVIS);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD | AIF_PEACEFUL);
	XS("St 1d8+30 Dx 1d8+30 To 1d8+15 Le 1d5+15 Wi 1d4+5 Ma 1d4+5 Pe 3d6 Ch 5d5");
	XR("see_invisible:0d0+20");
	XC("1d8", "2d2");
	XM("4d2", "1d3", "1d5+15", "1d5+5");
	XD("Squat, sturdy and built like a boulder here stands the King's twin brother and master smith.  Todin stands at his forge and works the bellows with one hand while nonchalantly shaping a sword with the hammer in his other hand.  The ruddy glow of the forge glimmers on his sweat drenched skin.  Truly he is a master smith as the weapons hanging about the room display his craft.");
	XL(SKT_HEALING, 6);
	XL(SKT_FINDWEAKNESS, 6);

	XVW(CN_TORIN, "Torin the Great, dwarven king", 'h', xLIGHTGREEN, CPT_NAMED_HE, CRL_UNIQUE, CR_HUMANOID);
	XBA("1d10+100", "0d0+800", "0d0+800", CS_NORMAL, "1d200+1200");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 100, GFS_SUPRESS_INVIS | GFS_SEE_INVIS);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD | AIF_PEACEFUL);
	XS("St 1d8+35 Dx 1d8+20 To 1d8+30 Le 1d5+25 Wi 1d5+25 Ma 1d5+25 Pe 5d6 Ch 4d5");
	XR("see_invisible:0d0+100");
	XC("1d3", "1d2");
	XM("1d4", "1d2", "1d5+30", "1d5+10");
	XD("Twin brother to the master smith, Torin's obvious strength and coloring are all that own the relationship.  The crown on his head glitters and his great golden beard flows over his expansive chest.  The hand he rests on the hammer at his belt belies his prowess in battle.  As your eyes drift to the door at the back of his throne room he clears his throat as if to address you, and draw your attention.");
	XL(SKT_HEALING, SKILL_MAX_LEVEL);
	XL(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL);

	XVW(CN_BEELZEVILE, "Beelzevile, the horned demon", '&', xLIGHTCYAN, CPT_NAMED_HE, CRL_UNIQUE, CR_DEMON);
	XBA("1d30+50", "0d0+1500", "0d0+900", CS_LARGE, "1d400+4000");
	XBO("head body ring ring", 100, GFS_SUPRESS_INVIS | GFS_SEE_INVIS);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD);
	XS("St 5d5+50 Dx 3d3+10 To 2d15+20 Le 9d5 Wi 3d4 Ma 5d4 Pe 1d3 Ch 1d3");
	XR("see_invisible:0d0+70");
	XC("0d0+10", "3d15");
	XM("0d0-10", "0d0+15", "5d5+100", "5d5+100");
	XD("How he got to the mushroom caves is anyones guess but it is commonly believed that some kobold shaman got a little carried away with his magic and summoned Beelzevile from the netherworld.  Standing 10 feet high and carrying a reek of sulphur about him, he strides around the cavern, furious at being trapped in this cold place.  His massive claws can shred flesh and steel alike.  You catch a glimpse of a magnificent ring on one of his hands as he noisely chews on the haunch of a kobold who wandered a little too close.");
	XAT(BR_POISON, 80);
	XAT(BR_FIRE, 100);
	XAT(BR_COLD, 100);

	XVW(CN_XSHEE_VOO, "Xshee-Voo, the Cyclope", 'H', xLIGHTMAGENTA, CPT_NAMED_HE, CRL_UNIQUE, CR_GIANT);
	XBA("1d30+120", "0d0+900", "0d0+900", CS_LARGE, "1d400+3000");
	XBO("head neck body cloak hand hand boots", 50);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM | AIF_COWARD);
	XS("St 5d5+150 Dx 1d10+10 To 1d10+80 Le 1d5+5 Wi 1d5+5 Ma 1d5+5 Pe 1d6 Ch 1d5");
	XR("");
	XC("1d5", "2d5");
	XM("0d0-10", "0d0+15", "1d5+70", "1d5+5");
	XD("Xshee-Voo has lived in his mountain cave for as long as anyone can remember.  He never shows himself outside, and the few that have been in his cave and returned speak of piles of bones and armor slowly decaying.  They also speak of his enormous club which looks to have been carved from the rock of the mountain and is written over with Runes of great power.");
	XL(SKT_HEALING, SKILL_MAX_LEVEL);
	XL(SKT_FINDWEAKNESS, SKILL_MAX_LEVEL);


	XVW(CN_MAGNUSH, "Magnush, the Kobold Lord", 'k', xRED, CPT_NAMED_HE, CRL_UNIQUE, CR_KOBOLD);
	XBA("1d10+120", "0d0+1000", "0d0+1000", CS_SMALL, "1d200+900");
	XBO("head neck body cloak hand hand ring ring gloves boots light_source tool missile_weapon missile", 5);
	XA(AIF_RANDOM_MOVE | AIF_ALLOW_PICK_UP | AIF_ALLOW_WEAR_ITEM);
	XS("St 4d5 Dx 3d5 To 5d3 Le 2d4 Wi 3d5 Ma 3d3 Pe 1d8 Ch 1d4");
	XR("");
	XC("1d4", "1d3");
	XM("1d4", "1d2", "2d6", "2d3");
	XD("Magnush the kobold lord stands before you with all the majesty he can muster.  Bigger than most kobolds (some say his father was a gnoll), he has managed to bully his way to the prime position in his pack.  His mangy fur and crooked legs give him an almost laughable appearance though and you have difficulty hiding your mirth from this overgrown puppy.");
	XL(SKT_HEALING, 10);
	XL(SKT_FINDWEAKNESS, 10);
	EQ(IM_SCROLL | IM_POTION | IM_BOOK | IM_FOOD, 6, 20);
	EQ(IM_WEAPON, IT_DAGGER, 100);
	COE(CET_DISEASE, 30);
	XL(SPELL_CURE_SERIOUS_WOUNDS);

	/////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////
	// Fill out creature sets for quick random generation

	for (int i = 0; i < CN_EOF; i++)
	{
		if (XCreatureStorage::creature_storage[i].name != NULL)
		{
			CREATURE_CLASS crc = XCreatureStorage::creature_storage[i].cr_class;
			XCreatureStorage::creature_set[vGetBitNumber(crc)].cn[XCreatureStorage::creature_set[vGetBitNumber(crc)].count] = (CREATURE_NAME)i;
			XCreatureStorage::creature_set[vGetBitNumber(crc)].count++;
		}
	}
}

