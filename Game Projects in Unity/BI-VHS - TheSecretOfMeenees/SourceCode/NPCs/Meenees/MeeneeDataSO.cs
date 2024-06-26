using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName = "MENU/NPC/MeeneeData", fileName = "MeeneeData")]
public class MeeneeDataSO : ScriptableObject
{
    [Header("Speed Values")]
    public string meeneeElement;
    public float speed;
    public float runSpeed;
    public float flyingSpeed;
    [Header("Talking Variables")]
    public float talkingSpeed;
    public float talkingPitch;
    [Space]
    public int maxEnergy;
    public int maxPlayfulness;

    //!!!!! THIS CANNOT BE IN SO, OBVIOUSLY!!!!!

    //[Header("Locations")]
    //public Transform myHome;
    //[Tooltip("Points of interest, such as other homes, cool spots, etc.")]
    //public Transform[] POI;

    //!!!!! THIS CANNOT BE IN SO, OBVIOUSLY!!!!!

    [Header("Layer")]
    public LayerMask whatIsInteractable; //simplifies checks when engaging in talk/play behaviours

}