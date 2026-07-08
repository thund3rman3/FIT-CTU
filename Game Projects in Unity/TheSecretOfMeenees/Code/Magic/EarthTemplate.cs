using System.Collections;
using System.Collections.Generic;
using UnityEngine;
[CreateAssetMenu(fileName ="MagicEarth",menuName ="MENU/Magic/MagicEarth", order =2)]
public class EarthTemplate : TemplateMagic
{
    // Start is called before the first frame update
    private void Awake() {
        Element = ElementType.Earth;
    }
}
