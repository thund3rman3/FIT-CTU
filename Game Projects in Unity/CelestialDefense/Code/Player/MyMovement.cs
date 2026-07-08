using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

/*
 * Controller class that runs all the movement of the player's character.
 * It controls jumping, movement, gathering resources or casting abilities
 */
public class MyMovement : MonoBehaviour
{
    //
    // Abilities info and control variables
    //
    public float boomCooldown = 60f;
    public float boomDamage = 60f;

    public float triggerCooldown = 120f;
    public float triggerMultiplier = 1.25f;
    public float triggerDuration = 20f;

    public float lightningCooldown = 40f;
    public float lightningDamage = 60f;
    public float lightningRadius = 20f;

    public float fortifyCooldown = 90f;
    public float fortifyDuration = 10f;
    public float boomRadius = 6f;

    public GameObject boomParticles;
    public GameObject lightningBolt;
    private GameObject lightningBoltInstance;

    public  float nextSpellTime = 0f;
    private float lastSpellTime = 0f;
    private GameObject particleInstance;
    private bool canMove = true;

    //
    // Movement control variables
    //
    [Header("Character movement")]
	[Min(0.01f), Tooltip("Walking speed in blocks per second")]
	public float walkingSpeed = 3f;
	[Min(0.01f), Tooltip("Rinning speed in blocks per second")]
	public float runningSpeed = 8f;
	[Min(0.01f), Tooltip("How fast does the model rotate around")]
	public float rotationSpeed = 360f;
	
	[Header("Camera rotation")]
	[SerializeField, Min(0.01f), Tooltip("How sensitive is rotation of the camera around")]
	private float mouseSensitivity = 4f;
	[SerializeField, Min(0.01f), Tooltip("How fast camera rotation is smoothen")]
	private float cameraRotationSpeed = 10f;

	[Header("Camera distance")]
	[SerializeField, Min(0.01f), Tooltip("How sensitive is adjusting the camera distance from player's character")]
	private float scrollSensitvity = 2f;
	[SerializeField, Min(0.01f), Tooltip("How fast camera distance is smoothen")]
	private float cameraScrollSpeed = 6f;
	[Min(0.01f), SerializeField] 
	private float maxCameraDistance = 80f;
	[Min(0.01f), SerializeField] 
	private float minCameraDistance = 5f;
 
	[SerializeField, Tooltip("Y axis offset from bottom of the character for camera to rotate around")]
	public float cameraPivotYOffset = 1.5f;

	private float actualCameraDistance = 10f;
	private Vector2 actualRotation = new Vector2(90, 30);

	private float desireCameraDistance = 10f;
	private Vector2 desireRotation = new Vector2(90, 30);

	//Components and children
	private Animator animator;
	private Rigidbody body;
    private BuildingAndInventory inventory;
	
	private GameObject model;
    private GameObject pickaxe;

    [SerializeField] private GameObject cameraObject;
    [SerializeField] GameObject warriorModel;
    [SerializeField] GameObject mageModel;
    [SerializeField] GameObject gathererModel;
    [SerializeField] GameObject canonnerModel;

    [SerializeField] Image abilityCooldownIcon;
    [SerializeField] AudioSource walkSound;
    [SerializeField] AudioSource castSound;

    //Helping variables
    private Vector3 desireDirection = new Vector3();
	private float actualSpeed = 0f;
	private bool isGrounded = true;
    private float lastJump = 0;
    private bool gathering = false;
    private bool useSpell = false;
    public bool resourcesMined = false;
    public int heroNumber = 0;
    [SerializeField] GameObject fader;

    //Poslední potice , kde se ještě dotýkal zamě
    public Vector3 lastValidPosition;
    public static MyMovement instance;

