using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/**
 * Class that control how much and on which spawner does enemies spawn.
 * It distributes budget of spawn points to Spawners which decide what 
 * enemies will be instanciated
 * It also contail all AI_enemies and update them.
 */
public class SpawningScheduler : MonoBehaviour
{
	public static SpawningScheduler instance;

    public GameObject[] enemiesPrefabs;

	public void Start()
	{
		instance = this;

        if (MainMenuControl.gameShouldBeLoaded)
        {
            SpawningData data = SpawningData.instance;

            waveCounter = data.waveCounter;

            if(data.waveStart == 0)
            {
                waveStart = data.waveStart;

                //Spawn enemies
                for (int i = 0; i < data.enemyCounter; i++)
                {
                    EnemyData d = data.enemies[i];
                    GameObject obj = Instantiate(
                        enemiesPrefabs[d.enemyType - 1],
                        new Vector3(d.position[0], d.position[1], d.position[2]),
                        new Quaternion());

                    AI_Enemy enemy = obj.GetComponent<AI_Enemy>();
                    enemy.health = d.health;
                }
            }
            else
            {
                waveStart = Time.time + data.waveStart;
            }
        }
    }

    [Tooltip("Number of wave we are currantly in")]
    public int waveCounter = 0;

    [Range(0, 100), Tooltip("When the distribution of enemies will stop changing")]
	public int maxWaveScaling = 20;

	[Range(0, 10), Tooltip("How much are distant spawners prefered")]
	public float distanceExponencial = 1.2f;

	[Tooltip("How many enemies are actually on the map")]
	public int enemyCounter = 0;

	//When new wave should start
	public static float waveStart = 0;

	[Header("Wave budeget function  A*wave^2 + B * wave + C + e^(1.2*sqrt(x))")]
	public float A = 1.5f;
	public float B = 32f;
	public float C = 500f;

    public HUD playerUI;

    public List<AI_Enemy> enemyList = new List<AI_Enemy>();

	public void RegisterEnemy(AI_Enemy enemy)
	{
        enemyList.Add(enemy);
        enemyCounter++;
	}
	public void RemoveEnemy(AI_Enemy enemy)
	{
        enemyList.Remove(enemy);

        enemyCounter--;
		if (enemyCounter <= 0)
		{
			enemyCounter = 0;

			//Set timer -> day is starting

			int timeBetweenWave = 90 + waveCounter * 5;
			waveStart = Time.time + timeBetweenWave;

            HUD.instance.setRemainingTime(Mathf.Clamp((int)(waveStart - Time.time), 0, 99999999));
        }
	}

	private void StartWave()
	{
        waveCounter++;
        HUD.instance.setWave(waveCounter);

		int budget = (int)(A * waveCounter * waveCounter + B * waveCounter + C + Mathf.Pow(2.7182f, 1.2f*Mathf.Sqrt(waveCounter)));
		Debug.Log("Whole budget is " + budget);

		GameObject[] objects = GameObject.FindGameObjectsWithTag("Spawner");

		float accumulativeDistance = 0f;
		
		for(int i = 0; i < objects.Length; i++)
		{
			GameObject obj = objects[i];
			
			accumulativeDistance += new Vector2(obj.transform.position.x - Meteor.location.x, obj.transform.position.z - Meteor.location.y).magnitude;
		}

		for (int i = 0; i < objects.Length; i++)
		{
			GameObject obj = objects[i];
			Spawner spawner = obj.GetComponent<Spawner>();

			float distance = new Vector2(obj.transform.position.x - Meteor.location.x, obj.transform.position.z - Meteor.location.y).magnitude;

			int currentBudget = (int)(distance * budget / accumulativeDistance);
			spawner.SpawnWave(currentBudget, waveCounter);
		}

	}

	public void FixedUpdate()
	{
		if(waveStart != 0 && waveStart <= Time.time)
		{
			waveStart = 0;
			StartWave();
		}
        else
        {
            //Update timer v UI
            playerUI.setRemainingTime(Mathf.Clamp((int)(waveStart - Time.time), 0, 99999999));
        }

        //Update all enemies
        int count = enemyList.Count;
        for (int i = 0; i < count; i++)
        {
            enemyList[i].AI_Update();
        }
	}

}
