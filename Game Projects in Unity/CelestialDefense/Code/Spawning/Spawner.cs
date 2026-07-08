using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public class EnemyType
{
    [SerializeField]
    public GameObject prefab;
    [SerializeField]
    public float rarity = 0.2f;
}

/**
 * Class of structure Spawner
 * It is spawning enemies of specified type based on 
 * wave number, given budget of points and enemies specified
 */
public class Spawner : MonoBehaviour
{
    [Tooltip("What enemies will be spawned here")]
    public List<EnemyType> enemyTypes = new List<EnemyType>();

    [Tooltip("How fast enemies are spawning")]
    public float spawnsPerSecond = 1f;

    public Vector2Int spawnerSize = new Vector2Int(2, 2);

    private List<GameObject> enemyToSpawn = new List<GameObject>();
    private int spawnPositionIndex = 0;

    public void Start()
    {
        spawnsPerSecond -= Random.Range(0f, 0.3f);
    }

    //Calculas how many enemies and of which type to spawn
    public void SpawnWave(int budget, int wave)
    {
        int maxBudget = budget;  

        float max = SpawningScheduler.instance.maxWaveScaling;
        float K = Mathf.Clamp((max/2f - wave) / (max/2f), 0f, 1f);
        float C = 1 - ((K + 1) / 2f);

        //Calculate what will be spawned
        float accumulatedProbability = 0f;
        for(int i = 0; i < enemyTypes.Count; i++)
        {
            EnemyType enemy = enemyTypes[i];
            float probabilty = C + K * enemy.rarity;
            accumulatedProbability += probabilty;
        }

        GameObject lowestPrefab = null;
        int lowestCost = int.MaxValue;
        for (int i = 0; i < enemyTypes.Count; i++)
        {
            EnemyType enemy = enemyTypes[i];
            float probabilty = C + K * enemy.rarity;

            probabilty = probabilty / accumulatedProbability;

            int enemyBudget = (int)(maxBudget * probabilty);
            int cost = enemy.prefab.GetComponent<AI_Enemy>().budgetCost;
            if(lowestCost > cost)
            {
                lowestCost = cost;
                lowestPrefab = enemy.prefab;
            }

            while (enemyBudget >= cost)
            {
                enemyBudget -= cost;
                budget -= cost;
                enemyToSpawn.Add(enemy.prefab);
            }
        }

        //With rest of the budget spawn the lowest cost
        while(budget >= lowestCost && lowestPrefab != null)
        {
            enemyToSpawn.Add(lowestPrefab);
            budget -= lowestCost;
        }

        //Start spawning
        InvokeRepeating("Spawn", 2f, 1f / spawnsPerSecond);
    }

    //One by one is spawing enemies until all of them are spawned
    private void Spawn()
    {
        if(enemyToSpawn.Count == 0)
        {
            CancelInvoke();
            return;
        }
        Vector3 randomOffset = new Vector3(spawnPositionIndex % (int)(0.1f + spawnerSize.x), 0, spawnPositionIndex / (int)(0.1f + spawnerSize.x));

        Vector3 spawnPosition = randomOffset + transform.position + new Vector3(0.5f, 1, 0.5f);

        int layer_mask = LayerMask.GetMask("EnemyInner");
        if (Physics.CheckSphere(spawnPosition, 0.5f, layer_mask)) return;

        int i = Random.Range(0, enemyToSpawn.Count);
        GameObject prefab = enemyToSpawn[i];
        enemyToSpawn.RemoveAt(i);

        GameObject obj = Instantiate(prefab, randomOffset + transform.position + new Vector3(0.5f, 0f, 0.5f), new Quaternion());
      
        spawnPositionIndex = (spawnPositionIndex + 1) % ((int)(spawnerSize.x + 0.1f) * (int)(spawnerSize.y + 0.1f));      
    }

}
