# LAB-73-Website-and-Server

Requirements: HttpBlueprint Plugin and WesocketBP plugin: https://www.fab.com/listings/d826ba07-7567-42ad-b19f-0c3e22e8ea0a

Edit Build.cs under lab111\Source\lab111

PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "WebSockets" });

The .cpp and .h is in a cpp GameInstance

Set new Game Instance

Connect to server Websocket through ws://127.0.0.1:3000/ws
