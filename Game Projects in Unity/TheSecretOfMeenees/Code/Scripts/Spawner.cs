using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Spawner : MonoBehaviour
{
    [SerializeField] GameObject[] enemyPrefabs;
    // Start is called before the first frame update
    void Start()
    {
        for (int i = 0; i < 4; i++)
        {
            GameObject obj = Instantiate(
                enemyPrefabs[0],
                new Vector3(0,0,0),
                new Quaternion());
        }
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
