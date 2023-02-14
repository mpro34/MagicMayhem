# MagicMayhem
#### A 3rd-Person multiplayer shooter game. This game is currently being built with Unreal Engine 5. A multiplayer steam sessions plugin can be exported to other projects that require steam sessions.
A Player is able to create a new steam session with HOST or join another steam session with JOIN.
![LoginScreen](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/LoginScreen_1.jpg)

There are 3 sections to the game mode, the first gets the players ready to start, the second is the actual death-match, and the third is the winner announcement and cooldown period. These three modes are illustrated below.
![GameStart Screenshot](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/Opening_1.jpg)
![Gameplay Screenshot](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/Gameplay_2.jpg)
![GameCooldown Screenshot](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/Closing_1.jpg)

## Getting Started:
1. Clone repo
2. Run `MagicMayhemTest.bat` to launch the game with an attached log.
3. Click "Host" or "Join" to create or join a Steam Session, respectfully.
NOTE: If trying to play with multiple people, change the number of players accordingly. Also, use the check boxes to change the game mode.

## Systems Implemented:
- [X] **Different game skin**. More realistic aesthetic to match the weapons.
  - [X] Character(s)
  - [X] Weapons
  - [X] Military scape
  - [X] Military props 
- [X] **Latency protections**. Add client-side prediction, reconcilliation and server-side rewind to various gameplay systems while testing at 150+ms ping.
- [X] **Team based-death match game mode** 
- [X] **Team Capture the flag game mode**
- [X] **User interface for weapons when near them**. So that you know what you are picking up.

## Work to be Completed:
https://trello.com/b/hYpKKJOR/magic-mayhem

#### Gameplay [Work-In-Progress]
- [ ] **Player Magic Ability System**. Players will no longer be able to use guns. Their weapons will be magic stones that are imbued with a magic element. Holding one gives the player a set of abilities to use against the zombie horde. Their off-hand is used only for melee. 
Use a data-driven approach with data-assets and the ability for non-coders to implement new abilities.
    - [ ] https://dev.epicgames.com/community/learning/courses/Xp/unreal-engine-improving-c-workflows-using-data/Pe7/unreal-engine-improving-c-workflows-using-data-introduction
- [ ] **Zombies Game Mode**. Able to play the game single player or multiplayer by AI enemy "zombies" to kill.
- [ ] **New mine launcher weapon**. Shoots mines a short distance. If an enemy gets close, it'll explode.
- [ ] **Titan Game Mode**. A new coop game mode where all players must defend multiple panels. Titans are spawned and will attack the players. If attacked, they will attack you.
- [ ] **Default sword mode**. If the player has no weapons/no ammo/no grenades. The player should play an animation to get a sword for use to attack. Or just make the sword another weapon that is the default weapon.
- [ ] **A shop that allows players to buy/sell weapons, health-packs and shields**. This is how players will upgrade weapons. Get kills then you get money. Then buy more powerful weapons and ammo for said weapons.

#### User Interface [Work-In-Progress]
- [ ] **In-game currency and HUD update**. Requires a form of currency that can be accumulated for each player. The HUD should display the amount.
- [ ] **A minimap**. Add a minimap to the HUD. This should show Enemies, Friendlies, weapons, power-ups, and ammo pickups.
- [ ] **HUD v2**. Should be similar to the BF2 version. ![BF2 HUD](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/swbf2_screenshot1.jpg)

#### Animation/Art [Work-In-Progress]
- [ ] **Player Grappling**. Players are able to grab opponents with a grappling hook in left hand and small weapon in right.
![User Interface for Weapons](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/weapon-ui-example.jpg)
- [ ] **More advanced hitreactions**. Add bloodspray / more realistc damage based on location of hit. Perhaps add physics.
- [ ] **Reskins of Character**. "Magic" themed character models and animations.
- [ ] **Reskins of Weapons**. Weapons should be "magic" related or each character can also have powers.

#### Graphics [Work-In-Progress]
- [ ] **Destructible Enviornments**: Barrels, etc.

#### Build [Work-In-Progress]
- [ ] **Nintendo Switch**: Get Switch SDK access and make game playable on the Nintendo Switch.
- [ ] **Steam**: Make game available on Steam.

## Story Goals:
- The world has been ravaged by a virus. Magicians of the world have tried to combat this threat to no avail. You are a team of Magician Soldiers trying to survive a zombie onslaught. There are no guns, your attacking consists of magic abilities through magic stones and melee. All abilities are modeled after elements that have either damage, utility or both. Each player is able to equip one magic stone at a time and a melee weapon. The zombie virus is highly infectious, thus making the smallest contact from a zombie possible to infect the player.

## Gameplay Goals:
- A round-based 3rd person shooter where the player will be shooting magic abilities and not bullets. 
- Left trigger will control the Magic stone, while the Right trigger will be Melee. Space will jump, Shift will sprint.
- The player(s) will be holed up in a location where a zombie horde will try to attack them. They will need to use their elements to defend against the onslaught.
- A unique feature will be the highly infectious nature of the virus, and once a player dies, they become a "good" zombie. A "good" zombie is able to attack other zombies to help their living teammates, but are very limited.
- Once all players become zombies, it's game over. The player(s) are to last the longest. There will be specials that get spawned based on the number of rounds completed.
- Also, when zombies are killed, their bodies will continue to exist. If enough zombie corpses pile, it will contaminate the area and infect all players within that area. To avoid this, players will have to burn the bodies or freeze an melee them to eliminate the virus.

The elements to start are Fire and Ice. Additions will be Storm and Void.
The Melee weapons to start will be Fist, Baseball Bat, Sword.
