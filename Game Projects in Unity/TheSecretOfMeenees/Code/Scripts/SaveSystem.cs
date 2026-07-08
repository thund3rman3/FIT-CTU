using UnityEngine;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;

public static class SaveSystem 
{
    public static void SaveData()
    {
        BinaryFormatter form = new BinaryFormatter();
        string path = Application.persistentDataPath + "/tSoM_savegame.Meenee";
        FileStream stream = new FileStream(path, FileMode.Create);

        SaveGame save = new SaveGame();

        form.Serialize(stream, save);
        stream.Close();
    }
    public static void DeleteSaveData()
    {
        File.Delete(Application.persistentDataPath + "/tSoM_savegame.Meenee");
    }
    public static SaveGame LoadData()
    {
        string path = Application.persistentDataPath + "/tSoM_savegame.Meenee";
        if(File.Exists(path))
        {
            BinaryFormatter form = new BinaryFormatter();
            FileStream stream = new FileStream(path, FileMode.Open);

            SaveGame save = form.Deserialize(stream) as SaveGame;
            stream.Close();
            Debug.Log(save.playerData);
            return save;
        }
        else
        {
            return null;
        }
    }
}
