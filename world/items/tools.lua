-- Tools: what the miscellany drawer of the pack holds. ItemKind.TOOL is
-- the category; an XTool that the use command can be pointed at is a
-- different thing, and needs a :Use() handler (see the Eye of Raa, in
-- world/locations/forlorn.lua).

Item.new("ancient_machine_part")
	:Plain("ancient_machine_part", ItemKind.TOOL)
	:View("ancient machine part", ']', xColor.xDARKGRAY)
	:Basic(1000, 15)
	:Register()
