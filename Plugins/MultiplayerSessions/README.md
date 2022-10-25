## Multiplayer Sessions Plugin

### Background
The Multiplayer Sessions plugin is a UE plugin that utilizes the UE Online Subsystem and the UE Online Subsystem Steam to create a new Multiplayer Sessions Subsystem. The goal for this plugin is to be able to add to any multiplayer project and it includes the following (once the plugin is installed):
1. A simple "Host" and "Join" UI menu with buttons.
2. Handles all operations related to steam sessions.


### How it Works
The plugin starts in the Menu class where once the user is able to Host a game or Join an existing game. On the Map where the menu will be displayed, you must call `UMenu::MenuSetup()` in order to setup the menu callbacks.

##### Example #1: "Host Button Clicked"
- `UMenu::HostButtonClicked()`: Calls the multiplayer sessions subsystem function to create a session.
- `UMultiplayerSessionsSubsystem::CreateSession()`: Calls the Online Subsystems' interface create session function.
- `SessionInterface->CreateSession(PlayerID, SessionName, FOnlineSessionSettings())`: Creates a session and broadcasts to delegates.
- `MultiplayerOnCreateSessionComplete.Broadcast` -> `UMenu::OnCreateSession()`: The callback in the UMenu class will then have access to the created session. 
- `GetWorld()->ServerTravel(m_PathToLobby)`: Once the session is created, teleport the player to the specified lobby map to wait for more players.

##### Example #2: "Join Button Clicked"
- `UMenu::JoinButtonClicked()`: Calls the multiplayer sessions subsystem function to find all available sessions.
- `UMultiplayerSessionsSubsystem::FindSessions()`: Calls the Online Subsystems' interface to find all sessions that meet the input criteria.
- `SessionInterface->FindSessions(PlayerID, FOnlineSessionSearch())`: Finds sessions and broadcasts the search results to delegates.
- `MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>())`: Broadcasts all the found sessions to `UMenu`.
- `UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults)`: Iterates over found sessions until a 'FreeForAll' matchtype session is found.
- `MultiplayerSessionsSubsystem::JoinSession(result)`: Attempts to join the session.
- `SessionInterface->JoinSession(PlayerID, SessionName, FOnlineSessionSearchResult())`: Joins a session and broadcasts to delegates.
- `MultiplayerOnJoinSessionComplete.Broadcast` ->  `UMenu::OnJoinSession()`: The callback in the UMenu class will then have access to the created session.
- `PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute)`: Once a created session is found, teleport the player to the specified lobby map.
