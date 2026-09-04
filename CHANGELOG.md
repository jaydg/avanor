# Changelog

## 0.6.0 — in development

The first work on Avanor since **0.5.8** in 2006. The game builds and runs on
modern systems again, it uses the whole of your terminal, and a surprising
amount that had quietly never worked now does.

---

### New things to do

**Bring Brida's daughter home.**
Brida keeps a house in the village, and she has lost her daughter. Giana went
into the valley and never came back — a bandit calling himself Rotmoth is
holding her in a cave east of the village, past where the bandits lurk near
the bridge, and he wants 150 gold coins for her.

You can pay him. You can also refuse, in which case he will not take it well.
Either way Giana follows you out of the cave, and once she is back among the
houses she stays there, where her mother can find her. Brida has little to
give you but she has not forgotten her own mother's craft, and what she
presses into your hands is worth having.

The quest was started by the original development team, but existed only in
fragments. It can now be started, played and finished.

**The beginner's dungeon can be finished in one visit.**
The Elder sends you to kill the demon in the caves west of the village, and
until now that was not a fight you could have. Beelzevile stood behind so
much armour that no weapon you could carry at that point could scratch him
at all, and he hit hard enough to end most characters in a single blow. The
quest could only be finished much later, by a hero who had gone off and
grown up somewhere else entirely.

The caves now work as the descent they look like. Take the other stair,
clear the kobold caverns, and **Magnush the Kobold Lord** is a real first
boss — he was previously weaker than the bandits on the road outside, and
died in two blows before he could do anything. He knows a healing spell and
carries scrolls and potions, and now lives long enough to use them, so you
will have to out-fight him rather than out-roll him.

That earns the levels to go back up and face the demon, who is now a fight
rather than a wall. He is still far stronger than you and will win a
straight exchange of blows — but he is slower than you are, which he always
was, and that is the opening. Back away, shoot, drink something, and pick
your ground.

**Clear the old road east.** Once the demon is dead and his farmers are
back at work, Elder Gridor has a second thing to ask. The road east has been
shut for years — five forest brothers hold the bridge, and the village has
not walked it since. He is asking at the point in the game where it becomes
a fight you can have rather than one you lose.

He also tells you what Brida has not had the chance to: one of them took one
of hers. If you had not yet met the girl's mother, you know now where to
ask.

The road is what he wants cleared, not the brotherhood. Jorgus keeps his
house behind the camp and his own counsel, and the man who teaches thieving
is not part of the bargain.

**The bandits at the bridge say so first.** Five forest brothers hold the
only crossing of the river, and until now the first a new character knew of
them was dying to one. They call out from the trees as you set foot on the
span — the last square from which turning back is still free, since their
ground begins on the far bank — and the villagers will warn you too, if you
ask anyone before you go. Cross anyway and that is your decision, which is
all the warning was ever meant to buy you.

They only shout at somebody about to cross. Coming home the other way, they
have nothing to say to you; and once they are dead the road is quiet.

**The Forlorn Mine, and the artifact at the bottom of it.** A ruined
head-house stands on the plain far south of the valley, roof down in two
places, an apron of spoil and dressed stone spreading out around it and
saplings coming up in what was the yard. The shaft inside it still goes
down — five to ten levels of galleries and halls, harder the deeper you
go, and at the very bottom the Eye of Raa.

The mine stands where one of the three unnamed random dungeons of old
stood — "Random Place Level 1" and so on, three of them, cut when the world
moved to Lua and never replaced, taking the Eye with them. This is one
mine rather than three caves, and it has a name and a reason to be there:
it was a gem mine, and the miners who sank it followed the seams down and
then kept following them, until they broke into something that was not a
seam. What came up through that hole took the mine in a night. Nobody has
been back for the tools, let alone the stones.

Every game digs it afresh, and the whole mine takes one shape — you will
not find a warren of passages on one level and a hall on the next.

**Caves that look like caves.**
A second kind of cavern joins the old blob-shaped ones: rounded chambers grown
one at a time and joined by single doorways cut through the rock, some of them
eroded down to corridors, with lakes poured in wherever they will not cut the
level in two. The outlines wander rather than following the grid they were
built on, so the walls read as stone rather than as masonry.

The Small Cave where Giana is held is built this way now — smaller than it
was, and wetter.

Nothing in it knows it is underground. Hand it trees instead of walls and
grass instead of floor and the same code lays out a forest of glades joined by
paths, which is waiting for a level to use it.

