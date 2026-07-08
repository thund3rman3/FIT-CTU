using System.Collections;
using System.Collections.Generic;
using UnityEngine;
[CreateAssetMenu(fileName ="MagicAir",menuName ="MENU/Magic/MagicAir", order =1)]
public class AirTemplate : TemplateMagic
{
    // Start is called before the first frame update
    private void Awake() {
        Element = ElementType.Air;
    }
}
