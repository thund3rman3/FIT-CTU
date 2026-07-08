using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;

public class LavaDeath : MonoBehaviour
{
    public AudioSource FireDeath;

    private void OnTriggerEnter(Collider other)
    {
        if (other.transform.CompareTag("Player"))
        {
            FireDeath.Play();
            other.gameObject.GetComponent<PlayerBehavior>().TakeDamage(525);
        }
        else if (other.gameObject.layer == LayerMask.NameToLayer("NPC"))
        {
            // Kill them 
        }
    }
}
