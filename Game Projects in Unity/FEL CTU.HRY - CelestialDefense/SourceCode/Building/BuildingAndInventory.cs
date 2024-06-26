using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

/*
 * Udržuje inventář hráče -> suroviny, budovy
 * Umožňuje stavět budovy
 */
public class BuildingAndInventory : MonoBehaviour
{
    public static BuildingAndInventory instance;

    public int[] buildingCount = new int[22];

    [SerializeField] HUD playerHUD;
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
	 * 21 - Gate
	 */
    public BuildingCost buildingCost = null;

    [Tooltip("What type of bulding is seleted to be build. 0 means nothing is selected")]
    public int selectedBuildingType = 1;
    public bool isDeleting = false;

    [Header("Resources")]
    public int wood = 10000;
    public int stone = 10000;
    public int iron = 10000;

    [Tooltip("Maximal distance for building")]
    public float maxBuildDistance = 20f;

    [SerializeField, Tooltip("Material for highlighter when I cannot place there")]
    private Material validMaterial;
    [SerializeField, Tooltip("Material for highlighter when I cannot place there")]
    private Material invalidMaterial;

    //reference to object that highlights the position where buid/destroy action will take place
    [SerializeField]
    private GameObject highlighter; 
    //Camera for raycasting
	private Camera playerCamera;

    //Add resources and updates them on UI
    public void AddResources(int wood, int stone, int iron)
    {
        this.wood += wood;
        this.stone += stone;
        this.iron += iron;

        //Update UI
        playerHUD.setWood(this.wood);
        playerHUD.setStone(this.stone);
        playerHUD.setIron(this.iron);
        playerHUD.updateBuildingCount();
    }

	private void Start()
	{
        instance = this;
        playerCamera = transform.root.transform.GetChild(0).GetComponent<Camera>();

        for (int i = 0; i < 22; i++)
        {
            buildingCount[i] = 0;
        }

        //If level is loading then get resources count
        if (MainMenuControl.gameShouldBeLoaded)
        {
            wood = PlayerSaveData.instance.wood;
            stone = PlayerSaveData.instance.stone;
            iron = PlayerSaveData.instance.iron;

            for(int i = 0; i < 22; i++)
            {
                buildingCount[i] = PlayerSaveData.instance.buildingsInInventory[i];
            }
        }

        playerHUD.setWood(wood);
        playerHUD.setStone(stone);
        playerHUD.setIron(iron);

        playerHUD.updateBuildingCount();
    }

