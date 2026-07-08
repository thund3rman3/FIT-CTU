stručný popis hry a seznam aspektů zadání.

# Game design
[Game design document](https://docs.google.com/document/d/1Y1kwq6UAZr7weqAP0IizQ_OhTe-I0TWTa9nhO2a8dlU/edit?tab=t.0#heading=h.p0xmrnqblcev)

**It is a 2D platform puzzle co-operation game**

## Design choices
- Obstacles and logical puzzles which require 2 players to solve
- Body swap mechanic

## Story
Many years ago, the beaver colony of the White Oak forest built a massive state-of-the-art dam. As years pass by, the dam deteriorates and threatens the very existence of this colony. Repairs need to be made, but the new generation of beavers has grown lazy - they just keep staring at their phones!

You and another player will take on the roles of the beavers of this colony. On your quest to repair the dam you will have to build a patch of wood and haul it to the top of the dam.

## Mechanics

- Body swap - The players are able to switch bodies with another inactive bober NPC in the level. They can’t switch bodies directly with their co-op partner.
    - This mechanic is realised by shooting out a “soul” from the player that can bounce on horizontal, vertical and diagonal surfaces until it either dissipates (after some time), hits a surface from which it can’t bounce (e.g. spikes) or reaches an inactive bober. At this point the original bober the player was controlling becomes inactive and the player takes control of the hit bober. Movement of the “soul” can’t be controlled by the player outside of the direction in which it shoots out from the player controlled bober.

- Interactables
    - Bomb crates - gives you one bomb if there is no other bomb spawned
    - Bomb - can be thrown and after that explodes so it destroys cracked environment blocks (destructibles)
    - Lever / (Partner) Pressure plate - can de/activate certain actions e.g. de/activate platforms, toggle moving platforms


## Chosen cards affecting design
- (20) **Developer duo**
- (5)  **Commonly used game engine** - Unity 6000.2.8f1
- (15) **Layered 2D graphics** - background parallax
- (5) **Sound samples** - soundtrack, walking, interactables
- (5)  **Saving/loading** - last played level can be saved
- (10) **Rigid body mechanics** - moving player while on platform
- (5)  **Frame-by-frame animation** - used mainly for the bobers, moving platform background chains, bomb explosion
- (10) **Multiplayer (over the network) with low latency** - co-op, Unity Netcode
- (15) **Destructible environment** - used in the second level (White Oak forest)
- (5)  **Recoloring** - used for the various colors of buttons/levers as well as for the player soul
- (15) **Procedual generation** - used for background

total: 110