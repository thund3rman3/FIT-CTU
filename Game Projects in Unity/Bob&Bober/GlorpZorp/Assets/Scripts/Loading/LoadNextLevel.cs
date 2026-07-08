using Unity.Netcode;
using UnityEngine;
using UnityEngine.SceneManagement;

public class LoadNextLevel : NetworkBehaviour
{
    [SerializeField] private string m_SceneToLoad;

    private int m_PlayersInZone = 0;

    private void OnTriggerEnter2D(Collider2D collision)
    {
        if (!IsServer) return;

        if (collision.CompareTag("Bober"))
        {
            PlaySoundClientRpc();
            m_PlayersInZone++;
            //Debug.Log($"[LevelLoader] Hráè vstoupil. ({m_PlayersInZone}/{NetworkManager.Singleton.ConnectedClients.Count})");

            if (m_PlayersInZone >= NetworkManager.Singleton.ConnectedClients.Count)
            {
                //Debug.Log($"[LevelLoader] Všichni jsou tady! Naèítám: {m_SceneToLoad}");

                NetworkManager.Singleton.SceneManager.LoadScene(
                    m_SceneToLoad,
                    LoadSceneMode.Single
                );
            }
        }
    }

    private void OnTriggerExit2D(Collider2D collision)
    {
        if (!IsServer) return;

        if (collision.CompareTag("Bober"))
        {
            m_PlayersInZone--;
            //Debug.Log($"[LevelLoader] Hráè odešel. ({m_PlayersInZone}/{NetworkManager.Singleton.ConnectedClients.Count})");
        }
    }

    [Rpc(SendTo.ClientsAndHost)]
    private void PlaySoundClientRpc()
    {
        GetComponent<AudioSource>().PlayOneShot(GetComponent<AudioSource>().clip);
    }
}