# MagicMayhem
#### A 3rd-Person multiplayer shooter game. This game is currently being built with Unreal Engine 5. A multiplayer steam sessions plugin can be exported to other projects that require steam sessions.
A Player is able to create a new steam session with HOST or join another steam session with JOIN.
![LoginScreen](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/LoginScreen_1.jpg)

There are 3 sections to the game mode, the first gets the players ready to start, the second is the actual death-match, and the third is the winner announcement and cooldown period. These three modes are illustrated below.
![GameStart Screenshot](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/Opening_1.jpg)
![Gameplay Screenshot](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/Gameplay_2.jpg)
![GameCooldown Screenshot](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/Closing_1.jpg)

## Systems to Implement (order of priority):
- [X] **Different game skin**. More realistic aesthetic to match the weapons.
  - [X] Character(s)
  - [X] Weapons
  - [X] Military scape
  - [X] Military props 
- [X] **Latency protections**. Add client-side prediction, reconcilliation and server-side rewind to various gameplay systems while testing at 150+ms ping.
- [X] **Team based-death match game mode** 
- [X] **Team Capture the flag game mode**
- [X] **User interface for weapons when near them**. So that you know what you are picking up.
- [ ] **Player Grappling**. Players are able to grab opponents with a grappling hook in left hand and small weapon in right.
![User Interface for Weapons](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/weapon-ui-example.jpg)
- [ ] **A minimap**. Add a minimap to the HUD. This should show Enemies, Friendlies, weapons, power-ups, and ammo pickups.
- [ ] **HUD v2**. Should be similar to the BF2 version. ![BF2 HUD](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/swbf2_screenshot1.jpg)
- [ ] **More advanced hitreactions**. Add bloodspray / more realistc damage based on location of hit. Perhaps add physics.
- [ ] **Destructible Enviornments**: Barrels, etc.
- [ ] **Vehicles**: Light and Heavy
- [ ] **Different classes**: Each class has two different weapons (light and heavy), a utility (grenades), and a special ability. Special weapons are able to be picked up by all classes: Rocket Launcher, Grenade Launcher, Sniper
1. Soldier - Assault Rifle, Pistol
2. Mechanic - Shotgun, Pistol
3. Medic - Smg, Heal-Darts
5. Recon - Sniper, Smg
