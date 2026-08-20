# DevSandbox
A multiplayer gameplay sandbox focused on modular, data-driven systems, AI, networking and performance.
<br> Planning to make this into a evergrowing sandbox containing my game dev shenanigans.

<img width="1917" height="995" alt="Editor Screenshot" src="https://github.com/user-attachments/assets/4a5c66e9-fa27-4283-8867-231c70ec1aa1" />


## Why
I wanted to get better at writing modular, component-based architecture in C++ alongside Unreal replication and multiplayer, while keeping things performant and actually understand how these systems work together

## Controls
WASD - Move
<br> Shift - Sprint
<br> Space - Jump
<br> Left Click - Fire
<br> Right Click - Parry
<br> E - Interact
<br> ESC - Quit
<br> R - Reset Level (Single Player)
<br> 1 - Scalability Settings

### Debug Only
~ - All Console Command
<br> T - God Mode

Custom Console Command
<br> su.SpawnBots [0/1] - Bot Spawning via timer
<br> su.DamagedMultiplier [DamageMultiplierNumber] - Global Damage Multiplier
<br> su.DebugDrawInteraction [0/1]- Draws Interaction Debug 

<br> KillAll - Kill all bots
<br> DeleteSaveGame - Delete Previous Save File
<br> HealSelf [Amount] - Heal Self By Amount With Default Max Health
<br> DamageSelf [Amount] - Damage Self By Amount With Default Max Health
<br> GrantCoin [Amount] - Damage Self By Amount With Default 1000 Coins
<br> MoveInDirectionBy [Move By In Cm] [Move In Axis] - Move yourself along axis in Cm With Default 200 Cm in Z axis

## Resources
Framework based on - https://github.com/tomlooman/ActionRoguelike
For a modular and scalable code which is optimized to be performant.
