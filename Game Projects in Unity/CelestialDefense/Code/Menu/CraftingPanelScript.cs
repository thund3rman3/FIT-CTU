using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

/*
 * Script for dealing with showing highlighting specific column/group of buildings in UI
 */
public class CraftingPanelScript : MonoBehaviour
{
    [SerializeField] GameObject costBar;
    [SerializeField] Text costWood;
    [SerializeField] Text costStone;
    [SerializeField] Text costIron;
    [SerializeField] BuildingCost buildingCost = null;

    [SerializeField] BuildingAndInventory playerBuildingScript;

    [SerializeField] GameObject[] groups;
    [SerializeField] Image[] icons;
    [SerializeField] RawImage[] selectionIcon;
    [SerializeField] Texture[] iconTextures;

    private int[] showCounter = new int[6];
    private int[] lastSelected = new int[6];

    private void Start()
    {
        lastSelected[0] = 1;
        lastSelected[1] = 4;
        lastSelected[2] = 7;
        lastSelected[3] = 10;
        lastSelected[4] = 15;
        lastSelected[5] = 18;

        showCounter[0] = 0;
        showCounter[1] = 0;
        showCounter[2] = 0;
        showCounter[3] = 0;
        showCounter[4] = 0;
        showCounter[5] = 0;

        selectItem(-1, 0);
    }

    public void hover(int groupID)
    {
        hideAll();
        if (groupID < 0 || groupID > 5)
        {   
            showGroups();
            return;
        }

        showCounter[groupID] = 1;
        showGroups();
    }

    public void unhover(int groupID) {
        if (groupID < 0 || groupID > 5) return;

        showCounter[groupID] = 0;

        showGroups();
    }

    private void showGroups()
    {
        for(int i = 0; i < 6; i++)
        {
            if (showCounter[i] > 0)
            {
                groups[i].SetActive(true);
            }
            else
                groups[i].SetActive(false);
        }
    }

    private void hideAll()
    {
        for (int i = 0; i < 6; i++)
        {
            showCounter[i] = 0;
            groups[i].SetActive(false);
        }
    }

    public void setCostBar(int itemID, int groupID)
    {
        if (itemID == -1)
        {
            itemID = lastSelected[groupID];
        }

        if (itemID == 0)
        {
            costBar.SetActive(false);
        }
        else
        {
            costBar.SetActive(true);
            BuildingBlueprint blueprint = getBlueprintFromID(itemID);
            costWood.text = blueprint.woodCost.ToString();
            costStone.text = blueprint.stoneCost.ToString();
            costIron.text = blueprint.ironCost.ToString();
        }
    }

    //Select in BuildingAndInventory what will be build and updates icons in UI
    public void selectItem(int itemID, int groupID)
    {

        if (HUD.instance != null && HUD.instance.isFreezed) return;

        if (groupID < 0 || groupID > 5 || itemID < -1 || itemID > 21)
        {
            for (int i = 0; i < 6; i++)
            {
                icons[i].enabled = false;
            }

            hideAll();
            playerBuildingScript.selectedBuildingType = 0;
            playerBuildingScript.isDeleting = true;
            HUD.instance.setRemoving(true);
            return;
        }

        if(itemID == -1)
        {
            //Use last used
            itemID = lastSelected[groupID];
        }

        HUD.instance?.setRemoving(false);
        playerBuildingScript.isDeleting = false;
        playerBuildingScript.selectedBuildingType = itemID;
        lastSelected[groupID] = itemID;

        hideAll();

        for(int i = 0; i < 6; i++)
        {
            if(i == groupID)
            {
                icons[i].enabled = true;
            }
            else
            {
                icons[i].enabled = false;
            }
        }

        switch (groupID)
        {
            case 0:
                {
                    selectionIcon[0].texture = iconTextures[itemID - 1 + 3];
                    break;
                }
            case 1:
                {
                    if(itemID == 21) selectionIcon[1].texture = iconTextures[14];
                    else selectionIcon[1].texture = iconTextures[itemID - 4 + 6];
                    break;
                }
            case 2:
                {
                    selectionIcon[2].texture = iconTextures[itemID - 7 + 0];
                    break;
                }
            case 3:
                {
                    if(itemID == 10)
                    {
                        selectionIcon[4].texture = iconTextures[9];
                        selectionIcon[3].texture = iconTextures[0];
                    }
                    else if (itemID == 11)
                    {
                        selectionIcon[4].texture = iconTextures[10];
                        selectionIcon[3].texture = iconTextures[1];
                    }
                    else if (itemID == 12)
                    {
                        selectionIcon[4].texture = iconTextures[11];
                        selectionIcon[3].texture = iconTextures[2];
                    }
                    else if (itemID == 13)
                    {
                        selectionIcon[4].texture = iconTextures[12];
                        selectionIcon[3].texture = iconTextures[1];
                    }
                    else if (itemID == 14)
                    {
                        selectionIcon[4].texture = iconTextures[13];
                        selectionIcon[3].texture = iconTextures[2];
                    }
                    break;
                }
            case 4:
                {
                    selectionIcon[5].texture = iconTextures[itemID - 15 + 0];
                    break;
                }
            case 5:
                {
                    selectionIcon[6].texture = iconTextures[itemID - 18 + 0];
                    break;
                }
        }
    }

    private void Update()
    {
        if (Input.GetKeyDown(KeyCode.Alpha1))
            selectItem(-1, 0);
        if (Input.GetKeyDown(KeyCode.Alpha2))
            selectItem(-1, 1);
        if (Input.GetKeyDown(KeyCode.Alpha3))
            selectItem(-1, 2);
        if (Input.GetKeyDown(KeyCode.Alpha4))
            selectItem(-1, 3);
        if (Input.GetKeyDown(KeyCode.Alpha5))
            selectItem(-1, 4);
        if (Input.GetKeyDown(KeyCode.Alpha6))
            selectItem(-1, 5);
    }

    private void LateUpdate()
    {
        if (EventSystem.current.IsPointerOverGameObject() == false)
        {
            hideAll();
        }
    }


    BuildingBlueprint getBlueprintFromID(int id)
    {
        switch (id)
        {
            case 1:
                return buildingCost.woodenWall;
            case 2:
                return buildingCost.stoneWall;
            case 3:
                return buildingCost.ironWall;
            case 4:
                return buildingCost.hole;
            case 5:
                return buildingCost.trap;
            case 6:
                return buildingCost.autoMiner;
            case 7:
                return buildingCost.canonLVL1;
            case 8:
                return buildingCost.canonLVL2;
            case 9:
                return buildingCost.canonLVL3;
            case 10:
                return buildingCost.wizardTower;
            case 11:
                return buildingCost.iceWizardTower;
            case 12:
                return buildingCost.frostWizardTower;
            case 13:
                return buildingCost.fireWizardTower;
            case 14:
                return buildingCost.MagmaWizardTower;
            case 15:
                return buildingCost.crossbowLVL1;
            case 16:
                return buildingCost.crossbowLVL2;
            case 17:
                return buildingCost.crossbowLVL3;
            case 18:
                return buildingCost.catapultLVL1;
            case 19:
                return buildingCost.catapultLVL2;
            case 20:
                return buildingCost.catapultLVL3;
            case 21:
                return buildingCost.gate;
            default:
                return null;
        }

    }
}
