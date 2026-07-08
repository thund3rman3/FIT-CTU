using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/**
 * Not used
 */
public class PlayerMovement : MonoBehaviour
{
    [SerializeField]
    private float moveSpeed;

    [SerializeField]
    private float walkSpeed;

    [SerializeField]
    private float runSpeed;

    [SerializeField]
    private bool isGrounded;

    [SerializeField]
    private float groundCheckDistance;

    [SerializeField]
    private LayerMask groundMask;

    [SerializeField]
    private float gravity;

    [SerializeField]
    private float jumpHeight;

    [SerializeField]
    private float turnSpeed = 360;
    
    private Camera cam;

    private Vector3 moveDirection;

    private Vector3 velocity;

    private CharacterController controller;

    private Animator anim;
    public bool moving_allow = true;

    // Start is called before the first frame update
    void Start()
    {
        cam = FindObjectOfType<Camera>();
       controller =  GetComponentInChildren<CharacterController>();
       anim = GetComponent<Animator>();
    }

    // Update is called once per frame
    void Update()
    {
        if (moving_allow)
        {
            Look();
            Move();
        }
        SetGravitation();
    }

    private void Look()
    {
        float moveZ = Input.GetAxis("Vertical");
        float moveX = Input.GetAxis("Horizontal");
       // moveDirection = new Vector3(moveX, 0, moveZ);
        moveDirection = (moveZ *  cam.transform.forward + moveX * cam.transform.right) ;
        moveDirection.y=0;
        moveDirection=moveDirection.normalized;
        if (moveDirection == Vector3.zero) return;

        var rotation = Quaternion.LookRotation((transform.position + moveDirection) -transform.position,Vector3.up);
        transform.rotation = Quaternion .RotateTowards(transform.rotation, rotation, turnSpeed * Time.deltaTime);
    }

    private void Move()
    {
        

        //movedirection *= movespeed;
        // moveDirection = transform.TransformDirection(moveDirection);
        // moveDirection = Vector3.Normalize(moveDirection);
        if (moveDirection != Vector3.zero)
        {
            if (Input.GetKey("left shift"))
            {
                Run();
            }
            else
            {
                Walk();
            }
            moveDirection =(transform.forward * moveDirection.normalized.magnitude) *moveSpeed *Time.deltaTime;
        }
        else
        {
            Idle();
        }
        controller.Move (moveDirection);
       
        if (isGrounded)
        {
            anim.SetBool("Jump", false);
            if (Input.GetKey("space"))
            {
                Jump();
            }
        }

        //controller.Move( moveDirection  * Time.deltaTime);
       
    }

    private void SetGravitation()
    {
        isGrounded =Physics.CheckSphere(transform.position, groundCheckDistance, groundMask);

        //print(isGrounded);
        if (isGrounded && velocity.y < 0)
        {
            velocity.y = -2f;
        }
        anim.SetBool("isGrounded", isGrounded);
        
        velocity.y += gravity * Time.deltaTime;
        controller.Move(velocity * Time.deltaTime);
        
    }

    private void Idle()
    {
        anim.SetFloat("Speed", 0f, 0.1f, Time.deltaTime);
    }

    private void Run()
    {
        moveSpeed = runSpeed;
        anim.SetFloat("Speed", 1f, 0.1f, Time.deltaTime);
    }

    private void Walk()
    {
        moveSpeed = walkSpeed;
        anim.SetFloat("Speed", 0.5f, 0.1f, Time.deltaTime);
    }

    private void Jump()
    {
        // print("Jump\n");
       
        velocity.y = Mathf.Sqrt(jumpHeight * -2 * gravity);
        anim.SetBool("Jump", true);
      
       
    }
}

// public static class Helpers
// {
//     private static Matrix4x4
//         _isoMatrix = Matrix4x4.Rotate(Quaternion.Euler(0, 45, 0));

//     public static Vector3 ToIso(this Vector3 input) =>
//         _isoMatrix.MultiplyPoint3x4(input);
// }
