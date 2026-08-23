# DevSandbox
A multiplayer gameplay sandbox focused on modular, data-driven systems, AI, networking and performance.
<br> Planning to make this into a evergrowing sandbox containing my game dev shenanigans.

## How to Run the Unreal Project Files
To get the Unreal project running on your system, first clone this github repo into a local folder using git or just download the zip by tapping the green code on top.
Then Download the Unreal 5.7 version from Epic games launcher as the game was made on that version, you could also use the latest version - but things may break during compiling. Then locate the zip and unzip it, you will find .uproject file, right click it -> more options -> 'generate Visual Studio project files' -> open up the .sln in Rider or Visual Studio Community and press Ctrl + Shift + B on load, Let it compile and launch it but tapping uproject or the play button - and you have the project files.

Live Link - https://deepdass.itch.io/devsandbox

## Project Structure
- See Source files here - [Source](Source/Projectidk), Every header/implementation files are in their own subfolders with flat split. (e.g. [Save Game Subsystem](Source/Projectidk/SaveSystem), [Action System](Source/Projectidk/ActionSystem))
- Pure/C++ inherited blueprint classes are inside [Core directory](Content/Core) inside content folder with their own subfolders
- Game Art Assets - [Content](Content)
- Configs Engine, Game specific including Gameplay Tags - [Configs](Config)
- Custom/ Open Source Plugins - [Plugins](Plugins)

<img width="1917" height="995" alt="Editor Screenshot" src="https://github.com/user-attachments/assets/4a5c66e9-fa27-4283-8867-231c70ec1aa1" />


## Why
I wanted to get better at writing modular, component-based architecture in C++ alongside Unreal replication and multiplayer, while keeping things performant and actually understand how these systems work together

## Controls
WASD - Move
<br> Shift - Sprint
<br> Space - Jump
<br> Left Click - Attack
<br> Q - Skill
<br> Right Click - Parry
<br> E - Interact

1 - Scalability Settings
<br> R - Restart level (Single Player)
<br> Tab - Save Game
<br> ESC - Quit

O/P - Shrink/Grow Object
<br> Scroll MB - Use Gravity Gun
<br> C - Throw Object when using Gravity Gun

### Debug Only
~ - All Console Command
<br> T - God Mode

Custom Console Command
<br> su.SpawnBots [0/1] - Bot Spawning via timer
<br> su.DamagedMultiplier [DamageMultiplierNumber] - Global Damage Multiplier
<br> su.DebugDrawInteraction [0/1]- Draws Interaction Debug

KillAll - Kill all bots
<br> DeleteSaveGame - Delete Previous Save File
<br> HealSelf [Amount] - Heal Self By Amount With Default Max Health
<br> DamageSelf [Amount] - Damage Self By Amount With Default Max Health
<br> GrantCoin [Amount] - Damage Self By Amount With Default 1000 Coins
<br> MoveInDirectionBy [Move By In Cm] [Axis] - Move yourself along axis in Cm With Default 200 Cm in Z axis

## Resources
- Framework based on - https://github.com/tomlooman/ActionRoguelike
For a modular and scalable code which is optimized to be performant.
- Unreal Docs and General forums