**The dwarves cut their own road, and it looks like it.** The six levels
between the valley and the dwarven gate used to be rooms and corridors, the
same shape as everywhere else underground. They are mazes now — square
corners, straight runs, passages doubling back on themselves: worked stone
rather than a cave somebody happened to find. Each level draws its own
pattern, so the descent changes character as you go down instead of being
one warren repeated six times, and the levels are twice as deep from north
to south as they were.

They are cut by a new generator, written by Kusigrosz and given to the
public domain. The Forlorn Mine above is dug by the same generator.

**A tower with an upstairs, and something to look at from it.**
Yohjishiro's tower now has a real second floor. Not a separate room that
happens to be reached by a staircase — a floor that sits directly above the
ground one, on the same piece of the valley. Everything the upper floor does
not cover is a hole in it, and what shows through the hole is the valley
underneath, drawn from where it actually is rather than painted on.

The tower has windows, which are walls you cannot walk through but can see
through. Stand at one and you are looking down at the ground you walked in
across — and at Yohjishiro's sheep, a flock of them grazing the grass either
side of the tower and wandering about while you watch. They are hers, and
they share her company: harm one and she will know about it.

**Shopping without arithmetic.**
Pressing `P` in a shop now brings up an itemised bill: every item you have
picked up and not paid for, each with its price, and a line along the bottom
showing what you owe in total against what is in your purse. You settle it a
piece at a time, so you can buy the boots and put the sword back. If you
cannot afford something the shop says so and leaves you in the list rather
than throwing you out.

While you are in the shop, anything you have picked up and not paid for is
marked `(unpaid)` in your inventory, the same way worn things are marked
`(worn)`. The bill tells you what you owe; this tells you which of the things
in your pack it is talking about, which is what you want when you cannot
afford the lot and have to decide what goes back on the shelf. A heap that is
part yours and part theirs says so — `heap of (3) small rations (1 unpaid)` —
so you put back the one you took and keep the two you walked in with.

The shopkeeper has also learned to stand in his own doorway while you are
carrying something you have not paid for. He steps aside the moment the bill
is settled, and goes back to wandering about his shop.

**Everything you own on one screen.**
Worn items no longer disappear out of your inventory when you put them on.
They stay in the list with a `(worn)` badge, so comparing the sword in your
hand against the one in your pack no longer means flipping between the
inventory and the equipment screens.

**Ground that isn't flat.**
No two patches of the same floor are quite the same shade any more. Each cell
strays a little from its tile's colour in brightness, in hue and in how much
colour it has, so a cavern floor reads as ground rather than as one character
stamped out a thousand times, and a stand of trees reads as a wood. Greys stay
grey and water stays the colour of water — the variation never changes what a
thing is, only how it catches the light. Turn it off if you would rather have
the flat colours.

**The window is yours.**
Avanor used to draw an 80×25 screen and ignore whatever room it actually had.
It now uses the whole terminal, and follows it when you resize the window —
the map grows, the menus and the title recentre, and lists re-flow. Narrow
windows no longer scramble the display.

**The achievements screen remembers everything you did.**
When your game ends it used to name four things: the gas pump, the crypt, the
Eye of Raa, and how the story finished. Every other errand you ran — the demon
in the caves, the road east, Brida's daughter, the machine parts — went
unmentioned, however it turned out. Each quest now carries its own line and
says what became of it, and the same list is what goes into the memorial file.

---

### Skills

**Tactics finally improves.** Your stance has always shaped how well you
strike, how hard you hit and how well you defend — but the skill behind it
could never rise above 4, however long you fought, because nothing counted
your practice. Landing a blow and turning one aside both count now, so a
warrior who spends a life in combat can become genuinely good at it.

**Archery had the same trouble and is fixed the same way.** Every shot you
loose is worked out from your archery — its range, its damage and, the
largest part of a ranged attack, whether it hits at all — and none of it ever
counted as practice. Firing a bow now teaches you something about firing a
bow.

**Halflings know how to dodge**, which suits the smallest and nimblest of the
races rather better than not knowing.

**Create Trap and Climbing are described in the manual.** Both worked; neither
was written down anywhere, so there was no way to discover Climbing existed or
what Create Trap could build.

---

### Magic and combat

**The villagers are mushroom farmers, and now they look it.** They were
described as sunburnt, leaning on pitchforks, gathering food from the fields
— but there are no fields in this story. The village lives on mushrooms, and
mushrooms come out of the caves to the west. Its people are pale as things
grown under a stone, they have spent their working lives underground, and
what they lean on is a long spear, because there are things down there that
bite. The goodwives take their turn below as well; they have a spear apiece
now instead of going bare-handed.

