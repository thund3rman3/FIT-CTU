using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using UnityEngine;
using UnityEngine.SceneManagement;

[System.Serializable]
public class GameData
{
    public string m_LastScene { get; private set; }
    public string m_SessionName { get; private set; }

    public GameData(string lastScene, string sessionName)
    {
        m_LastScene = lastScene;
        m_SessionName = sessionName;
    }
}


public static class SaveLoadManager
{
    public static void SaveGame()
    {
        BinaryFormatter formatter = new BinaryFormatter();
        string path = Application.persistentDataPath + "/savefile";
        FileStream stream = new FileStream(path, FileMode.Create);

        string lastScene = SceneManager.GetActiveScene().name.ToString();
        string sessionName = ConnectionManager.Instance.m_SessionName;

        formatter.Serialize(stream, new GameData(lastScene, sessionName));
        stream.Close();
    }

    public static GameData LoadGame()
    {
        string path = Application.persistentDataPath + "/savefile";
        if (File.Exists(path))
        {
            BinaryFormatter formatter = new BinaryFormatter();
            FileStream stream = new FileStream(path, FileMode.Open);
            GameData data = formatter.Deserialize(stream) as GameData;
            stream.Close();
            //Debug.Log("Game loaded: " + data);
            return data;
        }
        else
        {
            Debug.LogError("Save file not found in " + path);
            return null;
        }
    }
}
