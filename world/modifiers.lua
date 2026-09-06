-- Modifiers: what can be laid on a creature for a while.
--
-- A wound that bleeds, a poison working through the blood, a quickening, a
-- resistance held for as long as the potion lasts. Each one says what it is
-- called on the status line, what the one carrying it is told, and what it
-- shifts for as long as it lasts - and the engine does the shifting, and
-- takes it back when the modifier goes, so nothing has to remember to undo
-- itself.
--
--   Modifier.new(id)
--       :Called(text)          what the status line calls it. Unsaid, it
--                              shows nothing - safety is felt, not seen
--       :Severity(upto, text)  the same, but read by how much there is:
--                              the first band the value has not reached
--                              wins, and a ceiling of 0 means "and above"
--       :OnSet(text)           as it takes hold
--       :OnRemove(text)        as it lifts
--       :OnChange(more, less)  as it grows, and as it fades
--       :Applies(text)         each turn it actually does something
--       :While{ DV = n, HIT = n, Slower = n,
--               Stats = {[XStats.STR] = n},
--               Resistance = {fire = n} }
--                              what shifts while it is on
--       :Scale(n)              the amount it is laid on with is multiplied
--                              by this first - poison counted in doses
--       :ResistedBy(id)        what shortens it, or shrugs it off entirely
--       :Engine(which)         the few whose behaviour turn by turn the
--                              engine has to carry: "bleed", "poison",
--                              "stagger", "hold", "rot_body", "rot_strength"
--       :Register()
--
-- The engine acts on five of these by name: paralysis stops the hero's
-- input loop, poison forbids running, a heavy blow inflicts a wound, a stun
-- or a confusion, and a prayer for healing lifts a fixed list. Renaming
-- those ids means teaching the engine the new names.

Modifier.new("wound")
	:Severity(3, "<SEVERITY_MILD>graze")
	:Severity(8, "<SEVERITY_MILD>light cut")
	:Severity(20, "<SEVERITY_NOTABLE>severely cut")
	:Severity(50, "<SEVERITY_SEVERE>deep wound")
	:Severity(100, "<SEVERITY_SEVERE>deep gash")
	:Severity(0, "<SEVERITY_CRITICAL>mortal wound")
	:OnSet("You've been wounded.")
	:OnRemove("Your wounds heal.")
	:OnChange("You are wounded again.", "Your bleeding slows.")
	:Applies("You lose blood!")
	:Engine("bleed")
	:Register()

Modifier.new("poison")
	:Called("<SEVERITY_MILD>poisoned")
	:OnSet("You are poisoned!")
	:OnRemove("You feel relieved.")
	:OnChange("You are poisoned again!", "You feel somewhat relieved.")
	:Applies("You feel the poison coursing through your body.")
	:Scale(10)
	:ResistedBy("poison")
	:Engine("poison")
	:Register()

Modifier.new("confuse")
	:Called("<TEXT>confused")
	:OnSet("You are confused.")
	:OnRemove("Your thoughts clear.")
	:OnChange("Your confusion grows.", "You feel a little clearer.")
	:Applies("You stagger.")
	:ResistedBy("confuse")
	:Engine("stagger")
	:Register()

Modifier.new("stun")
	:Called("<SEVERITY_NOTABLE>stunned")
	:OnSet("You are stunned.")
	:OnRemove("You are no longer stunned.")
	:OnChange("You are stunned again.", "You feel a little steadier.")
	:While{ DV = -5, HIT = -10 }
	:ResistedBy("stun")
	:Register()

Modifier.new("paralyse")
	:Called("<SEVERITY_CRITICAL>paralysed")
	:OnSet("You are paralysed!")
	:OnRemove("You can move again.")
	:OnChange("You cannot move at all.", "You feel your limbs loosen.")
	:Engine("hold")
	:Register()

Modifier.new("heroism")
	:Called("<TEXT>hero")
	:OnSet("You feel like a hero.")
	:OnRemove("You feel less heroic.")
	:OnChange("You feel even braver.", "Your courage ebbs.")
	:While{ DV = 5, HIT = 10 }
	:Register()

Modifier.new("disease")
	:Called("<SEVERITY_SEVERE>diseased")
	:OnSet("You are diseased.")
	:OnRemove("You feel healthy again.")
	:OnChange("Your illness worsens.", "Your illness eases.")
	:While{ DV = -5, HIT = -5,
	        Stats = { [XStats.STR] = -3, [XStats.DEX] = -4, [XStats.TOU] = -3 } }
	:Engine("rot_body")
	:Register()

Modifier.new("weak")
	:Called("<SEVERITY_NOTABLE>weakened")
	:OnSet("You feel weak.")
	:OnRemove("Your strength returns.")
	:OnChange("You feel weaker still.", "You feel a little stronger.")
	:While{ Stats = { [XStats.STR] = -5 } }
	:Engine("rot_strength")
	:Register()

Modifier.new("see_invisible")
	:Called("<TEXT>seeing")
	:OnSet("Your eyes sharpen.")
	:OnRemove("Your eyes dull again.")
	:While{ Resistance = { see_invisible = 10 } }
	:Register()

Modifier.new("boost_speed")
	:Called("<TEXT>quickened")
	:OnSet("You speed up.")
	:OnRemove("You slow down again.")
	:OnChange("You speed up further.", "You slow a little.")
	:While{ Slower = -300 }
	:Register()

Modifier.new("slowness")
	:Called("<SEVERITY_NOTABLE>slowed")
	:OnSet("You slow down.")
	:OnRemove("You speed up again.")
	:OnChange("You slow further.", "You quicken a little.")
	:While{ Slower = 300 }
	:Register()

-- One resistance modifier per resistance the world declares, so that
-- "resistant to fire while the potion lasts" needs no special case
-- anywhere: it is a modifier like any other, worth the same 40 the four
-- hand-written classes were worth. Walked from world/resistances.lua
-- itself, so a world that adds a resistance to radiation gets a modifier
-- for it here without writing a line.
--
-- These say nothing on the status line - what they are is already on the
-- Resistances screen.
for _, r in ipairs(Resistance.All()) do
	local of = r.name:lower()

	Modifier.new("resist_" .. r.id)
		:OnSet("You feel safer.")
		:OnRemove("You feel less safe.")
		:OnChange("Your resistance to " .. of .. " grows.",
		          "Your resistance to " .. of .. " fades.")
		:While{ Resistance = { [r.id] = 40 } }
		:Register()
end
