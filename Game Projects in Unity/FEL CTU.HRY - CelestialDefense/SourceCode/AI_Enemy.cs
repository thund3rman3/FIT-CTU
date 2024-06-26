using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;
using Unity.Mathematics;

/*
 * Class holding everything about Enemy. It control it movement, etc
 */
public class AI_Enemy : MonoBehaviour
{
    public int enemyType = 0;

    //Place where projectile should aim
    public GameObject AimPosition;

    [Range(1, 100), Tooltip("How much budget spawning this enemy takes")]
	public int budgetCost = 5;
	
	[Tooltip("How much damage is enemy dealing per second")]
	public float damagePerSecond = 20f;
    public int scorePerKill = 10;

    [Tooltip("How much damage does it takes to kill the enemy")]
	public float maxHealth = 20f;
	public float health = 20f;
    public bool dead = false;

	[Tooltip("How many blocks per second can the enemy move")]
	public float speed = 2f;
	public float rotationRate = 180f;
	public float jumpImpulseStrenght = 2f;

    //Variables contrlling where will the enemy go
    private Vector3 goToLocation;
	private int2 nextLocation;
	private float lastJump = 2f;
	private float nextHeight = -30;

    [SerializeField] public bool isGargoyle = false;
    [SerializeField] public bool isWarlock = false;
    //Prefab of enemy that warlock is spawning
    public GameObject pixiePrefab = null;
    //When warlock can summon 
    private float nextCast = 0;

    private Rigidbody rigidBody;
    private Animator animatorController;
    private AudioSource hitSound;

    private float freezed40Until = 0;
    private float freezed60Until = 0;
    private float onFire5Until = 0;
    private float onFire10Until = 0;

    void Start()
    {
        hitSound = GetComponentInChildren<AudioSource>();
        animatorController = GetComponentInChildren<Animator>();
        rigidBody = GetComponentInChildren<Rigidbody>();
        health = maxHealth;
        lastJump = 2f + Time.time;
        nextCast = Time.time + 10;

        //Every Enemy has to be registered in SpawningScheduler
        //so that we can count how many of them remains
        SpawningScheduler.instance.RegisterEnemy(this);

        freezed40Until = Time.time;
        freezed60Until = Time.time;
        onFire5Until = Time.time;
        onFire10Until = Time.time;

        speed = UnityEngine.Random.Range(speed * 0.9f, speed * 1.1f);
    }
    public void freeze60For(float seconds)
    {
        freezed60Until = Time.time + seconds;
    }
    public void freeze40For(float seconds)
    {
        freezed40Until = Time.time + seconds;
    }

    public void putOnFire5For(float seconds)
    {
        onFire5Until = Time.time + seconds;
    }

    public void putOnFire10For(float seconds)
    {
        onFire10Until = Time.time + seconds;
    }

    private void OnDestroy()
	{
		SpawningScheduler.instance.RemoveEnemy(this);
	}

    public bool TakeDamage(float damage)
    {
        health -= damage;

		if (health <= 0)
        {
            if (dead) return true;

            //Umřel
            dead = true;
            if (animatorController != null)
            {
                animatorController.SetBool("Dead", true);
            }

            //Destroy colliders
            CapsuleCollider collider = GetComponentInChildren<CapsuleCollider>();
            SphereCollider sphereCollider = GetComponentInChildren<SphereCollider>();
            if (collider != null) Destroy(collider);
            if (sphereCollider != null) Destroy(sphereCollider);

            if (!isGargoyle) rigidBody.useGravity = false;           

            HUD.instance.AddScore(scorePerKill);

            StartCoroutine(destroyItself());
            return true;
        }

		return false;
    }

    //Wait for a moment and destroy itself
    IEnumerator destroyItself()
    {
        yield return new WaitForSeconds(2f);
        Destroy(gameObject);
    }

