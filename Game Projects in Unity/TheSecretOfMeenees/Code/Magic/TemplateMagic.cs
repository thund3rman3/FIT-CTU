using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TemplateMagic : ScriptableObject
{
    // Start is called before the first frame update
    public ElementType Element = ElementType.Air;
    public GameObject BallParticle = null;
    public float AtkDmg = 1;
    public enum ElementType{
        Air = 0,
        Earth = 1,
        Fire = 2,
        Water = 3
    }
}
public class SpellDataInfo{
    public TemplateMagic.ElementType ElementType;
    public float AtkDmg = 2;
    public ulong SpellType;
    public Color TextureColor;
    public GameObject BallParticle;
    public SpellDataInfo(TemplateMagic data, Color color, ulong spellType){
        AtkDmg = data.AtkDmg;
        ElementType = data.Element;
        BallParticle = data.BallParticle;
        TextureColor = color;
        SpellType = spellType;
    }
    public SpellDataInfo(SpellDataInfo data){
        AtkDmg = data.AtkDmg;
        ElementType = data.ElementType;
        TextureColor = data.TextureColor;
        SpellType = data.SpellType;
        BallParticle = data.BallParticle;
    }
}