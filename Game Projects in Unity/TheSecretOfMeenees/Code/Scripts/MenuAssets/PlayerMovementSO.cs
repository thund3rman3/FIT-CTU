using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using static PlayerMove;

[CreateAssetMenu(fileName = "PlayerMovement", menuName = "MENU/PlayerMovement")]
public class PlayerMovementSO: ScriptableObject
{
    [Header("Movement")]
    public float walkSpeed;
    public float sprintSpeed;

    [Header("Jumping")]
    public float jumpHeight;

    [Header("Crouching")]
    public float crouchSpeed;
    public float crouchYScale;

    [Header("Keybinds")]
    public KeyCode jumpKey = KeyCode.Space;
    public KeyCode sprintKey = KeyCode.LeftShift;
    public KeyCode crouchKey = KeyCode.LeftControl;

    [Header("Ground Check")]
    public float playerHeight;
    public LayerMask whatIsGround;

}
