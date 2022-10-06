# MagicMayhem
#### A 3rd-Person multiplayer shooter game. This game is currently being built with Unreal Engine 5. A multiplayer steam sessions plugin can be exported to other projects that require steam sessions.
A Player is able to create a new steam session with HOST or join another steam session with JOIN.
![LoginScreen](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/LoginScreen_1.jpg)

There are 3 sections to the game mode, the first gets the players ready to start, the second is the actual death-match, and the third is the winner announcement and cooldown period. These three modes are illustrated below.
![GameStart Screenshot](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/Opening_1.jpg)
![Gameplay Screenshot](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/Gameplay_2.jpg)
![GameCooldown Screenshot](https://github.com/mpro34/MagicMayhem/blob/master/Screenshots/Closing_1.jpg)

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

## Story/Gameplay Goals:
- The goals for this game were to have a team based 3rd-person shooter, where gun are the mainstay, however magic also plays a role in gameplay.
- Every game you play is unique. You start the game by choosing an archetype (Lighting, Fire, Ice, Earth). When you pick up weapons, some weapons will enhance your archetype, others will not. Every couple/several rounds you get the option of upgrading your archetype or choosing a magic ability from your archetype.
The elements will be able to interact with the environment.

#### Examples:
- I chose the Fire archetype. 
I pick up a pistol, the rounds are able to set things on fire.
After several rounds, I choose the “Fireball” ability which lets me shoot a fireball as my Grenade.

- I chose the Lightning archetype.
I pick up a pistol, the rounds are able to stun/paralyze things.
After several rounds, I choose the “Lightning Storm” ability, which lets me set paralyzing traps around the map.

- I chose the Ice archetype.
I pick up a pistol, the rounds are able to freeze things in place.
After several rounds, I choose the “Ice Walls” and build paths around the map.

- I chose the Earth archetype.
I pick up a pistol, the rounds are able to blind opponents momentarily.
After several rounds, I choose the “Earthquake” ability which makes the screen shake, affecting the aim of opponents.

