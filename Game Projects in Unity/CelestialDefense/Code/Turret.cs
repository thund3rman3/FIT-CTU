using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SocialPlatforms;

/**
 * Class controlling everything about towers/turrets. 
 * It allows for shooting projectiles
 */
public class Turret : MonoBehaviour
{
	[Header(("Tower setting"))]
	public float range = 3f; //How far it can shoot
	public float rotationSpeed = 5f; //Speed of rotation of the weapon - doesnt effect the ability to shoot
	[Tooltip("How long it takes in seconds to reload")]
	public float fireCooldown = 1f;
	public float damage = 50f;
    public float fireAnimDelay = 0.30f; //Delay between starting animation and firing the projectile
    public float fireSpeedMultiplier = 1f; //Multiplier for Trigger ability that adds attackspeed

    [SerializeField] private GameObject bullet; //What to shoot
	[SerializeField] private Transform bulletSpawn; // Where the projectile will be spawned
	[SerializeField] private Transform rotationTransform; //Transform of the base of the weapon so that we dont rotate whole turret
    [SerializeField] private Animator fireAnim;

    //What type of Turret it is
    [SerializeField] private bool isWizzard = false;
    [SerializeField] private bool isCataput = false;
    [SerializeField] private bool isCannon = false;

    //Whether projectile has some buff
    public bool fire5 = false;
    public bool fire10 = false;
    public bool freeze40 = false;
    public bool freeze60 = false;

    //Helpers
    private List<GameObject> targets = new List<GameObject>();
	public float lastShotTime = 0;
	private GameObject target;
	private AI_Enemy targetAI;
    private AudioSource sound;

	private void Start()
	{
        sound = GetComponent<AudioSource>();

        GetComponent<SphereCollider>().radius = range;
        lastShotTime = Time.time;
    }

    //Keeping track of what AI_Enemies are in range
	private void OnTriggerEnter(Collider other)
	{
		AI_Enemy enemy = other.gameObject.GetComponent<AI_Enemy>();
		if (enemy != null)
		{
			targets.Add(enemy.gameObject);
		}
	}
	private void OnTriggerExit(Collider other)
	{
		AI_Enemy enemy = other.gameObject.GetComponent<AI_Enemy>();
		if (enemy != null)
		{
			if(target == null || target.gameObject == enemy.gameObject)
			{
				target = null;
			}
			targets.Remove(enemy.gameObject);
		}
	}

    //Control for animation
    private bool startAnim = false;
	void FixedUpdate()
	{
		//Select new target - closest to the tower
		if ( ((targetAI != null && targetAI.dead) || target == null) && targets.Count > 0)
		{
			float shortDistance = Mathf.Infinity;
			GameObject nearestEnemy = null;
			foreach (GameObject enemy in targets)
			{
				if (enemy == null) continue;
                if (isCannon && enemy.GetComponent<AI_Enemy>().isGargoyle) continue;

				float distance = Vector3.Distance(transform.position, enemy.transform.position);
				if (distance < shortDistance && enemy.GetComponent<AI_Enemy>().dead == false)
				{
					shortDistance = distance;
					nearestEnemy = enemy;
				}
			}
			target = nearestEnemy;
			targetAI = nearestEnemy?.GetComponent<AI_Enemy>();
		}

        if (target != null)
        {
			//Rotate around
			Vector3 direction = new Vector3((target.transform.position.x) - (rotationTransform.position.x + 0.5f), 0f, (target.transform.position.z) - (rotationTransform.position.z + 0.5f));
			rotationTransform.rotation = Quaternion.RotateTowards(rotationTransform.rotation, Quaternion.LookRotation(direction), Time.deltaTime * rotationSpeed);

            //Try to shoot
            if (lastShotTime + fireCooldown * (1f / fireSpeedMultiplier) - fireAnimDelay <= Time.time && startAnim == false)
            {
                if(fireAnim != null) fireAnim.SetTrigger("Fire");
                startAnim = true;
            }

            if (lastShotTime + fireCooldown * (1f / fireSpeedMultiplier) <= Time.time)
			{
                startAnim = false;
                lastShotTime = Time.time + UnityEngine.Random.Range(-0.3f,0.3f);

                sound.Play();
                sound.volume = (12f - Vector3.Distance(transform.position, MyMovement.instance.transform.position)) / 12f;

                GameObject bullet_object = Instantiate(bullet, bulletSpawn.position, bulletSpawn.rotation);
				Bullet bullet_script = bullet_object.GetComponent<Bullet>();
                bullet_script.aimLocation = target.GetComponent<AI_Enemy>().AimPosition;

                //Buffs
                bullet_script.fire10 = fire10;
                bullet_script.fire5 = fire5;
                bullet_script.freeze40 = freeze40;
                bullet_script.freeze60 = freeze60;

                float multiplier = 1;
                if (isWizzard)
                {
                    multiplier = MainMenuControl.WizzardMultiplier;
                    bullet_script.firedPosition = bulletSpawn.position;
                }
                if (isCataput) multiplier = MainMenuControl.catapultMultiplier;
                if (isCannon) multiplier = MainMenuControl.cannonMultiplier;

                bullet_script.damage = damage * multiplier;
				if (bullet_script != null)
					bullet_script.SetTarget(target);
			}
		}
		
	}

}
