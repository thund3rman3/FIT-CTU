using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

/**
 * Representation of building button functions
 */
public class ItemSelectionColumn : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler, IPointerClickHandler
{
    [SerializeField] CraftingPanelScript parent;
    public int itemID = 0;  //ID of item that is selected
    public int groupID = 0; //Category of the item selected

    void IPointerEnterHandler.OnPointerEnter(PointerEventData eventData)
    {
        if(groupID >= 0 && groupID <= 5)
        {
            parent.hover(groupID);
            //Show the cost of the item in the costBar
            parent.setCostBar(itemID, groupID);
        }
    }

    void IPointerClickHandler.OnPointerClick(PointerEventData eventData)
    {
        parent.selectItem(itemID, groupID);
    }

    void IPointerExitHandler.OnPointerExit(PointerEventData eventData)
    {
        parent.setCostBar(0, groupID);
    }


    void Update()
    {
        //Select deletion tool
        if (groupID == -1 && Input.GetKeyDown(KeyCode.R))
        {
            parent.selectItem(itemID, groupID);
        }
    }
}
