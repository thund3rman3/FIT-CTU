using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using DigitalRuby.LightningBolt;
using UnityEngine.AI;
using Unity.Mathematics;
using Unity.Collections;

/*
 * Not used
 */
public class PlayerTool : MonoBehaviour
{
    // private bool _use = false;
    private Animator _anim;


    private const float PickaxeValue = 1.0f;
    private const float GatherningValue = 2f;
    private const float HammerValue = 3f;


    private PlayerMovement _movingScrip;
    private float _chooseTool = 1;
    private static readonly int Tool = Animator.StringToHash("Tool");
    private static readonly int Gathering = Animator.StringToHash("Gathering");

   public GameObject pickaxe;
   public GameObject hammer;

    public bool working;
    public RaycastHit Hit;

  

    // GameObject.FindWithTag("PlayerMovement");
    // Start is called before the first frame update
    void Start()
    {
        _anim = GetComponent<Animator>();
        _movingScrip = GetComponent<PlayerMovement>();
    }

    // Update is called once per frame
    void Update()
    {
        
        ChooseTool();


        

        if (Input.GetKey(KeyCode.Mouse0) && !Rotate())
        {

            _anim.SetBool(Gathering, true);
            _anim.SetFloat(Tool, _chooseTool);
            ShowTool();
            if (!_movingScrip.moving_allow) return;
            working = true;
            _movingScrip.moving_allow = false;
        }
        else
        {
            // _anim.SetLayerWeight(_anim.GetLayerIndex("Use tool"), 0);
            _anim.SetBool(Gathering, false);
            _anim.SetFloat(Tool, 0f, 0.1f, Time.deltaTime);
            HideTool();
            if (_movingScrip.moving_allow) return;
            working = false;
            _movingScrip.moving_allow = true;
        }
    }
    
  

    private bool Rotate()
    {
        if (Camera.main is { })
        {
            Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);

            if (!Physics.Raycast(ray, out Hit, 1000)) return true;
        }

        Vector3 camPosition = new Vector3(Hit.point.x, Hit.point.y, Hit.point.z);
        // Debug.Log("cam:" + camPosition.ToString() + " pl:" + transform.position.ToString() + ", " +
        //           hit.transform.CompareTag("Wall").ToString() + ", " +
        //           Vector3.Distance(transform.position, hit.transform.position).ToString());
        if (!Hit.transform.CompareTag("Wall") ||
            !(Vector3.Distance(transform.position, Hit.transform.position) < 2)) return true;
        var angle = Mathf.Atan2(camPosition.x - transform.position.x, camPosition.z - transform.position.z) * Mathf.Rad2Deg;
        var look = Quaternion.Euler(new Vector3(0, angle, 0));
        transform.rotation = Quaternion.Slerp(transform.rotation, look, Time.deltaTime * 1f);
        return false;
    }

    private void ChooseTool()
    {
        if (Input.GetKeyDown(KeyCode.Alpha1))
        {
            _chooseTool = PickaxeValue;
        }
        else if (Input.GetKeyDown(KeyCode.Alpha2))
        {
            _chooseTool = GatherningValue;
        }
        else if (Input.GetKeyDown(KeyCode.Alpha3))
        {
            _chooseTool = HammerValue;
        }
    }

    private void ShowTool()
    {
        switch (_chooseTool)
        {
            case PickaxeValue:
                hammer.SetActive(false);
                pickaxe.SetActive(true);
                break;
            case HammerValue:
                pickaxe.SetActive(false);
                hammer.SetActive(true);
                break;
        }
    }

    private void HideTool()
    {
        hammer.SetActive(false);
        pickaxe.SetActive(false);
    }
    

}