They are a little harder to kill than they were, too, which they had earned.
Once the demon in the caves is dead and the Elder sets them back to work,
they walk down past the snakes and the beetles four times a trip and they do
it for the rest of the game.

**Sandals made you easier to hit than bare feet.** So did plate mail, chain
mail and ring mail — the heaviest armour in the game was quietly *reducing*
your defence — and a war hammer, a knife, a scimitar, a great axe, a pike, a
halberd and a staff all came out of the forge less accurate than they should
be. Anything made of wood, stone, iron, bronze or brass carried its own
hidden penalty on top.

The cause was a conversion begun on New Year's Day 2004 and never finished.
Item statistics had been rewritten that day to use a different kind of roll,
one where the worst case is nothing rather than something, and most of the
tables were rewritten to match — but a few dozen entries were left in the old
notation, where they now read as a penalty instead of a floor. Sandals were a
line that got half converted: the protection column was corrected and the
defence column beside it was not.

Forty-three entries are now finished the way the rest were. Nothing an item
gives you can come out negative, and the spread of qualities is the one the
distribution was designed for: most specimens ordinary, the occasional one
notably better. The genuine penalties stay — heavy armour still costs
accuracy by its weight, and a ring can still be cursed.

**Every blow you strike counted twice.** For as long as anyone has played
this version, a weapon with no fire or frost on it — which is to say almost
every weapon in the game — dealt double what its dice said, and so did every
monster's claw and bite. Both sides of every fight have been hitting twice
as hard as the numbers on your character screen.

Fights are longer now, and they are the fights the monsters were built for.
Magnush lived about three turns and now lives about eight, long enough to
reach for the healing spell and the potions he has always carried. The
demon in the mushroom caves is no longer someone you can trade blows with
and expect to win — back away, shoot, and remember that he is slower than
you are. Weak things you used to flatten in one swing may take two.

**And now a branded weapon is worth carrying.** Because the doubling
applied to plain steel and not to fire, a sword of Fire was doing no more
than an ordinary one. It does now, and so does a weapon that slays orcs or
demons, against what it was made for.

**Resistance no longer betrays you at the top.** Fire resistance stacked
past 100% used to turn itself inside out: a creature — or a hero — who was
*completely* immune to fire took the full damage of a fire spell, while
someone merely half-resistant took half. Stacking rings and amulets of
resistance was punishing you for succeeding. Immunity now means immunity.

**A blow they never saw coming.** Backstabbing could be learned and raised
like any other skill, and did nothing whatsoever — the code meant to spot
the opening was sitting inside a comment block, and had been for the whole
modern history of the game. It works now. Strike with a weapon in hand at
something that has not yet realised you are an enemy, or that cannot see
you at all, and the blow lands for three times its damage. The chance
starts around one in ten and climbs with the skill, and every backstab that
lands teaches you something about finding the next one — a swing that
misses teaches nothing and triples nothing. Humans begin the game knowing
the skill, which makes the first blow of an ambush worth planning.

It cuts both ways. Vampires have backstabbing at its maximum *and* are
invisible, so unless you can see invisible, four blows in five arrive as
backstabs. They were already worse for draining your life; they are now
considerably worse than that.

**The schools of magic mean something now.** Every spell belongs to one of
five schools, and the code that was supposed to advance you through them had
been commented out — so every caster began knowing every school equally, none
of them ever improved, and what you knew of a school made no difference to
anything you cast.

Learning your first spell of a school now opens it, and every spell you cast
successfully teaches you a little more about the school it belongs to, the way
swinging a sword teaches you about swords. Rank by rank — Beginner, Novice,
Adept, Master, and at the last Grand Master — your grasp deepens, and *every*
spell you know within that school hits harder and reaches further for it, even
the ones you have barely practised. Press `#` to see how far you have come;
schools you know no spells of are not listed at all.

**Draining life now drains it.** The spell did damage and nothing else; half
of what it takes from you now goes to whatever cast it. Spectres and vampires
are meaningfully harder.

**Corpses do what they say.** Every corpse in the game granted Strength
regardless of what it was. Snakes give resistance to poison again,
salamanders resistance to fire, and the things that should make you ill make
you ill.

**A weapon too heavy for you now costs damage as well as accuracy.** Swinging
something you can barely lift always spoiled your aim; it now blunts the blow
as well, and the character screen shows you both penalties.

