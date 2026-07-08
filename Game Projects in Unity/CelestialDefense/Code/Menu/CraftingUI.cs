using UnityEngine;
using UnityEngine.UI;

/*
 * Not used
 */
public class CraftingUI : MonoBehaviour
{
    private GameObject openedCategory;
    public GameObject MenuButton;
    public GameObject Categories;

    void Update()
    {
        if (Input.GetMouseButtonDown(1))
        {
            Debug.Log("Close menu category");
            CloseCategory();
        }
    }

    public void OpenCraftingMenu()
    {
        gameObject.SetActive(true);
        MenuButton.SetActive(false);
    }

    public void CloseCraftingMenu()
    {
        CloseCategory();
        gameObject.SetActive(false);
        MenuButton.SetActive(true);
    }

    public void OpenCategory(GameObject category)
    {
        Categories.SetActive(false);
        category.SetActive(true);
        openedCategory = category;
    }

    public void CloseCategory()
    {
        if (openedCategory != null)
        {
            openedCategory.SetActive(false);
            Categories.SetActive(true);
            openedCategory = null;
        }
    }

    public void BackButtonFunction()
    {
        if (openedCategory != null)
        {
            CloseCategory();
        }
        else
        {
            CloseCraftingMenu();
        }
    }


}
