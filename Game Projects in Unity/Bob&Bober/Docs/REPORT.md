# Implementation of Task cards

- (5)  **Commonly used game engine** 
    - Unity 6000.2.8f1
    - Font: https://assetstore.unity.com/packages/2d/fonts/boldpixels-332078

- (15) **Layered 2D graphics** - background parallax
    - All of the games assets are original apart from the beaver idle model.
    - Parallax is implemented using a component (CParallax) that makes objects move with the camera. In Unity editor we are able to set a multipler that determines how much the object moves.
    - [Beaver model source](https://www.shutterstock.com/cs/image-vector/little-beaver-sitting-pixel-art-character-1461381407)

- (5) **Sound samples** 
    - Clicking sounds are handled by a script on each button. The bomb plays a sound when it explodes by searching for an audio source in the scene and triggering the sound at the start of the explosion animation. Levers, pressure plates, and scenes each have their own audio source. Scenes play a soundtrack, while the other objects play a clicking sound when their state changes.

    - [AudioSource Docs](https://docs.unity.com/en-us/unity-studio/develop/gameobjects/components/audio-source)

- (5)  **Saving/loading**
    - If the player opens the PauseMenu and clicks Disconnect or Quit, the button, in addition to its own functionality, calls the static class SaveLoadManager and its SaveGame method, which saves the last active scene and the session name in binary form to the application’s storage. If the player then selects Load after starting a new game, the game loads at the beginning of that level and the session will have the same name. If the player has not played any level yet, the option is disabled and the player must create a new session.
    - [Save and Load system video](https://www.youtube.com/watch?v=XOjd_qU2Ido)

- (10) **Rigid body mechanics** - moving player while on platform
    - Bobers and bombs both utilize rigid body mechanics on their movement.
    - This allows the bombs to be thrown around and for them to roll on the floor and to create smooth movement for the bobers.
    - Both bobers and bombs also move with moving platforms. If a platform moves horizontally, the platforms movement is passed to touching rigid bodies as forces that are applied to them. This approach doesn't work well for vertically moving platforms though (the movement is very jittery). We tried to fix this by parenting the rigid body objects to the moving platform when they touch it, but there were problems with this approach regarding the network setup of the objects. The solution we landed on is to simply set the Y position of the touching rigid body according to the movement of the platform. The movement is still very jittery but this way, the player is at least able to jump off the platform.

- (5)  **Frame-by-frame animation** - used mainly for the bobers, moving platform background chains, bomb explosion
    - Some objects in the game (bober, waterfall, moving platform path, bomb) utilize fram-by-frame animation using the CLoopAnimation component. This component takes sprites (to be looped) and a timer as input. If the component is set to animate, it changes the sprites with the set frequency.

- (10) **Multiplayer (over the network) with low latency** 
    - Multiplayer made with:  [Unity Netcode for GameObjects 2.7.0](https://docs.unity3d.com/Packages/com.unity.netcode.gameobjects@2.7/manual/index.html)
    - Other helpful multiplayer packages: Multiplayer Center 1.0.0, Widgets 1.0.5, Play Mode 1.6.3, Services 1.2.1
    - After selecting the correct package, I went through the configurator in Unity and chose a suitable network topology, i.e. **Distributed Authority**, where players exchange game information with each other without an external server authority. However, after a long effort to understand this concept and after many errors during spawning, ownership changes, and basically everything else, I switched to the other network architecture – **Client-Server**.

    - This topology allowed me to fix the multiplayer by giving the **Server** authority over objects, so an RPC message is sent to the Server, and the Server then sends the changed information to all clients. Changing the state of a visible object on the network fundamentally requires a **NetworkVariable**, thanks to which everyone knows the current state of the object. Objects that handle ownership within the network must inherit from a different class – **NetworkBehaviour** – and must have a **NetworkObject** component. Objects that need to synchronize transforms over the network must have a **NetworkTransform** component, and finally, objects that we want to spawn on the network must be listed among the **NetworkPrefabs** on the **NetworkManager**. I still encounter issues where the Server does not know something or the permissions for changing information are set incorrectly, and it is difficult to balance this properly.

    - As for extrapolating the movement of nearby players, we synchronize over the network the entire structure containing information about velocity, position, time, and the player’s rotation direction. Then, based on a formula, the target extrapolated position is computed using
**position + [velocity * (serverTime - bober.Timestamp)]**.
Because our Bober can jump, a gravity adjustment was necessary, so if it is jumping, an additional position offset of **1/2 * g * t²** is added. Then it is necessary to take into account that Bober could fall through the floor or pass through the ceiling, so we check in advance whether the target Y position is below the ground and reset it back to the ground. What follows is teleporting if the received position arrived late, **LERP** if it is somewhat farther away. For smaller distances, we send a **BoxCast** to avoid getting stuck on surrounding objects from any side, and then apply **LERP**.
    - [unity-client-side-prediction](https://codersblock.org/posts/unity-client-side-prediction/)

- (15) **Destructible environment** - used in the second level (White Oak forest)
    - One of the main puzzle mechanics in the game are bombs and destructible blocks.
    - The players can interact with a bomb crate, that can spawn 1 bomb at a time. It can be picked up and thrown, which starts an explosion timer (during this time it can be picked up again and thrown again until it explodes). If the bomb explodes near a destructible block, the block with fade away and reveal an area to the players.
    - The explosion of the bomb is realised using a trigger that briefly activates when the bomb explodes. Destructible blocks and bobers detect this trigger and act accordingly. The destructible object script (CDestructible) takes 2 lists as input - reveals & hides lists. The reveals list contains all of the objects that fade away when the destructible object is broken. The hides list contains objects that fade in when the object is destroyed. The only purpose for this list is to fix overlaying shadows in the scene.

- (5)  **Recoloring** - used for the various colors of buttons/levers as well as for the player soul
    - The player is able to recolor the arrow above their head and their soul in the ESC pause menu using a color picker.
    - The colors of levers and pressure plates can be changed in the Unity editor via the Unity color picker windows. These changes apply to the objects when the game is running.
    - [In-game color picker video](https://www.youtube.com/watch?v=otDHGmncBQY)

- (15) **Procedual generation** - used for background
    - Backgrounds for all of the scenes apart from Level 2 (for now) are all generated using wave function collapse (WFC). The backgrounds consist of 9 tiles, each of which has a defined set of possible neighbors on all 4 sides of them. The current implementation is a modification of BFS. This wouldn't work well with stricter neighbor conditions as there would be situations in which there would be 0 possible choices for a tile. However, the results look good and we didn't feel the need to implement stricter conditions. No 2 vertical or horizontal starts or ends of logs can be aligned and a log cross section can't neighbor itself (although there are 2 versions of cross sections that can neighbor with each other).
    - If there was a need to use strictier neighbor conditions, the structure of the code could be changed to DFS, which would back-track and try different tiles if it reached a tile with 0 possible choices.

## Implemetation — Others

### Connection
The ConnectionManager is responsible for connecting to Unity Multiplayer Services, signing the player in, creating a session, and joining a session based on how the InputField was filled in or what was clicked. The UI-related logic around this is handled by the UIManager. It also works in such a way that if a second player decides to create a session with the same name, they will join the one created by the first player, so that they do not end up competing for the same session. However, if a third player attempts to create a session with the same name, an error may occur.

I have done join menu by reimplementing Widget from [multiplayer widgets](https://docs.unity3d.com/Packages/com.unity.multiplayer.widgets@1.0/manual/get-started.html)