**A monster chooses its spell by what the spell is for**, rather than from a
list of five written into the engine, so a caster reaches for whatever it
knows and can actually land.

---

### Fixed

Things that used to lose your belongings:

- Items no longer vanish when picked up into a stack of the same kind
- Dropping items, and picking exactly one item off a pile, both work
- Corpses you are carrying rot at the proper rate
- Loot, the royal treasure room and the Eye of Raa all work again

Things that used to crash the game:

- A long-standing family of crashes around creatures and items being freed
  while something was still using them: swapping places with a companion,
  summoning, blinking and teleporting, a target dying mid-blow, an eaten
  corpse's delayed poison, and item stacks merging
- Talking to Roderick while a hand was empty
- Using an item, learning an alchemy recipe, and targeting past the map edge
- Quitting the game

Things that simply did not happen:

- The bard's dog was never created
- Ranged spells and missile animations
- Herb bushes never spread
- Dwarves were given the gnome's starting skills by mistake
- Creatures sacrificed the clothes off their own backs as useless junk
- Reading the high score table, writing recipes, the backspace key, and the
  status line
- Beating off the orc war party was never acknowledged at the end of the
  game: the achievements screen asked for thirty orc deaths, and only twenty
  orcs have ever existed
- Unidentified scrolls carried the same labels in every game. The nonsense
  name on a scroll you have not read is meant to be rolled afresh each game,
  so that learning one teaches you nothing about the next; it was rolled
  once as the program started, before the game had a seed
- Unread spell books always looked the same. A book you have not read is
  meant to be a mystery — an old tome, a thin book, something with a silver
  binding — and which look belongs to which spell is meant to be shuffled
  for each new game, so that finding out costs you the reading. The looks
  were handed out in the order the books were listed instead, and that order
  never changed, so burning hands was the old tome in every game anyone ever
  played. They are dealt afresh each game now

Things that went quietly wrong the longer you played:

- Guards, smiths and kings abandoned their posts for good. Rousing a group
  against an attacker — a monster killing one of them, a quarrel picked in
  the wrong room — unleashed every member of that group, and nothing ever
  leashed them again. The anger wore off; the wandering did not.
- Ahk-Ulan's errands outlived Ahk-Ulan. Kill him and his request for the
  three parts of an ancient machine stayed in your quest log for the rest of
  the game. It now ends when he does, and ends as a failure. His second
  errand had the opposite problem: once he asked you to kill Roderick, that
  line stayed in the log whether or not you had already done it.
- Collecting Torin's reward cost you the credit for earning it. Starting the
  gas pump at the bottom of the dwarven gold mine is worth 5000 points on
  the achievements screen at the end of the game — but the moment Torin
  thanked you and handed over your reward, the game stopped counting it, and
  only players who never went back to claim it were ever paid. The pump is
  what earns those points now, and nothing afterwards takes them away.

---

### For the curious

God mode (`avanor --god`) has two new keys. `^F` lights the whole level, walls
and creatures and all, and pressing it again puts the dark back — what it
shows you is never written into your character's memory, so switching it off
leaves the map as unexplored as it was. `^W` steps you anywhere on the level,
scrolling the map under the cursor so you are not limited to what you can see.
Neither costs a turn.

---

### Under the hood

Two decades of drift, in the order it was dealt with:

- Rebuilt on modern C++17; builds clean with all warnings on
- Replaced the bundled Lua 5.0 with LuaJIT, and the hand-written Lua glue with
  a proper binding layer
- Replaced the home-grown save format with a real serialization library
- Replaced the terminal layer, gaining 24-bit colour and window resizing
- Moved most of the game's content — creatures, items, levels, NPCs, their
  dialogue and their quests — out of C++ and into script, so it can be changed
  without a compiler
- Rewrote memory handling throughout; the engine no longer hands out pointers
  to things it has already freed
- Dropped DOS support and a great deal of dead code

---

## 0.5.8 and earlier

See below for the original development history, from 0.4.9 in 2003
through to the last release in 2006. The issue numbers reference the
project's bug tracker, which is lost.

---

## 0.5.7

- Fixed bug #214 (Eye of Raa crashes the game)
- Fixed bug #186 (Disappearing of picked items when overburdened)
- Fixed bug #131 (Using walk command to find invisible enemies)
- Fixed bug #140 (Obsidian helm changed to steel helm)
- Fixed bug #216 (swapping inside shop)
- Fixed bug #219/142 (Identify bug)
- Fixed bugs #218/220, #189, #213, and #279
- Added special rooms with creatures
- Added new monsters: Vampire, Grey Jellies
- Added furniture and traps
- Avanor castle is now part of a capital city
- Added pikes and halberds