    public void PlayHit()
    {
        hitSound.Play();
    }
	public void AI_Update()
	{
        if (dead)
        {
            rigidBody.velocity = new Vector3(0, 0, 0);
            return;
        }

        //Kill when it falls under map
        if (transform.position.y < -10) {
            TakeDamage(999999);
        }

        //Fire damage
        if (Time.time < onFire10Until) { TakeDamage(Time.deltaTime * 10); }
        if (Time.time < onFire5Until) { TakeDamage(Time.deltaTime * 5); }

        Vector3 previousPosition = transform.position;

        //Check whether we have to jump
        if (Time.time - lastJump >= 0.5f && nextHeight > (transform.position.y + 0.1))
		{
			lastJump = Time.time;
			rigidBody.AddForce(new Vector3(0, jumpImpulseStrenght, 0), ForceMode.Impulse);
		}

        //Find where to go
        int2 pos = new int2((int)transform.position.x, (int)transform.position.z);
        nextLocation = PathSearcher.getNextTarget(pos);
        int index = nextLocation.x + nextLocation.y * World.chunkXCount * ChunkData.chunkXSize;
        if (index < 0) return;

        goToLocation = new Vector3(nextLocation.x + 0.5f, transform.position.y, nextLocation.y + 0.5f);
        nextHeight = WorldMap.tiles[index].height + 1;

        JobTile nextTile = WorldMap.tiles[index];
		float nextTileDistance = new Vector2(nextLocation.x + 0.5f - transform.position.x, nextLocation.y + 0.5f - transform.position.z).magnitude;
		GameObject nextTileObject = WorldMap.tilesObjects[index];

        //Decide what to do - move or attack 
        bool moveNow = true;
        if (isGargoyle){
            if (nextTileObject != null && nextTileObject.GetComponent<Structure>().isMeteor &&
               nextTile.containStructure && nextTile.walkInPenalty > 1
               && nextTileDistance <= 1f )
            {
                moveNow = false;
            }
        }
        else if (isWarlock)
        {
            if (Time.time > nextCast)
            {
                //Can Summon
                nextCast = Time.time + 15;
                GameObject obj = Instantiate(pixiePrefab, transform.position + new Vector3(0.0f, 0f, 0.0f), new Quaternion());
            }
            else if (nextTileObject != null &&
               nextTile.containStructure && nextTile.walkInPenalty > 1
               && nextTileDistance <= 1f)
            {
                moveNow = false;
            }
        }
        else
        {
            if (nextTileObject != null &&
               nextTile.containStructure && nextTile.walkInPenalty > 1
               && nextTileDistance <= 1f)
            {
                moveNow = false;
            }
        }

        if (moveNow)
        {
            //Move the enemy
            float speedMultiplier = 1f;
            if (Time.time < freezed60Until) speedMultiplier = 0.6f;
            if (Time.time < freezed40Until) speedMultiplier = 0.4f;
            transform.position = Vector3.MoveTowards(transform.position, goToLocation, speed * Time.deltaTime * speedMultiplier);
            if (animatorController != null){
                animatorController.SetBool("Attack", false);
            }
        }
        else
        {
            //Damage the object
            Structure structure = nextTileObject.GetComponent<Structure>();
            structure.TakeDMG(Time.deltaTime * damagePerSecond);

            if (animatorController != null)
            {
                animatorController.SetBool("Attack", true);
            }
        }

        if (animatorController != null)
            animatorController.SetFloat("Speed", (previousPosition - transform.position).magnitude);

        //Rotate towards destination
        Vector3 rotDir = goToLocation - transform.position;
        rotDir.y = 0;// transform.position.y;
        Quaternion targetRotation = Quaternion.LookRotation(rotDir);

        transform.rotation = Quaternion.RotateTowards(transform.rotation, targetRotation, rotationRate * Time.deltaTime);

        //cancel any velocity in XY plane - avoid slipping
        rigidBody.velocity = new Vector3(0, rigidBody.velocity.y, 0);
    }
}
