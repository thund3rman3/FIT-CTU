using System.Collections;
using System.Collections.Generic;
using System.Linq;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UI;

public class SpellCast_Control : MonoBehaviour
{
    [SerializeField]public int[] currOrder;
    [SerializeField] public int EntryScale;
    [Space]
    [SerializeField] public AudioSource[] ping;
    public float[] played = { 0f, 0f, 0f, 0f, 0f, 0f };
    public List<int> Triggered = new();
    public float pitchUp;
    public float pitchStep = 0.2f;
    private bool cleaned;
    public GameObject spellConnector;
    public GameObject reticule;
    private Transform SpellContainer;
    [SerializeField] GameObject CastedSpell;
    [SerializeField] GameObject SpellProjectile;
    [SerializeField] GameObject SpellSelf;
    [SerializeField] GameObject SpellBlock;
    [SerializeField] TemplateMagic[] BaseSpellData = new TemplateMagic[4];
    //public bool drawNow;
    // Start is called before the first frame update
    void Start()
    {
        SpellContainer = transform.GetChild(2);
        pitchUp = 0.1f;
        for(int i= 0; i < 6; i++)
        {
            currOrder[i] = i * EntryScale;
            Animator anim = gameObject.transform.GetChild(1).transform.GetChild(i).GetComponent<Animator>();
            anim.SetInteger("Order", currOrder[i]);
        }
        //spellConnector.GetComponent<TrailRenderer>().time = -1f;
    }

    // Update is called once per frame
    void Update()
    {
        if (gameObject.transform.GetChild(1).gameObject.activeInHierarchy)
        {
            cleaned = true;
            for (int i = 0; i < 6; i++)
            {
                Animator anim = gameObject.transform.GetChild(1).transform.GetChild(i).GetComponent<Animator>();
                anim.SetInteger("Order", currOrder[i]--);
                anim.SetFloat("Speed", 1.0f / Time.timeScale);
                if(anim.GetBool("Highlighted") && played[i] < 0.1f)
                {
                    Triggered.Add(i);
                    PlaySound(i);
                    played[i] = pitchUp;
                    pitchUp += pitchStep;
                    //spellConnector.transform.position = gameObject.transform.GetChild(1).transform.GetChild(i).transform.GetChild(0).transform.position;
                    //if (drawNow) spellConnector.GetComponent<TrailRenderer>().time = 100f;
                    //else drawNow = true;
                }
            }
        }
        else if ( cleaned  )
        {
            //spellConnector.transform.position = reticule.transform.position; ;
            //spellConnector.GetComponent<TrailRenderer>().time = -1f;
            //FinishCast();
            Triggered.Clear();
            
            for (int i = 0; i < 6; i++)
            {
                //if (ping[i].isPlaying) return;
                Animator anim = gameObject.transform.GetChild(1).transform.GetChild(i).GetComponent<Animator>();
                restoreSpellCast(i, anim);
                anim.ResetTrigger("Selected");
                anim.ResetTrigger("Highlighted");
                ping[i].pitch -= played[i];
                played[i] = 0f;
                gameObject.transform.GetChild(1).transform.GetChild(i).transform.GetChild(0).GetComponent<Image>().color = new Color(0, 0, 0, 0);
            }
            pitchUp = 0.1f;
            cleaned = false;
            //drawNow = false;
        }
    }
    void PlaySound(int i)
    {
        ping[i].pitch += pitchUp;
        ping[i].time = 0.1f;
        ping[i].Play();
    }
    void restoreSpellCast(int i, Animator anim)
    {
        currOrder[i] = i * EntryScale;
        anim.SetInteger("Order", currOrder[i]);
    }
    public bool FinishCast(TemplateMagic.ElementType elementType, Color defined){
        ulong spell = 0;
        ulong spellType;
        DestroySpell();
        foreach (var item in Triggered)
            spell = (spell << 4) + (ulong)(1 + item);
        switch(spell){  //spels (rune + 1 => 1,2,3,4,5,6)
            case 0x14:
                CastedSpell = Instantiate(SpellProjectile, SpellContainer.transform.position, SpellContainer.transform.rotation);
                spellType = 0;
                Debug.Log(elementType + " projectile Spell Created");
                break;
           /* case 0x52:
                CastedSpell = Instantiate(SpellSelf, SpellContainer.transform.position, SpellContainer.transform.rotation);
                spellType = 1;
                Debug.Log(elementType + " self Spell Created");
                break;*/
            case 0x6532:
                CastedSpell = Instantiate(SpellBlock, SpellContainer.transform.position, SpellContainer.transform.rotation);
                spellType = 2;
                Debug.Log(elementType + " 'block' Spell Created");
                break;
            default:
                return false;
        }
        CastedSpell.GetComponent<MagicScript>().Data(new SpellDataInfo(BaseSpellData[(int)elementType],defined,spellType));
        CastedSpell.transform.parent = SpellContainer;
        return true;
    }
    public bool ConfirmSpell(){
        if (!CastedSpell.GetComponent<MagicScript>().Fire()) return false;
        CastedSpell.transform.parent = null;
        if(CastedSpell.GetComponent<MagicScript>().SpellData.SpellType != 2) CastedSpell.transform.rotation = transform.rotation;
        CastedSpell = null;
        Debug.Log("Spell Casted");
        return true;
    }
    public void DestroySpell(){
        if(CastedSpell != null){
            Destroy(CastedSpell);
            Debug.Log("CastedSpell");
        }
    }
}