    public void Start()
	{
        instance = this;
        heroNumber = MainMenuControl.hero_number;

        if (MainMenuControl.gameShouldBeLoaded)
        {
            heroNumber = PlayerSaveData.instance.heroClass;
        }

        //Remove unwanted models
        switch (heroNumber)
        {
            case 0: //Warrior
                Destroy(mageModel);
                Destroy(gathererModel);
                Destroy(canonnerModel);
                model = warriorModel;
                break;
            case 1: //Mage
                Destroy(warriorModel);
                Destroy(gathererModel);
                Destroy(canonnerModel);
                model = mageModel;
                break;
            case 2: //gatherer
                Destroy(warriorModel);
                Destroy(mageModel);
                Destroy(canonnerModel);
                model = gathererModel;
                break;
            case 3: //Cannoner
                Destroy(warriorModel);
                Destroy(mageModel);
                Destroy(gathererModel);
                model = canonnerModel;
                break;
            default:
                Destroy(mageModel);
                Destroy(gathererModel);
                Destroy(canonnerModel);
		        model = warriorModel;
                break;
        }
        
        animator = model.GetComponentInChildren<Animator>();
        pickaxe = model.GetComponent<PickaxeFinder>().pickaxe;
        inventory = GetComponent<BuildingAndInventory>();
        body = GetComponent<Rigidbody>();
        animator.SetBool("isGrounded", isGrounded);

        //When loading we have to get data from save file
        if (MainMenuControl.gameShouldBeLoaded)
        {
            float[] pos = PlayerSaveData.instance.lastValidPosition;
            lastValidPosition = new Vector3(pos[0], pos[1], pos[2]);

            pos = PlayerSaveData.instance.position;
            gameObject.transform.SetPositionAndRotation(new Vector3(pos[0], pos[1] + 1.0f, pos[2]), new Quaternion());

            lastSpellTime = Time.time;
            nextSpellTime = Time.time + PlayerSaveData.instance.cooldown;
            if(PlayerSaveData.instance.cooldown <= 0f)
                abilityCooldownIcon.fillAmount = 1;
            else
                abilityCooldownIcon.fillAmount = (Time.time - lastSpellTime) / (nextSpellTime - lastSpellTime);

            switch (PlayerSaveData.instance.heroClass)
            {
                case 0:
                    MainMenuControl.gatheringMultiplier = 1f;
                    MainMenuControl.cannonMultiplier = 1f;
                    MainMenuControl.catapultMultiplier = 1.25f;
                    MainMenuControl.WizzardMultiplier = 1f;
                    break;
                case 1:
                    MainMenuControl.gatheringMultiplier = 1f;
                    MainMenuControl.cannonMultiplier = 1f;
                    MainMenuControl.catapultMultiplier = 1f;
                    MainMenuControl.WizzardMultiplier = 1.25f;
                    break;
                case 2:
                    MainMenuControl.gatheringMultiplier = 1.25f;
                    MainMenuControl.cannonMultiplier = 1f;
                    MainMenuControl.catapultMultiplier = 1f;
                    MainMenuControl.WizzardMultiplier = 1f;
                    break;
                case 3:
                    MainMenuControl.gatheringMultiplier = 1f;
                    MainMenuControl.cannonMultiplier = 1.25f;
                    MainMenuControl.catapultMultiplier = 1f;
                    MainMenuControl.WizzardMultiplier = 1f;
                    break;
                default:
                    break;
            }
        }
    }