	void Update()
	{
        if (HUD.instance.isFreezed) return;

        //
        // Deleting mode
        //
        if(isDeleting)
        {
            highlighter.GetComponentInChildren<MeshRenderer>().enabled = false;

            //Raycast to find position of the highlighter
            Ray r = playerCamera.ScreenPointToRay(Input.mousePosition);
            int l = LayerMask.GetMask("MapCollision");
            RaycastHit h;

            if (Physics.Raycast(r, out h, 1000, l))
            {
                float distance = (transform.position - h.point).magnitude;
                Vector3 hl = h.point + h.normal * 0.1f;
                highlighter.transform.position = new Vector3((int)(hl.x), (int)(hl.y), (int)(hl.z));
                highlighter.GetComponentInChildren<MeshRenderer>().enabled = true;

                //Do I have enought resources and good distance
                if (distance <= maxBuildDistance)
                {
                    highlighter.GetComponentInChildren<MeshRenderer>().material = validMaterial;

                    if (Input.GetKeyDown(KeyCode.Mouse0))
                    {
                        JobTile tile = WorldMap.tiles[(int)(hl.x) + (int)(hl.z) * ChunkData.chunkXSize * World.chunkXCount];
                        GameObject obj = WorldMap.tilesObjects[(int)(hl.x) + (int)(hl.z) * ChunkData.chunkXSize * World.chunkXCount];
                        if (obj && tile.containStructure && obj.tag != "Spawner" && obj.tag != "Meteor" && obj.tag != "Resource")
                        {
                            int ID = obj.GetComponent<Structure>().buildingTypeID;
                            if (4 != ID && 5 != ID) //Do not store traps and holes
                            {
                                buildingCount[ID]++;
                            }
                            Destroy(obj);

                            playerHUD.updateBuildingCount();
                        }
                    }
                }
                else
                {
                    highlighter.GetComponentInChildren<MeshRenderer>().material = invalidMaterial;
                }
            }

            return;
        }

        //Reset selectedBuildingType if it is invalid
        if (selectedBuildingType < 0 || selectedBuildingType > 21) selectedBuildingType = 0;

        //Move highlighter
		highlighter.GetComponentInChildren<MeshRenderer>().enabled = true;

		Ray ray = playerCamera.ScreenPointToRay(Input.mousePosition);
		int layer_mask = LayerMask.GetMask("MapCollision");
		RaycastHit hit;

        if (Physics.Raycast(ray, out hit, 1000, layer_mask)) {

            highlighter.GetComponentInChildren<MeshRenderer>().material = invalidMaterial;

            Vector3 hitLocation = hit.point + hit.normal * 0.1f;
            highlighter.transform.position = new Vector3((int)(hitLocation.x), (int)(hitLocation.y), (int)(hitLocation.z));

            BuildingBlueprint blueprint = getBlueprintFromID(selectedBuildingType);

            Debug.Log("1 " + Time.time);

            if (blueprint == null) return;

            float distance = (transform.position - hit.point).magnitude;

            //Do I have enought resources and good distance
            if (distance > maxBuildDistance) return;
            
            int index = (int)(hitLocation.x) + (int)(hitLocation.z) * ChunkData.chunkXSize * World.chunkXCount;
            JobTile tile = WorldMap.tiles[index];

            if (tile.blocked == true && selectedBuildingType != 6) return;

            Debug.Log("2 " + Time.time);

            Structure str = null;
            if (tile.containStructure == true && selectedBuildingType != 6 && selectedBuildingType != 21)
            {
                //Check wheter we can replace
                str = WorldMap.tilesObjects[(int)(hitLocation.x) + (int)(hitLocation.z) * ChunkData.chunkXSize * World.chunkXCount]?.GetComponent<Structure>();
                if (str == null) return;

                int existID = str.buildingTypeID;

                if ( (selectedBuildingType == 2 && existID == 1) ||
                     (selectedBuildingType == 3 && existID == 1) ||
                     (selectedBuildingType == 3 && existID == 2) || 

                     (selectedBuildingType == 8 && existID == 7) ||
                     (selectedBuildingType == 9 && existID == 7) ||
                     (selectedBuildingType == 9 && existID == 8) ||

                     (selectedBuildingType == 11 && existID == 10) ||
                     (selectedBuildingType == 12 && existID == 10) ||
                     (selectedBuildingType == 12 && existID == 11) ||
                     (selectedBuildingType == 13 && existID == 10) ||
                     (selectedBuildingType == 14 && existID == 10) ||
                     (selectedBuildingType == 14 && existID == 13) ||

                     (selectedBuildingType == 16 && existID == 15) ||
                     (selectedBuildingType == 17 && existID == 15) ||
                     (selectedBuildingType == 17 && existID == 16) ||

                     (selectedBuildingType == 19 && existID == 18) ||
                     (selectedBuildingType == 20 && existID == 18) ||
                     (selectedBuildingType == 20 && existID == 19))
                {
                    //Can pass
                }
                else
                {
                    return;
                }

            }

            Debug.Log("OVERLAP COUNT: " + highlighter.GetComponentInChildren<OverlapCounter>().counter);

            if (highlighter.GetComponentInChildren<OverlapCounter>().counter > 0 && selectedBuildingType != 6 && selectedBuildingType != 21) return;

            JobTile hitTile = WorldMap.tiles[(int)(hitLocation.x) + (int)(hitLocation.z) * ChunkData.chunkXSize * World.chunkXCount];
            if (hitTile.height + 1 != (int)(hitLocation.y)) return;

            layer_mask = LayerMask.GetMask("Player", "Enemy", "Spawner");
            if (Physics.CheckBox(highlighter.transform.position + new Vector3(0.5f, 1.5f, 0.5f), new Vector3(0.4f, 0.4f, 0.4f), new Quaternion(), layer_mask) && selectedBuildingType != 21) return;


            Debug.Log("3 " + Time.time);

            //
            // Check resources
            //
            int Wcost = -blueprint.woodCost;
            int Scost = -blueprint.stoneCost;
            int Icost = -blueprint.ironCost;

            if(isDeleting == false)
            {
                if (str != null)
                {
                    BuildingBlueprint b = getBlueprintFromID(str.buildingTypeID);
                    Wcost += b.woodCost;
                    Scost += b.stoneCost;
                    Icost += b.ironCost;
                }

                if (-Wcost > wood || -Scost > stone || -Icost > iron) return;
            }

            //Exceptions for gates and autominer
            if (selectedBuildingType == 6 && 
                WorldMap.tilesObjects[index] != null && 
                WorldMap.tilesObjects[index].GetComponent<Resource>() == null) return;
            if (selectedBuildingType == 6 &&
                WorldMap.tilesObjects[index] == null) return;
            if (selectedBuildingType == 21 &&
                WorldMap.tilesObjects[index] == null) return;
            if (selectedBuildingType == 21 &&
                WorldMap.tilesObjects[index] &&
                WorldMap.tilesObjects[index].GetComponent<Structure>() &&
                WorldMap.tilesObjects[index].GetComponent<Structure>().noCollision)
                return;
            if (selectedBuildingType == 21)
            {
                Structure s = WorldMap.tilesObjects[index].GetComponent<Structure>();
                if (s == null) return;
                if (s.buildingTypeID != 1 && s.buildingTypeID != 2 && s.buildingTypeID != 3) return;
            }

            //Change material of highlighter
            highlighter.GetComponentInChildren<MeshRenderer>().material = validMaterial;


            Debug.Log("4 " + Time.time);

            if (isDeleting == false && Input.GetKey(KeyCode.Mouse0) && EventSystem.current.IsPointerOverGameObject() == false)
            {
                //Spawn intended prefab
                wood += Wcost;
                stone += Scost;
                iron += Icost;

                Debug.Log("5 " + Time.time);

                if (str == null)
                {
                    //Is not overriding
                    if(buildingCount[blueprint.ID] > 0 )
                    {
                        buildingCount[blueprint.ID]--;
                        wood -= Wcost;
                        stone -= Scost;
                        iron -= Icost;

                        playerHUD.updateBuildingCount();
                    }
                }
                
                if (selectedBuildingType == 21 )
                {
                    WorldMap.tilesObjects[index].GetComponent<Structure>().SetNoCollision();
                    playerHUD.setWood(wood);
                    playerHUD.setStone(stone);
                    playerHUD.setIron(iron);

                    return;
                }


                //Place building
                if (str != null)
                    Destroy(str.gameObject);
               
                Vector3 position = highlighter.transform.position;
                GameObject newObj = Instantiate(blueprint.prefab, position, new Quaternion());

                if (selectedBuildingType == 6)
                    newObj.GetComponent<Autominer>().inventory = this;

                AudioManagerScript.PlaySpatial("StructureBuild", position);

                //Update UI
                playerHUD.setWood(wood);
                playerHUD.setStone(stone);
                playerHUD.setIron(iron);
                playerHUD.updateBuildingCount();
            }
		}
		else
		{
			highlighter.GetComponentInChildren<MeshRenderer>().enabled = false;
		}
       
	}

    //Get blueprint with cost of the structure 
	public BuildingBlueprint getBlueprintFromID(int id)
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
