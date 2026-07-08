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
    float lastAlpha;

    // Start is called before the first frame update
    void Start()
    {
        panelActive = panel.activeSelf;
        lastState = true;
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(playerDataSO.pauseMenuKey))
        {
            panelActive = !panelActive;
            if (panelActive) //we are turning on the panel
            {
                lastState = UnityEngine.Cursor.visible;
                lastAlpha = overlay.alpha;
                overlay.alpha = 0.05f;
                
                UnityEngine.Cursor.lockState = CursorLockMode.None;
                UnityEngine.Cursor.visible = true;
                Time.timeScale = 0;
            }
            else if (!panelActive) //we are turning off the menu
            {
                overlay.alpha = lastAlpha;
                UnityEngine.Cursor.lockState = CursorLockMode.Locked;
                UnityEngine.Cursor.visible = lastState;
                Time.timeScale = 1.0f;
            }
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
        Time.timeScale = 1.0f;
        if (PersistData.playerData.EnabledElements[3] == true) SaveSystem.SaveData();
        SceneManager.LoadScene("MainMenu");
    }
}
