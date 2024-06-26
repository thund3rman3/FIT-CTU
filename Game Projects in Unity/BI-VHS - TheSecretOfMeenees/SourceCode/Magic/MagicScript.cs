using System;
using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEditor;
using UnityEngine;

public class MagicScript : MonoBehaviour
{
    // Start is called before the first frame update
    [SerializeField] Transform Model;
    [SerializeField] GameObject WateredLava;
    public SpellDataInfo SpellData;
    public bool DestroySet = false;
    public float DestroyTimer = 0f;
    //public float BlockTimer = 0f;
    public bool active;

    void Start()
    {
        Model = transform.GetChild(0);
    }
    void Update(){
        if(DestroySet){
            DestroyTimer -= Time.deltaTime;
            if (DestroyTimer < 0) Destroy(transform.gameObject);
        }
    }
    public void Data(SpellDataInfo spellData){
        SpellData = spellData;
        if(SpellData.SpellType == 0){
            GameObject tmp = Instantiate(SpellData.BallParticle, transform.position, transform.rotation);
            tmp.transform.parent = transform;
        }

        /*Color tmp = Color.black;
        switch(ElementType){
            case TemplateMagic.ElementType.Air:
                tmp = new Color(255, 255, 255);
                break;
            case TemplateMagic.ElementType.Earth:
                tmp = new Color(0, 255, 0);
                break;
            case TemplateMagic.ElementType.Fire:
                tmp = new Color(255, 0, 0);
                break;
            case TemplateMagic.ElementType.Water:
                tmp = new Color(0, 0, 255);
                break;
        }*/

        foreach (var mat in transform.GetChild(0).GetComponent<Renderer>().materials){
            mat.SetColor("_Color", SpellData.TextureColor);
            mat.EnableKeyword("_EMISSION");
            mat.SetColor("_EmissionColor", SpellData.TextureColor);
        }
    }
    public bool Fire(){
        switch(SpellData.SpellType){
            case 0:
                DestroyTimer = 8f;
                var tmp = transform.AddComponent<SpellProjectile>();
                tmp.Speed = 8f;
                break;
            case 1:
                DestroyTimer = -1f;
                //applay effect to player
                break;
            case 2:
                if(transform.GetChild(0).GetComponent<BlockPlacing>().Red.activeInHierarchy) return false;
                DestroyTimer = -1f;
                var trueSpell = transform.GetChild(0);
                var tmp22 = trueSpell.AddComponent<MagicScript>();
                tmp22.Data(SpellData);
                tmp22.WateredLava = WateredLava;
                tmp22.DestroyTimer = 6f;
                tmp22.DestroySet = true;
                tmp22.active = true;
                var tmp21 = trueSpell.AddComponent<BlockSpell>();
                tmp21.DeactivatePlacing();
                trueSpell.parent = null;
                var renderer = trueSpell.GetComponent<Renderer>();
                renderer.receiveShadows = true;
                renderer.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.On;
                if (SpellData.ElementType == TemplateMagic.ElementType.Earth)
                {
                    var collider = trueSpell.GetComponent<BoxCollider>();
                    collider.includeLayers = LayerMask.NameToLayer("Player");
                    collider.excludeLayers = collider.excludeLayers & LayerMask.NameToLayer("Player");
                }
                //ray place to surface
                break;
        }
        DestroySet = true;
        active = true;
        return true;
    }
    public void OnContactDestroys(Collision coll)
    {
        Transform othT = coll.transform;
        if (othT.CompareTag("AirDestroy") && SpellData.ElementType == TemplateMagic.ElementType.Air)
            othT.AddComponent<AirDestroy>();
        else if (othT.CompareTag("FireDestroy") && SpellData.ElementType == TemplateMagic.ElementType.Fire)
            othT.AddComponent<FireDestroy>();
        else if (othT.CompareTag("Lava") && SpellData.ElementType == TemplateMagic.ElementType.Water)
        {
            ContactPoint cont = coll.GetContact(0);
            GameObject stonedLava = Instantiate(WateredLava, cont.point, Quaternion.FromToRotation(Vector3.up, cont.normal));
            Destroy(transform.gameObject);
            return;
        }
        DestroySpell(coll);
    }
    void DestroySpell(Collision coll)
    {
        if (SpellData.SpellType == 0)
        {
            if (SpellData.ElementType != TemplateMagic.ElementType.Earth) Destroy(transform.gameObject);
            else
            {
                GetComponent<SpellProjectile>().enabled = false;
                transform.GetChild(0).GetComponent<ProjectileSpin>().enabled = false;
                var rigidbody = GetComponent<Rigidbody>();
                if (rigidbody.IsSleeping())
                {
                    rigidbody.WakeUp();
                }
                rigidbody.constraints = RigidbodyConstraints.None;
                rigidbody.useGravity = true;
                rigidbody.AddForceAtPosition(coll.contacts[0].normal*500f, coll.GetContact(0).point);
                active = false;
            }
        }
    }
    void OnCollisionEnter(Collision coll){
        Debug.Log("Taking damage from block");
        if (DestroySet && active)
        {
            Debug.Log("Am active");
            Transform othT = coll.transform;
            if (othT.CompareTag("Enemy"))
            {
                Debug.Log("And with enemy");
                if (SpellData.SpellType == 0)
                {
                    active = false;
                    othT.GetComponent<EnemyAI>().TakeDamage((float)SpellData.AtkDmg, SpellData.ElementType);
                }
                else if (SpellData.SpellType == 2)
                {
                    Debug.Log("Taking damage from block for real");
                    othT.GetComponent<EnemyAI>().TakeDamage((float)SpellData.AtkDmg*2f, SpellData.ElementType);
                }
            }
            //add player damage
            OnContactDestroys(coll);
        }
    }
}
