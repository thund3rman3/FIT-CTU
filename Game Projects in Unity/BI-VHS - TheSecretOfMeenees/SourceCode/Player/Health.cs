using System.Collections;
using System.Xml.Linq;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class Health : MonoBehaviour
{
    // Start is called before the first frame update
    Image HealthBar;
    TextMeshProUGUI Text;
    public float TimeToHeal = 1;
    float Timer = 0;
    private bool Ready = false;
    void Start()
    {
        HealthBar = transform.GetChild(1).GetComponent<Image>();
        Text = transform.GetChild(3).GetComponent<TextMeshProUGUI>();
        Ready = true;
    }

    // Update is called once per frame
    void Update()
    {
        if (Ready){
            /*if(PersistData.playerData.maxHealthPoints > PersistData.playerData.healthPoints){
                Timer += Time.deltaTime;
                if(Timer >= TimeToHeal){
                    Timer -= TimeToHeal;
                    PersistData.playerData.healthPoints++;
                } 
            } passive health regen*/
            HealthBar.fillAmount = (float) PersistData.playerData.healthPoints / PersistData.playerData.maxHealthPoints;
            float tmp = (HealthBar.fillAmount * 1);
            HealthBar.color = new Color(1f, tmp, tmp, 1f);
            //Debug.Log(new Color(1f, tmp, tmp, 1f) + ":" +HealthBar.color);
            Text.text =  PersistData.playerData.healthPoints + "/" + PersistData.playerData.maxHealthPoints;
        }
    }
}
