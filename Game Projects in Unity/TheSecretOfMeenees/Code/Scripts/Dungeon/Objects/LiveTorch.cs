using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UIElements;

public class LiveTorch : MonoBehaviour
{
    // Start is called before the first frame update
    Vector3 start;
    float time = 0.0f;
    void Start()
    {
        start = transform.position;
    }

    // Update is called once per frame
    void Update()
    {
        time += Time.deltaTime;
        if(time >= 0.3f){
            transform.position = start + new Vector3(Prefabs.RNG.Next(-8, 8), Prefabs.RNG.Next(-8, 8), Prefabs.RNG.Next(-8, 8)) / 1000;
            time = 0;
        }
    }
}
