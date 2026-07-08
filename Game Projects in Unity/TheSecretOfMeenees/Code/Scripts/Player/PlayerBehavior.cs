using System.Collections;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.Rendering.PostProcessing;
using UnityEngine.SceneManagement;
using UnityEngine.UIElements;

public class PlayerBehavior : MonoBehaviour
{
    public GameObject SpawnPoint;
    public GameObject RespawnPoint;
    public GameObject DeathScreen;
    public GameObject RespawnScreen;
    public GameObject DamageIndicator;
    public bool alive;

    private CharacterController _cc;
    private PostProcessVolume _volume;
    private Vignette _vignette;

    private Coroutine _MyCoroutineReference;

    private void Start()
    {
        alive = true;
        _cc = gameObject.GetComponent<CharacterController>();
        _cc.enabled = false;
        Debug.Log("In dungeon " + PersistData.playerData.inDungeon);
        Debug.Log("spawn: " + SpawnPoint.transform.position + " respawn: " + RespawnPoint.transform.position);
        if (PersistData.playerData.inDungeon)
        {
            posToRespawn();
            PersistData.playerData.inDungeon = false;
        }
        else
            posToStart();
        DeathScreen.SetActive(false);
        _cc.enabled = true;
        _volume = DamageIndicator.GetComponent<PostProcessVolume>();
        _volume.profile.TryGetSettings<Vignette>(out _vignette);

        _vignette.enabled.Override(false);
        PersistData.playerData.stopped = false;
    }

    private void OnTriggerEnter(Collider other)
    {
       switch(other.tag){
        case "TutorialExit":
            Debug.Log("Entering Village");
            PersistData.playerData.inDungeon = false;
            SceneManager.LoadScene("Village");
            break;
        case "WellExit":
            Debug.Log("Entering Dungeon");
            PersistData.playerData.inDungeon = true;
            PersistData.DelveIntoDungeon();

            SceneManager.LoadScene("Dungeon 1");
            break;
                
        case "DungExit":
            Debug.Log("Exiting Dungeon");

            SceneManager.LoadScene("Village");
            break;
        case "Dung1Exit":
            Debug.Log("Exiting Dungeon 1");
            SceneManager.LoadScene("Dungeon 2");
            break;
        case "Dung2Exit":
            Debug.Log("Exiting Dungeon 2");
            SceneManager.LoadScene("Dungeon 3");
            break;
        case "Dung3Exit":
            Debug.Log("Exiting Dungeon 3");

            PersistData.EmergeFromDungeon();

            SaveSystem.SaveData();
            SceneManager.LoadScene("Village");
            break;
        case "VillageExit":
            PersistData.playerData.inDungeon = true;
            Debug.Log("Exiting Village");
            SceneManager.LoadScene("Tutorial");
            break;
        case "EnemyHit":
            other.GetComponentInParent<EnemyAI>().DamagePlayer(transform);
            Debug.Log("taken have:" +PersistData.playerData.healthPoints);
            break;
        case "SmokeJump":
            PersistData.playerData.jumpHeight = PersistData.playerData.superJump;
            break;
        default:
            break;
       }
    }

    private void OnTriggerExit(Collider other)
    {
        if(other.CompareTag("SmokeJump"))
            PersistData.playerData.jumpHeight = PersistData.playerData.defaultJump;
    }


    public void posToStart()
    {
        Debug.Log("Started");
        transform.position = SpawnPoint.transform.position;
        transform.rotation = SpawnPoint.transform.rotation;
        _cc.enabled = true;
    }

    void posToRespawn()
    {
        Debug.Log("Respawned");
        if(SceneManager.GetActiveScene().name == "Village") RespawnScreen.SetActive(true);
        transform.position = RespawnPoint.transform.position;
        transform.rotation = RespawnPoint.transform.rotation;
        _cc.enabled = true;
    }
    
    public void TakeDamage(float dmg)
    {
        PersistData.playerData.healthPoints -= dmg;
        if(_MyCoroutineReference != null) StopCoroutine(_MyCoroutineReference);
        _MyCoroutineReference = StartCoroutine(TakeDamageEffect(dmg));
        if(PersistData.playerData.healthPoints <= 0)
        {
            SaveSystem.SaveData();
            KillPlayer();
        }
    }

    private void KillPlayer()
    {
        _cc.enabled = false;
        //bring out a small "ur dieded" screen, or at least a flash of white or something as you die DIE
        DeathScreen.SetActive(true);
        alive = false;
    }
    public void FinishDeath()
    {
        if (PersistData.playerData.brokenLegs && SceneManager.GetActiveScene().name == "Tutorial")
        {
            PersistData.playerData.healthPoints = 25;
            DeathScreen.SetActive(false);
            alive = true;
            _vignette.enabled.Override(false);
            
            posToStart();
            //transform.position = new Vector3(4.26f, 1f, 30.73f);
        }
        else //Load into the village scene, and restore player 
        {
            PersistData.EmergeFromDungeon();
            PersistData.playerData.healthPoints = PersistData.playerData.maxHealthPoints;
            PersistData.dungeonData.ClearData();
            PersistData.playerData.brokenLegs = false;
            PersistData.playerData.inDungeon = true;
            SceneManager.LoadScene("Village");
        }
        _cc.enabled = true;
    }

    private IEnumerator TakeDamageEffect(float dmg)
    {
        var minIntensity = PersistData.playerData.healthPoints / PersistData.playerData.maxHealthPoints;
        var intensity = dmg / PersistData.playerData.maxHealthPoints + (1f - minIntensity)/3f;

        _vignette.enabled.Override(true);
        _vignette.intensity.Override(intensity);

        yield return new WaitForSeconds(0.1f);

        while (intensity > 0)
        {
            intensity -= 0.01f;
            if (intensity < 0) intensity = 0;

            _vignette.intensity.Override(intensity);

            yield return new WaitForSeconds(0.05f);
        }

        _vignette.enabled.Override(false);
        _MyCoroutineReference = null;
        yield return new WaitForSeconds(0.05f);
    }
}