    public void Update()
	{
		//
		// Jump
		//
		if (Input.GetKeyDown(KeyCode.Space) && isGrounded && Time.time >= lastJump + 0.5f)
		{
            lastJump = Time.time;
         
			AudioManagerScript.Play("PlayerJump");

            Jump();
		}

        //
        // Gathering
        //       
        gathering = false;
        if (Input.GetKey(KeyCode.F))
        {
            Ray ray = cameraObject.GetComponent<Camera>().ScreenPointToRay(Input.mousePosition);
            int layer_mask = LayerMask.GetMask("MapCollision");
            RaycastHit hit;

            if (Physics.Raycast(ray, out hit, 1000, layer_mask))
            {
                int index = (int)(hit.point.x) + (int)(hit.point.z) * World.chunkXCount * ChunkData.chunkXSize;
                GameObject gOBJ = WorldMap.tilesObjects[index];

                if (gOBJ != null)
                {
                    Resource r = gOBJ.GetComponent<Resource>();
                    Vector3 dist = gOBJ.transform.position - transform.position;
                    dist.y = 0;

                    if (dist.magnitude <= 1.75f && r != null)
                    {
                        gathering = true;

                        Vector3 direction = gOBJ.transform.position - transform.position;
                        direction.y = 0;
                        model.transform.rotation = Quaternion.RotateTowards(model.transform.rotation, Quaternion.LookRotation(direction), rotationSpeed * Time.deltaTime);

                        if (resourcesMined)
                        {
                            if(r.woodPerGather > 0)
                            {
                                AudioManagerScript.Play("ResourceTreeHit");
                            }
                            else
                            {
                                AudioManagerScript.Play("ResourceStoneHit");
                            }
                            float multiplier = MainMenuControl.gatheringMultiplier;
                            inventory.AddResources((int)(r.woodPerGather * multiplier), (int)(r.stonePerGather * multiplier), (int)(r.ironPerGather * multiplier));
                        }
                    }
                }
            }
            else
            {
                Debug.LogError(hit.transform.gameObject.name);
            }

        }

        resourcesMined = false;

        pickaxe.SetActive(gathering);
        animator.SetBool("Gathering", gathering);

        //
        // Ability
        //
        if(!gathering && Input.GetKeyDown(KeyCode.Q) && nextSpellTime < Time.time)
        {
            if (heroNumber == 0)
            {
                //Warrior
                lastSpellTime = Time.time;
                nextSpellTime = Time.time + fortifyCooldown;
                StartCoroutine("CanMove", 3f);
                animator.SetTrigger("WarriorSpell");
                StartCoroutine("CastFortify");
                StartCoroutine("DecastFortify");
                StartCoroutine("AbilitySound");
            }
            else if (heroNumber == 1)
            {
                //Mage
                lastSpellTime = Time.time;
                Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);
                RaycastHit Hit;
                int layer_mask = LayerMask.GetMask("MapCollision");
                if (Physics.Raycast(ray, out Hit, 1000, layer_mask))
                {
                    nextSpellTime = Time.time + lightningCooldown;
                    StartCoroutine("CanMove", 2.7f);
                    animator.SetTrigger("MageSpell");
                    StartCoroutine("CastBolt", Hit.point);
                    StartCoroutine("DecastBolt");
                    StartCoroutine("AbilitySound");
                }
            }
            else if (heroNumber == 2)
            {
                //Gatherer
                lastSpellTime = Time.time;
                nextSpellTime = Time.time + triggerCooldown;
                StartCoroutine("CanMove", 2.3f);
                animator.SetTrigger("GathererSpell");
                StartCoroutine("CastTrigger");
                StartCoroutine("DecastTrigger");
                StartCoroutine("AbilitySound");
            }
            else if (heroNumber == 3)
            {
                //Cannoneer
                lastSpellTime = Time.time;
                nextSpellTime = Time.time + boomCooldown;
                StartCoroutine("CanMove", 3f);
                animator.SetTrigger("CannoneerSpell");
                StartCoroutine("CastBoom");
                StartCoroutine("DecastBoom");
                StartCoroutine("AbilitySound");
            }
        }

