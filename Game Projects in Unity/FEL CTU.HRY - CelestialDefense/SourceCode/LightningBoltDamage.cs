using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/**
 * Not used in game
 */
public class LightningBoltDamage : MonoBehaviour
{   
    private void Start()
    {
        GetComponent<CapsuleCollider>().radius = MyMovement.instance.lightningRadius;
    }
    private void OnTriggerEnter(Collider other)
    {
        AI_Enemy enemy = other.gameObject.GetComponent<AI_Enemy>();
        if (enemy != null)
        {
            enemy.TakeDamage(MyMovement.instance.lightningDamage);
        }
    }
}
