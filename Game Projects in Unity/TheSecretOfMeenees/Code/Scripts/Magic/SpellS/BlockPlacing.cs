using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlockPlacing : MonoBehaviour
{
    [SerializeField] int CollisionCount = 0;
    public GameObject Red;
    [SerializeField] GameObject Green;
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        Stabilize();
    }
    void Stabilize(){
        transform.rotation = Quaternion.Euler(90f,transform.parent.rotation.eulerAngles.y,0f);
    }
    void OnCollisionEnter(Collision other) {
        if(other.gameObject.CompareTag("Magic") || other.gameObject.CompareTag("Enemy"))
        {
            Debug.Log("Collision Block:" + CollisionCount);
            if (CollisionCount == 0) CorrectPlacement(false);
            CollisionCount++;
        }
        else if (other.gameObject.CompareTag("Lava") || other.gameObject.CompareTag("FireDestroy")
            || other.gameObject.CompareTag("AirDestroy") || other.gameObject.CompareTag("WaterDestroy"))
        {
            GetComponentInParent<MagicScript>().m_destroyableWithBlock = other.transform;
            Debug.Log(GetComponentInParent<MagicScript>().m_destroyableWithBlock.gameObject);
        }
    }
    void OnCollisionExit(Collision other) {
        if (other.gameObject.CompareTag("Magic") || other.gameObject.CompareTag("Enemy") || other.gameObject.CompareTag("Lava"))
        {
            Debug.Log("Collision Block:" + CollisionCount);
            CollisionCount--;
            if (CollisionCount == 0) CorrectPlacement(true);
        }
        else if (other.gameObject.CompareTag("Lava") || other.gameObject.CompareTag("FireDestroy")
            || other.gameObject.CompareTag("AirDestroy") || other.gameObject.CompareTag("WaterDestroy"))
        {
            GetComponentInParent<MagicScript>().m_destroyableWithBlock = null;
        }
    }
    
    void CorrectPlacement(bool correct){
        Debug.Log("Collision Block:" + correct);
        Red.SetActive(!correct);
        Green.SetActive(correct);
    }
}
