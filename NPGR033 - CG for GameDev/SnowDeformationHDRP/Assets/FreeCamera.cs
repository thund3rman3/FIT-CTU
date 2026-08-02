using UnityEngine;
using UnityEngine.InputSystem;

public class FreeCamera : MonoBehaviour
{
    private PlayerInputActions _controls;

    [Header("Settings")]
    [SerializeField] private float _mouseSensitivity = 50f;
    [SerializeField] private float _moveSpeed = 10f;

    private float _rotationX;
    private float _rotationY;

    private Vector2 _lookInput => _controls.Player.Look.ReadValue<Vector2>();
    private Vector2 _moveInput => _controls.Player.Move.ReadValue<Vector2>();

    void Awake()
    {
        _controls = new PlayerInputActions();
        Cursor.lockState = CursorLockMode.Locked;
        Cursor.visible = false;
    }

    void OnEnable()
    {
        _controls.Enable();
    }

    void OnDisable()
    {
        _controls.Disable();
    }

    void Update()
    {
        float mouseX = _lookInput.x  * _mouseSensitivity;
        float mouseY = _lookInput.y * _mouseSensitivity;
        _rotationX -= mouseY;
        _rotationX = Mathf.Clamp(_rotationX, -90f, 90f);
        _rotationY += mouseX;

        transform.localRotation = Quaternion.Euler(_rotationX, _rotationY, 0f);

        Vector3 direction = transform.forward * _moveInput.y + transform.right * _moveInput.x;
        float currentSpeed = Keyboard.current.shiftKey.isPressed ? _moveSpeed * 2.5f : _moveSpeed;
        transform.position += direction * currentSpeed * Time.deltaTime;
    }
}