        abilityCooldownIcon.fillAmount = (Time.time - lastSpellTime) / (nextSpellTime - lastSpellTime);

    }

    private IEnumerator AbilitySound()
    {
        yield return new WaitForSeconds(1.2f);
        castSound.Play();
    }

	void FixedUpdate()
	{
		actualSpeed = 0;
		if (canMove && !useSpell && !gathering && ( Mathf.Abs(Input.GetAxisRaw("Horizontal")) >= 0.05 || Mathf.Abs(Input.GetAxisRaw("Vertical")) >= 0.05))
		{
			actualSpeed = walkingSpeed;
			if (Input.GetKey(KeyCode.LeftShift))
			{
				actualSpeed = runningSpeed;
			}

			//
			// Move
			//
			Vector3 right = cameraObject.transform.right;
			right.y = 0;
			Vector3 forward = cameraObject.transform.forward;
			forward.y = 0;

			Vector3 direction = actualSpeed * (Input.GetAxisRaw("Horizontal") * right.normalized + Input.GetAxisRaw("Vertical") * forward.normalized).normalized;
			desireDirection = direction;

			//
			// Rotate
			//
			if (direction.magnitude > 0.1f)
			{
				model.transform.rotation = Quaternion.RotateTowards(model.transform.rotation, Quaternion.LookRotation(direction), rotationSpeed * Time.deltaTime);
			}
		}
		else
		{
			if (isGrounded)
			{
				desireDirection = new Vector3();
			}
			else{
				desireDirection *= 0.95f;
			}
		}

		animator.SetFloat("Speed", actualSpeed / runningSpeed, 0.1f, Time.deltaTime);

        Vector3 vel = new Vector3(desireDirection.x, body.velocity.y, desireDirection.z);
        body.velocity = vel;

        //
        // Ground check
        //
        int layer_mask = LayerMask.GetMask("MapCollision");

        if(isGrounded == false)
        {
            isGrounded = Physics.CheckSphere(transform.position, 0.05f, layer_mask);
            if (isGrounded)
            {
                AudioManagerScript.PlaySpatial("FallImpact", transform.position);
            }
        }
        else
            isGrounded = Physics.CheckSphere(transform.position, 0.05f, layer_mask);

		animator.SetBool("isGrounded", isGrounded);
        if(isGrounded == true)
        {
            animator.SetBool("Jump", false);
            lastValidPosition = new Vector3(transform.position.x, transform.position.y, transform.position.z);

            if (actualSpeed > 0 && walkSound.isPlaying == false)
            {
                walkSound.Play();
                if (actualSpeed > walkingSpeed)
                {
                    walkSound.pitch = 1.5f;
                }
                else
                {
                    walkSound.pitch = 1.1f;
                }
            }

        }
		
	}

	void LateUpdate()
	{
		//
		// Move camera
		//
		if (Input.GetMouseButton(2))
		{
			if (Input.GetAxis("Mouse X") != 0 || Input.GetAxis("Mouse Y") != 0)
			{
				desireRotation.x += Input.GetAxis("Mouse X") * mouseSensitivity;
				desireRotation.y -= Input.GetAxis("Mouse Y") * mouseSensitivity;
				desireRotation.y = Mathf.Clamp(desireRotation.y, 0f, 89f);
			}
		}

		if (Input.GetAxis("Mouse ScrollWheel") != 0f)
		{
			float scrollAmount = Input.GetAxis("Mouse ScrollWheel") * scrollSensitvity;
			scrollAmount *= (actualCameraDistance * 0.3f);
			desireCameraDistance += scrollAmount * -1f;
			desireCameraDistance = Mathf.Clamp(desireCameraDistance, minCameraDistance, maxCameraDistance);
		}

		//Interpolate Values
		actualRotation.x = Mathf.Lerp(actualRotation.x, desireRotation.x, cameraRotationSpeed * Time.deltaTime);
		actualRotation.y = Mathf.Lerp(actualRotation.y, desireRotation.y, cameraRotationSpeed * Time.deltaTime);
		actualCameraDistance = Mathf.Lerp(actualCameraDistance, desireCameraDistance, cameraScrollSpeed * Time.deltaTime);
	
		//Set position of the camera
		float horizontalAngle = -actualRotation.x / 180f * Mathf.PI;
		float verticalAngle = actualRotation.y / 180f * Mathf.PI;

		float x = Mathf.Cos(horizontalAngle) * Mathf.Cos(verticalAngle);
		float y = Mathf.Sin(verticalAngle);
		float z = Mathf.Sin(horizontalAngle) * Mathf.Cos(verticalAngle);
		cameraObject.transform.position = transform.position + new Vector3(0, cameraPivotYOffset, 0) + new Vector3(x, y, z).normalized * actualCameraDistance;

		cameraObject.transform.rotation = Quaternion.LookRotation((transform.position - cameraObject.transform.position).normalized);
	}

	private void Jump()
	{
		//Debug.Log("Jump");
		body.AddForce(new Vector3(0, 5, 0), ForceMode.Impulse);


        AudioManagerScript.PlaySpatial("PlayerJump", transform.position);

        animator.SetBool("Jump", true);
		
	}

    private IEnumerator CanMove(float CD)
    {
        canMove = false;
        yield return new WaitForSeconds(CD);
        canMove = true;
    }


    //
    // Abilities functions
    //
    private IEnumerator CastBoom()
    {
        yield return new WaitForSeconds(1.8f);

        particleInstance = Instantiate(boomParticles, transform.position + Vector3.up * 0.5f, Quaternion.Euler(-90, 0, 0), transform);
        particleInstance.GetComponent<ParticleSystem>()?.Play();

        AI_Enemy[] enemies = FindObjectsOfType<AI_Enemy>();

        foreach(AI_Enemy enemy in enemies)
        {
            if(Vector3.Distance(enemy.transform.position, transform.position) < boomRadius)
            {
                enemy.TakeDamage(boomDamage);
            }
        }
    }

    private IEnumerator DecastBoom()
    {
        yield return new WaitForSeconds(4f);

        if(particleInstance != null)
        {
            Destroy(particleInstance);
        }
    }

    private IEnumerator CastBolt(Vector3 pos)
    {
        yield return new WaitForSeconds(1.5f);

        lightningBoltInstance = Instantiate(lightningBolt, pos, new Quaternion());
        lightningBoltInstance.transform.GetChild(1).transform.position = pos;
        lightningBoltInstance.transform.GetChild(0).transform.position = pos + Vector3.up * 100;
    }

    private IEnumerator DecastBolt()
    {
        yield return new WaitForSeconds(3.5f);

        if (lightningBoltInstance != null)
        {
            Destroy(lightningBoltInstance);
        }
    }

    private IEnumerator CastTrigger()
    {
        yield return new WaitForSeconds(1.6f);
       
        Turret[] turrets = FindObjectsOfType<Turret>();
        foreach (Turret turret in turrets)
        {
            turret.fireSpeedMultiplier = triggerMultiplier;
            turret.gameObject.transform.GetChild(1).gameObject.SetActive(true);
        }
    }

    private IEnumerator DecastTrigger()
    {
        yield return new WaitForSeconds(1.6f + triggerDuration);

        Turret[] turrets = FindObjectsOfType<Turret>();
        foreach (Turret turret in turrets)
        {
            turret.fireSpeedMultiplier = 1f;
            turret.gameObject.transform.GetChild(1).gameObject.SetActive(false);
        }
    }

    private IEnumerator CastFortify()
    {   
        yield return new WaitForSeconds(1.6f);

        Structure[] structures = FindObjectsOfType<Structure>();
        foreach (Structure structure in structures)
        {
            GameObject tower = structure.gameObject;
            if (tower.transform.childCount > 0)
            {
                GameObject shield = tower.transform.GetChild(0).gameObject;
                shield.SetActive(true);
                structure.setProtection(true);
            }
        }
    }

    private IEnumerator DecastFortify()
    {
        yield return new WaitForSeconds(1.6f + triggerDuration);

        Structure[] structures = FindObjectsOfType<Structure>();
        foreach (Structure structure in structures)
        {
            GameObject tower = structure.gameObject;
            if (tower.transform.childCount > 0)
            {
                GameObject shield = tower.transform.GetChild(0).gameObject;
                shield.SetActive(false);
                structure.setProtection(false);
            }
        }
    }


    public void OnTriggerEnter(Collider other)
    {
        if(other.gameObject.GetComponent<WaterQuad>() != null)
        {
            //Make fade
            fader.GetComponent<Animator>().SetTrigger("Fade");
            canMove = false;
            StartCoroutine("FadeIn");
            StartCoroutine("FadeOut");
        }
    }

    private IEnumerator FadeIn()
    {
        yield return new WaitForSeconds(0.5f);
        transform.position = lastValidPosition;
    }
    private IEnumerator FadeOut()
    {
        yield return new WaitForSeconds(1);
        canMove = true;
    }

}
