using System;
using Unity.Mathematics;
using UnityEngine;
using UnityEngine.AI;

public class EnemyAI : MonoBehaviour {

    public uint ID; //0-goblin, 1-wolf, 2-golem

	public int budgetCost = 5;
	public float damagePerHit;
	public float maxHealth = 20f;
	public float health = 20f;
    public bool dead = false;

    public GameObject[] hitBox;
    private Animator animatorController;

    const int maxBoxes = 2;
    SphereCollider[] punchCollider = new SphereCollider[maxBoxes];

    private void Start() {
        animatorController = GetComponent<Animator>();
        health = maxHealth;
        for(int i = 0; i < hitBox.Length;i++) {
            if (hitBox[i].GetComponent<SphereCollider>())
                punchCollider[i] = hitBox[i].GetComponent<SphereCollider>();
        }
    }

    private void DestroyThis()
    {
        Destroy(gameObject, 3f);
    }



    //@return true - dead
    //@return false - alive
    public bool TakeDamage(float damage, TemplateMagic.ElementType element)
    {
        if (dead)
                return true;

        switch (ID)
        {
            case 0:
                if (element == TemplateMagic.ElementType.Air)//air-goblin
                    damage *= 2;
                break;
            case 1:
                if (element == TemplateMagic.ElementType.Fire)//fire-wolf
                    damage *= 2;
                break;
            case 2:
                if (element == TemplateMagic.ElementType.Water)//water-golem
                    damage *= 2;
                break;
            default:
                break;
        }

        health -= damage;

		if (health <= 0)//dying
        {
            dead = true;
            if (animatorController)
                animatorController.SetBool("Dead", true);

            GetComponent<NavMeshAgent>().isStopped = true;

            Debug.Log("Dying");
            Debug.Log("Destroyed in 3s");
            DestroyThis();
            return true;
        }
		return false;
    }

    public void hitPlayer()
    {
        foreach(var p in punchCollider)
            if(p)
                p.enabled = true;
    }
    public void unhitPlayer()
    {
        foreach (var p in punchCollider)
            if(p)
                p.enabled = false;
    }

    public void DamagePlayer(Transform player)
    {
        player.GetComponent<PlayerBehavior>().TakeDamage(damagePerHit);
    }
}