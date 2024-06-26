using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

public class SpellProjectile : MonoBehaviour
{
    // Update is called once per frame
    public float Speed = 1.5f;
    void Update()
    {
        transform.Translate(0, 0, Speed * Time.deltaTime, Space.Self);
    }
    /*
        other.GetComponent<EnemyAI>().TakeDamage(10f,ElementType);
    */
}
