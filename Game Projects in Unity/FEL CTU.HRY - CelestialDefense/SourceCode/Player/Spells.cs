using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEditor.UIElements;
using UnityEngine;
using UnityEngine.Serialization;

/*
 * Not used
 */
public class Spells : MonoBehaviour
{
    
    enum ClassNumbers
    {
        Warrior=0,
        Mage=1,
        Getherer=2,
        Cannoneer=3
    }
    
    public int shieldTime = 3;
    private GameObject[] towers;
    private ClassNumbers ClassType;
    
    public GameObject lightningBolt;
    public int boltTime=3;
    private Vector3 camPosition;
    private Animator animator;
    
    public float SplashAtackDistance =3f;
    public float SplashAtackDamage = 100f;
    public GameObject particleSystem_our;
    private GameObject PSUsed = null;
    private bool allowSpell = true;
    public float SpellCoolDown = 5f;
    public bool debug_mod = false;


    public int triggerTime = 5;
        

    // Start is called before the first frame update
    void Start()
    {
        ClassType = (ClassNumbers)MainMenuControl.hero_number; //tmp status for testing
        animator = GetComponentInChildren<Animator>();
      
    }

    // Update is called once per frame
    void Update()
    {
        if(allowSpell)
        {
            if (Input.GetKeyDown(KeyCode.Q) && (debug_mod || ClassType == ClassNumbers.Warrior))
            {
                //towers = Instantiate(Resources.Load("Wall"));
                towers = GameObject.FindGameObjectsWithTag("Wall");
                Debug.Log("Spell activated " + towers.Length);
                animator.SetTrigger("WarriorSpell");
                if(!debug_mod)
                StartCoroutine("Cooldown");
               // transform.GetComponentInParent<MyMovement>().SetUseSpell(true);
            }

            else if (Input.GetKeyDown(KeyCode.V) && (debug_mod || ClassType == ClassNumbers.Mage))
            {
                if (Camera.main is null) return;
                Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
                RaycastHit Hit;
                int layer_mask = LayerMask.GetMask("MapCollision");
                if (Physics.Raycast(ray, out Hit, 1000, layer_mask))
                {
                    camPosition = new Vector3(Hit.point.x, Hit.point.y, Hit.point.z);
                    animator.SetTrigger("MageSpell");
                    if (!debug_mod)
                        StartCoroutine("Cooldown");
                    //transform.GetComponentInParent<MyMovement>().SetUseSpell(true);
                }
            }
            else if (Input.GetKeyDown(KeyCode.C) && (debug_mod|| ClassType == ClassNumbers.Cannoneer))
            {
                
                animator.SetTrigger("CannoneerSpell");
                if(!debug_mod)
                StartCoroutine("Cooldown");
                //transform.GetComponentInParent<MyMovement>().SetUseSpell(true);
            }
            else if (Input.GetKeyDown(KeyCode.X) && (debug_mod || ClassType == ClassNumbers.Getherer))
            {
                towers = GameObject.FindGameObjectsWithTag("Wall");
                animator.SetTrigger("GathererSpell");
                if (!debug_mod)
                    StartCoroutine("Cooldown");
                //transform.GetComponentInParent<MyMovement>().SetUseSpell(true);
                StartCoroutine("TriggerBoostEnd");
            }
        }
     


    }

    public void TriggerBoost()
    {
        int layer = LayerMask.GetMask("Tower");
        foreach (GameObject tower in towers)
        {
            Turret tur = tower.GetComponent<Turret>();
            if (tur)
            {
                tur.fireCooldown *= 0.75f;
                if (tower.transform.childCount > 0)
                {
                    GameObject shield = tower.transform.GetChild(0).gameObject;
                    shield.SetActive(true);

                   if( shield.GetComponent<Material>())//changing color of material not working

                   // shield.GetComponent<Material>().SetColor("_Color",Color.cyan);
                   shield.GetComponent<Material>().SetColor("_Color",Color.cyan);

                }
           
            }
        }   
    }
    private IEnumerator TriggerBoostEnd()
    {
        
        yield return new WaitForSeconds(triggerTime);
        int layer = LayerMask.GetMask("Tower");
        foreach (GameObject tower in towers)
        {
           Turret tur = tower.GetComponent<Turret>();
           if (tur)
           {
               tur.fireCooldown /= 0.75f;
               if (tower.transform.childCount > 0)
               {
                   GameObject shield = tower.transform.GetChild(0).gameObject;
                   
                   shield.SetActive(false);
               }
            }
        }   
 
    }
    
    public void TowerProtectEvent()
    {
        TurnOnShieds();
        StartCoroutine("TurnOffShields");
    }

    public void SplashAttack()
    {  
        GameObject[] enemies = GameObject.FindGameObjectsWithTag("Enemy");
        Debug.Log(enemies.Length);
        if (!PSUsed)
        {
            PSUsed = Instantiate(particleSystem_our, transform.position+Vector3.up*0.5f, Quaternion.Euler(-90,0,0),transform);
            
        }
        PSUsed.GetComponent<ParticleSystem>()?.Play();
        foreach (var enemy in enemies)
        {
           // if (!enemy) continue;

            float distance = Vector3.Distance(transform.position, enemy.transform.position);
            if (distance < SplashAtackDistance)
            {
                enemy.GetComponent<AI_Enemy>().TakeDamage(SplashAtackDamage);
            }
        }
    }

    public void LightningBoltEvent()
    {
        GameObject bolt = Instantiate(lightningBolt, camPosition, new Quaternion());
        bolt.transform.GetChild(1).transform.position = camPosition;
        bolt.transform.GetChild(0).transform.position = camPosition + Vector3.up * 100;

        StartCoroutine("DestroyLight", bolt);
        
    }

   public void allowMovement()
   {
       //transform.GetComponentInParent<MyMovement>().SetUseSpell(false);
   }

   private IEnumerator DestroyLight(GameObject bolt)
    {
        // suspend execution for 5 seconds
        yield return new WaitForSeconds(boltTime);
       
        Destroy(bolt);
       // transform.GetComponentInParent<MyMovement>().SetUseSpell(false);
 
    }
   private IEnumerator Cooldown()
   {
       allowSpell = false;
       yield return new WaitForSeconds(SpellCoolDown);
       allowSpell = true;
   }
    private void TurnOnShieds()
    {
        foreach (GameObject tower in towers)
        {
            if(tower.transform.childCount>0){
                GameObject shield  = tower.transform.GetChild(0).gameObject;
                shield.SetActive(true);
               tower.GetComponent<Structure>()?.setProtection(true);
            
            }
        }
    }
    private IEnumerator TurnOffShields()
    {
        // suspend execution for 5 seconds
        yield return new WaitForSeconds(shieldTime);
       
        foreach (GameObject tower in towers)
        {
            if(tower.transform.childCount>0){
                GameObject shield  = tower.transform.GetChild(0).gameObject;
                shield.SetActive(false);
                tower.GetComponent<Structure>()?.setProtection(false);
            }
        }
     
    }
    

}
