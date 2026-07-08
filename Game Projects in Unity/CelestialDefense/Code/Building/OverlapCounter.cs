using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/**
 * Counts number of overlap. Used for detecting collision on highlighter
 */
public class OverlapCounter : MonoBehaviour
{
    public int counter = 0;

    private void OnTriggerEnter(Collider collision)
    {
        if(collision.isTrigger == false)
            counter++;
    }

    private void OnTriggerExit(Collider collision)
    {
        if (collision.isTrigger == false)
            counter--;
    }
}
