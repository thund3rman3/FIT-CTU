using Unity.Netcode;
using UnityEngine;
using UnityEngine.InputSystem;

public class ClientAuthMove : NetworkBehaviour
{
    [SerializeField] private Rigidbody2D rb;
    [SerializeField] private float moveSpeed = 5f;
    [SerializeField] private float jumpForce = 8f;
    [SerializeField] private Transform groundCheck;
    [SerializeField] private LayerMask groundLayer;
    private bool isFacingRight = true;

    private float horizontal; // 0> if going right, <0 if left

    [SerializeField] private PlayerInput m_PlayerInput;

    private void FixedUpdate()
    {
        if (!IsOwner || !IsSpawned) return;
        rb.linearVelocity = new Vector2(horizontal * moveSpeed, rb.linearVelocity.y);

        if(!isFacingRight && horizontal > 0f)
        {
            Flip();
        }
        else if(isFacingRight && horizontal < 0f)
        {
            Flip();
        }
    }

    public override void OnNetworkSpawn()
    {
        base.OnNetworkSpawn();

        if (!IsOwner)
        {
            if (m_PlayerInput != null)
            {
                m_PlayerInput.enabled = false;
            }
        }
    }

    public void Move(InputAction.CallbackContext ctx)
    {
        if (!IsOwner || !IsSpawned) return;
        horizontal = ctx.ReadValue<Vector2>().x;
    }

    public void Jump(InputAction.CallbackContext ctx)
    {
       if (!IsOwner || !IsSpawned) return;
        //Debug.Log("Jump input received " + IsGrounded());
        if (ctx.performed && IsGrounded())
        {
            rb.linearVelocity = new Vector2(rb.linearVelocity.x, jumpForce);
        }

        else if(ctx.canceled && rb.linearVelocity.y > 0f)
        {
            rb.linearVelocity = new Vector2(rb.linearVelocity.x, rb.linearVelocity.y * 0.5f);
        }
    }

    private bool IsGrounded()
    {
        Collider2D hit = Physics2D.OverlapCapsule(groundCheck.position, new Vector2(1.0f, 0.1f), CapsuleDirection2D.Horizontal, 0f, groundLayer.value);
        //Debug.Log(hit?.name);
        return hit;
    }

    private void Flip()
    {
        isFacingRight = !isFacingRight;
        Vector3 scale = transform.localScale;
        scale.x *= -1;
        transform.localScale = scale;
    }
}

