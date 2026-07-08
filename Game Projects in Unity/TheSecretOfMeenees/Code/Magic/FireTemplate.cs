using System.Collections;
using System.Collections.Generic;
using UnityEngine;
[CreateAssetMenu(fileName ="MagicFire",menuName ="MENU/Magic/MagicFire", order =3)]
public class FireTemplate : TemplateMagic
{
    // Start is called before the first fram
    private void Awake() {
        Element = ElementType.Fire;
    }
}
