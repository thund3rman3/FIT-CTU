using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/**
 * Script for minimap camera. Follows playerPosition.
 */
public class MinimapFollow : MonoBehaviour
{
    public Transform playerPosition;

    void LateUpdate()
    {
        Vector3 pos = playerPosition.position;
        pos.y = 100;
        transform.position = pos;
    }
}
