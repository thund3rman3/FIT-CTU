using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.InputSystem;

public class NotNetMove : MonoBehaviour
{
    private Rigidbody2D rb;
    [SerializeField] private float moveSpeed = 5f;
    [SerializeField] private float jumpForce = 8f;
    [SerializeField] private Transform groundCheck;
    [SerializeField] private LayerMask groundLayer;
    private bool isFacingRight = true;
    private bool isGrounded = false;

    private float horizontal; // 0> if going right, <0 if left

    [SerializeField] private PlayerInput m_PlayerInput;

    List <GameObject> groundCollisions = new List <GameObject> ();

    void Start()
    {
        rb = GetComponent<Rigidbody2D>();
    }

    private void Update()
    {
        // get orientation of the player
        horizontal = Input.GetAxis ( "Horizontal" );

        // flip the sprite if needed
        Flip();

        // is on ground check for jumping
        if ( groundCollisions.Count() != 0 ) { // on ground
            isGrounded = true;
        }
        else { // falling or mid-jump
            isGrounded = false;  
        }

        // jump pressed
        if ( Input.GetButton ( "Jump" ) && isGrounded ) {
            rb.linearVelocity = new Vector2 ( rb.linearVelocity.x, jumpForce );
            isGrounded = false;
        }
    }

    private void FixedUpdate()
    {
        rb.linearVelocity = new Vector2 ( horizontal * moveSpeed, rb.linearVelocity.y );
    }

    private void Flip()
    {
        if ( !isFacingRight && horizontal > 0.0f || isFacingRight && horizontal < 0.0f ) {
            isFacingRight = !isFacingRight;
            Vector3 scale = transform.localScale;
            scale.x *= -1;
            transform.localScale = scale;
        }
    }

    void OnCollisionEnter2D ( Collision2D c )
    {
        Vector2 direction = c.GetContact(0).normal;

        if ( direction.y >= 0.7 ) { // collision at the bottom
            groundCollisions.Add ( c.gameObject );
        }
    }

    void OnCollisionExit2D ( Collision2D c ) 
    {
        if ( groundCollisions.Contains ( c.gameObject ) ) { 
            groundCollisions.Remove ( c.gameObject ); 
        }
    }
}

