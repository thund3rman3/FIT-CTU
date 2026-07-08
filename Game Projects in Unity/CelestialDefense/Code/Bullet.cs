using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/**
 * Moves with projectile created by Turret. Can be bullet, spear or for example lightning bolt 
 */
public class Bullet : MonoBehaviour
{
    //Where and on what to aim
    private GameObject target;
    public GameObject aimLocation;

    public float speed = 70f;
    public float damage = 50;
    public float explosionRadius = 0f;

    //Info for creating lighting effect
    [SerializeField] private bool isBolt = false;
    [SerializeField] private GameObject boltStart;
    [SerializeField] private GameObject boltEnd;
    //Where projectile was fired from
    public Vector3 firedPosition;

    private bool used = false;

    //Whether projectile has enchants
    public bool fire5 = false;
    public bool fire10 = false;
    public bool freeze40 = false;
    public bool freeze60 = false;

    void FixedUpdate()
    {
        if (target == null)
        {
            Destroy(gameObject);
            return;
        }

        Vector3 dir = aimLocation.transform.position - transform.position;
        float distanceThisFrame = speed * Time.deltaTime;

        if (used == false && dir.magnitude <= distanceThisFrame)
        {
            used = true;
            AI_Enemy enemy = target.GetComponent<AI_Enemy>();
            enemy.TakeDamage(damage);
            if(freeze40)enemy.freeze40For(5);
            if(freeze60) enemy.freeze60For(5);
            if(fire5)enemy.putOnFire5For(4);
            if(fire10)enemy.putOnFire10For(4);

            Destroy(gameObject);
            return;
        }

        if (isBolt)
        {
            boltStart.transform.position = firedPosition;
            boltEnd.transform.position = aimLocation.transform.position;
        }

        transform.Translate(dir.normalized * distanceThisFrame, Space.World);
        transform.LookAt(aimLocation.transform.position);
    }
    
    public void SetTarget(GameObject _target)
    {
        target = _target;
    }
}
