using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraPlayer : MonoBehaviour
{
    [SerializeField] private Transform cam;
    [SerializeField] private Transform follow_target;

    [SerializeField] private float MouseSensitivity = 4f;
    [SerializeField] private float ScrollSensitvity = 2f;
    [SerializeField] private float RotateValue = 10f;
    [SerializeField] private float ScrollValue = 6f;
    [SerializeField] private float CamDistance = 10f;
    [SerializeField] private float MaxCamDistance = 100f;
    [SerializeField] private float MinCamDistance = 5f;

    private Vector3 rotation;

    void LateUpdate()
    {
        var parent = transform.parent;
        var parent1 = parent.parent;
        parent.position = parent1.GetChild(1).position +
                          parent1.GetChild(1).lossyScale.y * 2f * Vector3.up;


        if (Input.GetMouseButton(2))
        {
            if (Input.GetAxis("Mouse X") != 0 || Input.GetAxis("Mouse Y") != 0)
            {
                rotation.x += Input.GetAxis("Mouse X") * MouseSensitivity;
                rotation.y -= Input.GetAxis("Mouse Y") * MouseSensitivity;
                rotation.y = Mathf.Clamp(rotation.y, 0f, 90f);
            }
        }

        if (Input.GetAxis("Mouse ScrollWheel") != 0f)
        {
            float scrollAmount = Input.GetAxis("Mouse ScrollWheel") * ScrollSensitvity;
            scrollAmount *= (CamDistance * 0.3f);
            CamDistance += scrollAmount * -1f;
            CamDistance = Mathf.Clamp(CamDistance, MinCamDistance, MaxCamDistance);
        }

        Quaternion qtTransf = Quaternion.Euler(rotation.y, rotation.x, 0);
        follow_target.rotation = Quaternion.Lerp(follow_target.rotation, qtTransf, Time.deltaTime * RotateValue);

        if (cam.localPosition.z != CamDistance * -1f)
        {
            cam.localPosition = new Vector3(0f, 0f,
                Mathf.Lerp(cam.localPosition.z, CamDistance * -1f, Time.deltaTime * ScrollValue));
        }
    }
}