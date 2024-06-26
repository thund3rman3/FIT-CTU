using UnityEngine;

public class PlayerCam : MonoBehaviour
{
    float rotationX;
    public Transform player; // PLAYER WITH CAMERA 

    void Start()
    {
        Cursor.lockState = CursorLockMode.Locked;
        Cursor.visible = false;
    }
    void Update()
    {   
        if (true)
        {
            float mouseX = Input.GetAxis("Mouse X") * Time.deltaTime * PersistData.settings.sensitivity;
            float mouseY = Input.GetAxis("Mouse Y") * Time.deltaTime * PersistData.settings.sensitivity;

            rotationX -= mouseY;
            rotationX = Mathf.Clamp(rotationX, -90f, 90f);

           
            transform.localRotation = Quaternion.Euler(rotationX, 0f, 0f);
            player.Rotate(Vector3.up * mouseX);
        }
    }
}
