using Unity.Netcode;
using UnityEngine;
using UnityEngine.SceneManagement;

public class PauseMenuManager : NetworkBehaviour
{
    [SerializeField] private GameObject m_PauseMenu;
    private ConnectionManager m_ConnectionManager;

    private CPlayer m_Player;

    private void Start()
    {
        m_ConnectionManager = FindFirstObjectByType<ConnectionManager>();
        m_PauseMenu.SetActive ( false );
        m_Player = transform.parent.gameObject.GetComponent<CPlayer>();
    }

    private void Update()
    {
        if (Input.GetKeyDown(KeyCode.Escape) && IsOwner)
        {
            TogglePauseMenu();
        }
    }

    public void TogglePauseMenu()
    {
        m_PauseMenu.SetActive ( !m_PauseMenu.activeSelf );
        if (m_PauseMenu.activeSelf)
        {
            m_Player.m_Paused = true; // Pause the game
        }
        else
        {
            m_Player.m_Paused = false; // Resume the game
        }
    }

    public async void Disconnect()
    {
        await m_ConnectionManager.Disconnect();
        Destroy(m_ConnectionManager.gameObject);
        SceneManager.LoadScene("MainMenu");
        Time.timeScale = 1f;
    }

    public void Quit()
    {
        _ = m_ConnectionManager.Disconnect();
        Application.Quit();
    }

    public void Restart()
    {
        NetworkManager.Singleton.SceneManager.LoadScene(
                    SceneManager.GetActiveScene().name,
                    LoadSceneMode.Single
                    );
    }
}
