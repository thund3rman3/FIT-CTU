using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WaterDestroy : MonoBehaviour
{
    // Start is called before the first frame update
    [SerializeField] float DestroyTimer = 0f;
    [SerializeField] float amout = 0.1f;
    void Start()
    {
        transform.Translate(Vector3.down * amout * 2.5f);
        DestroyTimer = 0f;
    }

    // Update is called once per frame
    void Update()
    {
        DestroyTimer += Time.deltaTime;
        if(DestroyTimer < 2)
            transform.Translate(Vector3.up * amout * Time.deltaTime);
        else if (DestroyTimer > 10)
            Destroy(transform.gameObject);
        else if (DestroyTimer > 8)
            transform.Translate(Vector3.down * amout * Time.deltaTime);  
    }
}
