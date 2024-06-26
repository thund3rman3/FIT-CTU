using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/**
 * Just a simple script to keep the static functions of PathSearcher going
 */
public class PathSearcherOwner : MonoBehaviour
{
    private void Update()
    {
        PathSearcher.Update();
    }
}
