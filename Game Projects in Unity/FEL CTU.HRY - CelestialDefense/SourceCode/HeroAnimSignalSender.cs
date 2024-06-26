using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/**
 * Small script for capturing event from animation and notifing BuildingAndInventory script
 */
public class HeroAnimSignalSender : MonoBehaviour
{
    [SerializeField] MyMovement inventory;

    public void GetResources()
    {
        inventory.resourcesMined = true;
    }
}
