using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using TMPro;
using Unity.VisualScripting;
using UnityEngine.UIElements;

public class PlayerMove : MonoBehaviour
{
    public PlayerMovementSO PlayerMoveSO;
    public Transform orientation;
    public MovementState state;
    public CharacterController PlayerController;
    public GameObject capsule;

    float  moveSpeed;
    float startYScale;
    bool grounded = true;

    float horizontalInput;
    float verticalInput;

    Vector3 moveDirection;
    Vector3 velocity;
    float gravity = -9.81f;
    bool canCrouch = true;
    bool crouched = false;

    public enum MovementState
    {
        walking,
        sprinting,
        crouching,
        air
    }

    private void Start()
    {
        moveSpeed = PlayerMoveSO.walkSpeed;
        startYScale = transform.localScale.y;
    }

    private void Update()
    {
        // ground check
        grounded = Physics.Raycast(transform.position, Vector3.down, PlayerMoveSO.playerHeight * 0.5f+0.1f, PlayerMoveSO.whatIsGround);
       
        MyInput();
        StateHandler();
        MovePlayer();
    }

    private void MovePlayer()
    {
        if (!PersistData.playerData.stopped)
        {
            if (grounded && velocity.y < 0)
                velocity.y = -2f;

            // calculate movement direction
            moveDirection = orientation.forward * verticalInput + orientation.right * horizontalInput;
            PlayerController.Move(moveDirection * moveSpeed * Time.deltaTime);

            if (Input.GetKey(PlayerMoveSO.jumpKey) && grounded && state != MovementState.crouching && !PersistData.playerData.brokenLegs)
            {
                canCrouch = false;
                velocity.y = Mathf.Sqrt(PersistData.playerData.jumpHeight* -2f * gravity);
                Debug.Log("jumped " + velocity.y);
                Invoke("DoCrouch", PersistData.playerData.jumpHeight / 1.5f);
            }

            velocity.y += gravity * Time.deltaTime;

            PlayerController.Move(velocity * Time.deltaTime);
        }
    }



    private void MyInput()
    {
        horizontalInput = Input.GetAxis("Horizontal");
        verticalInput = Input.GetAxis("Vertical");

        // start crouch
        if (Input.GetKeyDown(PlayerMoveSO.crouchKey) && grounded && canCrouch && !crouched)
        {
            crouched = true;
            capsule.transform.localScale = new Vector3(capsule.transform.localScale.x, PlayerMoveSO.crouchYScale, capsule.transform.localScale.z);
            capsule.transform.position = new Vector3(capsule.transform.position.x, capsule.transform.position.y - 0.5f, capsule.transform.position.z);
        }

        // stop crouch
        if (Input.GetKeyUp(PlayerMoveSO.crouchKey) && grounded && canCrouch && crouched)
        {
            crouched = false;
            capsule.transform.localScale = new Vector3(capsule.transform.localScale.x, startYScale, capsule.transform.localScale.z);
            capsule.transform.position = new Vector3(capsule.transform.position.x, capsule.transform.position.y + 0.5f, capsule.transform.position.z);
        }
    }

    private void StateHandler()
    {
        // Mode - Crouching
        if (Input.GetKey(PlayerMoveSO.crouchKey) && grounded)
        {
            state = MovementState.crouching;
            moveSpeed = PlayerMoveSO.crouchSpeed;
        }

        // Mode - Sprinting
        else if (grounded && Input.GetKey(PlayerMoveSO.sprintKey))
        {
            state = MovementState.sprinting;
            moveSpeed = PlayerMoveSO.sprintSpeed;
        }

        // Mode - Walking
        else if (grounded)
        {
            state = MovementState.walking;
            moveSpeed = PlayerMoveSO.walkSpeed;
        }

        // Mode - Air
        else
            state = MovementState.air;
        if (PersistData.playerData.brokenLegs) moveSpeed = PlayerMoveSO.walkSpeed / 2f;
    }

    void DoCrouch()
    {
        canCrouch = true;
    }
}