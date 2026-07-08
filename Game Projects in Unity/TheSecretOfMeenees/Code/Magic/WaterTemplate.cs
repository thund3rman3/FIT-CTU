using System.Collections;
using System.Collections.Generic;
using UnityEngine;
[CreateAssetMenu(fileName ="MagicWater",menuName ="MENU/Magic/MagicWater", order =4)]
public class WaterTemplate : TemplateMagic
{
    // Start is called before the first frame update
    private void Awake() {
        Element = ElementType.Water;
    }
}
