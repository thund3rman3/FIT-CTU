using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Autominer : MonoBehaviour
{
    //How much to mine per gather
    [SerializeField] private int wood = 10;
    [SerializeField] private int stone = 10;
    [SerializeField] private int iron = 10;
    [SerializeField] private float timeBetweenHarvest = 10;
    public BuildingAndInventory inventory;
    
    //When next harvest will occure
    private float nextHarvest = 0;

    void Start()
    {
        nextHarvest = Time.time + timeBetweenHarvest + Random.Range(-timeBetweenHarvest, timeBetweenHarvest) / 2f;

        int index = (int)(transform.position.x + 0.1f) + (int)(transform.position.z + 0.1f) * World.chunkXCount * ChunkData.chunkXSize;
        Resource resource = WorldMap.tilesObjects[index].GetComponent<Resource>();

        if (resource.woodPerGather <= 0) wood = 0;
        if (resource.stonePerGather <= 0) stone = 0;
        if (resource.ironPerGather <= 0) iron = 0;

        StartCoroutine("AudioDelay");
    }

    IEnumerator AudioDelay()
    {
        GetComponent<AudioSource>().Stop();
        yield return new WaitForSeconds(Random.Range(0f, 0.3f));
        GetComponent<AudioSource>().Play();
    }

    void Update()
    {
        if (nextHarvest <= Time.time)
        {
            if (inventory == null) inventory = BuildingAndInventory.instance;
            float multiplier = MainMenuControl.gatheringMultiplier;
            inventory.AddResources((int)(wood * multiplier), (int)(stone * multiplier), (int)(iron * multiplier));
            nextHarvest = Time.time + timeBetweenHarvest;
        }
    }
}
