using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UIElements;

public class PauseMenu : MonoBehaviour
{
    public PlayerDataSO playerDataSO;
    public GameObject panel;
    public CanvasGroup overlay;

    bool panelActive, lastState;

    // Start is called before the first frame update
    void Start()
    {
        panelActive = panel.activeSelf;
        lastState = true;
    }

    // Update is called once per frame
    void Update()
    {
        lastState = UnityEngine.Cursor.visible;
        //Debug.Log(panelActive);
        if (Input.GetKeyDown(playerDataSO.pauseMenuKey))
        {
            panelActive = !panelActive;
            if (panelActive) //we are turning on the panel
            {
                overlay.alpha = 0.05f;
                
                UnityEngine.Cursor.lockState = CursorLockMode.None;
                UnityEngine.Cursor.visible = true;
                //Time.timeScale = 0;
            }
            else if (!lastState) //the cursor was not active, and we are turning off the menu
            {
                overlay.alpha = 1f;
                UnityEngine.Cursor.lockState = CursorLockMode.Locked;
                UnityEngine.Cursor.visible = false;
                //Time.timeScale = 1.0f;
            }
            else overlay.alpha = 1f;
            panel.SetActive(panelActive);
        }
    }

    public void PressedButtonExitGame()
    {
        if(PersistData.playerData.EnabledElements[3] == true) SaveSystem.SaveData();
        UnityEngine.Application.Quit();
    }

    public void PressedButtonExitToMenu()
    {
        if (PersistData.playerData.EnabledElements[3] == true) SaveSystem.SaveData();
        SceneManager.LoadScene("MainMenu");
    }
}
