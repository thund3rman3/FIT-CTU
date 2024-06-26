using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/**
 * Class controling Traps
 */
public class Trap : MonoBehaviour
{
    public float damage = 50;
    //How many time it can be step on
    public int maxUseCount = 20;

    private void OnTriggerEnter(Collider other)
    {
        AI_Enemy enemy = other.GetComponent<AI_Enemy>();
        if (enemy != null)
        {
            enemy.TakeDamage(damage);
            maxUseCount--;
            if(maxUseCount <= 0)
            {
                Destroy(gameObject);
            }
        }
    }


}