---

## 0.5.6

- Added player gender
- Added throne object in the throne room
- Added new items: Dress, Katana and Wakizashi
- Added new creatures: Dwarves in Dwarftown, Female citizens, Goodwives
- Modified Yohjishiro's Tower
- Some grammar fixes
- Added new ending of the game
- Now it is possible to target creatures over water
- Canceling targeting now does not use mana nor ammo
- Fixed crash when identifying some items
- Fixed bugs (#187, #87) with inventory
- Added new key '0' - force screen centering
- Fixed gains attributes calculations
- Added Throne of Avanor and of Dwarf city
- Potion of lifeless is now Potion of death
- Fixed bug where skeletons generated outside tomb (possible fix for #195)
- Improved Bloated status and added Overfed
- Avanor is now Open Source under the GNU General Public License

---

## 0.5.5

- Added new skill Athletics. Also improved time-to-time increasing of main stats
- Fixed crash when dying from hunger
- Made significant improvements to Mushroom quest
- Added new creatures
- Greatly improved AI (also partially fixed bug #173)
- Added possibility to issue orders to companion (Ctrl+O command)
- Added possibility to find a companion
- Added new class - Bard
- Fixed bug #169 (Foods do not stack)
- Fixed bug #168 (Hero can move even when overburdened)
- Fixed bug #172 (Changing color of some service messages when sick, paralyzed, stunned)
- Fixed bug #61 (Open command works only on the top chest if you make a pile of chests)
- Fixed bug #167 (After restoring game, shopping does not cost anything)
- Fixed bug #165 (multiple prompts for arrows reloading)
- Implemented feature #153 (mushrooms in the Mushroom Cave)
- Partially implemented feature #136 (Invisibility is overpowered). Improved AI. Some uniques always see invisible creatures
- Implemented feature #77
- Implemented feature #146 (indicate Exit key on the startup screen)

---

## 0.5.4

- Some crash fixes
- Some minor grammar fixes

---

## 0.5.3

- Added creature descriptions (special thanks to Uriah Otting)
- Some grammar improvements
- Some magic improvements
- Added new races: Half Elf and Gnome
- Added new classes: Ranger and Paladin
- Fixed crash with targeting
- Fixed bug #143 (Extra commas) - Upgraded page with enemy creature inventory description
- Fixed bug #145 (grammar)
- Fixed bug #156 (grammar)
- Fixed bug #127 - Now both 's' and '_' work for sacrifice
- Fixed bug #147 (spelling)
- Fixed bug #124 with wrong output message when trying to wear light source
- Fixed bug #160 (deprecated information in the manual)
- Added prompt for loading of missiles
- Pressing 'R' at start of game to restore it now starts a new game if no save exists
- Learning (Le) now affects reading books
- Added new command Ctrl+T - activate trap
- Now it is possible to wait a couple of turns while staying on stairs or items
- Traps are now more powerful and additional traps were added
- Implemented skill Cooking
- Each new level now grants at least one HP and PP
- Now it is possible to mix potions without requiring a particular order in the recipe
- Now it is possible to cancel spell casting without any consequences

---

## 0.5.2

- Fixed several bugs

---

## 0.5.1

- Fixed bug with overburdened output
- Fixed bug with QUEST after escape
- Added alchemy
- Improved herbalism

---

## 0.5.0

- Changed random generation for Books and Potions
- Added new quest from Roderick and new places
- Fixed bugs #109, #111, #114, #119, #105

---

## 0.4.11

- Slightly changed tactics. Also added new skill tactics
- Added potion of swiftness
- Added potion, scroll and spell of see invisible
- Changed representation of Self Knowledge spell, added Resistances and Intrinsics in memory file
- Fixed bug with memory file (#89)
- Fixed bug with Summon Monster scroll (#86)
- Added new place - Kobolds Cavern
- More intelligent creatures can carry gold
- Digging of wall allows to find a little gold

---

## 0.4.10

- Fixed bugs #88, #78
- Added PickAxe and Mining skill
- Added Backstabbing skill

---

## 0.4.9

- Added Torin, Dwarven King and new quest
- Added 'U' command to use external objects such as pumps
- Fixed bug with invisibility
