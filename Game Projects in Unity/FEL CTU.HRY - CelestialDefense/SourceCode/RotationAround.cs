using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/**
 * For rotating camera around the island in the Main menu
 */ 
public class RotationAround : MonoBehaviour
{
    public bool rotating = true;
    public float rotationSpeed = 10;

    public GameObject worldPrefab;
    void Update()
    {
        if (rotating)
        {
            transform.transform.Rotate( new Vector3(0,1,0), rotationSpeed * Time.deltaTime);   
        }
    }
}
