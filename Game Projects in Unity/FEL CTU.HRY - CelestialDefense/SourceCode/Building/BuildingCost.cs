using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/**
* Those classes holds info about what and for how much can be build
*/
[System.Serializable]
public class BuildingBlueprint
{
    public int ID;
    public int woodCost;
    public int stoneCost;
    public int ironCost;
    public GameObject prefab;
};

[CreateAssetMenu(order = 1)]
public class BuildingCost : ScriptableObject
{
    /*
    * 1  - Wooden wall
    * 2  - Stone wall
    * 3  - Iron wall
    * 4  - Hole
    * 5  - Trap
    * 6  - Auto-miner
    * 7  - Cannon lvl 1
    * 8  - Cannon lvl 2
    * 9  - Cannon lvl 3
    * 10 - Wizard tower
    * 11 - Ice Wizard tower
    * 12 - Frost Wizard tower
    * 13 - Fire Wizard tower
    * 14 - Magma Wizard tower
    * 15 - Crossbow lvl 1
    * 16 - Crossbow lvl 2
    * 17 - Crossbow lvl 3
    * 18 - Catapult lvl 1
    * 18 - Catapult lvl 2
    * 20 - Catapult lvl 3
    */
    public BuildingBlueprint woodenWall;
    public BuildingBlueprint stoneWall;
    public BuildingBlueprint ironWall;
    public BuildingBlueprint hole;
    public BuildingBlueprint trap;
    public BuildingBlueprint autoMiner;
    public BuildingBlueprint canonLVL1;
    public BuildingBlueprint canonLVL2;
    public BuildingBlueprint canonLVL3;
    public BuildingBlueprint wizardTower;
    public BuildingBlueprint iceWizardTower;
    public BuildingBlueprint frostWizardTower;
    public BuildingBlueprint fireWizardTower;
    public BuildingBlueprint MagmaWizardTower;
    public BuildingBlueprint crossbowLVL1;
    public BuildingBlueprint crossbowLVL2;
    public BuildingBlueprint crossbowLVL3;
    public BuildingBlueprint catapultLVL1;
    public BuildingBlueprint catapultLVL2;
    public BuildingBlueprint catapultLVL3;
    public BuildingBlueprint gate;
}
